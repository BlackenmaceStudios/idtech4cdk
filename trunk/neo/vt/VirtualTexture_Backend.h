// VirtualTexture_Backend.h
//

extern idCVar vt_backend_fbosize_width;
extern idCVar vt_backend_fbosize_height;

//
// bmVTTileReadback_t
//
struct bmVTTileReadback_t {
	byte	tileNum;			// 0 - 255
	byte	pageNum;			// Page number
	byte	dist;				// How far away the tile is.


	bmVTTileReadback_t() {
		tileNum = 0;
		pageNum = 0;
		dist = 0;
	}
};

//
// bmVirtualTextureBackend
//
#define VT_MAXTILES_IN_SCENE	9216
#define VT_MAXCHARTS			69
class bmVirtualTextureBackend {
public:
	void						Init( void );
	void						GenerateSceneTileInfo( struct drawSurf_s **drawSurfs, int numDrawSurfs );
	void						Bind( void );
	void						UpdateSceneVT( void );

	idImage						*sceneFbo;
	idImage						*sceneFboPublic;

private:
	void						ReadCurrentScene( void );
	void						ReadTilesInScene( byte *readbackBuffer, int width, int height );
	void						UploadAreaTiles( int pageId, int mipLevel,  bmVTTileReadback_t *areaTiles, int numTiles );

	int							sceneAreaDist[VT_MAXCHARTS];
	int							numSceneTiles[VT_MAXCHARTS];
	bmVTTileReadback_t			sceneTiles[VT_MAXCHARTS][VT_MAXTILES_IN_SCENE];
};

extern bmVirtualTextureBackend vtBackEnd;