// ClientEntity.h
//

/*
=============================================

bmClientEntity

Client Entities are not synced between the server and the client, 
they can be anything from particle emitters to toggleable models.
=============================================
*/

//
// bmClientEntity
//
class bmClientEntity  {
public:
	virtual void				Spawn( const idDict &spawnArgs, idRenderEntity *renderEntity, idRenderWorld *world );

	virtual void				Update( void );

	virtual void				Destroy( void );

protected:
	idRenderWorld				*GetRenderWorld() { return _world; }
	idVec3						GetOrigin() { return _renderEntity->GetParms()->origin; }
	idMat3						GetAxis() { return _renderEntity->GetParms()->axis; }
	idVec3						GetGravity() { return gravity; };

	idRenderEntity *			_renderEntity;
private:
	
	idRenderWorld *				_world;
	idVec3						gravity;
};

#include "ClientLight.h"
#include "ClientEffects.h"