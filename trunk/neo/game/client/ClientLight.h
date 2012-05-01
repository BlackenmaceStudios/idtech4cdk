// ClientLight.h
//

//
// bmClientEntityLight
//
class bmClientEntityLight : public bmClientEntity  {
public:
	virtual void				Spawn( const idDict &spawnArgs, idRenderEntity *renderEntity, idRenderWorld *world );

	virtual void				Update( void );

	virtual void				Destroy( void );
private:
	renderLight_t				light;
	qhandle_t					lightIndex;
};
