// GpuWorker_Program.cpp
//

#include "precompiled.h"
#include "../tr_local.h"

#include "GpuWorker_Local.h"

/*
========================
bmGPUWorkerProgram::LoadWorkerProgram
========================
*/
void bmGPUWorkerProgram::LoadWorkerProgram( const char *qpath ) {
	const char *buffer;
	size_t bufferSize = 0;
	cl_int deviceError = 0;
	cl_device_id deviceId;

	deviceId = gpuWorkerLocal.GetDeviceId();

	idStr fullPath = va("%s/%s%s", GPUWORKER_FOLDER, qpath, GPUWORKER_EXT);

	// Set the name of the program as the program path.
	name = qpath;

	// Read the worker program into memory.
	common->Printf("Loading GPU Worker Program: %s\n", fullPath.c_str() );
	bufferSize = fileSystem->ReadFile( fullPath.c_str(), (void **)&buffer);
	if(bufferSize <= 0) {
		common->FatalError("Failed to load GPU worker program %s\n", fullPath.c_str() );
		return;
	}

	common->Printf("...Program Size %d\n", bufferSize);

	// Create the program from the file buffer.
	deviceHandle = clCreateProgramWithSource( gpuWorkerLocal.GetDeviceContext(), 1, &buffer, &bufferSize, &deviceError );
	ID_GPUWORKER_CALLAPI( deviceError );

	// Check to see if there was a error creating the program.
	if(deviceHandle == NULL || ID_GPUWORKER_HASERROR) {
		common->FatalError("Failed to create GPU worker program %s -- Error %s\n", fullPath.c_str(), oclErrorString( bmGpuWorkerLocal::clError ) );
		return;
	}

	// Build the program -- this is were it does all the compilation(syntax checking, etc), and linking.
	deviceError = clBuildProgram( (cl_program)deviceHandle, 1, &deviceId, NULL, NULL, NULL );
	ID_GPUWORKER_CALLAPI( deviceError );
	if( ID_GPUWORKER_HASERROR ) {
		static char *deviceErrorLog;
		size_t log_size;

		// Get the proper size of the device error log.
		clGetProgramBuildInfo((cl_program)deviceHandle, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate the log buffer, and get the error log from the device.
		deviceErrorLog = new char[log_size + 1];
		clGetProgramBuildInfo((cl_program)deviceHandle, deviceId, CL_PROGRAM_BUILD_LOG, log_size, deviceErrorLog, NULL);

		idStr deviceLogTemp = deviceErrorLog;
		delete[] deviceErrorLog;

		common->FatalError( "GPU Worker Program(%s) Compile Error %s\n", fullPath.c_str(), deviceLogTemp.c_str() );
	}
}

/*
========================
bmGPUWorkerProgram::UploadMemory

Uploads a block of memory to the graphics card.
========================
*/
void bmGPUWorkerProgram::UploadMemory( gpuWorkerMemoryPtr_t memhandle, void *data, int size ) {
	// Force blocking right now.
	if(clEnqueueWriteBuffer( gpuWorkerLocal.GetCommandQueue(), (cl_mem)memhandle, true, 0, size, data, NULL, NULL, NULL ) <= 0) {
		common->FatalError("Upload GPU Memory block failed.\n");
	}
}

/*
========================
bmGPUWorkerProgram::CreateSharedTextureHandle

Creates a shared texture handle.
========================
*/
gpuWorkerMemoryPtr_t bmGPUWorkerProgram::CreateSharedTextureHandle( idImage *image ) {
	cl_mem ptr;

	// Create the shared gl texture2d object.
	ptr = clCreateFromGLTexture2D( gpuWorkerLocal.GetDeviceContext(), CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, image->texnum, &bmGpuWorkerLocal::clError );
	if(ptr == NULL || ID_GPUWORKER_HASERROR) {
		common->Warning("CreateSharedTextureHandle: Failed to create shared handle from image %s\n", image->Name() );
		gpuWorkerLocal.CheckLastAPICall();
		return NULL;
	}

	return (gpuWorkerMemoryPtr_t)ptr;
}
/*
========================
bmGPUWorkerProgram::CreateKernel

========================
*/
gpuWorkerKernelHandle_t	bmGPUWorkerProgram::CreateKernel( const char *kernelName ) {
	cl_kernel kernel;

	kernel = clCreateKernel( (cl_program)deviceHandle, kernelName, &bmGpuWorkerLocal::clError);
	if(kernel == NULL || ID_GPUWORKER_HASERROR) {
		common->FatalError("Failed to create kernel %s\n", kernelName );
	}

	return (gpuWorkerKernelHandle_t)kernel;
}

void bmGPUWorkerProgram::Free( void ) {

}