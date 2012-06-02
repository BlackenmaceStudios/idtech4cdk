
// ScriptDebugger.cpp : Defines the class behaviors for the application.
//

#include "precompiled.h"
#include "../sys/sys_local.h"
#include "../sys/win32/win_local.h"
#include "ScriptDebugger.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

idCVar Win32Vars_t::sys_arch( "sys_arch", "", CVAR_SYSTEM | CVAR_INIT, "" );
idCVar Win32Vars_t::sys_cpustring( "sys_cpustring", "detect", CVAR_SYSTEM | CVAR_INIT, "" );
idCVar Win32Vars_t::in_mouse( "in_mouse", "1", CVAR_SYSTEM | CVAR_BOOL, "enable mouse input" );
idCVar Win32Vars_t::win_allowAltTab( "win_allowAltTab", "0", CVAR_SYSTEM | CVAR_BOOL, "allow Alt-Tab when fullscreen" );
idCVar Win32Vars_t::win_notaskkeys( "win_notaskkeys", "0", CVAR_SYSTEM | CVAR_INTEGER, "disable windows task keys" );
idCVar Win32Vars_t::win_username( "win_username", "", CVAR_SYSTEM | CVAR_INIT, "windows user name" );
idCVar Win32Vars_t::win_xpos( "win_xpos", "3", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER, "horizontal position of window" );
idCVar Win32Vars_t::win_ypos( "win_ypos", "22", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER, "vertical position of window" );
idCVar Win32Vars_t::win_outputDebugString( "win_outputDebugString", "0", CVAR_SYSTEM | CVAR_BOOL, "" );
idCVar Win32Vars_t::win_outputEditString( "win_outputEditString", "1", CVAR_SYSTEM | CVAR_BOOL, "" );
idCVar Win32Vars_t::win_viewlog( "win_viewlog", "0", CVAR_SYSTEM | CVAR_INTEGER, "" );
idCVar Win32Vars_t::win_timerUpdate( "win_timerUpdate", "0", CVAR_SYSTEM | CVAR_BOOL, "allows the game to be updated while dragging the window" );
idCVar Win32Vars_t::win_allowMultipleInstances( "win_allowMultipleInstances", "0", CVAR_SYSTEM | CVAR_BOOL, "allow multiple instances running concurrently" );

Win32Vars_t	win32;

idGame	*			game = NULL;
idGameEdit *		gameEdit = NULL;
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

#define STDIO_PRINT   \
		va_list         argptr; \
        char            msg[1600]; \
        va_start (argptr,fmt); \
        idStr::vsnPrintf (msg, sizeof(msg), fmt, argptr); \
        va_end (argptr); \
		OutputDebugString( msg ); 

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
	virtual void			StartupVariable( const char *match, bool once );
	virtual void			InitTool( const toolFlag_t tool, const idDict *dict ) {}
	virtual void			ActivateTool( bool active ) {}
	virtual void			WriteConfigToFile( const char *filename ) {}
	virtual void			WriteFlaggedCVarsToFile( const char *filename, int flags, const char *setCmd ) {}
	virtual void			BeginRedirect( char *buffer, int buffersize, void (*flush)( const char * ) ) {}
	virtual void			EndRedirect( void ) {}
	virtual void			SetRefreshOnPrint( bool set ) {}
	virtual void			Printf( const char *fmt, ... ) {  STDIO_PRINT( fmt, fmt ); }
	virtual void			VPrintf( const char *fmt, va_list arg ) {  }
	virtual void			DPrintf( const char *fmt, ... ) { /*STDIO_PRINT( "", "" );*/ }
	virtual void			Warning( const char *fmt, ... ) { STDIO_PRINT( "WARNING: ", "\n" ); }
	virtual void			DWarning( const char *fmt, ...) { /*STDIO_PRINT( "WARNING: ", "\n" );*/ }
	virtual void			PrintWarnings( void ) {}
	virtual void			ClearWarnings( const char *reason ) {}
	virtual void			Error( const char *fmt, ... ) { MessageBox(NULL, fmt, "FATAL ERROR: ", 0 ); exit(0); }
	virtual void			FatalError( const char *fmt, ... ) { MessageBox(NULL, fmt, "FATAL ERROR: ", 0 ); exit(0); }
	virtual const idLangDict *GetLanguageDict() { return NULL; }
	virtual const char *	KeysFromBinding( const char *bind ) { return NULL; }
	virtual const char *	BindingFromKey( const char *key ) { return NULL; }
	virtual int				ButtonState( int key ) { return 0; }
	virtual int				KeyState( int key ) { return 0; }
	virtual int					GetActiveEditors() { return 0; }
	// Unified memory handler to handle cross dll issues.
	virtual bmMemoryHandler		*GetUnifiedMemoryHandler( void ) { return &memoryHandlerLocal; }


								// Dict global string pool is in the engine for shared memory to work properly.
	virtual idStrPool			*GetGlobalDictKeys( void ) { return &globalKeys; }
	virtual idStrPool			*GetGlobalDictValues( void ) { return &globalValues; }
private:
	static idStrPool		globalKeys;
	static idStrPool		globalValues;
};



// jmarshall: Moved here from idDict
idStrPool		idCommonLocal::globalKeys;
idStrPool		idCommonLocal::globalValues;

idCVar com_developer( "developer", "0", CVAR_BOOL|CVAR_SYSTEM, "developer mode" );

idCommonLocal		commonLocal;
idCommon *			common = &commonLocal;

#define		MAX_CONSOLE_LINES	32
int			com_numConsoleLines;
idCmdArgs	com_consoleLines[MAX_CONSOLE_LINES];

/*
==================
idCommonLocal::StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets should
be after execing the config and default.
==================
*/
void idCommonLocal::StartupVariable( const char *match, bool once ) {
	int			i;
	const char *s;

	i = 0;
	while (	i < com_numConsoleLines ) {
		if ( strcmp( com_consoleLines[ i ].Argv( 0 ), "set" ) ) {
			i++;
			continue;
		}

		s = com_consoleLines[ i ].Argv(1);

		if ( !match || !idStr::Icmp( s, match ) ) {
			cvarSystem->SetCVarString( s, com_consoleLines[ i ].Argv( 2 ) );
			if ( once ) {
				// kill the line
				int j = i + 1;
				while ( j < com_numConsoleLines ) {
					com_consoleLines[ j - 1 ] = com_consoleLines[ j ];
					j++;
				}
				com_numConsoleLines--;
				continue;
			}
		}
		i++;
	}
}


/*
==================
AddStartupCommands

Adds command line parameters as script statements
Commands are separated by + signs

Returns true if any late commands were added, which
will keep the demoloop from immediately starting
==================
*/
bool AddStartupCommands( void ) {
	int		i;
	bool	added;

	added = false;
	// quote every token, so args with semicolons can work
	for ( i = 0; i < com_numConsoleLines; i++ ) {
		if ( !com_consoleLines[i].Argc() ) {
			continue;
		}

		// set commands won't override menu startup
		if ( idStr::Icmpn( com_consoleLines[i].Argv(0), "set", 3 ) ) {
			added = true;
		}
		// directly as tokenized so nothing gets screwed
		cmdSystem->BufferCommandArgs( CMD_EXEC_APPEND, com_consoleLines[i] );
	}

	return added;
}

/*
==================
ParseCommandLine
==================
*/
void ParseCommandLine( int argc, const char **argv ) {
	int i, current_count;

	com_numConsoleLines = 0;
	current_count = 0;
	// API says no program path
	for ( i = 0; i < argc; i++ ) {
		if ( argv[ i ][ 0 ] == '+' ) {
			com_numConsoleLines++;
			com_consoleLines[ com_numConsoleLines-1 ].AppendArg( argv[ i ] + 1 );
		} else {
			if ( !com_numConsoleLines ) {
				com_numConsoleLines++;
			}
			com_consoleLines[ com_numConsoleLines-1 ].AppendArg( argv[ i ] );
		}
	}
}


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
=====================
Sys_DLL_Load
=====================
*/
ID_SYS_HANDLE Sys_DLL_Load( const char *dllName ) {
	ID_SYS_HANDLE	libHandle;
	libHandle = (ID_SYS_HANDLE)LoadLibrary( dllName );
	if ( libHandle ) {
		// since we can't have LoadLibrary load only from the specified path, check it did the right thing
		char loadedPath[ MAX_OSPATH ];
		GetModuleFileName( (HMODULE)libHandle, loadedPath, sizeof( loadedPath ) - 1 );
		//if ( idStr::IcmpPath( dllName, loadedPath ) ) {
			//Sys_Printf( "ERROR: LoadLibrary '%s' wants to load '%s'\n", dllName, loadedPath );
			//Sys_DLL_Unload( (int)libHandle );
			//return 0;
		//}
	}
	return (ID_SYS_HANDLE)libHandle;
}



/*
=====================
Sys_DLL_GetProcAddress
=====================
*/
void *Sys_DLL_GetProcAddress( ID_SYS_HANDLE dllHandle, const char *procName ) {
	return GetProcAddress( (HMODULE)dllHandle, procName ); 
}

/*
=====================
Sys_DLL_Unload
=====================
*/
void Sys_DLL_Unload( ID_SYS_HANDLE dllHandle ) {
	if ( !dllHandle ) {
		return;
	}
	if ( FreeLibrary( (HINSTANCE)dllHandle ) == 0 ) {
		int lastError = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
		    NULL,
			lastError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
	//	Sys_Error( "Sys_DLL_Unload: FreeLibrary failed - %s (%d)", lpMsgBuf, lastError );
	}
}


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

ID_SYS_HANDLE			idSysLocal::DLL_Load( const char *dllName ) { return Sys_DLL_Load( dllName ); }
void *			idSysLocal::DLL_GetProcAddress( ID_SYS_HANDLE dllHandle, const char *procName ) { return Sys_DLL_GetProcAddress(dllHandle, procName ); }
void			idSysLocal::DLL_Unload( ID_SYS_HANDLE dllHandle ) { Sys_DLL_Unload( dllHandle ); }
void idSysLocal::DLL_GetFileName( const char *baseName, char *dllName, int maxLength ) {
#ifdef _WIN32
	idStr::snPrintf( dllName, maxLength, "%s" CPUSTRING ".dll", baseName );
#elif defined( __linux__ )
	idStr::snPrintf( dllName, maxLength, "%s" CPUSTRING ".so", baseName );
#elif defined( MACOS_X )
	idStr::snPrintf( dllName, maxLength, "%s" ".dylib", baseName );
#else
#error OS define is required
#endif
}

sysEvent_t		idSysLocal::GenerateMouseButtonEvent( int button, bool down ) { sysEvent_t ev; memset( &ev, 0, sizeof( ev ) ); return ev; }
sysEvent_t		idSysLocal::GenerateMouseMoveEvent( int deltax, int deltay ) { sysEvent_t ev; memset( &ev, 0, sizeof( ev ) ); return ev; }

void			idSysLocal::OpenURL( const char *url, bool quit ) { }
void			idSysLocal::StartProcess( const char *exeName,  const char *cmdLine, bool quit ) { }

void			idSysLocal::FPU_EnableExceptions( int exceptions ) { }

idSysLocal		sysLocal;
idSys *			sys = &sysLocal;


void DebuggerClientInit( const char *cmdline );

void* PASCAL CObject::operator new(size_t s)
{
if(allocator == NULL)
		return malloc( s );

	return allocator->Allocate( s );
}

void PASCAL CObject::operator delete(void* p)
{
if(allocator == NULL) {
		free( p );
		return;
	}

	allocator->Free( p );
}

void PASCAL CObject::operator delete(void* p, void*)
{
if(allocator == NULL) {
		free( p );
		return;
	}

	allocator->Free( p );
}

void* AFX_CDECL operator new(size_t s, LPCSTR lpszFileName, int nLine) {
	if(allocator == NULL)
		return malloc( s );

	return allocator->Allocate( s );
}

void AFX_CDECL operator delete(void* p, LPCSTR lpszFileName, int nLine) {
	if(allocator == NULL) {
		free( p );
		return;
	}

	allocator->Free( p );
}

__int64 gameDLL;
void LoadGameDLL( void ) {
	char			dllPath[ MAX_OSPATH ];

	gameImport_t	gameImport;
	gameExport_t	gameExport;
	GetGameAPI_t	GetGameAPI;

	fileSystem->FindDLL( "game", dllPath, true );

	if ( !dllPath[ 0 ] ) {
		common->FatalError( "couldn't find game dynamic library" );
		return;
	}
	common->Printf( "Loading game DLL: '%s'\n", dllPath );
	gameDLL = sys->DLL_Load( dllPath );
	if ( !gameDLL ) {
		common->FatalError( "couldn't load game dynamic library" );
		return;
	}

	GetGameAPI = (GetGameAPI_t) Sys_DLL_GetProcAddress( gameDLL, "GetGameAPI" );
	if ( !GetGameAPI ) {
		Sys_DLL_Unload( gameDLL );
		gameDLL = NULL;
		common->FatalError( "couldn't find game DLL API" );
		return;
	}

	gameImport.version					= GAME_API_VERSION;
	gameImport.sys						= ::sys;
	gameImport.common					= ::common;
	gameImport.cmdSystem				= ::cmdSystem;
	gameImport.cvarSystem				= ::cvarSystem;
	gameImport.fileSystem				= ::fileSystem;
	gameImport.networkSystem			= NULL;
	gameImport.renderSystem				= NULL;
	gameImport.soundSystem				= NULL;
	gameImport.renderModelManager		= NULL;
	gameImport.uiManager				= NULL;
	gameImport.declManager				= NULL;
	gameImport.AASFileManager			= NULL;
	gameImport.collisionModelManager	= NULL;

	gameExport							= *GetGameAPI( &gameImport );

	if ( gameExport.version != GAME_API_VERSION ) {
		Sys_DLL_Unload( gameDLL );
		gameDLL = NULL;
		common->FatalError( "wrong game DLL API version" );
		return;
	}

	game								= gameExport.game;
	gameEdit							= gameExport.gameEdit;
}

// CScriptDebuggerApp

BEGIN_MESSAGE_MAP(CScriptDebuggerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScriptDebuggerApp construction

CScriptDebuggerApp::CScriptDebuggerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CScriptDebuggerApp object

CScriptDebuggerApp theApp;


// CScriptDebuggerApp initialization

BOOL CScriptDebuggerApp::InitInstance()
{
	idLib::common = common;
	idLib::cvarSystem = cvarSystem;
	idLib::fileSystem = fileSystem;
	idLib::sys = sys;

	Mem_Init();
	idLib::Init();

	common->Printf("Starting up...\n");

	// parse command line options
	int argc = 0;
	const char **argv;
	idCmdArgs args;
	const char *cmdline = strstr(GetCommandLine(), ".exe") + 5;
	if ( cmdline ) {
		// tokenize if the OS doesn't do it for us
		args.TokenizeString( cmdline, true );
		argv = args.GetArgs( &argc );
	}
	::ParseCommandLine( argc, argv );

	// initialize memory manager
	
	cmdSystem->Init();
	cvarSystem->Init();

	idCVar::RegisterStaticVars();

	AddStartupCommands();
	fileSystem->Init();

	LoadGameDLL();

	// initialize networking
	Sys_InitNetworking();

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	DebuggerClientInit( "" );

	// Delete the shell manager created above.
//	if (pShellManager != NULL)
//	{
//		delete pShellManager;
//	}
//
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

