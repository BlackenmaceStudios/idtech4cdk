// Vehicle_Base.h
//

class idLight;
class idSound;

class bmVehicle_Base;
class bmVehicleController;

#include "VehicleController.h"

/*
===============================================================================

bmVehicle_Base

===============================================================================
*/

class bmVehicle_Base : public idAFEntity_Base {
friend class bmVehicleController;
public:
	CLASS_PROTOTYPE( bmVehicle_Base );

							bmVehicle_Base( void );

	void					Spawn( void );
	
	bool					IsPlayerControllingVehicle( idPlayer *other );
	bool					IsVehicleControlled( void ) { return player != NULL; }

	void					UpdateViewAngles( idAngles &viewAngles );

	void					ToggleHeadlights( void );
	virtual void			Think( void );
	virtual void			PerformImpulse( int impulse ) { }

	idAngles				GetDeltaBodyAngles() { return deltaBodyAngles; }
	idAngles				GetBodyAngles();
private:
	void					InitHeadlights( void );
	void					InitSoundsAndSpeakers( void );
	
protected:
	idPlayer *				player;
	jointHandle_t			engineSpeakerJoint;
	jointHandle_t			eyesJoint;
	jointHandle_t			steeringWheelJoint;

	idLight	*				headlightRightHandle;
	jointHandle_t			headlightRightJoint;

	idLight	*				headlightLeftHandle;
	jointHandle_t			headlightLeftJoint;

	idSound *				engineSpeaker;
	idSound *				engineSpeaker2;
	idSound *				accelSpeaker;

	float					wheelRadius;
	float					steerAngle;
	float					steerSpeed;
	const idDeclParticle *	dustSmoke;

	idAngles				lastBodyAngles;
	idAngles				deltaBodyAngles;

	bool					headlightsActive;

	float					GetSteerAngle( void );

	virtual void			Enter_Vehicle( idPlayer *other );
	virtual void			Exit_Vehicle( void );
};
