// Game_Loading.cpp
//

#include "precompiled.h"

#include "Game_local.h"

/*
=====================
idGameLocal::BeginMapChange
=====================
*/
void idGameLocal::BeginMapChange( const char *mapPath ) {
	loadingGui = uiManager->FindGui( "guis/map/loading.gui", true, false, true );

	if(loadingGui == NULL) {
		common->FatalError( "Failed to load loading UI\n");
		return;
	}

	loadingGui->SetStateFloat( "map_loading", 0 );
	loadingGui->SetStateString( "loadStr", "This is a development build of the idTech4 community developers kit, some features may or not may not work properly.");
}

/*
=====================
idGameLocal::BeginMapChange
=====================
*/
void idGameLocal::EndMapChange( void ) {
	loadingGui = NULL;
}

/*
=====================
idGameLocal::LoadingFrame
=====================
*/
void idGameLocal::LoadingFrame( int frameTime, float pct ) {
	if(!loadingGui) {
		return;
	}

	// Update the loading percents.
	loadingGui->SetStateFloat( "map_loading", pct );
	loadingGui->SetStateString( "loadStr", "This is a development build of the idTech4 community developers kit, some features may or not may not work properly.");
	loadingGui->StateChanged( frameTime );


	loadingGui->Redraw( frameTime );
}