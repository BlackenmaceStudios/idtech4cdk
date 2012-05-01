/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "Game_local.h"

/*
===============================================================================

	idEntityFx

===============================================================================
*/

const idEventDef EV_Fx_KillFx( "_killfx" );
const idEventDef EV_Fx_Action( "_fxAction", "e" );		// implemented by subclasses

CLASS_DECLARATION( idEntity, idEntityFx )
EVENT( EV_Activate,	   	idEntityFx::Event_Trigger )
EVENT( EV_Fx_KillFx,	idEntityFx::Event_ClearFx )
END_CLASS



/*
================
idEntityFx::Start
================
*/
void idEntityFx::Start( int time ) {
	effect.Start( time );
}

/*
================
idEntityFx::Stop
================
*/
void idEntityFx::Stop( void ) {
	effect.Stop();
}


/*
================
idEntityFx::idEntityFx
================
*/
idEntityFx::idEntityFx() {
	
	fl.networkSync = true;
}

/*
================
idEntityFx::~idEntityFx
================
*/
idEntityFx::~idEntityFx() {
	effect.CleanUp();
}

/*
================
idEntityFx::Spawn
================
*/
void idEntityFx::Spawn( void ) {

	effect.Init( spawnArgs, NULL );

	if ( !spawnArgs.GetBool( "triggered" ) ) {

		if ( spawnArgs.GetBool( "test" ) || spawnArgs.GetBool( "start" ) || spawnArgs.GetFloat ( "restart" ) ) {
			PostEventMS( &EV_Activate, 0, this );
		}
	}
}

/*
================
idEntityFx::Think

  Clears any visual fx started when {item,mob,player} was spawned
================
*/
void idEntityFx::Think( void ) {
	if ( g_skipFX.GetBool() ) {
		return;
	}

	if ( thinkFlags & TH_THINK ) {
		effect.Run( gameLocal.time, GetPhysics()->GetOrigin(), GetPhysics()->GetAxis(), GetPhysics()->GetGravity() );
	}

	RunPhysics();
	Present();
}

/*
================
idEntityFx::Event_ClearFx

  Clears any visual fx started when item(mob) was spawned
================
*/
void idEntityFx::Event_ClearFx( void ) {

	if ( g_skipFX.GetBool() ) {
		return;
	}

	Stop();
	effect.CleanUp();
	BecomeInactive( TH_THINK );

	if ( spawnArgs.GetBool("test") ) {
		PostEventMS( &EV_Activate, 0, this );
	} else {
		if ( spawnArgs.GetFloat( "restart" ) || !spawnArgs.GetBool( "triggered")) {
			float rest = spawnArgs.GetFloat( "restart", "0" );
			if ( rest == 0.0f ) {
				PostEventSec( &EV_Remove, 0.1f );
			} else {
				rest *= gameLocal.random.RandomFloat();
				PostEventSec( &EV_Activate, rest, this );
			}
		}
	}
}

/*
================
idEntityFx::Event_Trigger
================
*/
void idEntityFx::Event_Trigger( idEntity *activator ) {
/*
	if ( g_skipFX.GetBool() ) {
		return;
	}

	float		fxActionDelay;
	const char *fx;

	if ( gameLocal.time < nextTriggerTime ) {
		return;
	}

	if ( spawnArgs.GetString( "fx", "", &fx) ) {
		effect.Setup( fx );
		Start( gameLocal.time );
		PostEventMS( &EV_Fx_KillFx, Duration() );
		BecomeActive( TH_THINK );
	}

	fxActionDelay = spawnArgs.GetFloat( "fxActionDelay" );
	if ( fxActionDelay != 0.0f ) {
		nextTriggerTime = gameLocal.time + SEC2MS( fxActionDelay );
	} else {
		// prevent multiple triggers on same frame
		nextTriggerTime = gameLocal.time + 1;
	}
	PostEventSec( &EV_Fx_Action, fxActionDelay, activator );
*/
}


/*
================
idEntityFx::StartFx
================
*/
idEntityFx *idEntityFx::StartFx( const char *fx, const idVec3 *useOrigin, const idMat3 *useAxis, idEntity *ent, bool bind ) {

	if ( g_skipFX.GetBool() || !fx || !*fx ) {
		return NULL;
	}

	idDict args;
	args.SetBool( "start", true );
	args.Set( "fx", fx );
	idEntityFx *nfx = static_cast<idEntityFx *>( gameLocal.SpawnEntityType( idEntityFx::Type, &args ) );
	if ( nfx->effect.Joint() && *nfx->effect.Joint() ) {
		nfx->BindToJoint( ent, nfx->effect.Joint(), true );
		nfx->SetOrigin( vec3_origin );
	} else {
		nfx->SetOrigin( (useOrigin) ? *useOrigin : ent->GetPhysics()->GetOrigin() );
		nfx->SetAxis( (useAxis) ? *useAxis : ent->GetPhysics()->GetAxis() );
	}

	if ( bind ) {
		// never bind to world spawn
		if ( ent != gameLocal.world ) {
			nfx->Bind( ent, true );
		}
	}
	nfx->Show();
	return nfx;
}


/*
=================
idEntityFx::ClientPredictionThink
=================
*/
void idEntityFx::ClientPredictionThink( void ) {
	if ( gameLocal.isNewFrame ) { 
		effect.Run( gameLocal.time, GetPhysics()->GetOrigin(), GetPhysics()->GetAxis(), GetPhysics()->GetGravity() );
	}
	RunPhysics();
	Present();
}

/*
===============================================================================

  idTeleporter
	
===============================================================================
*/

CLASS_DECLARATION( idEntityFx, idTeleporter )
	EVENT( EV_Fx_Action,	idTeleporter::Event_DoAction )
END_CLASS

/*
================
idTeleporter::Event_DoAction
================
*/
void idTeleporter::Event_DoAction( idEntity *activator ) {
	float angle;

	angle = spawnArgs.GetFloat( "angle" );
	idAngles a( 0, spawnArgs.GetFloat( "angle" ), 0 );
	activator->Teleport( GetPhysics()->GetOrigin(), a, NULL );
}
