// ptex_local.h
//

// ----------------------------
#undef snprintf
#undef strncpy
#undef strncmp
#undef strcmp
#define  FLT_EPSILON 1.19209290E-07F
#include <math.h>
// ----------------------------

#include "ptex/PtexPlatform.h"
#include "ptex/Ptexture.h"

/*
================
bmPtexInputHandler
================
*/
class bmPtexInputHandler : public PtexInputHandler
{
    public:
		virtual Handle open(const char* path) { return (Handle) fileSystem->OpenFileRead( path ); }
		virtual void seek(Handle handle, int64_t pos) { ((idFile *)handle)->Seek( pos, FS_SEEK_SET ); }
		virtual size_t read(void* buffer, size_t size, Handle handle) {
			return ((idFile *)handle)->Read(buffer, size );
		}
		virtual bool close(Handle handle) { fileSystem->CloseFile( ((idFile *)handle) ); return true; }
		virtual const char* lastError() { return strerror(errno); }
};

bmPtexInputHandler defaultPtexHandler;
void *defaultInputHandler = &defaultPtexHandler;