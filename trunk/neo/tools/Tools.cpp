// Tools.cpp
//

#include "precompiled.h"

#include "../sys/win32/win_local.h"

#include "Tools_Managed.h"

idSys *						sys = NULL;
idCommon *					common = NULL;
idCmdSystem *				cmdSystem = NULL;
idCVarSystem *				cvarSystem = NULL;
idFileSystem *				fileSystem = NULL;
idNetworkSystem *			networkSystem = NULL;
idRenderSystem *			renderSystem = NULL;
idSoundSystem *				soundSystem = NULL;
idRenderModelManager *		renderModelManager = NULL;
idUserInterfaceManager *	uiManager = NULL;
idDeclManager *				declManager = NULL;
idCollisionModelManager *	collisionModelManager = NULL;
idGame *					game = NULL;
idGameEdit *				gameEdit = NULL;
idCVar *					idCVar::staticVars = NULL;
class idImageManager		*globalImages = NULL;
idSession *					session = NULL;
bmRenderDevice				*renderDevice = NULL;
idEventLoop *				eventLoop = NULL;
idToolInterfaceLocal		toolInterfaceLocal;
idToolInterface				*toolInterface = &toolInterfaceLocal;
idConsole *		  			console;
Win32Vars_t					win32;
idRenderTriSurfaceManager * renderTriSurfaceManager;
idAASFileManager			* AASFileManager;
idCVar						 r_znear( "r_znear", "3", CVAR_RENDERER | CVAR_FLOAT, "near Z clip plane distance", 0.001f, 200.0f );

int			*com_editors_ptr;
HINSTANCE   dllHinstance;
// Stub functions
void idKeyInput::ClearStates( void ) { common->KeyClearStates(); }

void	RadiantShutdown( void ) {

}

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


/*
==================
Com_Editor_f

  we can start the editor dynamically, but we won't ever get back
==================
*/
static void Com_Editor_f( const idCmdArgs &args ) {
	afxCurrentInstanceHandle = win32.hInstance;
    afxCurrentResourceHandle = win32.hInstance;
	RadiantInit();
}

/*
=============
Com_ScriptDebugger_f
=============
*/
static void Com_ScriptDebugger_f( const idCmdArgs &args ) {
	// Make sure it wasnt on the command line
	if ( !( com_editors & EDITOR_DEBUGGER ) ) {
		common->Printf( "Script debugger is currently disabled\n" );
		// DebuggerClientLaunch();
	}
}

/*
==================
Com_EditLights_f
==================
*/
static void Com_EditLights_f( const idCmdArgs &args ) {
	LightEditorInit( NULL );
	cvarSystem->SetCVarInteger( "g_editEntityMode", 1 );
}


/*
==================
Com_EditSounds_f
==================
*/
static void Com_EditSounds_f( const idCmdArgs &args ) {
	SoundEditorInit( NULL );
	cvarSystem->SetCVarInteger( "g_editEntityMode", 2 );
}

/*
==================
Com_EditAFs_f
==================
*/
static void Com_EditAFs_f( const idCmdArgs &args ) {
	AFEditorInit( NULL );
}

/*
==================
Com_EditParticles_f
==================
*/
static void Com_EditParticles_f( const idCmdArgs &args ) {
	ParticleEditorInit( NULL );
}

/*
==================
Com_EditScripts_f
==================
*/
static void Com_EditScripts_f( const idCmdArgs &args ) {
	ScriptEditorInit( NULL );
}

/*
===============
ToolsAPI_Init
===============
*/

idToolInterface * ToolsAPI_Init( bmEngineClassExport *engineExport, void *winVars ) {
	toolInterfaceLocal.InitToolsManaged();

	engineExport->SetEngineClassPointers( "Tools API" );

	
	
 

	// set interface pointers used by idLib
	idLib::sys			= sys;
	idLib::common		= common;
	idLib::cvarSystem	= cvarSystem;
	idLib::fileSystem	= fileSystem;
// jmarshall
#ifdef _DEBUG
	idLib::InitCrashHandler();
#endif
// jmarshall end

	// initialize idLib
	idLib::Init();

	// register static cvars declared in the game
	idCVar::RegisterStaticVars();

	// initialize processor specific SIMD
	idSIMD::InitProcessor( "tools", true );

	win32 = *(Win32Vars_t *)winVars;

	// editors
	cmdSystem->AddCommand( "editor", Com_Editor_f, CMD_FL_TOOL, "launches the level editor Radiant" );
	cmdSystem->AddCommand( "editLights", Com_EditLights_f, CMD_FL_TOOL, "launches the in-game Light Editor" );
	cmdSystem->AddCommand( "editSounds", Com_EditSounds_f, CMD_FL_TOOL, "launches the in-game Sound Editor" );
	cmdSystem->AddCommand( "editAFs", Com_EditAFs_f, CMD_FL_TOOL, "launches the in-game Articulated Figure Editor" );
	cmdSystem->AddCommand( "editParticles", Com_EditParticles_f, CMD_FL_TOOL, "launches the in-game Particle Editor" );
	cmdSystem->AddCommand( "editScripts", Com_EditScripts_f, CMD_FL_TOOL, "launches the in-game Script Editor" );
	cmdSystem->AddCommand( "debugger", Com_ScriptDebugger_f, CMD_FL_TOOL, "launches the Script Debugger" );

	// compilers
	cmdSystem->AddCommand( "dmap", Dmap_f, CMD_FL_TOOL, "compiles a map", idCmdSystem::ArgCompletion_MapName );
	//cmdSystem->AddCommand( "renderbump", RenderBump_f, CMD_FL_TOOL, "renders a bump map", idCmdSystem::ArgCompletion_ModelName );
	//cmdSystem->AddCommand( "renderbumpFlat", RenderBumpFlat_f, CMD_FL_TOOL, "renders a flat bump map", idCmdSystem::ArgCompletion_ModelName );
	cmdSystem->AddCommand( "runAAS", RunAAS_f, CMD_FL_TOOL, "compiles an AAS file for a map", idCmdSystem::ArgCompletion_MapName );
	cmdSystem->AddCommand( "runAASDir", RunAASDir_f, CMD_FL_TOOL, "compiles AAS files for all maps in a folder", idCmdSystem::ArgCompletion_MapName );
	cmdSystem->AddCommand( "runReach", RunReach_f, CMD_FL_TOOL, "calculates reachability for an AAS file", idCmdSystem::ArgCompletion_MapName );
	cmdSystem->AddCommand( "roq", RoQFileEncode_f, CMD_FL_TOOL, "encodes a roq file" );
	
	afxContextIsDLL = true;
	
	win32.hInstance = dllHinstance;
	afxCurrentInstanceHandle = win32.hInstance;
    afxCurrentResourceHandle = win32.hInstance;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(toolsManaged == NULL) {
		common->FatalError( "Managed tools module failed to load\n");
	}

	afxTraceEnabled = false; 

	return &toolInterfaceLocal;
}
/*
=================
idToolInterfaceLocal::InitTool
=================
*/
bool idToolInterfaceLocal::FrameSysMsg( void *msg ) {
	if ( GUIEditorHandleMessage ( msg ) ) {	
		return true;
	}

	return false;
}


/*
=================
idToolInterfaceLocal::InitTool
=================
*/
void idToolInterfaceLocal::InitTool( const toolFlag_t tool, const idDict *dict ) {
	if ( tool & EDITOR_SOUND ) {
		SoundEditorInit( dict );
	} else if ( tool & EDITOR_LIGHT ) {
		LightEditorInit( dict );
	} else if ( tool & EDITOR_PARTICLE ) {
		ParticleEditorInit( dict );
	} else if ( tool & EDITOR_AF ) {
		AFEditorInit( dict );
	}
}

/*
=================
idToolInterfaceLocal::Frame
=================
*/
void idToolInterfaceLocal::Frame( void ) {
	if ( com_editors ) {
		if ( com_editors & EDITOR_GUI ) {
			// GUI editor
			GUIEditorRun();
		} else if ( com_editors & EDITOR_RADIANT ) {
			// Level Editor
			RadiantRun();
		}
		else if (com_editors & EDITOR_MATERIAL ) {
			//BSM Nerve: Add support for the material editor
			MaterialEditorRun();
		}
		else if (com_editors & EDITOR_MODELSTUDIO ) {
			//jmarshall bm: Add support for the model studio editor
			ModelStudioEditorRun();
		}
		else {
			if ( com_editors & EDITOR_LIGHT ) {
				// in-game Light Editor
				LightEditorRun();
			}
			if ( com_editors & EDITOR_SOUND ) {
				// in-game Sound Editor
				SoundEditorRun();
			}
			if ( com_editors & EDITOR_DECL ) {
				// in-game Declaration Browser
				DeclBrowserRun();
			}
			if ( com_editors & EDITOR_AF ) {
				// in-game Articulated Figure Editor
				AFEditorRun();
			}
			if ( com_editors & EDITOR_PARTICLE ) {
				// in-game Particle Editor
				ParticleEditorRun();
			}
			if ( com_editors & EDITOR_SCRIPT ) {
				// in-game Script Editor
				ScriptEditorRun();
			}
		}
	}
}

/*
==============
Sys_ListFiles
==============
*/
#include <io.h>
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

extern "C"
BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID
/*lpReserved*/)

{
	if (dwReason == DLL_PROCESS_ATTACH)
   {
		dllHinstance = (HINSTANCE)hInstance;
   }

	return true;
}


void GL_CheckErrors( bool ) {

}

/*
=================
R_StaticAlloc
=================
*/
void *R_StaticAlloc( int bytes ) {
	byte *buffer = (byte *)allocator->Allocate( bytes );

	memset( buffer, 0, bytes );

	return buffer;
}

/*
=================
R_ClearedStaticAlloc
=================
*/
void *R_ClearedStaticAlloc( int bytes ) {
	byte *buffer = (byte *)allocator->Allocate( bytes );

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
	byte *buffer = (byte *)allocator->Allocate( bytes );

	memset( buffer, 0, bytes );

	return buffer;
}

/*
==================
R_ClearedFrameAlloc
==================
*/
void *R_ClearedFrameAlloc( int bytes ) {
	byte *buffer = (byte *)allocator->Allocate( bytes );

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