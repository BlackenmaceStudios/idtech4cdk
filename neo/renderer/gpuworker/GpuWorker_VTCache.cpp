// GpuWorker_VTCache.cpp
//

#include "precompiled.h"
#include "../tr_local.h"

#include "GpuWorker_Local.h"
#include "GpuWorker_VTCache.h"

#include "../../vt/VirtualTextureFile.h"
#include "../../vt/VirtualTexture_Backend.h"

/*
=============================
bmVirtualTextureCacheGPUWorker::LoadWorkerProgram
=============================
*/
void bmVirtualTextureCacheGPUWorker::LoadWorkerProgram( const char *path ) {
	int vtCacheSize = 0;

	// Load the worker program.
	bmGPUWorkerProgram::LoadWorkerProgram( path );

	// Right now the cache holds level 2 data.
	vtCacheSize = sizeof(bmVirtualTextureHeader_t) + (VT_MAXCHARTS * (64 * 64));

	// Allocate the space needed for the cache on the gpu.
	common->Printf( "VirtualTexture GPU Cache Size %d bytes...\n", vtCacheSize );
	vtLevelTwoAddr = clCreateBuffer( gpuWorkerLocal.GetDeviceContext(), CL_MEM_READ_WRITE, vtCacheSize, NULL, &bmGpuWorkerLocal::clError );

	if(ID_GPUWORKER_HASERROR) {
		common->FatalError( "Failed to allocate VirtualTexture GPU cache...\n");
	}

	uploadTileKernel = CreateKernel( "UploadTile" );
}
/*
=============================
bmVirtualTextureCacheGPUWorker::CreateSharedTexturePage
=============================
*/
void bmVirtualTextureCacheGPUWorker::CreateSharedTexturePage( idImage *image ) {
	vtTexturePagesAddrPool.Append( CreateSharedTextureHandle( image ) );
}

/*
=============================
bmVirtualTextureCacheGPUWorker::UploadLevel2Cache
=============================
*/
void bmVirtualTextureCacheGPUWorker::UploadLevel2Cache( void *buffer, int size ) {
	UploadMemory( vtLevelTwoAddr, buffer, size );
}