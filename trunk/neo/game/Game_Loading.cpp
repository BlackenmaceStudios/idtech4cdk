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
	loadingGui->SetStateString( "mappath", va("LOADING: %s", mapPath ));

	UpdateLoadingString( "This is a development build of the idTech4 community developers kit, some features may or not may not work properly.");
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
idGameLocal::UpdateLoadingString
=====================
*/
void idGameLocal::UpdateLoadingString( const char *str ) {
	renderSystem->BeginFrame( renderSystem->GetScreenWidth(), renderSystem->GetScreenHeight() );
	loadingGui->SetStateString( "loadStr", str);
	LoadingFrame( 0, 0 );
	renderSystem->EndFrame(NULL, NULL);
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
	
	loadingGui->StateChanged( frameTime );


	loadingGui->Redraw( frameTime );
}