// ClientEffects.cpp
//

#include "precompiled.h"

#include "../Game_local.h"

/*
==================
bmClientEntityEffects::Spawn
==================
*/
void bmClientEntityEffects::Spawn( const idDict &spawnArgs, idRenderEntity *renderEntity, idRenderWorld *world ) {
	effect.Init( (idDict &)spawnArgs, NULL );
	effect.Start( gameLocal.time );
	
	bmClientEntity::Spawn( spawnArgs, renderEntity, world );

}

/*
==================
bmClientEntityEffects::Update
==================
*/
void bmClientEntityEffects::Update( void ) {
	effect.Run( gameLocal.mainMenuTime / 100, GetOrigin(), GetAxis(), GetGravity(), GetRenderWorld() );
	GetRenderWorld()->UpdateEntityDef( _renderEntity->GetIndex(), _renderEntity->GetParms() );
}
/*
==================
bmClientEntityEffects::Destroy
==================
*/
void bmClientEntityEffects::Destroy( void ) {

}