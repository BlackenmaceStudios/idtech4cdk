// Memory.h
//


#ifndef TOOLS_DLL
#ifndef NULL
#define NULL					((void *)0)
#endif
#endif

// Game typeinfo fails when this is included.
#ifndef TOOLS_DLL
#define BM_UNIFIED_MEMORY_ALLOCATOR
#endif

// jmarshall - cross dll memory allocation code.
class bmMemoryHandler {
public:
	virtual void			*Allocate( size_t size, const char *file = __FILE__, int line = __LINE__ ) = 0;
	virtual void			Free( void *ptr ) = 0;
	virtual void			*Realloc( void *ptr, int size ) = 0;

	virtual void			*AllocAlign( int size, size_t align ) = 0;
	virtual void			FreeAlign( void *ptr ) = 0;
};

extern bmMemoryHandler		*allocator;



#ifdef BM_UNIFIED_MEMORY_ALLOCATOR
// jmarshall
#include <malloc.h>

// jamrshall end
#ifndef TOOLS_DLL
#undef NULL
#endif
#endif
// jmarshall end