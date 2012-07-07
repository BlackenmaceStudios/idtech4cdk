// EngineClassExport.h
//

class idGame;
class idGameEdit;
class idSession;
class idConsole;
class idEventLoop;
class idConsole;
#define ENGINE_EXPORT_VERSION			3

extern class idImageManager *		globalImages;
extern idSession *					session;
extern idGame *					game ;
extern idGameEdit *				gameEdit ;
extern idEventLoop *			eventLoop ;
extern idConsole * console;

//
// bmEngineClassExport
//
class bmEngineClassExport {
public: 
				bmEngineClassExport();

	void		SetEngineClassPointers( const char *moduleName );
private:
	int							version;
	idSys *						sys ;
	idCommon *					common ;
	idCmdSystem *				cmdSystem ;
	idCVarSystem *				cvarSystem ;
	idFileSystem *				fileSystem ;
	idNetworkSystem *			networkSystem ;
	idRenderSystem *			renderSystem ;
	idSoundSystem *				soundSystem ;
	idRenderModelManager *		renderModelManager ;
	idUserInterfaceManager *	uiManager ;
	idDeclManager *				declManager ;
	idAASFileManager *			AASFileManager ;
	idCollisionModelManager *	collisionModelManager ;
	idGame *					game ;
	idGameEdit *				gameEdit ;
	bmRenderDevice				*renderDevice;
	idRenderTriSurfaceManager	*renderTriSurfaceManager;
	idSession *					session;
	idConsole *					console;
	idEventLoop *				eventLoop;
	bmKinectDevice				*kinectDevice;

	int *						com_editors;
	class idImageManager *		globalImages;
};

/*
====================
bmEngineClassExport::bmEngineClassExport
====================
*/
ID_INLINE bmEngineClassExport::bmEngineClassExport() {
	this->version = ENGINE_EXPORT_VERSION;
	this->sys = ::sys;
	this->globalImages = ::globalImages;
	this->kinectDevice = ::kinectDevice;
	this->common = ::common;
	this->cmdSystem = ::cmdSystem;
	this->cvarSystem = ::cvarSystem;
	this->fileSystem = ::fileSystem;
	this->networkSystem = ::networkSystem;
	this->renderSystem = ::renderSystem;
	this->soundSystem = ::soundSystem;
	this->renderModelManager = ::renderModelManager;
	this->uiManager = ::uiManager;
	this->declManager = ::declManager;
	this->AASFileManager = ::AASFileManager;
	this->collisionModelManager = ::collisionModelManager;
	this->game = ::game;
	this->gameEdit = ::gameEdit;
	this->renderDevice = ::renderDevice;
	this->session = ::session;
	this->console = ::console;
	this->eventLoop = ::eventLoop;
	this->console = ::console;

	this->renderTriSurfaceManager = ::renderTriSurfaceManager;
#ifndef TOOLS_DLL
	this->com_editors = &::com_editors;
#endif
}

/*
====================
bmEngineClassExport::SetEngineClassPointers
====================
*/
ID_INLINE void bmEngineClassExport::SetEngineClassPointers( const char *moduleName ) {
	sys = this->sys;
	common = this->common;
	
	if(this->version != ENGINE_EXPORT_VERSION) {
		common->FatalError( "%s import api version invalid api version %d expected %d\n", this->version, ENGINE_EXPORT_VERSION );
	}
	::kinectDevice = this->kinectDevice;
	::common = this->common;
	::session = this->session;
	::renderDevice = this->renderDevice;
	::globalImages = this->globalImages;
	::cmdSystem = this->cmdSystem;
	::cvarSystem = this->cvarSystem;
	::fileSystem = this->fileSystem;
	::networkSystem = this->networkSystem;
	::renderSystem = this->renderSystem;
	::soundSystem = this->soundSystem;
	::renderModelManager = this->renderModelManager;
	::uiManager = this->uiManager;
	::declManager = this->declManager;
	::AASFileManager = this->AASFileManager;
	::collisionModelManager = this->collisionModelManager;
	::game = this->game;
	::sys = this->sys;
	::gameEdit = this->gameEdit;
	::console = this->console;
	::eventLoop = this->eventLoop;
	::console = this->console;
	::renderTriSurfaceManager = this->renderTriSurfaceManager;
#ifdef TOOLS_DLL
	::com_editors_ptr = this->com_editors;
#endif

}