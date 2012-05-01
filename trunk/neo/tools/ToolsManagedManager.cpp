// ToolsManagedManager.cpp
//

#include "precompiled.h"


using namespace ToolsManaged;
extern bool g_editorAlive;

IToolsManaged *toolsManaged;


TOOLS_EXPORTFUNC(cmdSystem, BufferCommandText, (const char *str), (CMD_EXEC_NOW, str) )
TOOLS_EXPORTFUNC_RET(fileSystem, OSPathToRelativePath, (const char *str), ( str) )
TOOLS_EXPORTFUNC_RET(fileSystem, RelativePathToOSPath, (const char *str), ( str) )

// IdDict
TOOLS_EXPORTFUNC_NOOBJ( idDict, Set, (idDict *obj, const char *str, const char *str1), (str, str1) )
TOOLS_EXPORTFUNC_NOOBJ_RET( idDict, GetString, (idDict *obj, const char *key), (key) )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, idVec3, GetVector, (idDict *obj, const char *key), (key) )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, int, GetNumKeyVals, (idDict *obj), () )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, idKeyValueInstance, GetKeyValInstance, (idDict *obj, int index), (index) )

extern "C" __declspec(dllexport) idDict *TOOLAPI_Entity_GetTemplate( const char *type )
{
	idDeclEntityDef *def = (idDeclEntityDef *)declManager->FindType( DECL_ENTITYDEF, type );
	
	assert( def != NULL );

	return &def->dict;
}


void Brush_Update();
extern "C" __declspec(dllexport) void TOOLAPI_Brush_Update( void )
{
	Brush_Update();
}

// Export 

/*
==================
idToolInterfaceLocal::InitToolsManaged
==================
*/
void idToolInterfaceLocal::InitToolsManaged( void ) {
	CoCreateInstance( __uuidof(ToolsManagedPrivate), NULL, CLSCTX_INPROC_SERVER, __uuidof(IToolsManaged), (void **)&toolsManaged);
}

/*
==================
idToolInterfaceLocal::ShowDebugConsole
==================
*/
void idToolInterfaceLocal::ShowDebugConsole( void ) {
	toolsManaged->ShowDebugConsole();
}
/*
==================
idToolInterfaceLocal::RadiantPrint
==================
*/
void idToolInterfaceLocal::RadiantPrint( const char *text ) {
	_bstr_t bstrt(va("%s\r\n", text)); 

	
	if ( g_editorAlive ) {
		toolsManaged->PrintToConsole( bstrt );
	}

	
}
