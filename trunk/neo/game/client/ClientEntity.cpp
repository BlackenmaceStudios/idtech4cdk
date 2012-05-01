// ClientEntity.cpp
//

#include "precompiled.h"

#include "../Game_local.h"

/*
==================
bmClientEntity::Spawn
==================
*/
void bmClientEntity::Spawn( const idDict &spawnArgs, idRenderEntity *renderEntity, idRenderWorld *world ) {
	_renderEntity = renderEntity;
	_world = world;

	gravity = spawnArgs.GetVector( "gravityDir", "0 0 -1" );
}

/*
==================
bmClientEntity::Update
==================
*/
void bmClientEntity::Update( void ) {
	
}
/*
==================
bmClientEntity::Destroy
==================
*/
void bmClientEntity::Destroy( void ) {

}