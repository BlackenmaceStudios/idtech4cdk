// BSPCompilerEngine.cpp 
//

#include "precompiled.h"
#include "../../sys/sys_local.h"
#pragma hdrstop

idSession *			session = NULL;
idDeclManager *		declManager = NULL;
idEventLoop *		eventLoop = NULL;

int idEventLoop::JournalLevel( void ) const { return 0; }

class bmMemoryHandlerLocal : public bmMemoryHandler  {
public:
	virtual void			*Allocate( size_t size, const char *filename, int line ) { return malloc( size ); }
	virtual void			Free( void *ptr ) { free( ptr ); }

	virtual void			*AllocAlign( int size, size_t align ) { return malloc( size ); }
	virtual void			FreeAlign( void *ptr ) { free( ptr ); }
	virtual void			*Realloc( void *ptr, int size ) { return ::realloc( ptr, size ); }
};

bmMemoryHandlerLocal  memoryHandlerLocal;

/*
==============================================================

	idCommon

==============================================================
*/

#define STDIO_PRINT( pre, post )	\
	va_list argptr;					\
	va_start( argptr, fmt );		\
	printf( pre );					\
	vprintf( fmt, argptr );			\
	printf( post );					\
	va_end( argptr )


class idCommonLocal : public idCommon {
public:
							idCommonLocal( void ) {}




								// Clears the key states		
	virtual void				KeyClearStates( void ) { }

								// Returns wether a editor currently has focus or not.
	virtual bool				EditorActive( void )  { return false; }

	virtual void				PrintLoadingMessage( const char *msg ) { }
	virtual void			Init( int argc, const char **argv, const char *cmdline ) {}
	virtual void			Shutdown( void ) {}
	virtual void			Quit( void ) {}
	virtual bool			IsInitialized( void ) const { return true; }
	virtual void			Frame( void ) {}
	virtual void			GUIFrame( bool execCmd, bool network  ) {}
	virtual void			Async( void ) {}
	virtual void			StartupVariable( const char *match, bool once ) {}
	virtual void			InitTool( const toolFlag_t tool, const idDict *dict ) {}
	virtual void			ActivateTool( bool active ) {}
	virtual void			WriteConfigToFile( const char *filename ) {}
	virtual void			WriteFlaggedCVarsToFile( const char *filename, int flags, const char *setCmd ) {}
	virtual void			BeginRedirect( char *buffer, int buffersize, void (*flush)( const char * ) ) {}
	virtual void			EndRedirect( void ) {}
	virtual void			SetRefreshOnPrint( bool set ) {}
	virtual void			Printf( const char *fmt, ... ) { STDIO_PRINT( "", "" ); }
	virtual void			VPrintf( const char *fmt, va_list arg ) { vprintf( fmt, arg ); }
	virtual void			DPrintf( const char *fmt, ... ) { /*STDIO_PRINT( "", "" );*/ }
	virtual void			Warning( const char *fmt, ... ) { STDIO_PRINT( "WARNING: ", "\n" ); }
	virtual void			DWarning( const char *fmt, ...) { /*STDIO_PRINT( "WARNING: ", "\n" );*/ }
	virtual void			PrintWarnings( void ) {}
	virtual void			ClearWarnings( const char *reason ) {}
	virtual void			Error( const char *fmt, ... ) { STDIO_PRINT( "ERROR: ", "\n" ); exit(0); }
	virtual void			FatalError( const char *fmt, ... ) { STDIO_PRINT( "FATAL ERROR: ", "\n" ); exit(0); }
	virtual const idLangDict *GetLanguageDict() { return NULL; }
	virtual const char *	KeysFromBinding( const char *bind ) { return NULL; }
	virtual const char *	BindingFromKey( const char *key ) { return NULL; }
	virtual int				ButtonState( int key ) { return 0; }
	virtual int				KeyState( int key ) { return 0; }
	virtual int					GetActiveEditors() { return 0; }
	// Unified memory handler to handle cross dll issues.
	virtual bmMemoryHandler		*GetUnifiedMemoryHandler( void ) { return &memoryHandlerLocal; }
	virtual idStrPool			*GetGlobalDictKeys( void ) { return NULL; }
	virtual idStrPool			*GetGlobalDictValues( void ) { return NULL; }
};

idCVar com_developer( "developer", "0", CVAR_BOOL|CVAR_SYSTEM, "developer mode" );

idCommonLocal		commonLocal;
idCommon *			common = &commonLocal;

/*
==============================================================

	idSys

==============================================================
*/

void			Sys_Mkdir( const char *path ) {}
ID_TIME_T			Sys_FileTimeStamp( FILE *fp ) { return 0; }

#ifdef _WIN32

#include <io.h>
#include <direct.h>

void	Sys_GrabMouseCursor( bool grabIt ) {

}

int Sys_Milliseconds( void ) {
	static int tic = 0;

	return tic++;
}

const char *Sys_Cwd( void ) {
	static char cwd[1024];

	_getcwd( cwd, sizeof( cwd ) - 1 );
	cwd[sizeof( cwd ) - 1] = 0;

	/*int i = idStr::FindText( cwd, CD_BASEDIR, false );
	if ( i >= 0 ) {
		cwd[i + strlen( CD_BASEDIR )] = '\0';
	}*/

	return cwd;
}

const char *Sys_DefaultCDPath( void ) {
	return "";
}

const char *Sys_DefaultBasePath( void ) {
	return Sys_Cwd();
}

const char *Sys_DefaultSavePath( void ) {
	return cvarSystem->GetCVarString( "fs_basepath" );
}

const char *Sys_EXEPath( void ) {
	return "";
}

int Sys_ListFiles( const char *directory, const char *extension, idStrList &list ) {
	idStr		search;
	struct _finddata_t findinfo;
	int			findhandle;
	int			flag;

	if ( !extension) {
		extension = "";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	sprintf( search, "%s\\*%s", directory, extension );

	// search
	list.Clear();

	findhandle = _findfirst( search, &findinfo );
	if ( findhandle == -1 ) {
		return -1;
	}

	do {
		if ( flag ^ ( findinfo.attrib & _A_SUBDIR ) ) {
			list.Append( findinfo.name );
		}
	} while ( _findnext( findhandle, &findinfo ) != -1 );

	_findclose( findhandle );

	return list.Num();
}

#else

const char *	Sys_DefaultCDPath( void ) { return ""; }
const char *	Sys_DefaultBasePath( void ) { return ""; }
const char *	Sys_DefaultSavePath( void ) { return ""; }
int				Sys_ListFiles( const char *directory, const char *extension, idStrList &list ) { return 0; }

#endif

xthreadInfo *	g_threads[MAX_THREADS];
int				g_thread_count;

void			Sys_CreateThread( xthread_t function, void *parms, xthreadPriority priority, xthreadInfo &info, const char *name, xthreadInfo *threads[MAX_THREADS], int *thread_count ) {}
void			Sys_DestroyThread( xthreadInfo& info ) {}

void			Sys_EnterCriticalSection( int index ) {}
void			Sys_LeaveCriticalSection( int index ) {}

void			Sys_WaitForEvent( int index ) {}
void			Sys_TriggerEvent( int index ) {}

/*
==============
idSysLocal stub
==============
*/
void			idSysLocal::DebugPrintf( const char *fmt, ... ) {}
void			idSysLocal::DebugVPrintf( const char *fmt, va_list arg ) {}

double			idSysLocal::GetClockTicks( void ) { return 0.0; }
double			idSysLocal::ClockTicksPerSecond( void ) { return 1.0; }
cpuid_t			idSysLocal::GetProcessorId( void ) { return (cpuid_t)0; }
const char *	idSysLocal::GetProcessorString( void ) { return ""; }
const char *	idSysLocal::FPU_GetState( void ) { return ""; }
bool			idSysLocal::FPU_StackIsEmpty( void ) { return true; }
void			idSysLocal::FPU_SetFTZ( bool enable ) {}
void			idSysLocal::FPU_SetDAZ( bool enable ) {}

bool			idSysLocal::LockMemory( void *ptr, int bytes ) { return false; }
bool			idSysLocal::UnlockMemory( void *ptr, int bytes ) { return false; }

void			idSysLocal::GetCallStack( address_t *callStack, const int callStackSize ) { memset( callStack, 0, callStackSize * sizeof( callStack[0] ) ); }
const char *	idSysLocal::GetCallStackStr( const address_t *callStack, const int callStackSize ) { return ""; }
const char *	idSysLocal::GetCallStackCurStr( int depth ) { return ""; }
void			idSysLocal::ShutdownSymbols( void ) {}

ID_SYS_HANDLE			idSysLocal::DLL_Load( const char *dllName ) { return 0; }
void *			idSysLocal::DLL_GetProcAddress( ID_SYS_HANDLE dllHandle, const char *procName ) { return NULL; }
void			idSysLocal::DLL_Unload( ID_SYS_HANDLE dllHandle ) { }
void			idSysLocal::DLL_GetFileName( const char *baseName, char *dllName, int maxLength ) { }

sysEvent_t		idSysLocal::GenerateMouseButtonEvent( int button, bool down ) { sysEvent_t ev; memset( &ev, 0, sizeof( ev ) ); return ev; }
sysEvent_t		idSysLocal::GenerateMouseMoveEvent( int deltax, int deltay ) { sysEvent_t ev; memset( &ev, 0, sizeof( ev ) ); return ev; }

void			idSysLocal::OpenURL( const char *url, bool quit ) { }
void			idSysLocal::StartProcess( const char *exeName, bool quit ) { }

void			idSysLocal::FPU_EnableExceptions( int exceptions ) { }

idSysLocal		sysLocal;
idSys *			sys = &sysLocal;


void GL_CheckErrors( bool ) {

}

/*
=================
R_StaticAlloc
=================
*/
void *R_StaticAlloc( int bytes ) {
	byte *buffer = (byte *)malloc( bytes );

	memset( buffer, 0, bytes );

	return buffer;
}

/*
=================
R_ClearedStaticAlloc
=================
*/
void *R_ClearedStaticAlloc( int bytes ) {
	byte *buffer = (byte *)malloc( bytes );

	memset( buffer, 0, bytes );

	return buffer;
}

/*
=================
R_StaticFree
=================
*/
void R_StaticFree( void *data ) {
	free( data );
}

/*
================
R_FrameAlloc

This data will be automatically freed when the
current frame's back end completes.

This should only be called by the front end.  The
back end shouldn't need to allocate memory.

If we passed smpFrame in, the back end could
alloc memory, because it will always be a
different frameData than the front end is using.

All temporary data, like dynamic tesselations
and local spaces are allocated here.

The memory will not move, but it may not be
contiguous with previous allocations even
from this frame.

The memory is NOT zero filled.
Should part of this be inlined in a macro?
================
*/
void *R_FrameAlloc( int bytes ) {
	byte *buffer = (byte *)malloc( bytes );

	memset( buffer, 0, bytes );

	return buffer;
}

/*
==================
R_ClearedFrameAlloc
==================
*/
void *R_ClearedFrameAlloc( int bytes ) {
	byte *buffer = (byte *)malloc( bytes );

	memset( buffer, 0, bytes );

	return buffer;
}


/*
==================
R_FrameFree

This does nothing at all, as the frame data is reused every frame
and can only be stack allocated.

The only reason for it's existance is so functions that can
use either static or frame memory can set function pointers
to both alloc and free.
==================
*/
void R_FrameFree( void *data ) {
}

