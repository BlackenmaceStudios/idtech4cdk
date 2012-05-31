// GpuWorker_VTCache.h
//

//
// bmVirtualTextureCacheGPUWorker
//
class bmVirtualTextureCacheGPUWorker : public bmGPUWorkerProgram {
public:
	virtual void					LoadWorkerProgram( const char *path );

	void							CreateSharedTexturePage( idImage *image );
	void							UploadLevel2Cache( void *buffer, int size );
private:
	idList<gpuWorkerMemoryPtr_t>    vtTexturePagesAddrPool;
	gpuWorkerMemoryPtr_t			vtLevelTwoAddr;

	gpuWorkerKernelHandle_t			uploadTileKernel;
};