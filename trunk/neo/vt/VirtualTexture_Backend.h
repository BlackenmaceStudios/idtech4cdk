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
	void						UploadAreaTiles( int pageId, int mipLevel, idList<bmVTTileReadback_t> &areaTiles );

	int							sceneAreaDist[64];
	idList<bmVTTileReadback_t>	sceneTiles[64];
};

extern bmVirtualTextureBackend vtBackEnd;