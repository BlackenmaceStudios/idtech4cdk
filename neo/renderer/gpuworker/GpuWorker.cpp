// GpuWorker.cpp
//

#include "precompiled.h"
#include "../tr_local.h"

#include "GpuWorker_Local.h"

bmGpuWorkerLocal		gpuWorkerLocal;
bmGpuWorker				*gpuWorker = &gpuWorkerLocal;

cl_int					bmGpuWorkerLocal::clError;

/*
=============
bmGpuWorkerLocal::Init
=============
*/
void bmGpuWorkerLocal::Init( void ) {
	common->Printf( "----------- R_GPUWorker_Init ----------\n");

	

	common->Printf( "Init OpenCL...\n");

	// Get the opencl device platform id.
	common->Printf( "...Getting Device Platform ID.\n");
	ID_GPUWORKER_CALLAPI(oclGetPlatformID(&platform));
	CheckLastAPICall();

	// Get all the device ids
	common->Printf( "...Getting Device ID.\n");
	ID_GPUWORKER_CALLAPI(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL));
	CheckLastAPICall();

	cl_context_properties properties[] = {
	   CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(), 
	   CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(), 
	   CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 
     0};

	// Create the OpenCL device context
	common->Printf( "...Creating Device Context.\n");
	context = clCreateContext(properties, 1, &device, NULL, NULL, &clError);
	CheckLastAPICall();

	// Create the opencl command queue.
	common->Printf( "...Creating Device Command Queue.\n");
	queue = clCreateCommandQueue(context, device, 0, &clError);
	CheckLastAPICall();

}

/*
=============
bmGpuWorkerLocal::CheckLastAPICall
=============
*/
bmGPUWorkerProgram * bmGpuWorkerLocal::FindProgram( const char *path ) {
	for(int i = 0; i < programPool.Num(); i++) {
		if(!strcmp(programPool[i]->Name(), path)) {
			return programPool[i];
		}
	}

	return NULL;
}

/*
=============
bmGpuWorkerLocal::CheckLastAPICall
=============
*/
void bmGpuWorkerLocal::CheckLastAPICall( void ) {
	if(clError != CL_SUCCESS) {
		
		common->FatalError( "GpuWorker Device Error: %s\n", oclErrorString( clError ));
	}
}

/*
=============
bmGpuWorkerLocal::Shutdown
=============
*/
void bmGpuWorkerLocal::Shutdown( void ) {
	common->Printf( "----------- R_GPUWorker_Shutdown ----------\n");

	common->Printf( "Clearing render program workers...\n");
	for(int i = 0; i < programPool.Num(); i++) {
		programPool[i]->Free();
		delete programPool[i];
	}

	programPool.Clear();

	common->Printf( "Releasing command queue\n");
	clReleaseCommandQueue(queue);

	common->Printf( "Releasing OpenCL context\n");
	clReleaseContext(context);
}