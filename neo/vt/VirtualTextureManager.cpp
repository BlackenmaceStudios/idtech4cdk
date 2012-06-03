// VirtualTexture.cpp
//

#include "precompiled.h"

#include "VirtualTexture.h"
#include "VirtualTexture_Backend.h"

#include "../renderer/tr_local.h"


bmVirtualTextureManager		virtualTextureManagerLocal;
bmVirtualTextureManager		*virtualTextureManager = &virtualTextureManagerLocal;

idCVar vt_debug_atlas( "vt_debug_atlas", "0", CVAR_RENDERER | CVAR_BOOL | CVAR_CHEAT, "Debug the texture atlas" );
idCVar vt_num_pages( "vt_num_pages", va("%d",VT_NUMPAGES), CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT , "Number of pages to blit too." );
//
// bmVirtualTextureManager
//
bmVirtualTextureManager::bmVirtualTextureManager() {
	currentVirtualTextureFile = NULL;
}

//
// bmVirtualTextureManager
//
bmVirtualTextureManager::~bmVirtualTextureManager() {
	FreeVirtualTextureFile();
}

/*
=====================
bmVirtualTextureManager::Init
=====================
*/
void bmVirtualTextureManager::Init( void ) {
	common->Printf( "----------- VirtualTextureManager_Init -----------\n");

	
#ifndef BSPCOMPILER
	// Create all of our texture pages.
	for(int i = 0; i < VT_NUMPAGES; i++) {
		common->PrintLoadingMessage( va("INIT VIRTUAL TEXTURE PAGE...%d/%d",i+1, VT_NUMPAGES ));
		pages[i].Init( va( "%d", i) );
	}

	currentPage = 0;

	vtBackEnd.Init();
#endif
}
/*
====================
bmVirtualTextureManager::InitDevice
====================
*/
void bmVirtualTextureManager::InitDevice( void ) {
#ifndef BSPCOMPILER
	common->Printf( "----------- VirtualTextureManager_InitDevice -----------\n");

	vtBackEnd.InitDevice();
	for(int i = 0; i < VT_NUMPAGES; i++) {
		pages[i].InitDevice();
	}
#endif
}

/*
====================
bmVirtualTextureManager::FlipPage
====================
*/
void bmVirtualTextureManager::FlipPage( void ) {
	currentPage++;

	if(currentPage >= vt_num_pages.GetInteger()) {

		currentPage = 0;
	}
}

/*
====================
bmVirtualTextureManager::ResetPages
====================
*/
void bmVirtualTextureManager::ResetPages( void ) {
	for(int i = 0; i < VT_NUMPAGES; i++) {
		pages[i].ResetPage();
	}
}

/*
===========================
bmVirtualTextureManager::FreeVirtualTextureFile
===========================
*/
void bmVirtualTextureManager::FreeVirtualTextureFile( void ) {
	if(currentVirtualTextureFile == NULL) {
		return;
	}

	delete currentVirtualTextureFile;
	currentVirtualTextureFile = NULL;
}

/*
===========================
bmVirtualTextureManager::LoadVirtualTextureFile
===========================
*/
bmVirtualTextureFile * bmVirtualTextureManager::LoadVirtualTextureFile( const char *path ) {
	// Don't load over vt if it isn't present on the filesystem.
/*
	for(int i = 0; i < VIRTUALTEXTURE_NUMLEVELS; i++)
	{
		if(fileSystem->ReadFile( va( "vt/%s_level%d%s", path, i,VIRTUALTEXTURE_EXTEN ), NULL) == -1) {
			if(currentVirtualTextureFile != NULL)
			{
				common->Warning("Failed to open virtual texture %s, keeping old data\n", path );
			}
			return NULL;
		}
	}
*/
	// If there is a current virtual texture file thats currently open, free it.
	FreeVirtualTextureFile();

	// Create the virtual texture file from the map file.
	currentVirtualTextureFile = new bmVirtualTextureFile();
	if(!currentVirtualTextureFile->InitFromFile( path )) {
		FreeVirtualTextureFile();
		return NULL;
	}

	return currentVirtualTextureFile;
}

/*
===========================
bmVirtualTextureManager::CreateVirtualTextureFileForMap
===========================
*/
bmVirtualTextureFile* bmVirtualTextureManager::CreateNewVirtualTextureFile( const char *path, int numAreas ) {
#ifdef BSPCOMPILER
	// If there is a current virtual texture file thats currently open, free it.
	FreeVirtualTextureFile();

	// Create the virtual texture file from the map file.
	currentVirtualTextureFile = new bmVirtualTextureFile();
	if(!currentVirtualTextureFile->InitNewVirtualTextureFile( path, numAreas )) {
		FreeVirtualTextureFile();
		return NULL;
	}

	return currentVirtualTextureFile;
#else
	return NULL;
#endif
}