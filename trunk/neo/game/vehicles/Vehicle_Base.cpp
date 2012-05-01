// Vehicle_Base.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

/*
===============================================================================

  bmVehicle_Base

===============================================================================
*/

CLASS_DECLARATION( idAFEntity_Base, bmVehicle_Base )
END_CLASS

/*
================
bmVehicle_Base::bmVehicle_Base
================
*/
bmVehicle_Base::bmVehicle_Base( void ) {
	player				= NULL;
	eyesJoint			= INVALID_JOINT;
	steeringWheelJoint	= INVALID_JOINT;
	wheelRadius			= 0.0f;
	steerAngle			= 0.0f;
	steerSpeed			= 0.0f;
	dustSmoke			= NULL;
}

/*
================
bmVehicle_Base::Spawn
================
*/
void bmVehicle_Base::Spawn( void ) {
	const char *eyesJointName = spawnArgs.GetString( "eyesJoint", "eyes" );
	const char *steeringWheelJointName = spawnArgs.GetString( "steeringWheelJoint", "steeringWheel" );
	idStr engineSpeakerJointName;

	LoadAF();

	SetCombatModel();

	SetPhysics( af.GetPhysics() );

	fl.takedamage = true;

	if ( !eyesJointName[0] ) {
		gameLocal.Error( "bmVehicle_Base '%s' no eyes joint specified", name.c_str() );
	}
	eyesJoint = animator.GetJointHandle( eyesJointName );
	if(eyesJoint == INVALID_JOINT) {
		gameLocal.Error( "bmVehicle_Base missing eye joint %s", eyesJointName );
	}

	if ( !steeringWheelJointName[0] ) {
		gameLocal.Error( "bmVehicle_Base '%s' no steering wheel joint specified", name.c_str() );
	}
	steeringWheelJoint = animator.GetJointHandle( steeringWheelJointName );

	// Get the headlight joint.
	headlightRightJoint = animator.GetJointHandle( "Headlight_Right" );
	headlightLeftJoint = animator.GetJointHandle( "Headlight_Left" );

	spawnArgs.GetString( "engineSpeakerJoint", "badJoint", engineSpeakerJointName );
	engineSpeakerJoint = animator.GetJointHandle( engineSpeakerJointName.c_str() );

	// It doesn't matter if this vehicle doesn't have headlights(some vehicles might not), well catch it later if they get activated and the joints are invalid.
	if(headlightRightJoint != INVALID_JOINT && headlightLeftJoint != INVALID_JOINT ) {
		InitHeadlights();
	}

	spawnArgs.GetFloat( "wheelRadius", "20", wheelRadius );
	spawnArgs.GetFloat( "steerSpeed", "5", steerSpeed ); 

	player = NULL;
	steerAngle = 0.0f;

	const char *smokeName = spawnArgs.GetString( "smoke_vehicle_dust", "muzzlesmoke" );
	if ( *smokeName != '\0' ) {
		dustSmoke = static_cast<const idDeclParticle *>( declManager->FindType( DECL_PARTICLE, smokeName ) );
	}

	// Init our sounds and create our speakers for this entity.
	InitSoundsAndSpeakers();

	lastBodyAngles = GetBodyAngles();
}

/*
================
bmVehicle_Base::InitHeadlights
================
*/
void bmVehicle_Base::InitHeadlights( void ) {
	idDict args;
	idVec3 origin;
	idMat3 axis;

	// Headlight attributes.
	args.Set( "light_target", "600 0 48" );
	args.Set( "light_right", "0 -152 -16" );
	args.Set( "light_up", "0 -16 136" );
	args.Set( "start_off", "1" );
	args.SetFloat( "angle",  0);
	args.Set( "shadows", "1" );

	// Create the right headlight.
	animator.GetJointTransform( headlightRightJoint, gameLocal.time, origin, axis );
	args.Set( "origin", ( GetPhysics()->GetOrigin() + origin ).ToString() );
	args.Set( "axis", ( axis ).ToString() );
	headlightRightHandle = static_cast<idLight *>( gameLocal.SpawnEntityType( idLight::Type, &args ) );

	// Create the left headlight.
	animator.GetJointTransform( headlightLeftJoint, gameLocal.time, origin, axis );
	args.Set( "origin", ( GetPhysics()->GetOrigin() + origin ).ToString() );
	args.Set( "axis", ( axis ).ToString() );
	headlightLeftHandle = static_cast<idLight *>( gameLocal.SpawnEntityType( idLight::Type, &args ) );

	// Bind both lights and update.
	headlightRightHandle->Bind( this, true );
	headlightLeftHandle->Bind( this, true );

	headlightRightHandle->SetShader( "lights/headlights" );
	headlightLeftHandle->SetShader( "lights/headlights" );

	headlightRightHandle->UpdateVisuals();
	headlightLeftHandle->UpdateVisuals();
	renderEntity.gui[0]->SetStateString( "headlightstxt", "HEADLIGHTS: OFF" );
	headlightsActive = false;
}

/*
================
bmVehicle_Base::ToggleHeadlights
================
*/
void bmVehicle_Base::ToggleHeadlights( void ) {
	if(!headlightsActive) {
		headlightRightHandle->On();
		headlightLeftHandle->On();

		renderEntity.gui[0]->SetStateString( "headlightstxt", "HEADLIGHTS: ON" );

		renderEntity.shaderParms[7] = 1.0f;
		headlightsActive = true;
	}
	else {
		headlightRightHandle->Off();
		headlightLeftHandle->Off();
		renderEntity.shaderParms[7] = 0.0f;
		renderEntity.gui[0]->SetStateString( "headlightstxt", "HEADLIGHTS: OFF" );
		headlightsActive = false;
	} 
}



/*
================
bmVehicle_Base::GetBodyAngles
================
*/
idAngles bmVehicle_Base::GetBodyAngles() {
	return renderEntity.axis.ToAngles();
}

/*
================
bmVehicle_Base::UpdateViewAngles
================
*/
void bmVehicle_Base::UpdateViewAngles( idAngles &viewAngles ) {
    idMat3 axis, lookaxis;
	idVec3 origin, lookorigin;
	idVec3 vectorToTarget;
	idAngles anglesToTarget;
	idAngles turretAngles;
	idAngles deltaAngles; 

	axis = GetPhysics()->GetAxis();
	origin = GetPhysics()->GetOrigin();

	animator.GetJointTransform( headlightLeftJoint, gameLocal.time, lookorigin, lookaxis );

	idVec3 vttNormal = lookorigin - origin;
	vttNormal.Normalize();

	vectorToTarget	= vttNormal;
	anglesToTarget	= vectorToTarget.ToAngles().Normalize360();
	turretAngles	= viewAngles.Normalize360();
	deltaAngles		= (anglesToTarget - turretAngles).Normalize180();

	viewAngles = deltaAngles;
}

/*
================
InitSoundsAndSpeakers
================
*/
void bmVehicle_Base::InitSoundsAndSpeakers( void ) {
	idStr sndVehicleEnterShader, sndEngineIdle;
	
	// Get the sound spawn info stuff. 

	
	spawnArgs.GetString( "sndVehicleEnter", "badVehicleSnd", sndVehicleEnterShader );
	spawnArgs.GetString( "sndEngineIdle", "badVehicleSnd", sndEngineIdle );
	
	engineSpeaker = SpawnSpeakerOnJoint( engineSpeakerJoint,  sndVehicleEnterShader ); 
	engineSpeaker2 = SpawnSpeakerOnJoint( engineSpeakerJoint,  sndEngineIdle );
	accelSpeaker = SpawnSpeakerOnJoint( engineSpeakerJoint, "vehicle_accelerate" );
}

/*
================
bmVehicle_Base::Use
================
*/
void bmVehicle_Base::Enter_Vehicle( idPlayer *other ) {
	idVec3 origin;
	idMat3 axis;

	player = other;
	animator.GetJointTransform( eyesJoint, gameLocal.time, origin, axis );
	origin = renderEntity.origin + origin * renderEntity.axis;
	player->GetPhysics()->SetOrigin( origin );
	player->BindToBody( this, 0, true );

	af.GetPhysics()->SetComeToRest( false );
	af.GetPhysics()->Activate();

	engineSpeaker->DoSound( true );
	engineSpeaker2->DoSound( true );
}

/*
================
bmVehicle_Base::IsPlayerControllingVehicle
================
*/
bool bmVehicle_Base::IsPlayerControllingVehicle( idPlayer *other ) {
	if ( player ) {
		if ( player == other ) {
			return true;
		}
	}
	return false;
}

/*
================
bmVehicle_Base:::Exit_Vehicle
================
*/
void bmVehicle_Base::Exit_Vehicle( void ) {
	assert( IsVehicleControlled() );

	player->Unbind();
	player = NULL;

	af.GetPhysics()->PutToRest();
	engineSpeaker2->DoSound( false );
	accelSpeaker->DoSound( false );
}

/*
================
bmVehicle_Base::Think
================
*/
void bmVehicle_Base::Think( void ) {
	if ( thinkFlags & TH_THINK ) {
		if ( player ) {
			if (player->usercmd.forwardmove != 0) {
				if(!accelSpeaker->IsPlaying()) {
					accelSpeaker->DoSound( true );
				}
			}
			else
			{
				accelSpeaker->DoSound( false );
			}
		}
	}
}

/*
================
bmVehicle_Base::GetSteerAngle
================
*/
float bmVehicle_Base::GetSteerAngle( void ) {
	float idealSteerAngle, angleDelta;

	idealSteerAngle = player->usercmd.rightmove * ( 30.0f / 128.0f );
	angleDelta = idealSteerAngle - steerAngle;

	if ( angleDelta > steerSpeed ) {
		steerAngle += steerSpeed;
	} else if ( angleDelta < -steerSpeed ) {
		steerAngle -= steerSpeed;
	} else {
		steerAngle = idealSteerAngle;
	}

	return steerAngle;
}