// Effect.cpp
//

#include "precompiled.h"

#include "Game_local.h"

//
// bmEffect::bmEffect
//
bmEffect::bmEffect() {
	fxEffect = NULL;
	started = -1;
	nextTriggerTime = -1;
}

//
// bmEffect::~bmEffect
//
bmEffect::~bmEffect() {
	CleanUp();
	fxEffect = NULL;
}

/*
===================
bmEffect::Init
===================
*/
void bmEffect::Init( idDict &spawnArgs, idRenderEntity *entity ) {
	if ( g_skipFX.GetBool() ) {
		return;
	}

	const char *fx;
	nextTriggerTime = 0;
	fxEffect = NULL;
	if ( spawnArgs.GetString( "fx", "", &fx ) ) {
		systemName = fx;
	}
	if ( !spawnArgs.GetBool( "triggered" ) ) {
		Setup( fx );
	}
}

/*
================
bmEffect::Joint
================
*/
const char *bmEffect::Joint( void ) {
	return fxEffect ? fxEffect->joint.c_str() : NULL;
}


/*
================
bmEffect::Start
================
*/
void bmEffect::Start( int time ) {
	if ( !fxEffect ) {
		return;
	}
	started = time;
	for( int i = 0; i < fxEffect->events.Num(); i++ ) {
		idFXLocalAction& laction = actions[i];
		laction.start = time;
		laction.soundStarted = false;
		laction.shakeStarted = false;
		laction.particleSystem = -1;
		laction.decalDropped = false;
		laction.launched = false;
	}
}

/*
================
bmEffect::Stop
================
*/
void bmEffect::Stop( void ) {
	CleanUp();
	started = -1;
}

/*
================
bmEffect::CleanUp
================
*/
void bmEffect::CleanUp( void ) {
	if ( !fxEffect ) {
		return;
	}
	for( int i = 0; i < fxEffect->events.Num(); i++ ) {
		const idFXSingleAction& fxaction = fxEffect->events[i];
		idFXLocalAction& laction = actions[i];
		CleanUpSingleAction( fxaction, laction );		
	}
}

/*
================
bmEffect::CleanUpSingleAction
================
*/
void bmEffect::CleanUpSingleAction( const idFXSingleAction& fxaction, idFXLocalAction& laction ) {
	if ( laction.lightDefHandle != -1 && fxaction.sibling == -1 && fxaction.type != FX_ATTACHLIGHT ) {
		gameRenderWorld->FreeLightDef( laction.lightDefHandle );
		laction.lightDefHandle = -1;
	}
	if ( laction.modelDefHandle != -1 && fxaction.sibling == -1 && fxaction.type != FX_ATTACHENTITY ) {
		gameRenderWorld->FreeEntityDef( laction.modelDefHandle );
		laction.modelDefHandle = -1;
	}
	laction.start = -1;
}

/*
================
bmEffect::ApplyFade
================
*/
void bmEffect::ApplyFade( const idFXSingleAction& fxaction, idFXLocalAction& laction, const int time, const int actualStart, idRenderWorld *world ) {
	if ( fxaction.fadeInTime || fxaction.fadeOutTime ) {
		float fadePct = (float)( time - actualStart ) / ( 1000.0f * ( ( fxaction.fadeInTime != 0 ) ? fxaction.fadeInTime : fxaction.fadeOutTime ) );
		if (fadePct > 1.0) {
			fadePct = 1.0;
		}
		if ( laction.modelDefHandle != -1 ) {
			laction.renderEntity.shaderParms[SHADERPARM_RED] = (fxaction.fadeInTime) ? fadePct : 1.0f - fadePct;
			laction.renderEntity.shaderParms[SHADERPARM_GREEN] = (fxaction.fadeInTime) ? fadePct : 1.0f - fadePct;
			laction.renderEntity.shaderParms[SHADERPARM_BLUE] = (fxaction.fadeInTime) ? fadePct : 1.0f - fadePct;
	
			world->UpdateEntityDef( laction.modelDefHandle, &laction.renderEntity );
		}
		if ( laction.lightDefHandle != -1 ) {
			laction.renderLight.shaderParms[SHADERPARM_RED] = fxaction.lightColor.x * ( (fxaction.fadeInTime) ? fadePct : 1.0f - fadePct );
			laction.renderLight.shaderParms[SHADERPARM_GREEN] = fxaction.lightColor.y * ( (fxaction.fadeInTime) ? fadePct : 1.0f - fadePct );
			laction.renderLight.shaderParms[SHADERPARM_BLUE] = fxaction.lightColor.z * ( (fxaction.fadeInTime) ? fadePct : 1.0f - fadePct );

			world->UpdateLightDef( laction.lightDefHandle, &laction.renderLight );
		}
	}
}

/*
===================
bmEffect::Setup
===================
*/
void bmEffect::Setup( const char *fx ) {
	if ( started >= 0 ) {
		return;					// already started
	}

	// early during MP Spawn() with no information. wait till we ReadFromSnapshot for more
	if ( gameLocal.isClient && ( !fx || fx[0] == '\0' ) ) {
		return;
	}

	systemName = fx;
	started = 0;

	fxEffect = static_cast<const idDeclFX *>( declManager->FindType( DECL_FX, systemName.c_str() ) );

	if ( fxEffect ) {
		idFXLocalAction localAction;

		memset( &localAction, 0, sizeof( idFXLocalAction ) );

		actions.AssureSize( fxEffect->events.Num(), localAction );

		for( int i = 0; i<fxEffect->events.Num(); i++ ) {
			const idFXSingleAction& fxaction = fxEffect->events[i];

			idFXLocalAction& laction = actions[i];
			if ( fxaction.random1 || fxaction.random2 ) {
				laction.delay = fxaction.random1 + gameLocal.random.RandomFloat() * ( fxaction.random2 - fxaction.random1 );
			} else {
				laction.delay = fxaction.delay;
			}
			laction.start = -1;
			laction.lightDefHandle = -1;
			laction.modelDefHandle = -1;
			laction.particleSystem = -1;
			laction.shakeStarted = false;
			laction.decalDropped = false;
			laction.launched = false;
		}
	}
}

/*
===================
bmEffect::Run
===================
*/
void bmEffect::Run( int frameTime, idVec3 origin, idMat3 axis, idVec3 gravity, idRenderWorld *world ) {
	int ieff, j;
	idEntity *ent = NULL;
	const idDict *projectileDef = NULL;
	idProjectile *projectile = NULL;

	int time = frameTime;

	if ( !fxEffect ) {
		return;
	}

	for( ieff = 0; ieff < fxEffect->events.Num(); ieff++ ) {
		const idFXSingleAction& fxaction = fxEffect->events[ieff];
		idFXLocalAction& laction = actions[ieff];

		//
		// if we're currently done with this one
		//
		if ( laction.start == -1 ) {
			continue;
		}

		//
		// see if it's delayed
		//
		if ( laction.delay ) {
			if ( laction.start + (time - laction.start) < laction.start + (laction.delay * 1000) ) {
				continue;
			}
		}

		//
		// each event can have it's own delay and restart
		//
		int actualStart = laction.delay ? laction.start + (int)( laction.delay * 1000 ) : laction.start;
		float pct = (float)( time - actualStart ) / (1000 * fxaction.duration );
		if ( pct >= 1.0f ) {
			laction.start = -1;
			float totalDelay = 0.0f;
			if ( fxaction.restart ) {
				if ( fxaction.random1 || fxaction.random2 ) {
					totalDelay = fxaction.random1 + gameLocal.random.RandomFloat() * (fxaction.random2 - fxaction.random1);
				} else {
					totalDelay = fxaction.delay;
				}
				laction.delay = totalDelay;
				laction.start = time;
			} 
			continue;
		}

		if ( fxaction.fire.Length() ) {
			for( j = 0; j < fxEffect->events.Num(); j++ ) {
				if ( fxEffect->events[j].name.Icmp( fxaction.fire ) == 0 ) {
					actions[j].delay = 0;
				}
			}
		}

		idFXLocalAction *useAction;
		if ( fxaction.sibling == -1 ) {
			useAction = &laction;
		} else {
			useAction = &actions[fxaction.sibling];
		}
		assert( useAction );

		switch( fxaction.type ) {
			case FX_ATTACHLIGHT:
			case FX_LIGHT: {
				if ( useAction->lightDefHandle == -1 ) {
					if ( fxaction.type == FX_LIGHT ) {
						memset( &useAction->renderLight, 0, sizeof( renderLight_t ) );
						useAction->renderLight.origin = origin + fxaction.offset;
						useAction->renderLight.axis = axis;
						useAction->renderLight.lightRadius[0] = fxaction.lightRadius;
						useAction->renderLight.lightRadius[1] = fxaction.lightRadius;
						useAction->renderLight.lightRadius[2] = fxaction.lightRadius;
						useAction->renderLight.shader = declManager->FindMaterial( fxaction.data, false );
						useAction->renderLight.shaderParms[ SHADERPARM_RED ]	= fxaction.lightColor.x;
						useAction->renderLight.shaderParms[ SHADERPARM_GREEN ]	= fxaction.lightColor.y;
						useAction->renderLight.shaderParms[ SHADERPARM_BLUE ]	= fxaction.lightColor.z;
						useAction->renderLight.shaderParms[ SHADERPARM_TIMESCALE ]	= 1.0f;
						useAction->renderLight.shaderParms[ SHADERPARM_TIMEOFFSET ] = -MS2SEC( time );
//						useAction->renderLight.referenceSound = refSound.referenceSound;
						useAction->renderLight.pointLight = true;
						if ( fxaction.noshadows ) {
							useAction->renderLight.noShadows = true;
						}
						useAction->lightDefHandle = world->AddLightDef( &useAction->renderLight );
					}
					if ( fxaction.noshadows ) {
						for( j = 0; j < fxEffect->events.Num(); j++ ) {
							idFXLocalAction& laction2 = actions[j];
							if ( laction2.modelDefHandle != -1 ) {
								laction2.renderEntity.noShadow = true;
							}
						}
					}
				}
				ApplyFade( fxaction, *useAction, time, actualStart, world );
				break;
			}
			case FX_SOUND: {
				if ( !useAction->soundStarted ) {
					useAction->soundStarted = true;
					const idSoundShader *shader = declManager->FindSound(fxaction.data);
//					StartSoundShader( shader, SND_CHANNEL_ANY, 0, false, NULL );
					for( j = 0; j < fxEffect->events.Num(); j++ ) {
						idFXLocalAction& laction2 = actions[j];
						if ( laction2.lightDefHandle != -1 ) {
//							laction2.renderLight.referenceSound = refSound.referenceSound;
							gameRenderWorld->UpdateLightDef( laction2.lightDefHandle, &laction2.renderLight );
						}
					}
				}
				break;
			}
			case FX_DECAL: {
				if ( !useAction->decalDropped ) {
					useAction->decalDropped = true;
					gameLocal.ProjectDecal( origin, gravity, 8.0f, true, fxaction.size, fxaction.data ); 
				}
				break;
			}
			case FX_SHAKE: {
				/*
				if ( !useAction->shakeStarted ) {
					idDict args;
					args.Clear();
					args.SetFloat( "kick_time", fxaction.shakeTime );
					args.SetFloat( "kick_amplitude", fxaction.shakeAmplitude );
					for ( j = 0; j < gameLocal.numClients; j++ ) {
						idPlayer *player = gameLocal.GetClientByNum( j );
						if ( player && ( player->origin - origin ).LengthSqr() < Square( fxaction.shakeDistance ) ) {
							if ( !gameLocal.isMultiplayer || !fxaction.shakeIgnoreMaster || GetBindMaster() != player ) {
								player->playerView.DamageImpulse( fxaction.offset, &args );
							}
						}
					}
					if ( fxaction.shakeImpulse != 0.0f && fxaction.shakeDistance != 0.0f ) {
						idEntity *ignore_ent = NULL;
						if ( gameLocal.isMultiplayer ) {
							ignore_ent = this;
							if ( fxaction.shakeIgnoreMaster ) {
								ignore_ent = GetBindMaster();
							}
						}
						// lookup the ent we are bound to?
						gameLocal.RadiusPush( origin, fxaction.shakeDistance, fxaction.shakeImpulse, this, ignore_ent, 1.0f, true );
					}
					useAction->shakeStarted = true;
				}
				*/
				break;
			}
			case FX_ATTACHENTITY:
			case FX_PARTICLE:
			case FX_MODEL: {
				if ( useAction->modelDefHandle == -1 ) {
					memset( &useAction->renderEntity, 0, sizeof( renderEntity_t ) );
					useAction->renderEntity.origin = origin + fxaction.offset;
					useAction->renderEntity.axis = (fxaction.explicitAxis) ? fxaction.axis : axis;
					useAction->renderEntity.hModel = renderModelManager->FindModel( fxaction.data );
					useAction->renderEntity.shaderParms[ SHADERPARM_RED ]		= 1.0f;
					useAction->renderEntity.shaderParms[ SHADERPARM_GREEN ]		= 1.0f;
					useAction->renderEntity.shaderParms[ SHADERPARM_BLUE ]		= 1.0f;
					useAction->renderEntity.shaderParms[ SHADERPARM_TIMEOFFSET ] = -MS2SEC( time );
					useAction->renderEntity.shaderParms[3] = 1.0f;
					useAction->renderEntity.shaderParms[5] = 0.0f;
					if ( useAction->renderEntity.hModel ) {
						useAction->renderEntity.bounds = useAction->renderEntity.hModel->Bounds( &useAction->renderEntity );
					}
					useAction->modelDefHandle = world->AddEntityDef( &useAction->renderEntity );
				} else if ( fxaction.trackOrigin ) {
					useAction->renderEntity.origin = origin + fxaction.offset;
					useAction->renderEntity.axis = fxaction.explicitAxis ? fxaction.axis : axis;
				}
				ApplyFade( fxaction, *useAction, time, actualStart, world );
				break;
			}
			case FX_LAUNCH: {
				if ( gameLocal.isClient ) {
					// client never spawns entities outside of ClientReadSnapshot
					useAction->launched = true;
					break;
				}
				if ( !useAction->launched ) {
					useAction->launched = true;
					projectile = NULL;
					// FIXME: may need to cache this if it is slow
					projectileDef = gameLocal.FindEntityDefDict( fxaction.data, false );
					if ( !projectileDef ) {
						gameLocal.Warning( "projectile \'%s\' not found", fxaction.data.c_str() );
					} else {
						gameLocal.SpawnEntityDef( *projectileDef, &ent, false );
						if ( ent && ent->IsType( idProjectile::Type ) ) {
//							projectile = ( idProjectile * )ent;
//							projectile->Create( this, origin, axis[0] );
//							projectile->Launch( origin, axis[0], vec3_origin );
						}
					}
				}
				break;
			}
		}
	}
}