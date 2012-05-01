// Image_fbo.cpp 
//

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"

/*
================
idImage::BindFBO
================
*/
void idImage::BindFBO( void ) {
	qglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboHandle);
}

/*
================
idImage::UnBindFBO
================
*/
void idImage::UnBindFBO( void ) {
	qglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

/*
================
idImage::GenerateFrameBufferImage
================
*/
void idImage::GenerateFrameBufferImage( int width, int height ) {
	PurgeImage();

	filter = TF_LINEAR;
	allowDownSize = false;
	repeat = TR_REPEAT;
	depth = TD_HIGH_QUALITY;

	uploadHeight = height;
	uploadWidth = width;
	type = TT_2D;

	if(qglGenTextures == NULL)
		return;

	// generate the texture number
	qglGenTextures( 1, &texnum );
	qglBindTexture(GL_TEXTURE_2D, texnum);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//NULL means reserve texture memory, but texels are undefined
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	qglGenFramebuffersEXT(1, &fboHandle);

	BindFBO();

	qglGenRenderbuffersEXT(1, &fboDepthBuffer);
	qglBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboDepthBuffer);
	qglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, width, height);
	qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboDepthBuffer);
	qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboDepthBuffer);

	

	// Attach the image to the fbo.
	qglGenRenderbuffersEXT(1, &fboColorBuffer);
	qglBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboColorBuffer);
    qglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, width, height);
    qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_RENDERBUFFER_EXT, fboColorBuffer);

	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texnum, 0);

	// Check the FBO status to ensure it got created properly.
	GLenum status = qglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		common->FatalError( "Failed to generate FBO image for %s\n", imgName);
	}
	UnBindFBO();
	qglBindTexture(GL_TEXTURE_2D, 0);
}

/*
================
idImage::GenerateFrameBufferDepthImage
================
*/
void idImage::GenerateFrameBufferDepthImage( int width, int height ) {
	PurgeImage();

	if(qglGenTextures == NULL)
		return;

	filter = TF_LINEAR;
	allowDownSize = false;
	repeat = TR_REPEAT;
	depth = TD_HIGH_QUALITY;

	uploadHeight = height;
	uploadWidth = width;
	type = TT_2D;
	qglGenFramebuffersEXT(1, &fboHandle);
	
	// Try to use a texture depth component
	qglGenTextures(1, &texnum);
	qglBindTexture(GL_TEXTURE_2D, texnum);
	
	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Remove artefact on the edges of the shadowmap
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	
	//glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
	
	
	
	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	qglTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	qglBindTexture(GL_TEXTURE_2D, 0);
	
	// create a framebuffer object
	qglGenFramebuffersEXT(1, &fboHandle);
	qglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboHandle);
	
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	qglDrawBuffer(GL_NONE);
	qglReadBuffer(GL_NONE);
	
	// attach the texture to FBO depth attachment point
	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, texnum, 0);
	
	// check FBO status
	GLenum status = qglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		common->FatalError( "Failed to generate FBO image for %s\n", imgName);
	}
	UnBindFBO();
	qglBindTexture(GL_TEXTURE_2D, 0);

}

/*
================
idImage::GenerateFrameBufferColorTargetFromFBO
================
*/
void idImage::GenerateFrameBufferColorTargetFromFBO( void ) {
	idStr parentImgName = imgName;

	int magicChr = parentImgName.Find( '/' );
	parentImgName[magicChr] = 0;

	if(qglGenTextures == NULL)
		return;

	idImage *parentImage = globalImages->GetImage( parentImgName.c_str() );
	if(parentImage == NULL) {
		common->FatalError( "GenerateFrameBufferColorTargetFromFBO: Can't find FBO %s to attach color target.\n",  parentImgName.c_str());
	}

	if(parentImage->fboHandle == -1) {
		common->FatalError( "GenerateFrameBufferColorTargetFromFBO: %s is not a fbo\n", parentImgName.c_str() );
	}

	PurgeImage();

	filter = TF_LINEAR;
	allowDownSize = false;
	repeat = TR_REPEAT;
	depth = TD_HIGH_QUALITY;

	uploadHeight = parentImage->uploadHeight;
	uploadWidth = parentImage->uploadWidth;
	type = TT_2D;

	// Bind the FBO
	parentImage->BindFBO();

	qglGenRenderbuffersEXT(1, &fboColorBuffer);
	qglBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboColorBuffer);
    
    

	// Create the texture.
	qglGenTextures(1, &texnum);
    qglBindTexture(GL_TEXTURE_2D, texnum);
	if(strstr( imgName.c_str(), "xyz")) {
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, uploadWidth, uploadHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		qglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA32F_ARB, uploadWidth, uploadHeight);
	}
	else if(strstr( imgName.c_str(), "normal")) {
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, uploadWidth, uploadHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		qglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA16F_ARB, uploadWidth, uploadHeight);
	}
	else {
		common->FatalError( "Illegal colortarget");
	}
    qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT + parentImage->numAdditionalColorTargets,GL_RENDERBUFFER_EXT, fboColorBuffer);

	// Attach the color target to the fbo.
    qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT + parentImage->numAdditionalColorTargets, GL_TEXTURE_2D, texnum, 0);

	// check FBO status
	GLenum status = qglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		common->FatalError( "Failed to attach FBO colortarget for %s\n", imgName);
	}

	// Unbind the FBO.
	parentImage->UnBindFBO();

	// Assign this image as a additional color target now that it's valid. 
	parentImage->fboColorTargets[parentImage->numAdditionalColorTargets++] = this;
}

/*
================
idImage::GenerateFrameBufferCubeImage
================
*/
void idImage::GenerateFrameBufferCubeImage( int width, int height ) {
	PurgeImage();

	if(qglGenTextures == NULL)
		return;

	filter = TF_LINEAR;
	allowDownSize = false;
	repeat = TR_REPEAT;
	depth = TD_HIGH_QUALITY;

	uploadHeight = width;
	uploadWidth = height;
	type = TT_CUBIC;

	// color cube map
	qglGenTextures(1, &texnum);
	qglBindTexture(GL_TEXTURE_CUBE_MAP, texnum);
	qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	qglTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	qglTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE );
	//qglTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
	//qglTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	for (uint face = 0; face < 6; face++) {
		qglTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	
	// framebuffer object
	qglGenFramebuffersEXT(1, &fboHandle);
	qglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboHandle);
	qglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA32F_ARB, uploadWidth, uploadHeight);
	
	qglGenRenderbuffersEXT(1, &fboDepthBuffer);
	qglBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboDepthBuffer);
	qglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,  GL_DEPTH_COMPONENT32 , uploadWidth, uploadHeight);
	qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboDepthBuffer);


	// "_px.tga", "_nx.tga", "_py.tga", "_ny.tga", 
		//"_pz.tga", "_nz.tga"
	

	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texnum, 0);
	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, texnum, 0);
	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texnum, 0);
	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, texnum, 0);
	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texnum, 0);
	qglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texnum, 0);

	qglDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	// Check the FBO status to ensure it got created properly.
	GLenum status = qglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		common->FatalError( "Failed to generate FBO image for %s\n", imgName);
	}

	UnBindFBO();
	qglBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

static void R_Image_FrameBufferCubeImage( idImage *image ) {
	image->GenerateFrameBufferImage( SCREEN_WIDTH, SCREEN_HEIGHT );
}

/*
=======================
R_CreateFrameBufferCubeImage
=======================
*/
idImage *R_CreateFrameBufferCubeImage( const char *name ) {
	return NULL;
}