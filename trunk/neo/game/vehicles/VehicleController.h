// VehicleController.h
//

//
// bmVehicleController
//
class bmVehicleController {
public:
				
	void		Init( idPlayer *player );
	void		Clear( void );

	void		UseVehicleCommand( void );
	void		PerformImpulse( int impulse );
	void		UpdateViewAngles( idAngles &viewAngles );

	bool		IsVehicleInFocus( void ) { return focusVehicle != NULL; }
	bool		UpdateVehicleFocus( idEntity *ent, idVec3 start, idVec3 end );
	void		ClearFocusedVehicle( void );

				// Returns true if were driving, if not than we aren't : ).
	bool		IsDriving( void ) { return controlVehicle != NULL; }

	void		Save( idSaveGame *savefile ) const;
	void		Restore( idRestoreGame *savefile );
private:
	idPlayer *			player;

	bmVehicle_Base *		focusVehicle;
	bmVehicle_Base *		controlVehicle;
};