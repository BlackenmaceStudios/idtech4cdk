// VirtualTextureFile.cpp
//

#include "precompiled.h"

#include "VirtualTexture.h"
#include "../renderer/tr_local.h"


#define DEFAULT_TEXTUREPATH "textures/editor/blockout.tga";



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
bmVirtualTextureFile::InitFromFile
===========================
*/
bool bmVirtualTextureFile::InitFromFile( const char *path ) {
#ifndef BSPCOMPILER
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
#else
	return false;
#endif
}



/*
===========================
bmVirtualTextureFile::ReadTile
===========================
*/

#if !VT_LOAD_FROMMEMORY
void bmVirtualTextureFile::ReadTile(  int pageNum, int tileNum, int mipLevel, byte *tileBuffer ) {
	int bufferpos = sizeof(bmVirtualTextureHeader_t) + (((4096 * 4096) + (2048 * 2048) + (1024 * 1024)) * pageNum);

	
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

	assert(tileBuffer != NULL);
	if(bufferpos >= f->Length()) {
		common->FatalError( "VT_ReadTile: %d > %d\n", bufferpos, f->Length());
	}

	f->Seek(bufferpos, FS_SEEK_SET);

	switch( mipLevel)
	{
		case 0:
			{
				int len = f->Read( tileBuffer, VIRTUALTEXTURE_TILEMEMSIZE );
				if(len < VIRTUALTEXTURE_TILEMEMSIZE) {
					common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
				}
			}
		break;
		case 1:
			{
				int len = f->Read( tileBuffer, 128 * 128 );
				if(len < 128 * 128) {
					common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
				}
			}
		break;

		case 2:
			{
				int len = f->Read( tileBuffer, 64 * 64 );
				if(len < 64 * 64) {
					common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
				}
			}
		break;
	}

}
#else
byte *bmVirtualTextureFile::ReadTile(  int pageNum, int tileNum, int mipLevel ) {
	INT_PTR bufferpos = (((4096 * 4096) + (2048 * 2048) + (1024 * 1024)) * pageNum);
	
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
bmVirtualTextureFile::BindAtlas
===========================
*/
void bmVirtualTextureFile::BindAtlas( void ) { 
	vtAtlasImage->Bind(); 
}