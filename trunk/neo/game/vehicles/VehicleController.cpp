// VehicleController.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

/*
==========================
bmVehicleController::Init
==========================
*/
void bmVehicleController::Init( idPlayer *player ) {
	Clear();

	this->player = player;
}

/*
==========================
bmVehicleController::Clear
==========================
*/
void bmVehicleController::Clear( void ) {
	focusVehicle = NULL;
	controlVehicle = NULL;
	player = NULL;
}

/*
==========================
bmVehicleController::UseVehicleCommand
==========================
*/
void bmVehicleController::UseVehicleCommand( void ) {
	if(controlVehicle != NULL) {
		if(controlVehicle->IsVehicleControlled()) {
			if(controlVehicle->IsPlayerControllingVehicle( player )) {
				controlVehicle->Exit_Vehicle();
				player->Show();
				controlVehicle = NULL;
			}
		}

		return;
	}

	if(focusVehicle == NULL) {
		return;
	}

	// Set the controlled vehicle to be the vehicle we currently have in focus, and enter that vehicle.
	controlVehicle = focusVehicle;
	controlVehicle->Enter_Vehicle( player );

	// Hide the player(should do some kind of animation?)
	player->Hide();

	// Clear the focused vehicle.
	ClearFocusedVehicle();
}

/*
==========================
bmVehicleController::ClearFocusedVehicle
==========================
*/
void bmVehicleController::ClearFocusedVehicle( void ) {
	player->hud->SetStateString( "focusTxt", "" );
	focusVehicle = NULL;
}

/*
==========================
bmVehicleController::UpdateViewAngles
==========================
*/
void bmVehicleController::UpdateViewAngles( idAngles &viewAngles ) {
	controlVehicle->UpdateViewAngles( viewAngles );
	
	

	//viewAngles.yaw = controlVehicle->GetBodyAngles().yaw;
	
	

}

/*
================
bmVehicleController::PerformImpulse
================
*/
void bmVehicleController::PerformImpulse( int impulse ) {
	switch(impulse) {
		case IMPULSE_41:
			controlVehicle->ToggleHeadlights();
			break;
	}

	controlVehicle->PerformImpulse( impulse );
}

/*
==========================
bmVehicleController::Save
==========================
*/
bool bmVehicleController::UpdateVehicleFocus( idEntity *ent, idVec3 start, idVec3 end ) {
	trace_t		trace;

	// If we are driving than don't focus on the vehicle.
	if(IsDriving()) {
		return false;
	}

	if(IsVehicleInFocus()) {
		return true;
	}

	gameLocal.clip.TracePoint( trace, start, end, MASK_SHOT_RENDERMODEL, player );
	if ( ( trace.fraction < 1.0f ) && ( trace.c.entityNum == ent->entityNumber ) ) {
		player->ClearFocus();
		player->hud->SetStateString( "focusTxt", "Press 'e' to enter vehicle" );
		focusVehicle = static_cast<bmVehicle_Base *>( ent );
		return true;
	}

	return false;
}

/*
==========================
bmVehicleController::Save
==========================
*/
void bmVehicleController::Save( idSaveGame *savefile ) const {

}

/*
==========================
bmVehicleController::Save
==========================
*/
void bmVehicleController::Restore( idRestoreGame *savefile ) {

}