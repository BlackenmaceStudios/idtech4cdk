// Game_MainMenu.cpp
//

#include "precompiled.h"

#include "Game_local.h"

idCVar g_mainMenuWorldFOV(						"g_mainMenuWorldFOV",					"80",			CVAR_GAME | CVAR_INTEGER, "main menu world FOV" );

/*
=====================
idGameLocal::LoadMainMenuWorld
=====================
*/
void idGameLocal::LoadMainMenuWorld( const char *map ) {
	common->Printf( "---------- LoadMainMenuWorld -----------\n" );

	// Alloc the new render world.
	mainMenuRenderWorld = renderSystem->AllocRenderWorld();

	common->PrintLoadingMessage( "LOADING MAINMENU WORLD" );

	// Load in the main menu world.
	if(!mainMenuRenderWorld->InitFromMap( va("%s", map) )) {
		common->Warning( "Failed to load main menu world %s\n", map );
		FreeMainMenuWorld();
		return;
	}

	common->Printf( "World Loaded...\n" );

	common->Printf( "Spawning MainMenu World Entities...\n");
	mainMenuWorldMapFile = new idMapFile();

	// Load the main menu world map file.
	if ( !mainMenuWorldMapFile->Parse( idStr( map ) + ".map" ) ) {
		common->Warning( "Failed to load main menu world %s\n", map );
		FreeMainMenuWorld();
		return;
	}

	// Spawn all the needed mainmenu entities in the scene.
	for(int i = 0; i < mainMenuWorldMapFile->GetNumEntities(); i++ ) {
		idMapEntity *ent = mainMenuWorldMapFile->GetEntity( i );

		// Info player start is handled seperatly for now.
		if(!strcmp(ent->epairs.GetString( "classname" ), "info_player_start")) {
			renderEntity_t entity;

			gameEdit->ParseSpawnArgsToRenderEntity( &ent->epairs, &entity );

			SetMainMenuWorldViewFromEntity( &entity );
		}
		else {
			idDict dict;

			// Get the decl for this entity.
			idDeclEntityDef *decl = (idDeclEntityDef *)declManager->FindType( DECL_ENTITYDEF, ent->epairs.GetString( "classname" ) );
			if(decl == NULL) {
				common->Warning("Failed to find decl for entity %s", ent->epairs.GetString( "classname" ));
				continue;
			}

			dict.Copy( ent->epairs );
			dict.Copy( decl->dict );

			SpawnClientEntity( dict, mainMenuRenderWorld );
		}
	}

}

/*
=====================
idGameLocal::SetMainMenuWorldViewFromEntity
=====================
*/
void idGameLocal::SetMainMenuWorldViewFromEntity( const renderEntity_t *entity ) {
	memset( &mainMenuRenderWorldView, 0, sizeof( mainMenuRenderWorldView ) );

	// copy global shader parms
	for( int i = 0; i < MAX_GLOBAL_SHADER_PARMS; i++ ) {
		mainMenuRenderWorldView.shaderParms[ i ] = gameLocal.globalShaderParms[ i ];
	}
	mainMenuRenderWorldView.globalMaterial = gameLocal.GetGlobalMaterial();
	mainMenuRenderWorldView.time = gameLocal.time;

	// calculate size of 3D view
	mainMenuRenderWorldView.x = 0;
	mainMenuRenderWorldView.y = 0;
	mainMenuRenderWorldView.width = SCREEN_WIDTH;
	mainMenuRenderWorldView.height = SCREEN_HEIGHT;
	mainMenuRenderWorldView.viewID = 0;


	mainMenuRenderWorldView.vieworg = entity->origin;
	mainMenuRenderWorldView.viewaxis = entity->axis;

	
}

/*
=====================
idGameLocal::FreeMainMenuWorld
=====================
*/
void idGameLocal::FreeMainMenuWorld( void ) {
	if(!mainMenuRenderWorld) {
		common->Warning( "FreeMainMenuWorld called when world isn't valid.\n");
		return;
	}

	common->Printf( "---------- FreeMainMenuWorld -----------\n" );

	if(mainMenuWorldMapFile)
	{
		delete mainMenuWorldMapFile;
		mainMenuWorldMapFile = NULL;
	}

	renderSystem->FreeRenderWorld( mainMenuRenderWorld );
	mainMenuRenderWorld = NULL;
}

/*
=====================
idGameLocal::MainMenuFrame
=====================
*/
void idGameLocal::MainMenuFrame( idUserInterface *mainMenuGui, int frametime ) {
	if(!mainMenuRenderWorld) {
		return;
	}

	mainMenuTime = frametime;

	mainMenuGui->SetStateString( "gameversion", GAME_WINDOW_TITLE );

	CalcFov( g_mainMenuWorldFOV.GetInteger(), mainMenuRenderWorldView.fov_x, mainMenuRenderWorldView.fov_y );
	mainMenuRenderWorldView.time = frametime;
	mainMenuRenderWorld->RenderScene( &mainMenuRenderWorldView );

	gameLocal.DrawDefferedPass();
}