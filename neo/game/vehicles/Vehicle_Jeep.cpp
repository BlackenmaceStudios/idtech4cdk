// Vehicle_Jeep.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

/*
===============================================================================

  bmVehicle_Jeep

===============================================================================
*/

CLASS_DECLARATION( bmVehicle_Base, bmVehicle_Jeep )
END_CLASS

/*
================
bmVehicle_Jeep::bmVehicle_Jeep
================
*/
bmVehicle_Jeep::bmVehicle_Jeep( void ) {
	int i;
	for ( i = 0; i < 4; i++ ) {
		suspension[i] = NULL;
	}

	isEmergencyLightsActive = false;
}

/*
================
bmVehicle_Jeep::~bmVehicle_Jeep
================
*/
bmVehicle_Jeep::~bmVehicle_Jeep( void ) {
	delete wheelModel;
	wheelModel = NULL;
}

/*
================
bmVehicle_Jeep::Spawn
================
*/
void bmVehicle_Jeep::Spawn( void ) {
	static const char *wheelJointKeys[] = {
		"wheelJointFrontLeft",
		"wheelJointFrontRight",
		"wheelJointRearLeft",
		"wheelJointRearRight"
	};
	static idVec3 wheelPoly[4] = { idVec3( 2, 2, 0 ), idVec3( 2, -2, 0 ), idVec3( -2, -2, 0 ), idVec3( -2, 2, 0 ) };

	int i;
	idVec3 origin;
	idMat3 axis;
	idTraceModel trm;

	trm.SetupPolygon( wheelPoly, 4 );
	trm.Translate( idVec3( 0, 0, -wheelRadius ) );
	wheelModel = new idClipModel( trm );

	InitEmergencyLights();

	for ( i = 0; i < 4; i++ ) {
		const char *wheelJointName = spawnArgs.GetString( wheelJointKeys[i], "" );
		if ( !wheelJointName[0] ) {
			gameLocal.Error( "bmVehicle_Jeep '%s' no '%s' specified", name.c_str(), wheelJointKeys[i] );
		}
		wheelJoints[i] = animator.GetJointHandle( wheelJointName );
		if ( wheelJoints[i] == INVALID_JOINT ) {
			gameLocal.Error( "bmVehicle_Jeep '%s' can't find wheel joint '%s'", name.c_str(), wheelJointName );
		}

		GetAnimator()->GetJointTransform( wheelJoints[i], 0, origin, axis );
		origin = renderEntity.origin + origin * renderEntity.axis;

		suspension[i] = new idAFConstraint_Suspension();
		suspension[i]->Setup( va( "suspension%d", i ), af.GetPhysics()->GetBody( 0 ), origin, af.GetPhysics()->GetAxis( 0 ), wheelModel );
		suspension[i]->SetSuspension(	g_vehicleSuspensionUp.GetFloat(),
										g_vehicleSuspensionDown.GetFloat(),
										g_vehicleSuspensionKCompress.GetFloat(),
										g_vehicleSuspensionDamping.GetFloat(),
										g_vehicleTireFriction.GetFloat() );

		af.GetPhysics()->AddConstraint( suspension[i] );
	}

	memset( wheelAngles, 0, sizeof( wheelAngles ) );
	BecomeActive( TH_THINK );
}

/*
================
bmVehicle_Jeep::InitEmergencyLights
================
*/
void bmVehicle_Jeep::InitEmergencyLights( void ) {
	idDict args;
	idVec3 origin;
	idMat3 axis;

	// Headlight attributes.
	args.Set( "light_radius", "240 240 240");
	args.Set( "start_off", "1" );
	args.Set( "shadows", "1" );

	// Get the siren joints.
	frontBlueSirenJoint = animator.GetJointHandle( "Front_Blue_Siren" );
	frontRedSirenJoint = animator.GetJointHandle( "Front_Red_Siren" );


	// Create the blue code light.
	animator.GetJointTransform( frontBlueSirenJoint, gameLocal.time, origin, axis );
	args.Set( "origin", ( GetPhysics()->GetOrigin() + origin ).ToString() );
	args.Set( "axis", ( axis ).ToString() );
	args.Set( "_color", "0.1 0.28 0.5" );
	frontBlueSirenHandle = static_cast<idLight *>( gameLocal.SpawnEntityType( idLight::Type, &args ) );

	// Create the red code light.
	animator.GetJointTransform( frontRedSirenJoint, gameLocal.time, origin, axis );
	args.Set( "origin", ( GetPhysics()->GetOrigin() + origin ).ToString() );
	args.Set( "axis", ( axis ).ToString() );
	args.Set( "_color", "0.42 0.28 0.1" );
	frontRedSirenHandle = static_cast<idLight *>( gameLocal.SpawnEntityType( idLight::Type, &args ) );

	// Bind both lights and update.
	frontRedSirenHandle->Bind( this, true );
	frontBlueSirenHandle->Bind( this, true );

	frontRedSirenHandle->SetShader( "lights/biground1_strobe2" );
	frontBlueSirenHandle->SetShader( "lights/biground1_strobe" );

	frontBlueSirenHandle->UpdateVisuals();
	frontRedSirenHandle->UpdateVisuals();
	renderEntity.gui[0]->SetStateString( "emergencylighttxt", "CODE 3 EMERGENCY LIGHTS: OFF" );
	isEmergencyLightsActive = false;
	renderEntity.shaderParms[6] = 0.0f;
	renderEntity.shaderParms[5] = 0.0f;

	siren = SpawnSpeakerOnJoint( frontRedSirenJoint,  "Vehicle_siren01" );
}

/*
================
bmVehicle_Jeep::ToggleEmergencyLights
================
*/
void bmVehicle_Jeep::ToggleEmergencyLights( void ) {
	if(!isEmergencyLightsActive) {
		frontBlueSirenHandle->On();
		frontRedSirenHandle->On();

		renderEntity.gui[0]->SetStateString( "emergencylighttxt", "CODE 3 EMERGENCY LIGHTS: ON" );

		renderEntity.shaderParms[6] = 1.0f;
		renderEntity.shaderParms[5] = 1.0f;
		isEmergencyLightsActive = true;
	}
	else {
		frontBlueSirenHandle->Off();
		frontRedSirenHandle->Off();
		renderEntity.shaderParms[6] = 0.0f;
		renderEntity.shaderParms[5] = 0.0f;
		renderEntity.gui[0]->SetStateString( "emergencylighttxt", "CODE 3 EMERGENCY LIGHTS: OFF" );
		isEmergencyLightsActive = false;
		siren->DoSound( false );
	} 
}

/*
================
bmVehicle_Jeep::PerformImpulse
================
*/
void bmVehicle_Jeep::PerformImpulse( int impulse ) {
	switch(impulse) {
		case IMPULSE_42:
			ToggleEmergencyLights();
			break;
	}
}

/*
================
bmVehicle_Jeep::Think
================
*/
void bmVehicle_Jeep::Think( void ) {
	int i;
	float force = 0.0f, velocity = 0.0f, steerAngle = 0.0f;
	idVec3 origin;
	idMat3 axis;
	idRotation wheelRotation, steerRotation;

	bmVehicle_Base::Think();

	

	if ( thinkFlags & TH_THINK ) {

		if ( player ) {
			if (player->usercmd.forwardmove != 0 && isEmergencyLightsActive) {
				if(!siren->IsPlaying()) {
					siren->DoSound( true );
				}
			}

			// capture the input from a player
			velocity = g_vehicleVelocity.GetFloat();
			if ( player->usercmd.forwardmove < 0 ) {
				velocity = -velocity;
			}
			force = idMath::Fabs( player->usercmd.forwardmove * g_vehicleForce.GetFloat() ) * (1.0f / 128.0f);
			steerAngle = GetSteerAngle();
		}
		
		renderEntity.gui[0]->SetStateFloat( "speed", force );

		// update the wheel motor force and steering
		for ( i = 0; i < 2; i++ ) {

			// front wheel drive
			if ( velocity != 0.0f ) {
				suspension[i]->EnableMotor( true );
			} else {
				suspension[i]->EnableMotor( false );
			}
			suspension[i]->SetMotorVelocity( velocity );
			suspension[i]->SetMotorForce( force );

			// update the wheel steering
			suspension[i]->SetSteerAngle( steerAngle );
		}

		// adjust wheel velocity for better steering because there are no differentials between the wheels
		if ( steerAngle < 0.0f ) {
			suspension[0]->SetMotorVelocity( velocity * 0.5f );
		} else if ( steerAngle > 0.0f ) {
			suspension[1]->SetMotorVelocity( velocity * 0.5f );
		}

		// update suspension with latest cvar settings
		for ( i = 0; i < 4; i++ ) {
			suspension[i]->SetSuspension(	g_vehicleSuspensionUp.GetFloat(),
											g_vehicleSuspensionDown.GetFloat(),
											g_vehicleSuspensionKCompress.GetFloat(),
											g_vehicleSuspensionDamping.GetFloat(),
											g_vehicleTireFriction.GetFloat() );
		}

		// run the physics
		RunPhysics();

		// move and rotate the wheels visually
		for ( i = 0; i < 4; i++ ) {
			idAFBody *body = af.GetPhysics()->GetBody( 0 );

			origin = suspension[i]->GetWheelOrigin();
			velocity = body->GetPointVelocity( origin ) * body->GetWorldAxis()[0];
			wheelAngles[i] += velocity * MS2SEC( gameLocal.msec ) / wheelRadius;

			// additional rotation about the wheel axis
			wheelRotation.SetAngle( RAD2DEG( wheelAngles[i] ) );
			wheelRotation.SetVec( 0, -1, 0 );

			if ( i < 2 ) {
				// rotate the wheel for steering
				steerRotation.SetAngle( steerAngle );
				steerRotation.SetVec( 0, 0, 1 );
				// set wheel rotation
				animator.SetJointAxis( wheelJoints[i], JOINTMOD_WORLD, wheelRotation.ToMat3() * steerRotation.ToMat3() );
			} else {
				// set wheel rotation
				animator.SetJointAxis( wheelJoints[i], JOINTMOD_WORLD, wheelRotation.ToMat3() );
			}

			// set wheel position for suspension
			origin = ( origin - renderEntity.origin ) * renderEntity.axis.Transpose();
			GetAnimator()->SetJointPos( wheelJoints[i], JOINTMOD_WORLD_OVERRIDE, origin );
		}

		// Set the steering wheel rotation.
		steerRotation.SetVec( -1, 0, 0 );
		GetAnimator()->SetJointAxis( steeringWheelJoint, JOINTMOD_WORLD, steerRotation.ToMat3() );
		
/*
		// spawn dust particle effects
		if ( force != 0.0f && !( gameLocal.framenum & 7 ) ) {
			int numContacts;
			idAFConstraint_Contact *contacts[2];
			for ( i = 0; i < 4; i++ ) {
				numContacts = af.GetPhysics()->GetBodyContactConstraints( wheels[i]->GetClipModel()->GetId(), contacts, 2 );
				for ( int j = 0; j < numContacts; j++ ) {
					gameLocal.smokeParticles->EmitSmoke( dustSmoke, gameLocal.time, gameLocal.random.RandomFloat(), contacts[j]->GetContact().point, contacts[j]->GetContact().normal.ToMat3() );
				}
			}
		}
*/
	}

	UpdateAnimation();
	if ( thinkFlags & TH_UPDATEVISUALS ) {
		Present();
		LinkCombat();
	}
}