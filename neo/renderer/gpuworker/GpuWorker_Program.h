// GpuWorker_Program.h
//

#define GPUWORKER_FOLDER						"renderWorkerProgs"
#define GPUWORKER_EXT							".renderworkerprog"

// ----------------------------------
typedef void *			gpuWorkerProgramHandle_t;
typedef void *			gpuWorkerMemoryPtr_t;
typedef void *			gpuWorkerKernelHandle_t;
// ----------------------------------

//
// bmGPUWorkerProgram
//
class bmGPUWorkerProgram {
public:
									// Loads in the worker program.
	virtual void					LoadWorkerProgram( const char *path );

									// Frees the render program handles.
	virtual void					Free( void );

									// Returns the name of the current worker program.
	const char *					Name() { return name.c_str(); };

	
protected:
	gpuWorkerProgramHandle_t		deviceHandle;		// Handle to the gpu program device handle.

									// Uploads memory to the GPU.
	virtual void					UploadMemory( gpuWorkerMemoryPtr_t memhandle, void *data, int size );

									// Create a raw shared texture handle.
	virtual gpuWorkerMemoryPtr_t    CreateSharedTextureHandle( idImage *image );

									// Creates a kernel handle(a kernel a function were going to execute at some point).
	virtual gpuWorkerKernelHandle_t	CreateKernel( const char *kernelName );
private:
	idStr							name;
};