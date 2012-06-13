// BSPCompiler.cpp
//

#include "precompiled.h"
#include "../sys/win32/win_local.h"

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