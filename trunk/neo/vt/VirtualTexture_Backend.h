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


	void Reset(void) {
		tileNum = 0;
		pageNum = 0;
		dist = 0;
	}
};

//
// bmVirtualTextureBackend
//
#define VT_MAXTILES_IN_SCENE	9216
#define VT_MAXCHARTS			260
class bmVirtualTextureBackend {
public:
	void						Init( void );
	void						InitDevice( void );
	void						GenerateSceneTileInfo( struct drawSurf_s **drawSurfs, int numDrawSurfs );
	void						Bind( void );
	void						UpdateSceneVT( void );

	idImage						*sceneFbo;
	idImage						*sceneFboPublic;

	class bmVirtualTextureCacheGPUWorker *gpuTextureWorkerProgram;
private:
	void						ReadCurrentScene( void );
	void						ReadTilesInScene( byte *readbackBuffer, int width, int height );
	void						UploadAreaTiles( int pageId, int mipLevel,  bmVTTileReadback_t *areaTiles, int numTiles );

	int							*sceneAreaDist;
	int							*numSceneTiles;
	idList<bmVTTileReadback_t *> sceneTiles;

	
};

extern bmVirtualTextureBackend vtBackEnd;