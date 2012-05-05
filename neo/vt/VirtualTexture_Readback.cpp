// VirtualTexture_Readback.cpp
//

#include "precompiled.h"

#include "../renderer/tr_local.h"
#include "VirtualTexture.h"
#include "VirtualTexture_Backend.h"

/*
========================
bmVirtualTextureBackend::ReadTilesInScene
========================
*/
void bmVirtualTextureBackend::ReadTilesInScene( byte *readbackBuffer, int width, int height ) {
	bmVTTileReadback_t tile;
	int numCharts;

	numCharts = virtualTextureManager->GetCurrentVirtualTextureFile()->NumCharts();

	// Clear all the scene tiles. 
	for(int i = 0; i < numCharts; i++)
	{
		sceneTiles[i].Clear();
	}

	// Go through all the tiles and parse them. 
	for(int i = 0; i < width * height; i++, readbackBuffer+= 4) {
		tile.tileNum = readbackBuffer[0];
		tile.pageNum = readbackBuffer[2];

		sceneTiles[tile.pageNum].Append( tile );
	}
}

/*
========================
bmVirtualTextureBackend::ReadCurrentScene
========================
*/
void bmVirtualTextureBackend::ReadCurrentScene( void ) {
	// Read in the readback PBO.
	
	
	byte *sceneTileDataPtr = sceneFbo->pbo->ReadPBO( true );
	byte *sceneTileData = sceneTileDataPtr;

	
	//renderDevice->BeginDeviceSync();
	// Get all the tiles that are in the scene.
	ReadTilesInScene( sceneTileDataPtr, vt_backend_fbosize_width.GetInteger(), vt_backend_fbosize_height.GetInteger() );

	// Unbind the PBO.
	sceneFbo->pbo->UnbindPBO();

	//renderDevice->ForceDeviceSync();


#if 0
	// Get the current valid world page.
	bmVirtualTexturePage *page = virtualTextureManager->GetWorldPage();

	bool pageFlipped = false;
	for(int i = (vt_backend_fbosize_width.GetInteger() * vt_backend_fbosize_height.GetInteger()) - 1; i != 0 ; i--, sceneTileData+=4) {
		
		int tileNum = sceneTileData[0];
		int vtSheetNum = sceneTileData[2];

		assert(vtSheetNum != 0);

		int realTimeNum = (tileNum) * (vtSheetNum);

		bmVirtualTexturePageTile_t *tile = page->BlitTileToPage( backEnd.viewDef->renderWorld->vt, realTimeNum );

		if(tile == NULL)
		{
		
			assert(!pageFlipped);

			pageFlipped = true;
			virtualTextureManager->FlipPage();
			page = virtualTextureManager->GetWorldPage();
			
			tile = page->BlitTileToPage( backEnd.viewDef->renderWorld->vt, realTimeNum );
			
			assert(tile != NULL);
		}
		
		

		bmVirtualTexturePageTile_t *tilepos = page->GetTileInfo( tileNum );

		R_FillImageBufferRegion<byte>( sceneFboPublic->rawBuffer, idVec4(tile->px, tile->py, vtSheetNum, 255), tilepos->px, tilepos->py, 1, 1, 255 );
	}
	sceneFbo->pbo->UnbindPBO();
#endif
}