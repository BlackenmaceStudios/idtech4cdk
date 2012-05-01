// ClientEffects.h
//

//
// bmClientEntityEffects
//
class bmClientEntityEffects : public bmClientEntity  {
public:
	virtual void				Spawn( const idDict &spawnArgs, idRenderEntity *renderEntity, idRenderWorld *world );

	virtual void				Update( void );

	virtual void				Destroy( void );

private:
	bmEffect					effect;
};