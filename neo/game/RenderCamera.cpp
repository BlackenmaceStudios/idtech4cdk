// RenderCamera.cpp
//

#include "precompiled.h"
#include "game_local.h"

/*
==================
bmRenderCamera::Spawn
==================
*/
void bmRenderCamera::Spawn( void ) {

}

/*
===================
bmRenderCamera::Think
===================
*/
void bmRenderCamera::Think( void ) {

}

/*
================
bmRenderCamera::Present

Present is called to allow entities to generate refEntities, lights, etc for the renderer.
================
*/
void bmRenderCamera::Present( void ) {
	// don't present to the renderer if the entity hasn't changed
	if ( !( thinkFlags & TH_UPDATEVISUALS ) ) {
		return;
	}
	BecomeInactive( TH_UPDATEVISUALS );

	// add to refresh list
	if ( modelDefHandle == -1 ) {
		modelDefHandle = gameRenderWorld->AddEntityDef( &renderEntity );
	} else {
		gameRenderWorld->UpdateEntityDef( modelDefHandle, &renderEntity );
	}
}
