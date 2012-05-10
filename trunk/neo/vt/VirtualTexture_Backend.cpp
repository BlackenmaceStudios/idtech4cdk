// VirtualTexture_Backend.cpp
//

#include "precompiled.h"
#include "VirtualTexture.h"

#include "../renderer/tr_local.h"
#include "VirtualTexture_Backend.h"


idCVar vt_backend_fbosize_width( "vt_backend_fbosize_width", "128", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT, "Size of the FBO to readback tile data" );
idCVar vt_backend_fbosize_height( "vt_backend_fbosize_height", "72", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT, "Size of the FBO to readback tile data" );
idCVar vt_skiprender( "vt_skiprender", "0", CVAR_RENDERER | CVAR_BOOL | CVAR_CHEAT, "Skips rendering the virtual texture." );

bmVirtualTextureBackend vtBackEnd;
float vtProjectionMatrix[16];

static void R_FrameBufferImageVT( idImage *image ) {
	image->GenerateFrameBufferImage( vt_backend_fbosize_width.GetInteger(), vt_backend_fbosize_height.GetInteger() );
	image->CreatePBO();
}

/*
===============
R_EmptyTexturePage
===============
*/
static void R_FeedBackTileTable( idImage *image ) {
	int	c = 16 * 16;
	byte	*data = new byte[c * 4];

	for ( int i = 0 ; i < c ; i+=4 ) {
		((int *)data)[i+0] = 0;
		((int *)data)[i+1] = 0;
		((int *)data)[i+2] = 0;
		((int *)data)[i+3] = 0;
	}

	// FIXME: this won't live past vid mode changes
	//image->GenerateImage( data, 256,256,  TF_NEAREST, false, TR_CLAMP, TD_HIGH_QUALITY );
	image->GenerateRectImage( data, 16,16 );
	image->rawBuffer = data;

	globalImages->BindNull();
	//image->CreatePBO();
}


/*
====================
bmVirtualTextureBackend::Init
====================
*/
void bmVirtualTextureBackend::Init( void ) {
	common->Printf( "----------- VirtualTextureManager_BackEnd_Init -----------\n");

	sceneFbo = globalImages->ImageFromFunction( "_vt_feedback", R_FrameBufferImageVT );
	sceneFboPublic = globalImages->ImageFromFunction( "_vt_scenefbo_public",  R_FeedBackTileTable );

	for(int i = 0; i < 16; i++)
	{
		sceneTiles[i].Resize(9216);
	}
}
/*
====================
bmVirtualTextureBackend::UUploadAreaTiles
====================
*/
void bmVirtualTextureBackend::UploadAreaTiles( int pageId, int mipLevel,  idList<bmVTTileReadback_t> &areaTiles ) {
	bmVirtualTexturePage *page;

	// Get the current valid world page.
	page = virtualTextureManager->GetWorldPage();

	memset(&sceneFboPublic->rawBuffer[0], 0, 16 * 16 * 4);

	bmVirtualTexturePageTile_t *vtTile, *pageTile, *lastVTTile = NULL;
	for(int i = 0; i < areaTiles.Num(); i++) {
		bmVTTileReadback_t *rbtile;

		rbtile = &areaTiles[i];
		vtTile = page->BlitTileToPage( backEnd.viewDef->renderWorld->vt, rbtile->pageNum, rbtile->tileNum, mipLevel );

		if(vtTile == NULL) {
			assert(false);
		}

		if(lastVTTile == vtTile) {
			continue;
		}

		pageTile = page->GetTileInfo( rbtile->tileNum );
	
		R_FillImageBufferRegion( sceneFboPublic->rawBuffer, idVec4(vtTile->px, vtTile->py, rbtile->pageNum, 255), pageTile->px, pageTile->py, 1, 1, 16 );
		lastVTTile = vtTile;
	}


	// Upload the current page.
	virtualTextureManager->GetWorldPage()->Upload(mipLevel);


	// Upload the remap buffer.
	
	sceneFboPublic->UploadScratch( sceneFboPublic->rawBuffer,16,16 ); 

}

/*
====================
bmVirtualTextureBackend::UpdateSceneVT
====================
*/
void RB_VirtualTexture_DrawUnpackBuffer( int areaNum ) ;

void bmVirtualTextureBackend::UpdateSceneVT( void ) {
	int numCharts = 0;

	numCharts = virtualTextureManager->GetCurrentVirtualTextureFile()->NumCharts();

	// Read in the current tiles for the scene.
	ReadCurrentScene();

	// Begin the virtual texture pass.
	RB_BeginVirtualTexturePass();

	virtualTextureManager->FlipToDefaultPage();

	for(int i = 0; i < numCharts; i++) {
		// If a area isn't visibile in the current scene, no need to render it.
		if(sceneTiles[i].Num() <= 0) {
			continue;
		}

		//renderDevice->BeginDeviceSync();
		
		// Upload the tiles for this area.
		sceneAreaDist[i] = 255 - sceneAreaDist[i];
		if(sceneAreaDist[i] > 170)
		{
			UploadAreaTiles( i, 2,sceneTiles[i]);
		}
		else if(sceneAreaDist[i] > 120)
		{
			UploadAreaTiles( i, 1,sceneTiles[i]);
		}
		else
		{
			UploadAreaTiles( i, 0,sceneTiles[i]);
		}
		//renderDevice->ForceDeviceSync();


		// Render the tiles for this area.

		//renderDevice->BeginDeviceSync();
		RB_RenderVirtualTextureArea( i );
		//renderDevice->ForceDeviceSync();
		
		virtualTextureManager->FlipPage();
		virtualTextureManager->GetWorldPage()->ResetPage();
	}

	// Do any clean up for the VT passes.
	RB_EndVirtualTexturePass();

}

/*
====================
bmVirtualTextureBackend::GenerateSceneTileInfo
====================
*/
void bmVirtualTextureBackend::GenerateSceneTileInfo( drawSurf_t **drawSurfs, int numDrawSurfs ) {
	idScreenRect oldViewport, oldScissor;
	idScreenRect viewport,scissor;

	if(vt_skiprender.GetBool())
		return;

	if(backEnd.viewDef == NULL || backEnd.viewDef->renderWorld == NULL || backEnd.viewDef->renderWorld->vt == NULL)
		return;

	

	viewport = oldViewport = backEnd.viewDef->viewport;
	scissor = oldScissor = backEnd.viewDef->scissor;

	scissor.x2 = viewport.x2 = vt_backend_fbosize_width.GetInteger()-1;
	scissor.y2 = viewport.y2 = vt_backend_fbosize_height.GetInteger()-1;

	((viewDef_t *)backEnd.viewDef)->viewport = viewport;
	((viewDef_t *)backEnd.viewDef)->scissor = scissor;

	//RB_SetupVTProjectionMatrix();

	

	// Render the scene to a small FBO to get the visible tiles in the scene.
	//renderDevice->BeginDeviceSync();
	sceneFbo->BindFBO();
	
	RB_Draw_DiffuseOnly( drawSurfs, numDrawSurfs );

	sceneFbo->pbo->ReadPBO( false ); //ReadDriverPixels();
	sceneFbo->UnBindFBO();
	//renderDevice->ForceDeviceSync();

	((viewDef_t *)backEnd.viewDef)->viewport = oldViewport;
	((viewDef_t *)backEnd.viewDef)->scissor = oldScissor;

	
	UpdateSceneVT();
	
//	
	//qglMatrixMode( GL_PROJECTION );
	//qglLoadMatrixf( backEnd.viewDef->projectionMatrix);
	//qglMatrixMode( GL_MODELVIEW );
}
/*
====================
bmVirtualTextureBackend::Bind
====================
*/
void bmVirtualTextureBackend::Bind( void ) { 
	sceneFboPublic->Bind(); 
}