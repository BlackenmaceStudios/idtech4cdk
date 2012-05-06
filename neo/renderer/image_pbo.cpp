// image_pbo.cpp
//

#include "precompiled.h"

#include "tr_local.h"

bmImagePBO::~bmImagePBO() {
	qglDeleteBuffersARB( 1, &pboReadbackHandle );
	qglDeleteBuffersARB( 2, &pboWriteHandle[0]);
}

/*
==============
bmImagePBO::CreatePBO
==============
*/
void idImage::CreatePBO( void ) {
	if(pbo != NULL) {
		delete pbo;
		pbo = NULL;
	}
	
	pbo = new bmImagePBO();
	if ( glConfig.isInitialized ) {
		pbo->Init( this );
	}
}

/*
==============
bmImagePBO::Init
==============
*/
void bmImagePBO::Init(idImage *img) {
	// Generate the pixel buffer object.
	qglGenBuffersARB( 1, &pboReadbackHandle);
	qglGenBuffersARB( 2, &pboWriteHandle[0]);
	

	qglBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboReadbackHandle);
    qglBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, img->uploadWidth * img->uploadHeight * 4, 0, GL_STREAM_READ_ARB);

	buffer = NULL;
	UnbindPBO();


	_img = img;
}
/*
==============
bmImagePBO::WriteToPBO
==============
*/
void bmImagePBO::WriteToPBO( int pbo, byte *buffer, int DestX, int DestY, int Width, int Height ) {
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboWriteHandle[pbo]);
	qglTexSubImage2D( GL_TEXTURE_2D, 0, DestX, DestY, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
}

/*
==============
bmImagePBO::ReadPBO
==============
*/
byte* bmImagePBO::ReadPBO( bool readBack ) {
	if(!readBack)
	{
		qglReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

		qglBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboReadbackHandle);

		qglReadPixels( 0, 0, _img->uploadWidth, _img->uploadHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		qglBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);

	}
	else
	{
		qglBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboReadbackHandle);
		buffer = (byte *)qglMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
	}
	
	return buffer;
}



/*
==============
bmImagePBO::UnbindPBO
==============
*/
void bmImagePBO::UnbindPBO( void ) {
	
	if(buffer != NULL)
	{
		qglUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB); 
		buffer = NULL;
	}
	qglBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
}