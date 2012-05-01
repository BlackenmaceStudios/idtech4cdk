// tools_local.h
//

#include "../renderer/qgl_linked.h"

#include "tools_platform.h"
#import "../../base/tools/ToolsManaged.tlb" raw_interfaces_only
#include "radiant/EditorWindow.h"

#ifdef TOOLS_DLL
	#define com_editors (*(com_editors_ptr))
	#define R_StaticAlloc Mem_Alloc
	#define R_StaticFree R_StaticFree
#endif

#define TOOLS_EXPORTFUNC(obj, func,  parms, caller) extern "C" __declspec(dllexport) void TOOLAPI_##obj##_##func parms  { obj##->##func caller ; }
#define TOOLS_EXPORTFUNC_RET(obj, func,  parms, caller) extern "C" __declspec(dllexport) void *TOOLAPI_##obj##_##func parms  { return (void *)obj##->##func caller ; }

#define TOOLS_EXPORTFUNC_NOOBJ(className, func,  parms, caller) extern "C" __declspec(dllexport) void TOOLAPI_##className##_##func parms  { obj##->##func caller ; }
#define TOOLS_EXPORTFUNC_NOOBJ_RET(className, func,  parms, caller) extern "C" __declspec(dllexport) void *TOOLAPI_##className##_##func parms  { return (void *)obj##->##func caller ; }
#define TOOLS_EXPORTFUNC_NOOBJ_RETTYPE(className, type, func,  parms, caller) extern "C" __declspec(dllexport) type TOOLAPI_##className##_##func parms  { return obj##->##func caller ; }


//
// idToolInterfaceLocal
//
class idToolInterfaceLocal : public idToolInterface
{
public:
	virtual void	Frame( void );
	virtual void	RadiantPrint( const char *text );
	virtual void	InitTool( const toolFlag_t tool, const idDict *dict );
	virtual bool	FrameSysMsg( void *msg );
	virtual byte    *CompressImage( byte *uncompressedBuffer, int width, int height );
	void			InitToolsManaged( void );
	void			ShowDebugConsole( void );
	virtual void	ComputeUVAtlasForModel( class bmVTModel *model, int startTris, int numTris );
private:
	void			BuildUVAtlasInternalModel( class bmVTModel *model, int startTris, int numTri );
	void			SetUVsFromInternalUVAtlas( class bmVTModel *model ) ;
};

void	RadiantInit( void );
void	RadiantRun( void );
void	RadiantShutdown( void );

// in-game Light Editor
void	LightEditorInit( const idDict *spawnArgs );
void	LightEditorShutdown( void );
void	LightEditorRun( void );


// in-game Sound Editor
void	SoundEditorInit( const idDict *spawnArgs );
void	SoundEditorShutdown( void );
void	SoundEditorRun( void );


// in-game Articulated Figure Editor
void	AFEditorInit( const idDict *spawnArgs );
void	AFEditorShutdown( void );
void	AFEditorRun( void );


// in-game Particle Editor
void	ParticleEditorInit( const idDict *spawnArgs );
void	ParticleEditorShutdown( void );
void	ParticleEditorRun( void );


// in-game Script Editor
void	ScriptEditorInit( const idDict *spawnArgs );
void	ScriptEditorShutdown( void );
void	ScriptEditorRun( void );


// in-game Declaration Browser
void	DeclBrowserInit( const idDict *spawnArgs );
void	DeclBrowserShutdown( void );
void	DeclBrowserRun( void );
void	DeclBrowserReloadDeclarations( void );


// GUI Editor
void	GUIEditorInit( void );
void	GUIEditorShutdown( void );
void	GUIEditorRun( void );
bool	GUIEditorHandleMessage( void *msg );


// Script Debugger
void	DebuggerClientLaunch( void );
void	DebuggerClientInit( const char *cmdline );
bool	DebuggerServerInit( void );
void	DebuggerServerShutdown( void );
void	DebuggerServerPrint( const char *text );
void	DebuggerServerCheckBreakpoint( idInterpreter *interpreter, idProgram *program, int instructionPointer );

//Material Editor
void	MaterialEditorInit( void );
void	MaterialEditorRun( void );
void	MaterialEditorShutdown( void );
void	MaterialEditorPrintConsole( const char *msg );

// jmarshall - Model Studio
void	ModelStudioEditorInit( const idDict *spawnArgs );
void	ModelStudioEditorRun( void );
void	ModelStudioEditorShutdown( void );
// jmarshall end

extern ToolsManaged::IToolsManaged *toolsManaged;
extern idToolInterfaceLocal		toolInterfaceLocal;