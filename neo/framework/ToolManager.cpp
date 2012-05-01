// ToolManager.cpp
//

#include "precompiled.h"
#include "../sys/win32/win_local.h"
idToolInterface *toolInterface;

bmToolManager	toolManager;
ToolsAPI_Init_t dllEntry;


/*
==================
bmToolManager::LoadTools
==================
*/
void bmToolManager::LoadTools( void ) {
	char				dllPath[ MAX_OSPATH ];
	bmEngineClassExport engineExport;


	common->PrintLoadingMessage( "DEVELOPMENT EDITORS" );
	common->Printf( "ToolManager::Init Tools...\n");
	fileSystem->FindDLL( "tools/Tools", dllPath, false );
	if ( !dllPath[ 0 ] ) {
		return;
	}
	toolDLLHandle = sys->DLL_Load( dllPath );
	if ( !toolDLLHandle ) {
		common->Warning("Failed to load Tools.dll\n");
		return;
	}

	// look up the dll interface functions
	dllEntry = ( ToolsAPI_Init_t )sys->DLL_GetProcAddress( toolDLLHandle, "ToolsAPI_Init" ); 
	if(dllEntry == NULL) {
		common->Warning("Failed to find export function for tools dll\n");
		return;
	}

	common->Printf("Loading Tools...\n");
	toolInterface = dllEntry( &engineExport, &win32 );

	common->PrintLoadingMessage( "FINISHING INIT" );
}

/*
==================
bmToolManager::LoadTools
==================
*/
void bmToolManager::InitTool( const toolFlag_t tool, const idDict *dict ) {
	if(toolInterface == NULL)
		return;

	toolInterface->InitTool( tool, dict );
}

/*
==================
bmToolManager::LoadTools
==================
*/
void bmToolManager::Frame( void ) {
	if(toolInterface == NULL)
		return;

	toolInterface->Frame();
}