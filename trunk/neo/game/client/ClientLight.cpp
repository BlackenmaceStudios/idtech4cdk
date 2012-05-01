// ClientLight.cpp
//

#include "precompiled.h"

/*
==================
bmClientEntityLight::Spawn
==================
*/
void bmClientEntityLight::Spawn( const idDict &spawnArgs, idRenderEntity *renderEntity, idRenderWorld *world ) {
	gameEdit->ParseSpawnArgsToRenderLight( &spawnArgs, &light );

	world->AddLightDef( &light );

	bmClientEntity::Spawn( spawnArgs, renderEntity, world );
}

/*
==================
bmClientEntityLight::Update
==================
*/
void bmClientEntityLight::Update( void ) {

}
/*
==================
bmClientEntityLight::Destroy
==================
*/
void bmClientEntityLight::Destroy( void ) {
	GetRenderWorld()->FreeLightDef( lightIndex );
}