// VirtualTextureFile.cpp
//

#include "precompiled.h"

#include "VirtualTexture.h"
#include "../renderer/tr_local.h"


#define DEFAULT_TEXTUREPATH "textures/editor/blockout.tga";

idCVar vt_debug_tiles( "vt_debug_tiles", "0", CVAR_RENDERER | CVAR_BOOL , "Write out each tile as a dds texture." );

byte *compressedBuffer = NULL;

bmVirtualTextureFile::bmVirtualTextureFile() {
	f = NULL;
	fileBuffer = NULL;
}

bmVirtualTextureFile::~bmVirtualTextureFile() {
	if(f != NULL) {
		fileSystem->CloseFile( f );
		f = NULL;
	}

	if(fileBuffer != NULL)
	{
		delete fileBuffer;
		fileBuffer = NULL;
	}
}

//
// R_CreateImageAtlasTexture
//
static void R_CreateImageAtlasTextureInt( idImage *image ) {
	image->Generate2DImageAtlas( 4096, 256, true );
}

//
// R_CreateImageAtlasTexture
//
static void R_CreateImageAtlasTexture( idImage *image ) {
	image->Generate2DImageAtlas( 4096, 256, false );
}



/*
===========================
bmVirtualTextureFile::InitNewVirtualTextureFile
===========================
*/
bool bmVirtualTextureFile::InitNewVirtualTextureFile( const char *path, int numAreas ) {
	header.Reset();

	common->Printf( "----------- VT_InitNewVirtualTextureFile ----------\n" );

	vtpath = va( "%s%s", path, VIRTUALTEXTURE_EXTEN );

	// Open the virtual texture for writing.
	f = fileSystem->OpenFileWrite( va( "%s%s_temp", path, VIRTUALTEXTURE_EXTEN ), "fs_devpath" );
	if(f == NULL) {
		common->Warning("Failed to open %s for writing\n", path );
		return false;
	}

	// Write the header out first so everything gets positioned correctly and we can write in real time.
	header.numCharts = numAreas;
	header.WriteToFile( f );

	imglist.Clear();
	

	
	byte *buffer;
	int imgWidth = 0;
	int imgHeight = 0;
	ID_TIME_T timeStamp;

	// Allocate these for resuse inorder not to fragment up memory.
	buffer = (byte *)R_StaticAlloc( VIRTUALTEXTURE_PAGESIZE * VIRTUALTEXTURE_PAGESIZE * 4 );
	byte *mippedLevel = (byte *)R_StaticAlloc( 2048 * 2048 * 4 );
	byte *mippedLevel2 = (byte *)R_StaticAlloc( 1024 * 1024 * 4 );
	for(int i = 0; i < numAreas; i++)
	{
		idStr targaPath = path + idStr(va( "_u%d_v1", i + 1));
		targaPath.SetFileExtension( ".tga" );

		//R_LoadImage( targaPath.c_str(), &buffer, &imgWidth, &imgHeight, &timeStamp, true );
		LoadTGA(targaPath.c_str(), &buffer, &imgWidth, &imgHeight, &timeStamp, false );

		// Check the image stats.
		if(imgWidth != VIRTUALTEXTURE_PAGESIZE || imgHeight != VIRTUALTEXTURE_PAGESIZE) {
			common->FatalError("InitNewVirtualTextureFile: Chart image dimensions aren't %d\n", VIRTUALTEXTURE_PAGESIZE);
		}

		if(imgWidth <= 0 || imgHeight <= 0) {
			common->Warning("Area texture for VT not found...\n");
			return false;
		}
		else
		{
			common->Printf( "------- VT_BuildArea(%d/%d) ----------\n", i + 1, numAreas);
		}

		int tileSize = VIRTUALTEXTURE_TILESIZE;
		
		common->Printf("...Mipmap(level 0) (%dx%d)\n", imgWidth, imgHeight); 

		int maxTiles = (imgWidth / tileSize) * (imgWidth / tileSize);
		int currentTile = 0;

		for(int h = 0; h < imgWidth / tileSize; h++) {
			for(int w = 0; w < imgWidth / tileSize; w++, header.numTiles++, currentTile++) {

				//common->Printf( "...Tile %d/%d", currentTile, maxTiles);
				WriteTile( buffer, w * tileSize, h * tileSize, imgWidth, tileSize );
			}
		}

		

		// Write the next mipmap level.
		R_MipMap( buffer, mippedLevel, imgWidth, imgHeight, true );
		imgWidth = imgWidth >> 1;
		imgHeight = imgHeight >> 1;
		tileSize = tileSize >> 1;

		common->Printf("...Mipmap (%dx%d) %d\n", imgWidth, imgHeight, tileSize );
		for(int h = 0; h < imgWidth / tileSize; h++) {
			for(int w = 0; w < imgWidth / tileSize; w++) {

				//common->Printf( "...Tile %d/%d", currentTile, maxTiles);
				WriteTile( mippedLevel, w * tileSize, h * tileSize, imgWidth, tileSize );
			}
		}
		

		// Write the next mipmap level.
		R_MipMap( mippedLevel, mippedLevel2, imgWidth, imgHeight, true );
		imgWidth = imgWidth >> 1;
		imgHeight = imgHeight >> 1;
		tileSize = tileSize >> 1;

		common->Printf("...Mipmap (%dx%d) %d\n", imgWidth, imgHeight, tileSize );
		for(int h = 0; h < imgWidth / tileSize; h++) {
			for(int w = 0; w < imgWidth / tileSize; w++) {

				//common->Printf( "...Tile %d/%d", currentTile, maxTiles);
				WriteTile( mippedLevel2, w * tileSize, h * tileSize, imgWidth, tileSize );
			}
		}
		
	}
	R_StaticFree( buffer );
	R_StaticFree( mippedLevel2 );
	R_StaticFree( mippedLevel );

	delete[] compressedBuffer;

	common->Printf("Virtual Texture written successfully\n");
	return true;
}

/*
===========================
bmVirtualTextureFile::WriteTile
===========================
*/
void R_WriteDDS( const char *path, byte *data, int uploadWidth, int uploadHeight );
void bmVirtualTextureFile::WriteTile( byte *buffer, int DestX, int DestY, int DiemWidth, int vtTileSize ) {
	unsigned int  		x, y, z, ConvBps, ConvSizePlane;
	byte 	*Converted;
	int Depth = 1;
	unsigned int 		c;
	unsigned int 		StartX, StartY, StartZ;
	byte 	*SrcTemp;
	float 	Back;
	int DestZ = 0;
	unsigned int Width = vtTileSize;
	unsigned int Height = vtTileSize;

	if(compressedBuffer == NULL) {
		compressedBuffer = new byte[vtTileSize * vtTileSize];
	}
	memset(&compressedBuffer[0], 0, vtTileSize * vtTileSize);
	
	ConvBps 	  = 4 * Width;
	ConvSizePlane = ConvBps   * Height;
	
	//@NEXT in next version this would have to be removed since Dest* will be unsigned
	StartX = DestX >= 0 ? 0 : -DestX;
	StartY = DestY >= 0 ? 0 : -DestY;
	
	// Limit the copy of data inside of the destination image
	if (Width  + DestX > DiemWidth)  Width  = DiemWidth  - DestX;
	if (Height + DestY > DiemWidth) Height = DiemWidth - DestY;
	if (Depth  + DestZ > DiemWidth)  Depth  = 1;
	
	byte *tempBuffer = new byte[vtTileSize * vtTileSize * 4];
	memset(&tempBuffer[0], 0, vtTileSize * vtTileSize);
	const unsigned int  bpp_without_alpha = 4 - 1;
		for (z = 0; z < Depth; z++) {
			for (y = 0; y < Height; y++) {
				for (x = 0; x < Width; x++) {
					const unsigned int   SrcIndex  = (z+0)*ConvSizePlane + (y+0)*ConvBps + (x+0)*4;
					const unsigned int   DestIndex = (z+DestZ)*(DiemWidth * DiemWidth) + (y+DestY)*(DiemWidth * 4) + (x+DestX)*4;
					
					tempBuffer[SrcIndex + 0] = buffer[DestIndex + 0];
					tempBuffer[SrcIndex + 1] = buffer[DestIndex + 1];
					tempBuffer[SrcIndex + 2] = buffer[DestIndex + 2];
					tempBuffer[SrcIndex + 3] = buffer[DestIndex + 3];
					
					
					
				}

			}
		}

	int numBytes = 0;
	byte border[4] = { 0, 0, 0, 0 };
	//R_SetBorderTexels( tempBuffer, vtTileSize, vtTileSize, border );
	CompressImageDXT5YCoCg( tempBuffer, &compressedBuffer[0], vtTileSize, vtTileSize, numBytes ); 

	//byte *cbuf = toolInterface->CompressImage( tempBuffer, vtTileSize, vtTileSize );

	
	f->Write( &compressedBuffer[0], vtTileSize * vtTileSize );

	if(vt_debug_tiles.GetBool()) {
		R_WriteDDS(va("vt/tiles/%d.dds", header.numTiles), &compressedBuffer[0], vtTileSize, vtTileSize );
	}

	delete[] tempBuffer;
}

/*
===========================
bmVirtualTextureFile::WriteSurfaceTileToVT
===========================
*/
int bmVirtualTextureFile::WriteSurfaceTileToVT( const char *path ) {
/*
	const idMaterial *mat;

	// Load in the material, we do this before searching if the image is already loaded because we need to test
	// if the material is valid or not.
	mat = declManager->FindMaterial( path, false );
	if(mat == NULL) {
		path = DEFAULT_TEXTUREPATH;
	}
	else {
		idImage *img = mat->GetDiffuseImage();

		if(img == NULL)
		{
			path = DEFAULT_TEXTUREPATH;
		}
		else
		{
			path = img->imgName.c_str();
		}
	}

	for(int i = 0; i < imglist.Num(); i++) {
		if(!imglist[i].Cmp( path )) {
			return i + 1;
		}
	}

	byte border[4];
	border[0] = 0;
	border[1] = 0;
	border[2] = 0;
	border[3] = 0;

	byte *buffer;
	int imgWidth = 0;
	int imgHeight = 0;
	ID_TIME_T timeStamp;

	R_LoadImage( path, &buffer, &imgWidth, &imgHeight, &timeStamp, true );

	R_SetBorderTexels( buffer, imgWidth, imgHeight, border );

//	f->WriteInt( imgWidth );
//	f->WriteInt( imgHeight );
	f->Write( buffer, imgWidth * imgHeight * 4 );

	imglist.Append( path );
	return imglist.Num();
*/
	return -1;
}

/*
===========================
bmVirtualTextureFile::InitFromFile
===========================
*/
bool bmVirtualTextureFile::InitFromFile( const char *path ) {
	if( f != NULL  ) {
		common->Error( "VTInitFromFile: Persistant data is still valid\n");
	}

	// Open the file reading.
	f = fileSystem->OpenFileRead( va( "vt/%s%s", path, VIRTUALTEXTURE_EXTEN ) );
	if( f == NULL ) {
		common->Warning("Failed to open virtual texture %s\n", path );
		return false;
	}

	if(!header.InitFromFile( f )) {
		common->Warning("Virtual texture is an invalid format\n");
		return false;
	}

#if VT_LOAD_FROMMEMORY
	fileBufferLen = f->Length();

	game->UpdateLoadingString("Init Virtual Texture..");

	common->Printf("Init VT Heap %d\n", fileBufferLen);
	fileBuffer = new byte[fileBufferLen];

	f->Read( fileBuffer, fileBufferLen );

	fileSystem->CloseFile( f );
	f = NULL;
#endif

	
	vtAtlasImage2 = globalImages->ImageFromFunction( va( "_vt_atlas2", path), R_CreateImageAtlasTexture );
	vtAtlasImage = globalImages->ImageFromFunction( va( "_vt_atlas", path), R_CreateImageAtlasTextureInt );
	

	return true;
}

/*
===========================
bmVirtualTextureFile::ReadTile
===========================
*/

#if !VT_LOAD_FROMMEMORY
void bmVirtualTextureFile::ReadTile(  int pageNum, int tileNum, byte *tileBuffer ) {
	int bufferpos = sizeof(bmVirtualTextureHeader_t) + ((4096 * 4096) * pageNum) + (VIRTUALTEXTURE_TILEMEMSIZE * (tileNum));

	assert(tileBuffer != NULL);
	if(bufferpos >= f->Length()) {
		common->FatalError( "VT_ReadTile: %d > %d\n", bufferpos, f->Length());
	}

	f->Seek(bufferpos, FS_SEEK_SET);

	int len = f->Read( tileBuffer, VIRTUALTEXTURE_TILEMEMSIZE );
	if(len < VIRTUALTEXTURE_TILEMEMSIZE) {
		common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
	}

}
#else
byte *bmVirtualTextureFile::ReadTile(  int pageNum, int tileNum, int mipLevel ) {
	int bufferpos = (((4096 * 4096) + (2048 * 2048) + (1024 * 1024)) * pageNum);
	
	if(mipLevel == 0)
	{
		bufferpos += (VIRTUALTEXTURE_TILEMEMSIZE * (tileNum));
	}
	else
	{
		bufferpos += 4096 * 4096;

		if(mipLevel == 1)
		{
			bufferpos += ((128 * 128) * tileNum);
		}
		else if(mipLevel == 2)
		{
			bufferpos += 2048 * 2048;
			bufferpos += ((64 * 64) * tileNum);
		}
		else
		{
			common->FatalError("VT_ReadTile: Unknown mipLevel %d\n", mipLevel);
		}
	}

	if(bufferpos >= fileBufferLen) {
		common->FatalError( "VT_ReadTile: %d > %d", bufferpos, fileBufferLen );
	}

	return fileBuffer + bufferpos;
}
#endif

/*
===========================
bmVirtualTextureFile::FinishVirtualTextureWrite
===========================
*/
void bmVirtualTextureFile::FinishVirtualTextureWrite( void ) {
	idStr temppath;

	if( f == NULL ) {
		common->FatalError( "FinishVirtualTextureWrite: f == null\n");
	}
	//header.numTiles = imglist.Num();

	common->Printf("-------VT_FinishTextureWrite------\n");
	common->Printf( "Wrote %d tiles - %d bytes\n", header.numTiles, f->Length());

	temppath = f->GetFullPath();

	// Re-write the header and close the file.
	header.WriteToFile( f );
	f->Flush();

	fileSystem->CloseFile( f );
	f = NULL;

	// Rename the temp file to the real path.
	fileSystem->RenameFile( fileSystem->RelativePathToOSPath( vtpath.c_str() ), temppath.c_str() );
}
/*
===========================
bmVirtualTextureFile::BindAtlas
===========================
*/
void bmVirtualTextureFile::BindAtlas( void ) { 
	vtAtlasImage->Bind(); 
}