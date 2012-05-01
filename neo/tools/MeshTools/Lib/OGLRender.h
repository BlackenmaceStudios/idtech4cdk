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


#ifndef OGLRENDER_INCLUDED
#define OGLRENDER_INCLUDED
/* *************************************************************************************************************************************
	OGL rendering class
************************************************************************************************************************************** */
#ifdef NOISY_COMPILATION
#pragma message("Entering OGL Render.h")
#endif


/* *************************************************************************************************************************************
	Headers	
************************************************************************************************************************************** */
#include "../LSCM/MeshBuilderStdafx.h"
#include "Matrix.h"

#include <math.h>
#include "gl\gl.h"
#include "gl\glu.h"


/* *************************************************************************************************************************************
	Class definitions (virtual class)
************************************************************************************************************************************** */
class OGLRender
{
private:
	HGLRC	m_hRC;													// Rendering Context
	HDC		m_hDC;													// Device Context

	HGLRC	FirstContext;
	HGLRC	SecondContext;


	OGLRender() 
	{
		FirstContext = NULL;
		SecondContext = NULL;
		m_Initialised = false;
	};

	~OGLRender()
	{
		if(SecondContext == m_hRC)
		{
			wglDeleteContext(m_hRC);
		}
		else
		{
			wglDeleteContext(m_hRC);
			FirstContext = SecondContext;
		}
	}

public:

	static OGLRender* Get();	// a singleton

	bool m_Initialised;

	void EnableContext( void )
	{
		wglMakeCurrent(m_hDC, m_hRC);								// Make the rendering context current
	}

	void DisableContext( void )
	{
		wglMakeCurrent(m_hDC, NULL);								// Allow other rendering contexts to co-exist
	}

	/* General */
	void SetTexturing( void );
	void SetFlatShaded( void );
	void StartRender( void );
	void EndRender( void );
	void SetBackgroundColour(float r, float g, float b, float a);
	void DeleteRenderer( HWND hWnd );
	void SetUpOGL( HWND hWnd );
	void ReSizeWindow( int Left, int Right, int Top, int Bottom, float near, float far );
	void GenerateCheckerTexture();
	void GenerateFlatTexture();

	GLuint LoadTexture(unsigned char* pBinaryData);


	Matrix ProjMatrix;
	int ScreenWidth;
	int ScreenHeight;

	Matrix InternalProjMatrix;
	int InternalScreenWidth;
	int InternalScreenHeight;

	void SetWindow( int Left, int Right, int Top, int Bottom , float near, float far);
	void SetDefaultRenderContext( void );


	void SetChecker();
	void SetColourTexture();
	void SetFlat();


	float BackgroundRed;
	float BackgroundGreen;
	float BackgroundBlue;

	GLuint m_GLTextureID;

	GLuint m_Checker;
	GLuint m_JustinID;
	GLuint m_FlatID;
};


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
#ifdef NOISY_COMPILATION
#pragma message("Leaving OGL Render.h")
#endif
#endif //OGLRENDER_INCLUDED
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */	

