// GpuWorker_Local.h
//

#include <cl/Opencl.h>
#include <cl/cl_utils.h>

#define ID_GPUWORKER_CALLAPI( x )	bmGpuWorkerLocal::clError = x
#define ID_GPUWORKER_HASERROR		bmGpuWorkerLocal::clError != CL_SUCCESS
//
// bmGpuWorkerLocal
//
class bmGpuWorkerLocal : public bmGpuWorker {
public:
	virtual void					Init( void );
	virtual void					Shutdown( void );

	virtual bmGPUWorkerProgram		*FindProgram( const char *path );
	virtual void					AppendProgram( bmGPUWorkerProgram *program ) { programPool.Append( program ); };

	cl_context						GetDeviceContext( void ) { return context; }
	cl_device_id					GetDeviceId( void ) { return (cl_device_id)device; }
	cl_command_queue				GetCommandQueue( void ) { return queue; }

	static cl_int					clError;
	void							CheckLastAPICall( void );
private:
	


	cl_platform_id platform;
	cl_context context;
	cl_command_queue queue;
	cl_device_id device;

	idList<bmGPUWorkerProgram *>	programPool;
};

extern bmGpuWorkerLocal		gpuWorkerLocal;