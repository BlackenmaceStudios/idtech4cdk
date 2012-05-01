// Effect.h
//

/*
===============================================================================

  Special effects.

===============================================================================
*/

typedef struct {
	renderLight_t			renderLight;			// light presented to the renderer
	qhandle_t				lightDefHandle;			// handle to renderer light def
	renderEntity_t			renderEntity;			// used to present a model to the renderer
	int						modelDefHandle;			// handle to static renderer model
	float					delay;
	int						particleSystem;
	int						start;
	bool					soundStarted;
	bool					shakeStarted;
	bool					decalDropped;
	bool					launched;
} idFXLocalAction;

//
// bmEffect
//
class bmEffect {
public:
							bmEffect();
							~bmEffect();
	void					Init( idDict &args, idRenderEntity *entity );
	void					Start( int time );
	void					Stop( void );
	void					Run( int frameTime, idVec3 origin, idMat3 axis, idVec3 gravity, idRenderWorld *world = gameRenderWorld );
	void					CleanUp( void );
	const char *			Joint( void ) ;
private:
	
	void					Setup( const char *fx );
	void					ApplyFade( const idFXSingleAction& fxaction, idFXLocalAction& laction, const int time, const int actualStart, idRenderWorld *world );
	void					CleanUpSingleAction( const idFXSingleAction& fxaction, idFXLocalAction& laction );

	int						started;
	int						nextTriggerTime;
	const idDeclFX *		fxEffect;				// GetFX() should be called before using fxEffect as a pointer
	idList<idFXLocalAction>	actions;
	idStr					systemName;
};