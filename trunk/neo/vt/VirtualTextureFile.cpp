// VirtualTextureFile.cpp
//

#include "precompiled.h"

#include "VirtualTexture.h"
#include "../renderer/tr_local.h"


#define DEFAULT_TEXTUREPATH "textures/editor/blockout.tga";



bmVirtualTextureFile::bmVirtualTextureFile() {
	for(int i = 0; i < VIRTUALTEXTURE_NUMLEVELS; i++)
	{
		f[i] = NULL;
		fileBuffer[i] = NULL;
	}
}

bmVirtualTextureFile::~bmVirtualTextureFile() {
	for(int i = 0; i < VIRTUALTEXTURE_NUMLEVELS; i++)
	{
		if(f[i] != NULL) {
			fileSystem->CloseFile( f[i] );
			f[i] = NULL;
		}

		if(fileBuffer[i] != NULL)
		{
			delete fileBuffer[i];
			fileBuffer[i] = NULL;
		}
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
	for(int i = 0; i < VIRTUALTEXTURE_NUMLEVELS; i++)
	{
		if( f[i] != NULL  ) {
			common->Error( "VTInitFromFile: Persistant data is still valid\n");
		}

		// Open the file reading.
		f[i] = fileSystem->OpenFileRead( va( "vt/%s_level%d%s", path, i, VIRTUALTEXTURE_EXTEN ) );
		if( f[i] == NULL ) {
			common->Warning("Failed to open virtual texture %s\n", path );
			return false;
		}

		if(!header.InitFromFile( f[i] )) {
			common->Warning("Virtual texture is an invalid format\n");
			return false;
		}

		if(i > 0)
		{
			fileBufferLen = f[i]->Length64();

			game->UpdateLoadingString("Init Virtual Texture..");

			common->Printf("Init VT Heap %d\n", fileBufferLen);
			fileBuffer[i] = new byte[fileBufferLen];

			f[i]->Read( fileBuffer[i], fileBufferLen );

			fileSystem->CloseFile( f[i] );
			f[i] = NULL;
		}
	}


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
byte *bmVirtualTextureFile::ReadTile(  __int64 pageNum, __int64 tileNum, __int64 mipLevel, byte *tileBuffer ) {
	__int64 bufferpos = 0;
	
	if(mipLevel == 0)
	{
		bufferpos = sizeof(bmVirtualTextureHeader_t) ;
	}


	switch(mipLevel) {
		case 0:
			{
				bufferpos += (4096 * 4096) * pageNum;
				bufferpos += (VIRTUALTEXTURE_TILEMEMSIZE * (tileNum));
			}
			break;

		case 1:
			bufferpos += ((2048 * 2048) * pageNum);
			bufferpos += ((128 * 128) * (tileNum));
			memcpy( tileBuffer, fileBuffer[mipLevel] + bufferpos, 128 * 128 );
			break;

		case 2:
			bufferpos += ((1024 * 1024) * pageNum);
			bufferpos += ((64 * 64) * (tileNum));
			memcpy( tileBuffer, fileBuffer[mipLevel] + bufferpos, 64 * 64 );
			break;
	}
	
	if(mipLevel > 0) {
		
	//	return fileBuffer[mipLevel] + bufferpos;
		return NULL;
	}

	
	assert(tileBuffer != NULL);
	if(bufferpos >= f[mipLevel]->Length64()) {
		common->FatalError( "VT_ReadTile: %d > %d\n", bufferpos, f[mipLevel]->Length());
	}

	f[mipLevel]->Seek64(bufferpos, FS_SEEK_SET);

	switch( mipLevel)
	{
		case 0:
			{
				int len = f[mipLevel]->Read( tileBuffer, VIRTUALTEXTURE_TILEMEMSIZE );
				if(len < VIRTUALTEXTURE_TILEMEMSIZE) {
					common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
				}
			}
		break;
		case 1:
			{
				int len = f[mipLevel]->Read( tileBuffer, 128 * 128 );
				if(len < 128 * 128) {
					common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
				}
			}
		break;

		case 2:
			{
				int len = f[mipLevel]->Read( tileBuffer, 64 * 64 );
				if(len < 64 * 64) {
					common->FatalError( "VT_ReadTile: read < VIRTUALTEXTURE_TILEMEMSIZE" );
				}
			}
		break;
	}

	return NULL;
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