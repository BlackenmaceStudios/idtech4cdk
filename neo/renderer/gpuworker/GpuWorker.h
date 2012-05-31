// GpuWorker.h
//

#include "GpuWorker_Program.h"

/*
=======================================

GPU Worker

GPU Woker uses OpenCL to pass off a lot of work of various processing intensive operations to the GPU.
This is great for stuff like texture readback, upload of various texture blobs that do not conform with
the standard less-than-par OpenGL texture formats.
=======================================
*/

//
// bmGpuWorkerHelper
//
class bmGpuWorkerHelper {
public:
	template <class T>
	static bmGPUWorkerProgram		*LoadProgram( const char *path );
};

//
// bmGpuWorker
//
class bmGpuWorker {
friend class bmGpuWorkerHelper;
public:
	virtual void					Init( void ) = 0;
	virtual void					Shutdown( void ) = 0;
								
	virtual bmGPUWorkerProgram		*FindProgram( const char *path ) = 0;
protected:
	virtual void					AppendProgram( bmGPUWorkerProgram *program ) = 0;
};

extern bmGpuWorker				*gpuWorker;

/*
======================
bmGpuWorkerHelper::LoadProgram
======================
*/
template <class T>
ID_INLINE bmGPUWorkerProgram *bmGpuWorkerHelper::LoadProgram( const char *path ) {
	T *program = NULL;

	// Check to see if the program is already loaded.
	program = (T *)gpuWorker->FindProgram( path );
	if(program != NULL) {
		return program;
	}

	// Create the new program object.
	program = new T();

	// Load in the worker program(if this fails its a fatal error).
	program->LoadWorkerProgram( path );

	// Call the protected function to append the program.
	gpuWorker->AppendProgram( program );

	return program;
}

