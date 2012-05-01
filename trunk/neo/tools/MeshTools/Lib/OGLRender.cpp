/*  
	Roadkill 1.1 UV Unwrap utility
    Copyright (C) 2007  Andy Swann

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


/* *************************************************************************************************************************************
	GL rendering wrapper

	More terrible 10 year old code..  so sorry.  Not quite as bad as Matrix, Vertex and Vector..
************************************************************************************************************************************** */

#include "../LSCM/MeshBuilderStdafx.h"

#include "OGLRender.h"
#include "GridBitmap.h"



OGLRender* OGLRender::Get()
{
	static OGLRender* pOGLRender = NULL;

	if(pOGLRender == NULL)
	{
		pOGLRender = new OGLRender;
	}

	return(pOGLRender);
}



/************************************************
	What colour do you want the background?		*
************************************************/
void OGLRender::SetBackgroundColour(float r, float g, float b, float a)
{
	BackgroundRed = r;
	BackgroundGreen = g;
	BackgroundBlue = b;

}

/****************************************************************************
	Start Rendering process
**************************************************************************** */
void OGLRender::StartRender( void )
{
	glClearColor( BackgroundRed, BackgroundGreen, BackgroundBlue, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);								// Clear the window with current clearing color

	ProjMatrix = InternalProjMatrix;
	ScreenWidth = InternalScreenWidth;
	ScreenHeight = InternalScreenHeight;
}


/****************************************************************************
	Finish rendering
**************************************************************************** */
void OGLRender::EndRender( void )
{	
	glFlush();																		// Ensure everything is processed now
	SwapBuffers(m_hDC);																// And have a look at our drawing
}

/****************************************************************************
	Set texturing
**************************************************************************** */
void OGLRender::SetTexturing( void )
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

/****************************************************************************
	No texturing
**************************************************************************** */
void OGLRender::SetFlatShaded( void )
{
	glDisable(GL_TEXTURE_2D);
}
/****************************************************************************
	Delete this rendering context etc.
**************************************************************************** */
void OGLRender::DeleteRenderer( HWND hWnd )
{
	wglDeleteContext( m_hRC );								// Clean up rendering context stuff
	::ReleaseDC( hWnd,m_hDC );
}

/****************************************************************************
	Window requires resizing -- entry point from windows message pump
**************************************************************************** */
void OGLRender::ReSizeWindow( int Left, int Right, int Top, int Bottom, float Near, float Far )
{
	wglMakeCurrent(m_hDC,m_hRC);
	SetWindow( Left, Right, Top, Bottom, Near, Far);
//	wglMakeCurrent(NULL,NULL);	
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	Hardware class
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/************************************
*	Set Viewport and Projection		*
*************************************/
void OGLRender::SetWindow( int Left, int Right, int Top, int Bottom, float Near, float Far)
{
	float Aspect;

	int Width = Right - Left;
	int Height = Bottom - Top;

	glViewport(Left, Top, Width, Height );							// Set Viewport to window dimensions
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	InternalProjMatrix.OGLProjMatrix(Width, Height, -Near, -Far, 60.0f );		// Set projection matrix
	InternalScreenWidth = Width;
	InternalScreenHeight = Height;

	Aspect = (GLfloat)Width/(GLfloat)Height;
//	gluPerspective(60.0f, Aspect, Near, Far);
//	gluOrtho2D(0, w, h, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(InternalProjMatrix.M);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();	// Set correct matrix mode

}



/****************************************************************************
	Set a default scene up.  For now
**************************************************************************** */
void OGLRender::SetDefaultRenderContext( void )
{
	float LocalView[] = {0.0};

	glEnable(GL_DEPTH_TEST);														// Hidden surface removal
	glDisable(GL_CULL_FACE);															// Do not calculate inside of solid object
//	glFrontFace(GL_CCW);															// All polys wound counter clockwise

	SetBackgroundColour(0.647f, 0.635f, 0.647f, 0.0f);									// Set background colour to !black
	glEnable( GL_LIGHTING );
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, LocalView );						// Looking down -z
	glEnable( GL_LIGHT1 );

//	glBlendFunc(GL_ONE, GL_SRC_ALPHA);
//	glEnable(GL_BLEND);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	

	float	Ambient[4];
	float	Diffuse[4];
	float	Specular[4];
	float	Position[4];
	float	Direction[4];

	Ambient[0]	= 0.20f;
	Ambient[1]	= 0.20f;
	Ambient[2]	= 0.20f;
	Ambient[3]	= 0;

	Diffuse[0]	= 0.50f;
	Diffuse[1]	= 0.50f;
	Diffuse[2]	= 0.50f;
	Diffuse[3]	= 0;

	Specular[0]	= 0.20f;
	Specular[1]	= 0.20f;
	Specular[2]	= 0.20f;
	Specular[3]	= 0;

	Position[0]	= 100.0f;
	Position[1]	= -100.0f;
	Position[2]	= 100.0f;
	Position[3]	= 1.0;	

	Direction[0] = 0.0f;
	Direction[1] = 1.0f;
	Direction[2] = 0.0f;
	Direction[3]	= 1.0;	
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, Ambient );
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Diffuse );
//	glLightfv(GL_LIGHT1, GL_SPECULAR, Specular );
	glLightfv(GL_LIGHT1, GL_POSITION, Position );

	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Direction );

	GenerateCheckerTexture();


	/* Set defualt ambient light */
//	float	Ambient[4] = { 0.5, 0.5, 0.5, 1.0 };
//	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, Ambient );
}



/****************************************************************************
	Set OGL up (for now)
**************************************************************************** */
void OGLRender::SetUpOGL( HWND hWnd )
{
	int nPixelFormat;					// Pixel format index
	m_hDC = ::GetDC(hWnd);											// Get the Device context
	m_Initialised = true;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),								// Size of this structure
		1,															// Version of this structure	
		PFD_DRAW_TO_WINDOW |										// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |										// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,											// Double buffered mode
		PFD_TYPE_RGBA,												// RGBA Color mode
		24,															// Want 24bit color 
		0,0,0,0,0,0,												// Not used to select mode
		0,0,														// Not used to select mode
		0,0,0,0,0,													// Not used to select mode
		32,															// Size of depth buffer
		0,															// Not used to select mode
		0,															// Not used to select mode
		PFD_MAIN_PLANE,												// Draw in main plane
		0,															// Not used to select mode
		0,0,0 };													// Not used to select mode

	
	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);					// Choose a pixel format that best matches that described in pfd
	SetPixelFormat(m_hDC, nPixelFormat, &pfd);						// Set the pixel format for the device context
	
	m_hRC = wglCreateContext(m_hDC);								// Create the rendering context

	wglMakeCurrent(m_hDC,m_hRC);									// Make the rendering context current, perform initialization, then deselect it
	SetDefaultRenderContext( );
	SetTexturing( );
	wglMakeCurrent(NULL,NULL);

	if(FirstContext == NULL)
	{
		FirstContext = m_hRC;
	}
	else
	{
		wglShareLists(FirstContext, m_hRC);
		SecondContext = m_hRC;
	}
}



void OGLRender::GenerateCheckerTexture()
{
	GLubyte* rgb;
	bool Black;

	GenerateFlatTexture();

	rgb = (GLubyte*)calloc((64 * 64) * 4, sizeof(GLubyte));			// make a 64 bit texture

	for(int YIndex = 0; YIndex < 64; YIndex++)
	{
		for(int XIndex = 0; XIndex < 64; XIndex++)
		{
			Black = (bool)((XIndex >> 2) & 1) ^ ((YIndex >> 2) & 1);

			int TexelIndex = (YIndex * 64 + XIndex) * 4;

			if(Black == true)
			{
				rgb[TexelIndex++] = 55;
				rgb[TexelIndex++] = 55;
				rgb[TexelIndex++] = 55;
				rgb[TexelIndex] = 0xff;
			}
			else
			{
				rgb[TexelIndex++] = 200;
				rgb[TexelIndex++] = 200;
				rgb[TexelIndex++] = 200;
				rgb[TexelIndex] = 0xff;
			}
		}
	}


	glGenTextures(1, &m_GLTextureID);
	glBindTexture(GL_TEXTURE_2D, m_GLTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb);

	free(rgb);

	m_Checker = m_GLTextureID;
	m_JustinID = LoadTexture(GridBitmapData);
}



void OGLRender::GenerateFlatTexture()
{
	GLubyte* rgb;

	rgb = (GLubyte*)calloc((64 * 64) * 4, sizeof(GLubyte));			// make a 64 bit texture

	for(int YIndex = 0; YIndex < 64; YIndex++)
	{
		for(int XIndex = 0; XIndex < 64; XIndex++)
		{
			int TexelIndex = (YIndex * 64 + XIndex) * 4;

			rgb[TexelIndex++] = 0xff;
			rgb[TexelIndex++] = 0xff;
			rgb[TexelIndex++] = 0xff;
			rgb[TexelIndex] = 0xff;
		}
	}


	glGenTextures(1, &m_GLTextureID);
	glBindTexture(GL_TEXTURE_2D, m_GLTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb);

	free(rgb);

	m_FlatID = m_GLTextureID;
}




GLuint OGLRender::LoadTexture(unsigned char* pBinaryData)
{
	int BPP, Mips;

	unsigned char* pCFontBitmap = pBinaryData;			//(unsigned char*)&FontBitmapData;
	unsigned int* pIFontBitmap = (unsigned int*)pBinaryData;

	unsigned char* pTexturePixels = NULL;
	unsigned int* pGL_Pixels = NULL;
	unsigned int* pTexturePalette = NULL;


	int TextureWidth = pIFontBitmap[0];
	int TextureHeight = pIFontBitmap[1];
	BPP = pIFontBitmap[2];
	Mips = pIFontBitmap[3];
	pIFontBitmap += 4;
	pCFontBitmap += 16;

	if(BPP == 8)
	{
		pTexturePalette = new unsigned int[256];
		for(int Index = 0; Index < 256; Index++) 
		{ 
			pTexturePalette[Index] = *pIFontBitmap;
			pIFontBitmap++;
			pCFontBitmap+=4; 
		}


	int PaletteIndex;
	int* pPalette = (int*)pTexturePalette;

	for(int Index = 0; Index < 8; Index++)
	{
		PaletteIndex = (Index * 32) + 8;

		for(int Index2 = 0; Index2 < 8; Index2++)
		{
			int Temp;
			Temp = pPalette[PaletteIndex];
			pPalette[PaletteIndex] = pPalette[PaletteIndex + 8];
			pPalette[PaletteIndex + 8] = Temp;
			PaletteIndex++;
		}
	}


		pTexturePixels = new unsigned char[TextureWidth * TextureHeight];
		for(int Index = 0; Index < TextureWidth * TextureHeight; Index++) 
		{ 
			pTexturePixels[Index] = *pCFontBitmap; 
			pCFontBitmap++;
		}


		pGL_Pixels = new unsigned int[TextureWidth * TextureHeight];

	
		for(int Height = 0; Height < TextureHeight; Height++)
		{
			for(int Width = 0; Width < TextureWidth; Width++)
			{
				unsigned char Pixel = pTexturePixels[(Height * TextureHeight) + Width];
				unsigned int Colour = pTexturePalette[Pixel];

				pGL_Pixels[((TextureHeight-1 - Height) * TextureHeight) + Width] = Colour;
			}
		}

	}

	GLuint TextureHandle;


	glGenTextures(1, &TextureHandle);
	glBindTexture(GL_TEXTURE_2D, TextureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pGL_Pixels);

	delete[] pTexturePixels;
	delete[] pGL_Pixels;
	delete[] pTexturePalette;

	return(TextureHandle);
}



void OGLRender::SetChecker()
{
	m_GLTextureID = m_Checker;
}


void OGLRender::SetColourTexture()
{
	m_GLTextureID = m_JustinID;
}


void OGLRender::SetFlat()
{
	m_GLTextureID = m_FlatID;
}
