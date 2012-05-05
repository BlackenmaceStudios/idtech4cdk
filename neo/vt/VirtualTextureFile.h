// VirtualTextureFile.h
//

#define VT_LOAD_FROMMEMORY		3

#define VIRTUALTEXTURE_EXTEN	".virtualtexture"
#define VIRTUALTEXTURE_IDEN		(('F'<<24)+('T'<<16)+('V'<<8)+'J')
#define VIRTUALTEXTURE_VERSION	2

#define VIRTUALTEXTURE_PAGESIZE 4096
#define VIRTUALTEXTURE_TILESIZE 256

#define VIRTUALTEXTURE_TILEMEMSIZE (VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE)

//
// bmVirtualTextureHeader_t
//
struct bmVirtualTextureHeader_t {
	int			iden;			// Iden 
	int			version;		// Version
	int			mapcrc;			// CRC of the map file
	int			numCharts;
	int			numTiles;		// How many tiles are in the virtualtexture file.

	//
	// bmVirtualTextureHeader_t
	//
	bmVirtualTextureHeader_t() {
		Reset();
	}


	bool InitFromFile( idFile *f ) {
		f->Read( this, sizeof( bmVirtualTextureHeader_t ) );

		if(iden != VIRTUALTEXTURE_IDEN) {
			common->FatalError( "VT has an invalid iden\n");
		}

		if(version != VIRTUALTEXTURE_VERSION) {
			common->Warning( "VT version mismatch\n" );
			return false;
		}

		if(numTiles <= 0) {
			common->Warning("VT with no tiles\n");
			return false;
		}

		return true;
	}

	//
	// Reset
	//
	void Reset( void )
	{
		iden    = VIRTUALTEXTURE_IDEN;
		version = VIRTUALTEXTURE_VERSION;
		mapcrc = 0;
		numTiles = 0;
	}

	//
	// WriteToFile
	//
	void WriteToFile( idFile *f ) {
		f->Seek( 0, FS_SEEK_SET );
		f->Write( this, sizeof( bmVirtualTextureHeader_t ) );
	}
};

//
// bmVirtualTextureFile
//
class bmVirtualTextureFile {
public:
								~bmVirtualTextureFile();

								bmVirtualTextureFile();

	bool						InitFromFile( const char *path );
	bool						InitNewVirtualTextureFile( const char *path, int numAreas );
#if !VT_LOAD_FROMMEMORY
	void						ReadTile( int pageNum, int tileNum, byte *buffer );
#else
	byte *						ReadTile(  int pageNum, int tileNum );
#endif
	int							WriteSurfaceTileToVT( const char *path );

	void						WriteTile( byte *buffer, int xoff, int yoff, int DiemWidth );

	void						FinishVirtualTextureWrite( void );

	void						BindAtlas( void );

	int							NumCharts( void ) { return header.numCharts; }

	idImage						*vtAtlasImage2;
private:

	idFile *f;
	byte *fileBuffer;
	int fileBufferLen;

	idFile *image;
	idList< idStr > imglist;

	idImage						*vtAtlasImage;
	
	bmVirtualTextureHeader_t	header;
};