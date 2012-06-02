// Game_ScriptDebugger.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "../tools/debugger/DebuggerScript.h"

#define SCRIPT_DEBUGGER_PATH					"tools/ScriptDebugger.exe"

/*
=======================
idGameLocal::LoadDebugScript
=======================
*/
rvDebuggerScript *idGameLocal::LoadDebugScript( const char *path ) {
	rvDebuggerScript *script = new rvDebuggerScript();

	if(!script->Load( path )) {
		common->FatalError( "LoadDebugScript: Failed to load %s\n", path );
		return NULL;
	}

	return script;
}

/*
=======================
idGameLocal::LaunchScriptDebugger_f
=======================
*/

#ifndef ID_DEMO_BUILD
void idGameLocal::LaunchScriptDebugger_f( const idCmdArgs &args ) {
	const char	*qpath;

	if(gameLocal.gamestate != GAMESTATE_ACTIVE) {
		common->Warning("Can't launch the script debugger until a map is active\n");
		return;
	}

	qpath = fileSystem->RelativePathToOSPath(SCRIPT_DEBUGGER_PATH, "fs_basepath" );
	if(!qpath) {
		common->Warning("Failed to launch script debugger, watcher not found...\n");
		return;
	}

	sys->StartProcess( qpath, va("+set fs_basepath %s", cvarSystem->GetCVarString( "fs_basepath" ) ), false );
}	
#endif