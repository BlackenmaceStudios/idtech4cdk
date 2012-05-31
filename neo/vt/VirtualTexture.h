// VirtualTexture.h
//

/*
========================================

Virtual Texturing:

A virtual texutre is a gigantic texture 


========================================
*/

#define VIRTUALTEXTURE_PAGESIZE	4096
#define VT_NUMPAGES 2

#include "VirtualTextureFile.h"
#include "./dxt/dxt.h"

class bmVirtualTexturePage;
extern idCVar vt_debug_atlas;
extern idCVar vt_syncrender;

//
// bmVirtualTexturePageTile
//
struct bmVirtualTexturePageTile_t {
	int tileNum;
	int frameNum;
	int realTileNum;
	int x;
	int y;
	int px;
	int py;
	int index;
	bmVirtualTexturePage *page;

	byte *buffer;


	bmVirtualTextureFile *vtfile;

	bmVirtualTexturePageTile_t() {
		tileNum = -1;
		vtfile = NULL;
		frameNum = 0;
		x = 0;
		y = 0;
#if !VT_LOAD_FROMMEMORY
		buffer = new byte[VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE * 4];
#endif
	}
	~bmVirtualTexturePageTile_t() {
#if !VT_LOAD_FROMMEMORY
		if(buffer != NULL) {
			delete buffer;
			buffer = NULL;
		}
#endif
	}

	bool IsDirty( int frameTime ) {
		return frameNum < frameTime || tileNum == -1;
	}
};

//
// bmVirtualTextureManager
//
class bmVirtualTexturePage {
public:
	void				Init( const char *name );
	void				InitDevice( void );

	bmVirtualTexturePageTile_t	*BlitTileToPage( bmVirtualTextureFile *vtfile, int pageNum, int tileNum, int mipLevel );
	bmVirtualTexturePageTile_t	*GetTileInfo( int tileNum ) { return &tiles[tileNum]; }
	void				ResetPage( void );

	void				Bind( void );
	void				UnBind( void );

	void				Upload( int mipLevel );
private:
	int					frameNum;
	int					pageTime;
	int					currentMipLevel;
	idImage				*image[3]; // Hack for mipmaps.
	bool				isPageDirty;
	int					numActiveTiles;
	int					lastBlittedTile;

	bmVirtualTexturePageTile_t *nextFreeTile;
	bmVirtualTexturePageTile_t *lastTile;
	bmVirtualTexturePageTile_t *tiles;
};

//
// bmVirtualTextureManager
//
class bmVirtualTextureManager {
public:
								bmVirtualTextureManager();
								~bmVirtualTextureManager();

	// Init the virtual texture manager.
	void						Init( void );
	void						InitDevice( void );

	bmVirtualTexturePage		*GetWorldPage( int pageId ) { return &pages[pageId]; };
	bmVirtualTexturePage		*GetWorldPage() { return &pages[currentPage]; };
	void						ResetPages( void );
	void						FlipPage( void );
	void						FlipToDefaultPage( void ) { currentPage = 0; }

	bmVirtualTextureFile		*LoadVirtualTextureFile( const char *path );

	// Create a virtual texture file from a map.
	bmVirtualTextureFile		*CreateNewVirtualTextureFile( const char *path, int numAreas );

	bmVirtualTextureFile		*GetCurrentVirtualTextureFile( void ) { return currentVirtualTextureFile; }

	void						FreeVirtualTextureFile( void );
private:
	int							currentPage;
	bmVirtualTexturePage		pages[VT_NUMPAGES];
	bmVirtualTextureFile		*currentVirtualTextureFile;
};

extern bmVirtualTextureManager		*virtualTextureManager;