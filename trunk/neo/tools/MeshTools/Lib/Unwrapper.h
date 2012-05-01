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


#ifndef UNWRAPPER
#define UNWRAPPER

#include "../LSCM/MeshBuilderStdafx.h"
#include "View3D.h"
#include "ViewUV.h"
#include "RKMesh.h"
#include "OBJLoader.h"
#include "Undo.h"



#define VIEWNONE 0
#define DVIEW3D 1
#define DVIEWUV 2
#define CENTREBAR 3


class Unwrapper
{
public:

	static Unwrapper* Get();							// Unwrapper is a singleton

	~Unwrapper()
	{
		if(m_pOBJLoader != NULL) delete m_pOBJLoader;
		m_pOBJLoader = NULL;
	};

	bool Initialise(char* FileName);
	void Render();
	void ResizeViewport(RECT ViewportDimensions);
	bool HandleInput(int MouseState, int MouseX, int MouseY);

	bool SaveOBJ(char* FileNameOut, char* FileNameIn) { return(m_pOBJLoader->Save(FileNameOut, FileNameIn)); }
	void AllKeysReleased();
	void Delete();

private:

	Unwrapper()
	{
		WhichWindow = VIEWNONE;
		DecideWhichWindow = false;
		m_pOBJLoader = NULL;
		m_CentreBarX = 0.5f;
	};

	void RenderMeshViewport();
	void RenderUVViewport();
	void RenderCentreBar();
	void RenderAxis(float ScreenCentreX, float ScreenCentreY);

	RECT m_ViewportDimensions;
	OBJLoader* m_pOBJLoader;				// The .OBJ loader

	int WhichWindow;
	bool DecideWhichWindow;
	float m_CentreBarX;


};



#endif		// UNWRAPPER