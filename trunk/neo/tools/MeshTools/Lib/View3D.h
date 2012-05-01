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


#ifndef VIEW3D
#define VIEW3D

#include "../LSCM/MeshBuilderStdafx.h"
#include "Matrix.h"
#include "MouseDefines.h"
#include "RKMesh.h"


class View3D
{
public:
	static View3D* Get();							// 3D View is a singleton
	void Initialise();

	void StartRendering();
	void EndRendering();

	void SetCameraLocation(Vector Location) { m_CameraLocation.SetVector(Location); }

	void ChangeRotation(int DeltaX, int DeltaY);
	void ChangeTranslationXY(int DeltaX, int DeltaY);
	void ChangeTranslationZ(int DeltaX, int DeltaY);

	bool HandleInput(int MouseState, int MouseX, int MouseY, int LastMouseX, int LastMouseY);
	void SetViewDimensions(RECT View) { m_ViewDimensions = View; }
	RECT GetViewDimensions() { return(m_ViewDimensions); }

	Matrix GetObjectMatrix() { return(m_CameraMatrix); }
	Matrix GetAxisMatrix() { return(m_AxisMatrix); }
	void DrawDragBox();

	void SetMode(int ModeIn);
	bool UnSetMode(int ModeIn);

	void HandleSelections();
	void AllKeysReleased();
	bool CentreRoundSelectedEdges();

	RECT GetSelection() {return(m_SelectionBox); }
	bool IsSelectionBoxActive() { return(m_SelectionBoxActive); }
	bool DoFinalise() 
	{
		bool ReturnVal = m_FinaliseSelection;
		m_FinaliseSelection = false;
		return(ReturnVal);
	}
	int m_CurrentMode;

	void SetALT(short ALTIn) { m_AltKey = ALTIn; }


private:
	View3D() 
	{
		m_CameraRotation.SetVector(0.0f, 0.0f, 0.0f, 1.0f);					// change later to more interesting set up angle
		m_CameraLocation.SetVector(0.0f, 0.0f, 0.0f, 1.0f);					// change later to calc extents of object

		Vector Scale;
		Scale.SetVector(1.0f, 1.0f, 1.0f, 1.0f);
		m_AxisMatrix.SetRotLocSca(&m_CameraRotation, &m_CameraLocation, &Scale);

		m_SelectionBoxActive = false;
		m_SelectionBox.left = 0;
		m_SelectionBox.right = 0;
		m_SelectionBox.top = 0;
		m_SelectionBox.bottom = 0;
		m_CurrentMode = RK_NO_SELECT;
		m_SHIFTHeld = false;
		m_CTRLHeld = false;
		ModeText[0] = 0;

		m_MouseLeftButtonHeld = false;
		m_MouseMiddleButtonHeld = false;
		m_MouseRightButtonHeld = false;
	};

	~View3D() {};

	bool MouseEdgeSelect(int MouseState, int MouseX, int MouseY);
	bool MouseRotate(int MouseState, int MouseX, int MouseY);
	bool SelectEdgeLoop(int MouseX, int MouseY);
	bool SelectShell(int MouseX, int MouseY);
	bool MoveCentrePoint(int MouseState, int MouseX, int MouseY);

	bool NewMouseHandler();


	void DrawHelpText();

	Vector m_CameraRotation;
	Vector m_CameraLocation;
	Matrix m_CameraMatrix;
	Matrix m_AxisMatrix;
	Vector m_ObjectTrans;

	int m_LastMouseX;
	int m_LastMouseY;
	bool m_MouseLeftButtonHeld;
	bool m_MouseMiddleButtonHeld;
	bool m_MouseRightButtonHeld;

	RECT m_ViewDimensions;

	bool m_SelectionBoxActive;
	RECT m_SelectionBox;
	bool m_FinaliseSelection;

	int m_BackupMode;

	bool m_SHIFTHeld;
	bool m_CTRLHeld;

	char ModeText[100];

	short m_AltKey;
};


#endif		// View_3D
