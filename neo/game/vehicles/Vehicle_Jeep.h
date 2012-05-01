// Vehicle_Jeep.h
//

/*
===============================================================================

bmVehicle_Jeep

===============================================================================
*/

class bmVehicle_Jeep : public bmVehicle_Base {
public:
	CLASS_PROTOTYPE( bmVehicle_Jeep );

							bmVehicle_Jeep( void );
							~bmVehicle_Jeep( void );

	void					ToggleEmergencyLights( void );

	void					Spawn( void );
	virtual void			Think( void );
	virtual void			PerformImpulse( int impulse );
protected:
	bool					isEmergencyLightsActive;
	void					InitEmergencyLights( void );

	jointHandle_t			frontBlueSirenJoint;
	jointHandle_t			frontRedSirenJoint;

	idSound *				siren;

	idLight *				frontRedSirenHandle;
	idLight *				frontBlueSirenHandle;

	idClipModel *			wheelModel;
	idAFConstraint_Suspension *	suspension[4];
	jointHandle_t			wheelJoints[4];
	float					wheelAngles[4];
};