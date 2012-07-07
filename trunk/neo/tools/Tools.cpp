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
bmKinectDevice					*kinectDevice;
int			*com_editors_ptr;
HINSTANCE   dllHinstance;
const char *testModelPath;
// Stub functions
void idKeyInput::ClearStates( void ) { common->KeyClearStates(); }

#include "radiant/QE3.H"
extern "C" __declspec(dllexport) void *TOOLAPI_renderSystem_AllocRenderWorld( ) 
{
	return g_qeglobals.testRenderWorld;
}

extern "C" __declspec(dllexport) void TOOLAPI_Editor_SetTestModelIdle( const char *modelPath ) 
{
	if(!modelPath) {
		//memcpy( &g_qeglobals.testModel.joints[0], &g_qeglobals.testModel.bindPoseJoints[0], sizeof(idJointMat) * g_qeglobals.testModel.numJoints );
		gameEdit->ANIM_CreateIdleAnimForModel( testModelPath, g_qeglobals.testModel );
		return;
	}
	gameEdit->ANIM_CreateIdleAnimForModel( modelPath, g_qeglobals.testModel );
}
//-20 -100 88
#define KINECT_ANGLES(x) x
extern "C" __declspec(dllexport) void TOOLAPI_Editor_SetTestModelJointRotation( int jointHandle, idQuat quat ) 
{
	idAngles angle, temp = quat.ToAngles();

	// roll = yaw

	// Kinect = +X left, +Y up, +Z forward in body coordinate system
    // Avatar = +Z Up, +X forward, +Y left
	gameEdit->ANIM_SetJointRoationForModel( jointHandle, idQuat( quat.x, quat.y, quat.z, quat.w), g_qeglobals.testModel );
	g_qeglobals.testRenderWorld->UpdateEntityDef( g_qeglobals.testModelWorldHandle, &g_qeglobals.testModel );
}

extern "C" __declspec(dllexport) int TOOLAPI_Editor_GetNumJointsForTestModel() 
{
	if(g_qeglobals.testModel.hModel == NULL)
		return 0;

	return g_qeglobals.testModel.hModel->NumJoints();
}

extern "C" __declspec(dllexport) const char * TOOLAPI_Editor_GetJointNameForTestModel(int jointId) 
{
	if(g_qeglobals.testModel.hModel == NULL)
		return NULL;

	return  g_qeglobals.testModel.hModel->GetJointName((jointHandle_t)jointId);
}

extern "C" __declspec(dllexport) void TOOLAPI_RenderWorld_DrawEditorWorld(float x, float y, float z, float pitch, float yaw, float roll) 
{
	renderView_t	refdef;
	memset( &refdef, 0, sizeof( refdef ) );
	refdef.vieworg = idVec3( x, y, z );
	refdef.megaProject = &g_qeglobals.megaproject;



	// the editor uses opposite pitch convention
	refdef.viewaxis = idAngles( pitch, yaw, roll ).ToMat3();
	
	refdef.width = SCREEN_WIDTH;
	refdef.height = SCREEN_HEIGHT;
	refdef.fov_x = 90;
	refdef.fov_y = 2 * atan((float)SCREEN_HEIGHT / SCREEN_WIDTH) * idMath::M_RAD2DEG;
	
	//refdef.forceUpdate = true;

	// only set in animation mode to give a consistent look 
	//refdef.time = eventLoop->Milliseconds();

	g_qeglobals.testRenderWorld->RenderScene( &refdef );
	
	//game->DrawDefferedPass( SCREEN_WIDTH, SCREEN_HEIGHT );
}

extern "C" __declspec(dllexport) idRenderEntity *TOOLAPI_RenderWorld_AddModelAtPosition( idRenderWorld *world, const char *modelPath, float x, float y, float z ) 
{
	g_qeglobals.testModel.hModel = gameEdit->ANIM_GetModelFromName( modelPath );
	g_qeglobals.testModel.forceUpdate = true;
	gameEdit->ANIM_CreateIdleAnimForModel( modelPath, g_qeglobals.testModel );
	testModelPath = modelPath;
	
	//g_qeglobals.testModel.referenceBounds = g_qeglobals.testModel.bounds;
	g_qeglobals.testRenderWorld->UpdateEntityDef( g_qeglobals.testModelWorldHandle, &g_qeglobals.testModel );

	return (idRenderEntity *)&g_qeglobals.testModel;
}

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
void R_InitTriSurfData( void ) ;
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
	idSIMD::InitProcessor( "ScriptDebugger", true );

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
	R_InitTriSurfData();
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
	allocator->Free( data );
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
	allocator->Free( data );
}