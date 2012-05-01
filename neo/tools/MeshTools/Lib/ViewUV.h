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


#ifndef VIEWUV
#define VIEWUV

#include "../LSCM/MeshBuilderStdafx.h"
#include "Matrix.h"
#include "MouseDefines.h"
#include "RKMesh.h"


class ViewUV
{
public:
	static ViewUV* Get();							// UV View is a singleton
	void Initialise();

	void SetCameraLocation(Vector Location) { m_CameraLocation.SetVector(Location); }
	void ChangeTranslationXY(int DeltaX, int DeltaY);
	void ChangeTranslationZ(int DeltaX, int DeltaY);
	void StartRendering();
	void EndRendering();

	bool HandleInput(int MouseState, int MouseX, int MouseY, int LastMouseX, int LastMouseY);
	void SetViewDimensions(RECT View) { m_ViewDimensions = View; }
	RECT GetViewDimensions() { return(m_ViewDimensions); }

	Matrix GetObjectMatrix() { return(m_CameraMatrix); }
	void DrawDragBox();

	void SetMode(int ModeIn);
	bool UnSetMode(int ModeIn);

	void HandleSelections();
	void AllKeysReleased();

	RECT GetSelection() {return(m_SelectionBox); }
	bool IsSelectionBoxActive() { return(m_SelectionBoxActive); }
	bool DoFinalise() 
	{
		bool ReturnVal = m_FinaliseSelection;
		m_FinaliseSelection = false;
		return(ReturnVal);
	}

	int m_CurrentMode;

	int GetUVScale() { return(m_UVScale); }
	bool TextureScaleUp();
	bool TextureScaleDown();
	void SetALT(short ALTIn) { m_AltKey = ALTIn; }

private:
	ViewUV() 
	{
		m_CameraLocation.SetVector(-0.5f, -0.5f, -1.0f, 1.0f);
		m_CurrentMode = RK_NO_SELECT;
		m_SHIFTHeld = false;
		m_CTRLHeld = false;
		m_SelectionBoxActive = false;
		m_SelectionBox.left = 0;
		m_SelectionBox.right = 0;
		m_SelectionBox.top = 0;
		m_SelectionBox.bottom = 0;

		m_UVScale = 2;

		ModeText[0] = 0;
	};

	~ViewUV() {};

	void DrawGrid();
	bool MouseEdgeSelect(int MouseState, int MouseX, int MouseY);
	bool MouseRotate(int MouseState, int MouseX, int MouseY);
	bool SelectEdgeLoop(int MouseX, int MouseY);
	bool SelectShell(int MouseX, int MouseY);

	bool TranslateUVs(int DeltaX, int DeltaY);
	bool StretchUVs(int MouseX, int MouseY);
	bool RotateUVs(int MouseState, int MouseX, int MouseY);
	void DrawHelpText();

	void MoveCentrePoint(int MouseState, int MouseX, int MouseY);
	bool NewMouseHandler();


	Vector m_CameraLocation;
	Matrix m_CameraMatrix;

	RECT m_ViewDimensions;

	bool m_SelectionBoxActive;
	RECT m_SelectionBox;
	bool m_FinaliseSelection;


	bool m_SHIFTHeld;
	bool m_CTRLHeld;

	int m_LastMouseX;
	int m_LastMouseY;
	bool m_MouseLeftButtonHeld;
	bool m_MouseMiddleButtonHeld;
	bool m_MouseRightButtonHeld;

	int m_MouseOriginX;
	int m_MouseOriginY;

	int m_BackupMode;

	char ModeText[100];

	int m_UVScale;
	short m_AltKey;

};


#endif		// UV_VIEW
