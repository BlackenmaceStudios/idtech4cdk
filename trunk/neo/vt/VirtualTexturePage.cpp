// VirtualTexturePage.cpp
//

#include "precompiled.h"

#include "VirtualTexture.h"
#include "../renderer/tr_local.h"

idCVar vt_page_size( "vt_page_size", "4096", CVAR_RENDERER | CVAR_INTEGER , "Size of the paging texture" );

/*
===============
R_EmptyTexturePage
===============
*/
static void R_EmptyTexturePage( idImage *image ) {
	int	c = vt_page_size.GetInteger() * vt_page_size.GetInteger();
	byte	*data = new byte[c * 4];

	for ( int i = 0 ; i < c ; i++ ) {
		((int *)data)[i] = 0;
	}

	// FIXME: this won't live past vid mode changes
	image->GenerateImage( data, vt_page_size.GetInteger(), vt_page_size.GetInteger(), 
		TF_NEAREST, false, TR_REPEAT, TD_HIGH_QUALITY,0, IMAGE_COMPRESS_DXT5 );

	delete data;
	image->rawBuffer = NULL;
	//image->CreatePBO();
}
/*
====================
bmVirtualTexturePage::ResetPage
====================
*/
void bmVirtualTexturePage::ResetPage( void ) {
	int numTiles = vt_page_size.GetInteger() / VIRTUALTEXTURE_TILESIZE;

	for(int i = 0; i < numTiles * numTiles; i++) {
		tiles[i].realTileNum = -1;
	}

	nextFreeTile = &tiles[0];
	lastTile = NULL;
	lastBlittedTile = 0;
	numActiveTiles = 0;
}


/*
====================
bmVirtualTexturePage::BlitTileToPage
====================
*/
bmVirtualTexturePageTile_t	*bmVirtualTexturePage::BlitTileToPage( bmVirtualTextureFile *vtfile, int pageNum, int tileNum ) {
	int i;

	bmVirtualTexturePageTile_t *freeTile = NULL;
	int numTiles = vt_page_size.GetInteger() / VIRTUALTEXTURE_TILESIZE;

	if(lastTile != NULL && lastTile->realTileNum == (tileNum) + (pageNum * 255)) {
		return lastTile;
	}

	for(i = 0; i < numTiles * numTiles; i++) {
//		if(tiles[i].IsDirty(frameNum) && freeTile == NULL) {
//			freeTile = &tiles[i];
//		}

		// If this tile is already uploaded we don't need to do it again.
		if(tiles[i].vtfile == vtfile && tiles[i].realTileNum == (tileNum) + (pageNum * 255)) {
			lastTile = &tiles[i];

			return lastTile;
		}
	}

	

	
	if(numActiveTiles >= (VIRTUALTEXTURE_PAGESIZE / VIRTUALTEXTURE_TILESIZE) * (VIRTUALTEXTURE_PAGESIZE / VIRTUALTEXTURE_TILESIZE) ) {
		nextFreeTile = &tiles[0];
		lastTile = NULL;
		lastBlittedTile = 0;
		numActiveTiles = 0;
	}


	freeTile = nextFreeTile++;
	assert( freeTile != NULL );
#if !VT_LOAD_FROMMEMORY
	vtfile->ReadTile( pageNum, tileNum, freeTile->buffer );
#else
	freeTile->buffer = vtfile->ReadTile( pageNum, tileNum );
#endif
	// Blit the tile into image.
	freeTile->vtfile = vtfile;
	freeTile->realTileNum = (tileNum) + (pageNum * 255);
	freeTile->frameNum = frameNum + 1;
	freeTile->page = this;

	numActiveTiles++;
	lastTile = freeTile;

	isPageDirty = true;
	
	return lastTile;
}

/*
====================
bmVirtualTexturePage::Init
====================
*/
void bmVirtualTexturePage::Init( const char *name ) {
	common->Printf( "Creating Virtual Texture Page(%dx%d)\n", vt_page_size.GetInteger(), vt_page_size.GetInteger());

	// Create the virtual texture page.
	image = globalImages->ImageFromFunction( va( "_vtpage_%s", name), R_EmptyTexturePage );

	frameNum = 0;
	isPageDirty = false;
	pageTime = 0;

	int numTiles = VIRTUALTEXTURE_PAGESIZE / VIRTUALTEXTURE_TILESIZE;
	tiles = new bmVirtualTexturePageTile_t[numTiles * numTiles];

	int tileNum = 0;
	for(int h = 0; h < numTiles; h++ ) {
		for(int w = 0; w < numTiles; w++, tileNum++ ) {
			tiles[tileNum].x = (VIRTUALTEXTURE_TILESIZE) * w;
			tiles[tileNum].y = (VIRTUALTEXTURE_TILESIZE) * h;
			tiles[tileNum].tileNum = tileNum;
			tiles[tileNum].px = w ;
			tiles[tileNum].py = h; 
		}
	}


	nextFreeTile = &tiles[0];
	lastTile = NULL;
	lastBlittedTile = numActiveTiles = 0;
}


/*
====================
bmVirtualTexturePage::Upload
====================
*/
void bmVirtualTexturePage::Upload( void ) {
	if(!isPageDirty) {
		return;
	}

	//image->UploadScratch( image->rawBuffer, image->uploadWidth, image->uploadHeight );

	isPageDirty = false;
	image->Bind(); 


	for(int i = lastBlittedTile; i < numActiveTiles; i++) {
		if(tiles[i].buffer == NULL)
			continue;

		image->CopyBufferIntoRegion( tiles[i].buffer, tiles[i].x, tiles[i].y, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE );
		//image->pbo->WriteToPBO(0, tiles[i].buffer, tiles[i].x, tiles[i].y, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE );
	}
	//image->pbo->Unbind();
}

/*
====================
bmVirtualTexturePage::Bind
====================
*/
void bmVirtualTexturePage::Bind( void ) { 

	image->Bind(); 
}

/*
====================
bmVirtualTexturePage::UnBind
====================
*/
void bmVirtualTexturePage::UnBind( void ) { 
	int numTiles = (vt_page_size.GetInteger() /  VIRTUALTEXTURE_TILESIZE);
	//if(numActiveTiles >= (numTiles * numTiles) - 1) {
		
	//}

	lastBlittedTile = numActiveTiles;

	frameNum++; 
}