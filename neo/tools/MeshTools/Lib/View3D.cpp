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


//  Set of functions to deal with input and rendering of the 3D viewport.
//	Not entirely happy with this as almost all the functionality of ViewUV is the same, I should have used inheritance better and duplicated less code.

//  Oh well, I'll fix this up for RoadKill 1.2



#include "../LSCM/MeshBuilderStdafx.h"
#include "View3D.h"
#include "Matrix.h"
#include "Undo.h"
#include "../LSCM/EdgeSelect.h"
#include "Font.h"

extern bool gSingleSelect;


View3D* View3D::Get()
{
	static View3D* pView3D = NULL;

	if(pView3D == NULL)
	{
		pView3D = new View3D;
	}

	return(pView3D);
}



void View3D::Initialise()
{
	Vector Scale;
	Scale.SetVector(1.0f, 1.0f, 1.0f, 1.0f);

	m_CameraMatrix.SetRotLocSca(&m_CameraRotation, &m_CameraLocation, &Scale);
	m_AxisMatrix.SetRotLocSca(&m_CameraRotation, &m_CameraLocation, &Scale);
	m_ObjectTrans.SetVector(0,0,0,0);
}


void View3D::StartRendering()
{
	Vector AxisRot;
	AxisRot.SetVector(-m_CameraRotation.X, m_CameraRotation.Y, m_CameraRotation.Z, 1.0f);

	m_CameraMatrix.SetRollingMatrix(&m_CameraRotation, &m_CameraLocation);
	m_AxisMatrix.SetRollingMatrix(&AxisRot, &m_CameraLocation);


	Matrix Bollocks, TransMat;
	Vector Trans, Rot, Scale;
	Scale.SetVector(1.0f, 1.0f, 1.0f, 1.0f);

	Vector NewTrans;
	NewTrans.SetVector(m_ObjectTrans.X, -m_ObjectTrans.Y, -m_ObjectTrans.Z, 1.0f);

	Rot.SetVector(0,0,0,0);
	TransMat.SetRotLocSca(&Rot, &NewTrans, &Scale);			// m_ObjectTrans

	Bollocks.MultiplyMatrix(&TransMat, &m_CameraMatrix);		//  I think I'd had enough of my awful matrix functions that day!!  :^)

	m_CameraMatrix = Bollocks;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_CameraMatrix.M);


	if(m_CurrentMode == RK_MOVE_CENTREPOINT)						// only draw the centre point when the move button is held
	{
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(2.0f, 2.0f, 2.0f);
		glVertex3f(-m_ObjectTrans.X - 1.0f, m_ObjectTrans.Y, m_ObjectTrans.Z);
		glVertex3f(-m_ObjectTrans.X + 1.0f, m_ObjectTrans.Y, m_ObjectTrans.Z);

		glVertex3f(-m_ObjectTrans.X, m_ObjectTrans.Y - 1.0f, m_ObjectTrans.Z);
		glVertex3f(-m_ObjectTrans.X, m_ObjectTrans.Y + 1.0f, m_ObjectTrans.Z);

		glVertex3f(-m_ObjectTrans.X, m_ObjectTrans.Y, m_ObjectTrans.Z - 1.0f);
		glVertex3f(-m_ObjectTrans.X, m_ObjectTrans.Y, m_ObjectTrans.Z + 1.0f);
		glEnd();

		glLineWidth(1.0f);
	}


	glEnable(GL_LIGHTING);

}


void View3D::EndRendering()
{
	if(m_SelectionBoxActive == true) DrawDragBox();
	DrawHelpText();
}


void View3D::DrawDragBox()
{
	RECT ScreenRect = View3D::Get()->GetViewDimensions();
	float ScreenCentreX = (float)(ScreenRect.right - ScreenRect.left) / 2.0f;
	float ScreenCentreY = (float)(ScreenRect.bottom - ScreenRect.top) / 2.0f;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glLineWidth(1.0f);
	glBegin(GL_QUADS);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glVertex4f((m_SelectionBox.left-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.top-ScreenCentreY)/-ScreenCentreY,0.0f, 1.0f);
	glVertex4f((m_SelectionBox.right-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.top-ScreenCentreY)/-ScreenCentreY,0.0f, 1.0f);
	glVertex4f((m_SelectionBox.right-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.bottom-ScreenCentreY)/-ScreenCentreY,0.0f, 1.0f);
	glVertex4f((m_SelectionBox.left-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.bottom-ScreenCentreY)/-ScreenCentreY, 0.0f, 1.0f);
	glEnd();

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void View3D::DrawHelpText()
{
	RECT ScreenRect = View3D::Get()->GetViewDimensions();
	float ScreenCentreX = (float)(ScreenRect.right - ScreenRect.left) / 2.0f;
	float ScreenCentreY = (float)(ScreenRect.bottom - ScreenRect.top) / 2.0f;


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.01f);

	Font::Get()->PrintString(ModeText, 10,(ScreenCentreY*2)-20,0, 0xffffff, ScreenCentreX, ScreenCentreY);


	glDisable(GL_ALPHA_TEST);		//GL_BLEND);
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



void View3D::ChangeRotation(int DeltaX, int DeltaY)
{
	float ZRot = (float)DeltaX * (3.1412f/10.0f);
	float XRot = (float)DeltaY * (3.1412f/10.0f);

	m_CameraRotation.X += XRot;
	m_CameraRotation.Y += ZRot;

	if(m_CameraRotation.X > 90.0f) m_CameraRotation.X = 90.0f;
	if(m_CameraRotation.X < -90.0f) m_CameraRotation.X = -90.0f;
}


void View3D::ChangeTranslationXY(int DeltaX, int DeltaY)
{
	float ScreenWidth = (float)(m_ViewDimensions.right - m_ViewDimensions.left);
	float ScreenHeight = (float)(m_ViewDimensions.bottom - m_ViewDimensions.top);
	float Scalar = m_CameraLocation.Z;

	if(Scalar > -1.0f && Scalar < 1.0f) Scalar = -1.0f;
	if(Scalar > -1.0f) Scalar = -Scalar;

	float MoveX = (float)(DeltaX) * 1.15f * Scalar * 1/ScreenWidth;
	float MoveY = (float)(DeltaY) * 1.15f * Scalar * 1/ScreenWidth;

	m_CameraLocation.X -= MoveX;
	m_CameraLocation.Y += MoveY;
}


void View3D::ChangeTranslationZ(int DeltaX, int DeltaY)
{
	int absDeltaX, absDeltaY;

	absDeltaX = abs(DeltaX);
	absDeltaY = abs(DeltaY);

	if(absDeltaX > absDeltaY)
	{
		m_CameraLocation.Z += DeltaX * 0.01f;
	}
	else
	{
		m_CameraLocation.Z += DeltaY * 0.01f;
	}
}



// If we have a marquee (single click creates a small marquee) Select.... selectes edges/uv based on current mode..
// While you're holding and dragging about the mouse I'm setting and unsetting NEWSELECTED and NEWNOTSELECTED flags.
// When we Finalise the NEWSELECTED and NEWNOTSELECTED flags are logically combined with the last set of selections.
// After finalising all edge and UV flags are either SELECTED or NOTSELECTED

void View3D::HandleSelections()
{
	bool AnyChanges = false;

	if(m_SelectionBoxActive == true)
	{
		if(m_CurrentMode == RK_EDGE_SELECT) RKMesh::Get()->SelectFaceEdges(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, gSingleSelect, false, false);
		if(m_CurrentMode == RK_EDGE_TOPOLOGICAL) RKMesh::Get()->SelectFaceEdges(&m_CameraMatrix, m_SelectionBox, true, m_CTRLHeld, true, false, true);
		if(m_CurrentMode == RK_UV_SELECT) RKMesh::Get()->SelectFaceUVs(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, false);
	}

	if(DoFinalise() == true)
	{
		if(m_CurrentMode == RK_EDGE_SELECT) AnyChanges = RKMesh::Get()->FinaliseEdgeSelection();
		if(m_CurrentMode == RK_EDGE_TOPOLOGICAL) AnyChanges = RKMesh::Get()->FinaliseClosestSelection(true);			//m_SHIFTHeld);
		if(m_CurrentMode == RK_EDGE_LOOP) { AnyChanges = RKMesh::Get()->FinaliseLoopSelection(m_SHIFTHeld); m_CurrentMode = RK_EDGE_SELECT; }
		if(m_CurrentMode == RK_UV_SELECT) AnyChanges = RKMesh::Get()->FinaliseUVSelection();
		if(m_CurrentMode == RK_UV_SHELL) { AnyChanges = RKMesh::Get()->FinaliseShellSelection(m_SHIFTHeld); m_CurrentMode = RK_UV_SELECT; }
	}

	if(AnyChanges == true)
	{	
		UndoList::Get()->CopyMesh();
	}
}



bool View3D::HandleInput(int MouseState, int MouseX, int MouseY, int LastMouseX, int LastMouseY)
{
	bool UpdateWindow = false;
	static bool Rotating = false;

	if(MouseState == RK_KEY_DOWN) return(false);
	if(MouseState == RK_KEY_UP) return(false);

	m_LastMouseX = LastMouseX;
	m_LastMouseY = LastMouseY;

	UpdateWindow = NewMouseHandler();

/*
	if(MouseState == RK_MOUSE_LB_DOWN)	m_MouseLeftButtonHeld = true;
	if(MouseState == RK_MOUSE_LB_UP) m_MouseLeftButtonHeld = false;

	if(MouseState == RK_MOUSE_MB_DOWN) m_MouseMiddleButtonHeld = true;
	if(MouseState == RK_MOUSE_MB_UP) m_MouseMiddleButtonHeld = false;

	if(MouseState == RK_MOUSE_RB_DOWN) m_MouseRightButtonHeld = true;
	if(MouseState == RK_MOUSE_RB_UP) m_MouseRightButtonHeld = false;
*/
	if(MouseState == RK_MOUSE_LB_DBLCLK && m_CurrentMode == RK_EDGE_SELECT && Rotating == false) return(SelectEdgeLoop(MouseX, MouseY));
	if(MouseState == RK_MOUSE_LB_DBLCLK && m_CurrentMode == RK_UV_SELECT && Rotating == false) return(SelectShell(MouseX, MouseY));


	if(m_AltKey)			// some windows code leaked through to here..  sorry
	{
		if(Rotating == false)
		{
			if(m_CurrentMode == RK_EDGE_SELECT || m_CurrentMode == RK_UV_SELECT || m_CurrentMode == RK_EDGE_TOPOLOGICAL || m_CurrentMode == RK_EDGE_LOOP || m_CurrentMode == RK_UV_SHELL) 
			{
				if(m_SelectionBoxActive == true)
				{
					m_SelectionBoxActive = false;
					m_SelectionBox.left = -1;
					m_SelectionBox.right = -1;
					m_SelectionBox.top = -1;
					m_SelectionBox.bottom = -1;
					UpdateWindow = true;
					m_FinaliseSelection = true;
				}
			}
		}

		UpdateWindow |= MouseRotate(MouseState, MouseX, MouseY);
		m_SelectionBox.left = MouseX - 3;
		m_SelectionBox.right = MouseX + 3;
		m_SelectionBox.top = MouseY - 3;
		m_SelectionBox.bottom = MouseY + 3;
		Rotating = true;
	}
	else
	{
		if(m_CurrentMode == RK_EDGE_SELECT || m_CurrentMode == RK_UV_SELECT || m_CurrentMode == RK_EDGE_TOPOLOGICAL || m_CurrentMode == RK_EDGE_LOOP || m_CurrentMode == RK_UV_SHELL)
		{
			UpdateWindow |= MouseEdgeSelect(MouseState, MouseX, MouseY);
		}

		if(m_CurrentMode == RK_MOVE_CENTREPOINT)
		{
			UpdateWindow |= MoveCentrePoint(MouseState, MouseX, MouseY);
		}

		Rotating = false;
	}

	return(UpdateWindow);						// update window
}



bool View3D::MouseRotate(int MouseState, int MouseX, int MouseY)
{
	int DeltaX = MouseX - m_LastMouseX;
	int DeltaY = MouseY - m_LastMouseY;


	if(MouseState == RK_MOUSE_MOVE && m_MouseLeftButtonHeld == true)
	{
		ChangeRotation(DeltaX, DeltaY);
		return(true);
	}

	if(MouseState == RK_MOUSE_MOVE && m_MouseMiddleButtonHeld == true)
	{
		ChangeTranslationXY(DeltaX, DeltaY);
		return(true);
	}

	if(MouseState == RK_MOUSE_MOVE && m_MouseRightButtonHeld == true)
	{
		ChangeTranslationZ(DeltaX, DeltaY);
		return(true);
	}

	return(false);
}



bool View3D::MoveCentrePoint(int MouseState, int MouseX, int MouseY)
{
	Vector MoveXIn, MoveZIn;
	Vector MoveXOut, MoveZOut;
	float DeltaX = (float)(MouseX - m_LastMouseX);
	float DeltaY = (float)(MouseY - m_LastMouseY);

	float ScreenWidth = (float)(m_ViewDimensions.right - m_ViewDimensions.left);
	float ScreenHeight = (float)(m_ViewDimensions.bottom - m_ViewDimensions.top);
	float Scalar = m_CameraLocation.Z;

	if(Scalar > -1.0f && Scalar < 1.0f) Scalar = -1.0f;
	if(Scalar > -1.0f) Scalar = -Scalar;

	float MoveX = -(DeltaX) * 1.15f * Scalar * 1/ScreenWidth;
	float MoveY = -(DeltaY) * 1.15f * Scalar * 1/ScreenWidth;


	MoveXIn.SetVector(0.0f, 0.0f, 0.0f, 1.0f);
	MoveZIn.SetVector(0.0f, 0.0f, 0.0f, 1.0f);

	if(MouseState == RK_MOUSE_MOVE && m_MouseLeftButtonHeld == true)
	{
		Matrix RotMatrix, RotMatrixOut;

		Vector Rot, Trans;

		Rot.SetVector(m_CameraRotation.X, -m_CameraRotation.Y, m_CameraRotation.Z, 1.0f);
		Trans.SetVector(0, 0, 0, 1.0f);
		RotMatrix.SetRollingMatrix(&Rot, &Trans);
		RotMatrix.Inv(&RotMatrixOut);

		MoveXIn.SetVector(MoveX, MoveY, 0.0f, 1.0f);
		RotMatrixOut.RotateVector(&MoveXIn, &MoveXOut);
		RotMatrix.RotateVector(&MoveXIn, &MoveZOut);

		m_ObjectTrans.X += MoveXOut.X;
		m_ObjectTrans.Y += MoveXOut.Y;
		m_ObjectTrans.Z += MoveXOut.Z;

		return(true);
	}


	if(MouseState == RK_MOUSE_MOVE && m_MouseRightButtonHeld == true)
	{
		int absDeltaX, absDeltaY;
		float MoveZ = 0;

		absDeltaX = (int)abs(DeltaX);
		absDeltaY = (int)abs(DeltaY);

		if(absDeltaX > absDeltaY)
		{
			MoveZ = DeltaX * 0.01f;
		}
		else
		{
			MoveZ = DeltaY * 0.01f;
		}


		Matrix RotMatrix, RotMatrixOut;

		Vector Rot, Trans;

		Rot.SetVector(m_CameraRotation.X, -m_CameraRotation.Y, m_CameraRotation.Z, 1.0f);
		Trans.SetVector(0, 0, 0, 1.0f);
		RotMatrix.SetRollingMatrix(&Rot, &Trans);
		RotMatrix.Inv(&RotMatrixOut);

		MoveXIn.SetVector(0.0f, 0.0f, -MoveZ, 1.0f);
		RotMatrixOut.RotateVector(&MoveXIn, &MoveXOut);

		m_ObjectTrans.X += MoveXOut.X;
		m_ObjectTrans.Y += MoveXOut.Y;
		m_ObjectTrans.Z += MoveXOut.Z;

		return(true);
	}

	return(false);
}



bool View3D::CentreRoundSelectedEdges()
{
	float CentreX, CentreY, CentreZ;

	RKMesh::Get()->FindCentreOfEdges(&CentreX, &CentreY, &CentreZ);

	m_ObjectTrans.X = -CentreX;
	m_ObjectTrans.Y = CentreY;
	m_ObjectTrans.Z = CentreZ;

	return(true);
}



bool View3D::SelectEdgeLoop(int MouseX, int MouseY)
{
	if(m_CurrentMode != RK_EDGE_SELECT) return(false);

	m_SelectionBoxActive = true;
	m_SelectionBox.left = MouseX - 3;
	m_SelectionBox.right = MouseX + 3;
	m_SelectionBox.top = MouseY - 3;
	m_SelectionBox.bottom = MouseY + 3;

	RKMesh::Get()->SelectFaceEdges(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, true, true, false);
	m_FinaliseSelection = true;
	m_CurrentMode = RK_EDGE_LOOP;
	m_SelectionBoxActive = false;

	return(false);
}


bool View3D::SelectShell(int MouseX, int MouseY)
{
	m_SelectionBoxActive = true;
	m_SelectionBox.left = MouseX - 3;
	m_SelectionBox.right = MouseX + 3;
	m_SelectionBox.top = MouseY - 3;
	m_SelectionBox.bottom = MouseY + 3;

	RKMesh::Get()->SelectFaceUVs(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, true);
	m_FinaliseSelection = true;
	m_CurrentMode = RK_UV_SHELL;
	m_SelectionBoxActive = false;

	return(false);
}



bool View3D::MouseEdgeSelect(int MouseState, int MouseX, int MouseY)
{
	bool UpdateViewport = false;

	if(m_SelectionBoxActive == false && m_MouseLeftButtonHeld == true)					//MouseState == RK_MOUSE_LB_DOWN)
	{
		m_SelectionBoxActive = true;
		m_SelectionBox.left = MouseX - 3;
		m_SelectionBox.right = MouseX + 3;
		m_SelectionBox.top = MouseY - 3;
		m_SelectionBox.bottom = MouseY + 3;
		UpdateViewport = true;
		m_FinaliseSelection = false;
	}

	if(m_SelectionBoxActive == true && m_MouseLeftButtonHeld == false)					//MouseState == RK_MOUSE_LB_UP)
	{
		m_SelectionBoxActive = false;
		m_SelectionBox.left = -1;
		m_SelectionBox.right = -1;
		m_SelectionBox.top = -1;
		m_SelectionBox.bottom = -1;
		UpdateViewport = true;
		m_FinaliseSelection = true;
	}

	if(MouseState == RK_MOUSE_MOVE && m_MouseLeftButtonHeld == true)
	{
		m_SelectionBox.right = MouseX + 2;
		m_SelectionBox.bottom = MouseY + 2;
		UpdateViewport = true;
	}

	HandleSelections();

	return(UpdateViewport);
}



void View3D::SetMode(int ModeIn)
{
	if(ModeIn == m_CurrentMode) return;			// bastard windows re-sending keydown!

	if(ModeIn == RK_SHIFT_HELD) { m_SHIFTHeld = true; return; }
	if(ModeIn == RK_CTRL_HELD) { m_CTRLHeld = true; return; }

	if(ModeIn == RK_EDGE_SELECT) strcpy(ModeText, "Mode: Edge Select");
	if(ModeIn == RK_EDGE_TOPOLOGICAL) strcpy(ModeText, "Mode: Edge Topological");

	if(ModeIn == RK_UV_SELECT) strcpy(ModeText, "Mode: UV Select");
	if(ModeIn == RK_UV_TRANSLATE) strcpy(ModeText, "Mode: UV Move");
	if(ModeIn == RK_UV_STRETCH) strcpy(ModeText, "Mode: UV Stretch");

	if(ModeIn == RK_MOVE_CENTREPOINT) m_BackupMode = m_CurrentMode;

	m_CurrentMode = ModeIn;
}



bool View3D::UnSetMode(int ModeIn)
{
	bool UpdateViewport = false;

	if(ModeIn == RK_SHIFT_HELD) 
	{ 
		m_SHIFTHeld = false;
		return(UpdateViewport); 
	}

	if(ModeIn == RK_CTRL_HELD) 
	{ 
		m_CTRLHeld = false;
		return(UpdateViewport); 
	}

	if(ModeIn == RK_EDGE_TOPOLOGICAL) 
	{
		strcpy(ModeText, "Mode: Edge Select");
		m_CurrentMode = RK_EDGE_SELECT;
		return(UpdateViewport);
	}

	if(ModeIn == RK_MOVE_CENTREPOINT)
	{
		SetMode(m_BackupMode);
		return(true);
	}

	m_CurrentMode = RK_NO_SELECT;
	return(UpdateViewport); 
}



void View3D::AllKeysReleased()				// this gest called when we lose the windows focus
{
	m_MouseLeftButtonHeld = false;
	m_MouseMiddleButtonHeld = false;
	m_MouseRightButtonHeld = false;

	m_SHIFTHeld = false;
	m_CTRLHeld = false;

	if(m_CurrentMode == RK_EDGE_TOPOLOGICAL) m_CurrentMode = RK_EDGE_SELECT;
	if(m_CurrentMode == RK_MOVE_CENTREPOINT) SetMode(m_BackupMode);
}




bool View3D::NewMouseHandler()
{
	bool UpdateWindow = false;

	if(GetAsyncKeyState(VK_LBUTTON))
	{
		m_MouseLeftButtonHeld = true;
	}
	else
	{
		UpdateWindow = true;
		m_MouseLeftButtonHeld = false;
	}


	if(GetAsyncKeyState(VK_MBUTTON))
	{
		m_MouseMiddleButtonHeld = true;
	}
	else
	{
		m_MouseMiddleButtonHeld = false;
	}


	if(GetAsyncKeyState(VK_RBUTTON))
	{
		m_MouseRightButtonHeld = true;
	}
	else
	{
		m_MouseRightButtonHeld = false;
	}

	return(UpdateWindow);
}