// VirtualTexturePage.cpp
//

#include "precompiled.h"

#include "VirtualTexture.h"
#include "../renderer/tr_local.h"

#ifndef BSPCOMPILER
#include "VirtualTexture_Backend.h"
#include "../renderer/gpuworker/GpuWorker_VTCache.h"
#endif

idCVar vt_page_size( "vt_page_size", "4096", CVAR_RENDERER | CVAR_INTEGER , "Size of the paging texture" );

/*
===============
R_EmptyTexturePage
===============
*/
static void R_EmptyTexturePage( idImage *image ) {
	int	c = vt_page_size.GetInteger() * vt_page_size.GetInteger();

	// FIXME: this won't live past vid mode changes
	image->GenerateImage( NULL, vt_page_size.GetInteger(), vt_page_size.GetInteger(), 
		TF_LINEAR, false, TR_CLAMP_TO_BORDER, TD_HIGH_QUALITY,0, IMAGE_COMPRESS_DXT5 );


	image->rawBuffer = NULL;
	//image->CreatePBO();
}

/*
===============
R_EmptyTexturePage2
===============
*/
static void R_EmptyTexturePage2( idImage *image ) {
	int	c = (vt_page_size.GetInteger() / 2) * (vt_page_size.GetInteger() / 2);

	// FIXME: this won't live past vid mode changes
	image->GenerateImage( NULL, vt_page_size.GetInteger() /2, vt_page_size.GetInteger()/2, 
		TF_LINEAR, false, TR_CLAMP_TO_BORDER, TD_HIGH_QUALITY,0, IMAGE_COMPRESS_DXT5 );


	image->rawBuffer = NULL;
	//image->CreatePBO();
}

/*
===============
R_EmptyTexturePage2
===============
*/
static void R_EmptyTexturePage3( idImage *image ) {
	int	c = (vt_page_size.GetInteger() / 4) * (vt_page_size.GetInteger() / 4);

	// FIXME: this won't live past vid mode changes
	image->GenerateImage( NULL, vt_page_size.GetInteger() /4, vt_page_size.GetInteger()/4, 
		TF_LINEAR, false, TR_CLAMP_TO_BORDER, TD_HIGH_QUALITY,0, IMAGE_COMPRESS_DXT5 );


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
bmVirtualTexturePageTile_t	*bmVirtualTexturePage::BlitTileToPage( bmVirtualTextureFile *vtfile, int pageNum, int tileNum, int mipLevel ) {
	int i;
#ifndef BSPCOMPILER
	bmVirtualTexturePageTile_t *freeTile = NULL;
	int numTiles = vt_page_size.GetInteger() / VIRTUALTEXTURE_TILESIZE;

	if(lastTile != NULL && lastTile->realTileNum == (tileNum) + (pageNum * 1255)) {
		return lastTile;
	}

	for(i = 0; i < numActiveTiles/*numTiles * numTiles*/; i++) {
//		if(tiles[i].IsDirty(frameNum) && freeTile == NULL) {
//			freeTile = &tiles[i];
//		}

		// If this tile is already uploaded we don't need to do it again.
		if(tiles[i].vtfile == vtfile && tiles[i].realTileNum == (tileNum) + (pageNum * 1255)) {
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
	vtfile->ReadTile( pageNum, tileNum, mipLevel, freeTile->buffer );
#else
	freeTile->buffer = vtfile->ReadTile( pageNum, tileNum, mipLevel );
#endif
	// Blit the tile into image.
	freeTile->vtfile = vtfile;
	freeTile->realTileNum = (tileNum) + (pageNum * 1255);
	freeTile->frameNum = frameNum + 1;
	freeTile->page = this;

	numActiveTiles++;
	lastTile = freeTile;

	isPageDirty = true;
	
	return lastTile;
#else
	return NULL;
#endif
}
/*
====================
bmVirtualTexturePage::InitDevice
====================
*/
void bmVirtualTexturePage::InitDevice( void ) {
#ifndef BSPCOMPILER
	//vtBackEnd.gpuTextureWorkerProgram->CreateSharedTexturePage( image[2] );
#endif
}

/*
====================
bmVirtualTexturePage::Init
====================
*/
void bmVirtualTexturePage::Init( const char *name ) {
	common->Printf( "Creating Virtual Texture Page(%dx%d)\n", vt_page_size.GetInteger(), vt_page_size.GetInteger());
#ifndef BSPCOMPILER
	// Create the virtual texture page.
	image[0] = globalImages->ImageFromFunction( va( "_vtpage_%s", name), R_EmptyTexturePage );
	image[1] = globalImages->ImageFromFunction( va( "_vtpage_%s_mip1", name), R_EmptyTexturePage2 );
	image[2] = globalImages->ImageFromFunction( va( "_vtpage_%s_mip2", name), R_EmptyTexturePage3 );

	

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
#if !VT_LOAD_FROMMEMORY
			tiles[tileNum].buffer = new byte[ VIRTUALTEXTURE_TILEMEMSIZE ];
#endif
		}
	}


	nextFreeTile = &tiles[0];
	lastTile = NULL;
	lastBlittedTile = numActiveTiles = 0;
#endif
}


/*
====================
bmVirtualTexturePage::Upload
====================
*/
void bmVirtualTexturePage::Upload( int mipLevel ) {
	if(!isPageDirty) {
		return;
	}

	//image->UploadScratch( image->rawBuffer, image->uploadWidth, image->uploadHeight );
	currentMipLevel = mipLevel;
	isPageDirty = false;
	image[mipLevel]->Bind(); 

	int imageScale = 1;
	// Fix me bit shifting would be nice here...
	switch(mipLevel) {
		case 0:
			imageScale = 1;
		break;
		case 1:
			imageScale = 2;
		break;

		case 2:
			imageScale = 4;
		break;

		default:
			common->FatalError("VT_Upload: miplevel unknown\n");
		break;
	}


	for(int i = lastBlittedTile; i < numActiveTiles; i++) {
		if(tiles[i].buffer == NULL)
			continue;

		image[mipLevel]->CopyBufferIntoRegion( tiles[i].buffer, 0, tiles[i].x / (imageScale), tiles[i].y / (imageScale), VIRTUALTEXTURE_TILESIZE / (imageScale), VIRTUALTEXTURE_TILESIZE / (imageScale) );
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
#ifndef BSPCOMPILER
	image[currentMipLevel]->Bind(); 
#endif
}

/*
====================
bmVirtualTexturePage::UnBind
====================
*/
void bmVirtualTexturePage::UnBind( void ) { 
	int numTiles = (vt_page_size.GetInteger() /  VIRTUALTEXTURE_TILESIZE);
	nextFreeTile = &tiles[0];
	lastTile = NULL;
	lastBlittedTile = 0;
	numActiveTiles = 0;

	lastBlittedTile = numActiveTiles;

	frameNum++; 
}