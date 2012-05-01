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


#include "../LSCM/MeshBuilderStdafx.h"
#include "ViewUV.h"
#include "Undo.h"
#include "Font.h"
#include "OGLRender.h"

extern bool gSingleSelect;


ViewUV* ViewUV::Get()
{
	static ViewUV* pViewUV = NULL;

	if(pViewUV == NULL)
	{
		pViewUV = new ViewUV;
	}

	return(pViewUV);
}



void ViewUV::Initialise()
{
	Vector Scale, Rot;
	Scale.SetVector(1.0f, 1.0f, 1.0f, 1.0f);
	Rot.SetVector(0.0f, 0.0f, 0.0f, 0.0f);

	m_CameraMatrix.SetRotLocSca(&Rot, &m_CameraLocation, &Scale);
}



void ViewUV::StartRendering()
{
	m_CameraMatrix.SetTranslation(&m_CameraLocation);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_CameraMatrix.M);

	glDisable(GL_LIGHTING);

	DrawGrid();

}


void ViewUV::EndRendering()
{
	if(m_SelectionBoxActive == true) DrawDragBox();
	DrawHelpText();
}


void ViewUV::DrawDragBox()
{
	RECT ScreenRect = ViewUV::Get()->GetViewDimensions();
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
	glBegin(GL_QUADS);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glVertex4f((m_SelectionBox.left-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.top-ScreenCentreY)/-ScreenCentreY,0.0f,1.0f);
	glVertex4f((m_SelectionBox.right-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.top-ScreenCentreY)/-ScreenCentreY,0.0f,1.0f);
	glVertex4f((m_SelectionBox.right-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.bottom-ScreenCentreY)/-ScreenCentreY,0.0f,1.0f);
	glVertex4f((m_SelectionBox.left-ScreenCentreX)/ScreenCentreX, (m_SelectionBox.bottom-ScreenCentreY)/-ScreenCentreY, 0.0f,1.0f);
	glEnd();
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



void ViewUV::DrawHelpText()
{
	RECT ScreenRect = GetViewDimensions();
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



void ViewUV::DrawGrid()
{
	float Ambient[3] = {0.2f, 0.2f, 0.2f};
	float Diffuse[3] = {0.7f, 0.7f, 0.7f};
	float Specular[3] = {0.3f, 0.3f, 0.3f};

	OGLRender::Get()->SetTexturing();
	glDisable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT,GL_AMBIENT, Ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, Diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR, Specular );
	glBindTexture(GL_TEXTURE_2D, OGLRender::Get()->m_GLTextureID);


	float Multiplier;

	if(m_UVScale == 1) Multiplier = 0.5f;
	if(m_UVScale == 2) Multiplier = 1.0f;
	if(m_UVScale == 3) Multiplier = 2.0f;
	if(m_UVScale == 4) Multiplier = 4.0f;

	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glBegin(GL_QUADS);

	for(int IndexY = -2; IndexY < 3; IndexY++)
	{
		for(int IndexX = -2; IndexX < 3; IndexX++)
		{
			glColor4f(0.7f, 0.7f, 0.7f, 0.2f);

			if(IndexX == 0 && IndexY == 0) 	glColor4f(0.7f, 0.7f, 0.7f, 2.0f);
	
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f((float)IndexX, (float)IndexY, -0.001f);

			glTexCoord2f(1.0f * Multiplier, 0.0f);
			glVertex3f((float)IndexX + 1.0f, (float)IndexY, -0.001f);

			glTexCoord2f(1.0f * Multiplier, 1.0f * Multiplier);
			glVertex3f((float)IndexX + 1.0f, (float)IndexY + 1.0f, -0.001f);

			glTexCoord2f(0.0f, 1.0f * Multiplier);
			glVertex3f((float)IndexX, (float)IndexY + 1.0f, -0.001f);
		}
	}

	glEnd();

	glDisable(GL_BLEND); 


	OGLRender::Get()->SetFlatShaded();

	glLineWidth(1.0f);
	glBegin(GL_LINES);

	float LinePos = -2.0f;

	int Fat = 0;

	for(int Index = 0; Index < 18+33; Index++)
	{
		if(Fat == 0)
		{
			glLineWidth(3.0f);
			glColor3f(0.0f, 0.2f, 0.7f);
		}
		else
		{
			glLineWidth(1.0f);
			glColor3f(0.5f, 0.5f, 0.5f);
		}

		Fat++;

		if(Fat == 10) Fat = 0;


		glVertex3f(LinePos, -2.0f, 0.00f);
		glVertex3f(LinePos, 3.0f, 0.00f);

		glVertex3f(-2.0f, LinePos, 0.00f);
		glVertex3f(3.0f, LinePos, 0.00f);

		LinePos += 0.1f;
	}
	glEnd();

	
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	float CentreX = RKMesh::Get()->m_UVRotateCentreX;
	float CentreY = RKMesh::Get()->m_UVRotateCentreY;
	
	glColor3f(2.0f, 2.0f, 0.0f);
	glVertex3f(CentreX-0.02f, CentreY, 0.01f);
	glVertex3f(CentreX+0.02f, CentreY, 0.01f);

	glVertex3f(CentreX, CentreY-0.02f, 0.01f);
	glVertex3f(CentreX, CentreY+0.02f, 0.01f);

	glEnd();
	glLineWidth(1.0f);
}



void ViewUV::ChangeTranslationXY(int DeltaX, int DeltaY)
{
	float ScreenWidth = (float)(m_ViewDimensions.right - m_ViewDimensions.left);
	float ScreenHeight = (float)(m_ViewDimensions.bottom - m_ViewDimensions.top);

	float MoveX = (float)(DeltaX) * 1.15f * m_CameraLocation.Z * 1/ScreenWidth;			// 1.15f
	float MoveY = (float)(DeltaY) * 1.15f * m_CameraLocation.Z * 1/ScreenWidth;

	m_CameraLocation.X -= MoveX;
	m_CameraLocation.Y += MoveY;
}



void ViewUV::ChangeTranslationZ(int DeltaX, int DeltaY)
{
	int absDeltaX, absDeltaY;

	absDeltaX = abs(DeltaX);
	absDeltaY = abs(DeltaY);

	if(absDeltaX > absDeltaY)
	{
		m_CameraLocation.Z += (float)(DeltaX) * 0.01f;
	}
	else
	{
		m_CameraLocation.Z += (float)(DeltaY) * 0.01f;
	}

	if(m_CameraLocation.Z > -0.01f) m_CameraLocation.Z = -0.01f;
}



bool ViewUV::TranslateUVs(int DeltaX, int DeltaY)
{
	if(DeltaX == 0 && DeltaY == 0) return(false);

	float ScreenWidth = (float)(m_ViewDimensions.right - m_ViewDimensions.left);
	float ScreenHeight = (float)(m_ViewDimensions.bottom - m_ViewDimensions.top);

	float MoveX = (float)(DeltaX) * 1.15f * m_CameraLocation.Z * 1/ScreenHeight;
	float MoveY = (float)(DeltaY) * 1.15f * m_CameraLocation.Z * 1/ScreenHeight;

	RKMesh::Get()->TranslateSelectedUVs(-MoveX, MoveY);

	return(true);
}



void ViewUV::HandleSelections()
{
	bool AnyChanges = false;

	if(m_SelectionBoxActive == true)
	{
		if(m_CurrentMode == RK_EDGE_SELECT) RKMesh::Get()->SelectUVEdges(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, gSingleSelect, false, false);
		if(m_CurrentMode == RK_EDGE_TOPOLOGICAL) RKMesh::Get()->SelectUVEdges(&m_CameraMatrix, m_SelectionBox, true, m_CTRLHeld, true, false, true);
//		if(m_CurrentMode == RK_EDGE_LOOP) RKMesh::Get()->SelectUVEdges(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, true);
		if(m_CurrentMode == RK_UV_SELECT) RKMesh::Get()->SelectUVUVs(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, false);
	}

	if(DoFinalise() == true)
	{
		if(m_CurrentMode == RK_EDGE_SELECT) AnyChanges = RKMesh::Get()->FinaliseEdgeSelection();
		if(m_CurrentMode == RK_EDGE_TOPOLOGICAL) AnyChanges = RKMesh::Get()->FinaliseClosestSelection(true);		//m_SHIFTHeld);
		if(m_CurrentMode == RK_EDGE_LOOP) { AnyChanges = RKMesh::Get()->FinaliseLoopSelection(m_SHIFTHeld); m_CurrentMode = RK_EDGE_SELECT; }
		if(m_CurrentMode == RK_UV_SELECT) AnyChanges = RKMesh::Get()->FinaliseUVSelection();
		if(m_CurrentMode == RK_UV_SHELL) { AnyChanges = RKMesh::Get()->FinaliseShellSelection(m_SHIFTHeld); m_CurrentMode = RK_UV_SELECT; }
	}

	if(AnyChanges == true)			// && m_SHIFTHeld == false) 
	{	
		UndoList::Get()->CopyMesh();
	}

}




bool ViewUV::HandleInput(int MouseState, int MouseX, int MouseY, int LastMouseX, int LastMouseY)
{
	bool UpdateWindow = false;
	static bool Stretch = false;
	static bool Rotating = false;
	static bool UVRotate = false;

	if(MouseState == RK_KEY_DOWN) return(false);
	if(MouseState == RK_KEY_UP) return(false);

	m_LastMouseX = LastMouseX;
	m_LastMouseY = LastMouseY;

	UpdateWindow = NewMouseHandler();


	if(MouseState == RK_MOUSE_LB_DBLCLK && m_CurrentMode == RK_EDGE_SELECT) return(SelectEdgeLoop(MouseX, MouseY));
	if(MouseState == RK_MOUSE_LB_DBLCLK && m_CurrentMode == RK_UV_SELECT) return(SelectShell(MouseX, MouseY));


	if(Rotating == false)
	{
		if(m_CurrentMode == RK_UV_STRETCH && m_MouseLeftButtonHeld == true && Stretch == false)
		{
			Stretch = RKMesh::Get()->StartLiveStretch(false, true);
			m_MouseOriginX = MouseX;
			m_MouseOriginY = MouseY;
		}

		if(m_CurrentMode == RK_UV_STRETCH && m_MouseLeftButtonHeld == false && Stretch == true) 
		{
			RKMesh::Get()->EndLiveStretch();
			UndoList::Get()->CopyMesh();
			Stretch = false;
		}
	}

/*
	if(MouseState == RK_MOUSE_LB_DOWN) m_MouseLeftButtonHeld = true;
	if(MouseState == RK_MOUSE_LB_UP) m_MouseLeftButtonHeld = false;

	if(MouseState == RK_MOUSE_MB_DOWN) m_MouseMiddleButtonHeld = true;
	if(MouseState == RK_MOUSE_MB_UP) m_MouseMiddleButtonHeld = false;

	if(MouseState == RK_MOUSE_RB_DOWN) m_MouseRightButtonHeld = true;
	if(MouseState == RK_MOUSE_RB_UP) m_MouseRightButtonHeld = false;
*/


	if(m_AltKey)
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

		if(m_CurrentMode == RK_UV_STRETCH && m_MouseLeftButtonHeld == true && Stretch == true)
		{
			UpdateWindow |= StretchUVs(MouseX, MouseY);
		}

		if(m_CurrentMode == RK_UV_ROTATE)
		{
			UpdateWindow |= RotateUVs(MouseState, MouseX, MouseY);
		}

		if(m_CurrentMode == RK_MOVE_CENTREPOINT)
		{
			MoveCentrePoint(MouseState, MouseX, MouseY);
			UpdateWindow = true;
		}


		Rotating = false;
	}

	return(UpdateWindow);						// update window
}


bool ViewUV::MouseRotate(int MouseState, int MouseX, int MouseY)
{
	if(MouseState == RK_MOUSE_MOVE && m_MouseLeftButtonHeld == true)
	{
		int DeltaX = MouseX - m_LastMouseX;
		int DeltaY= MouseY - m_LastMouseY;

		ChangeTranslationXY(DeltaX, DeltaY);
		return(true);
	}

	if(MouseState == RK_MOUSE_MOVE && m_MouseMiddleButtonHeld == true)
	{
		int DeltaX = MouseX - m_LastMouseX;
		int DeltaY= MouseY - m_LastMouseY;

		ChangeTranslationXY(DeltaX, DeltaY);
//		m_LastMouseX = MouseX;
//		m_LastMouseY = MouseY;
		return(true);
	}

	if(MouseState == RK_MOUSE_MOVE && m_MouseRightButtonHeld == true)
	{
		int DeltaX = MouseX - m_LastMouseX;
		int DeltaY = MouseY - m_LastMouseY;

		ChangeTranslationZ(DeltaX, DeltaY);
//		m_LastMouseX = MouseX;
//		m_LastMouseY = MouseY;
		return(true);

	}

	return(false);
}


bool ViewUV::SelectEdgeLoop(int MouseX, int MouseY)
{
	if(m_CurrentMode != RK_EDGE_SELECT) return(false);

	m_SelectionBoxActive = true;
	m_SelectionBox.left = MouseX - 3;
	m_SelectionBox.right = MouseX + 3;
	m_SelectionBox.top = MouseY - 3;
	m_SelectionBox.bottom = MouseY + 3;

	RKMesh::Get()->SelectUVEdges(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, true, true, false);
	m_FinaliseSelection = true;
	m_CurrentMode = RK_EDGE_LOOP;
	m_SelectionBoxActive = false;

	return(false);
}



bool ViewUV::SelectShell(int MouseX, int MouseY)
{
	m_SelectionBoxActive = true;
	m_SelectionBox.left = MouseX - 3;
	m_SelectionBox.right = MouseX + 3;
	m_SelectionBox.top = MouseY - 3;
	m_SelectionBox.bottom = MouseY + 3;

	RKMesh::Get()->SelectUVUVs(&m_CameraMatrix, m_SelectionBox, m_SHIFTHeld, m_CTRLHeld, true);
	m_FinaliseSelection = true;
	m_CurrentMode = RK_UV_SHELL;
	m_SelectionBoxActive = false;

	return(false);
}



bool ViewUV::MouseEdgeSelect(int MouseState, int MouseX, int MouseY)
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


bool ViewUV::StretchUVs(int MouseX, int MouseY)
{
	int Delta = (m_MouseOriginY - MouseY) >> 4;

	if(Delta < 0) Delta = 0;
	if(Delta > 10) Delta = 10;

	RKMesh::Get()->LiveStretch(Delta);

	return(true);
}



bool ViewUV::RotateUVs(int MouseState, int MouseX, int MouseY)
{
	bool UpdateViewport = false;
	int DeltaX = MouseX - m_LastMouseX;
	int DeltaY = MouseY - m_LastMouseY;
	static int function = 0;
	

//	if((MouseState == RK_MOUSE_LB_DOWN || MouseState == RK_MOUSE_MB_DOWN || MouseState == RK_MOUSE_RB_DOWN) && function == 0) 
	if((m_MouseLeftButtonHeld == true || m_MouseMiddleButtonHeld == true || m_MouseRightButtonHeld == true) && function == 0) 
	{
		RKMesh::Get()->StartLiveUnwrap(false, true);
	}

	if((MouseState == RK_MOUSE_MOVE && m_MouseLeftButtonHeld == true) && (function == 0 || function == 1))
	{
		RKMesh::Get()->RotateUVs(DeltaX, DeltaY);
		UpdateViewport = true;
		function = 1;
	}


	if((MouseState == RK_MOUSE_MOVE && m_MouseMiddleButtonHeld == true) && (function == 0 || function == 2))
	{
		UpdateViewport = TranslateUVs(MouseX - m_LastMouseX, MouseY - m_LastMouseY);
		function = 2;
	}


	if((MouseState == RK_MOUSE_MOVE && m_MouseRightButtonHeld == true) && (function == 0 || function == 3))
	{
		RKMesh::Get()->ScaleUVs(DeltaX, DeltaY);
		UpdateViewport = true;
		function = 3;
	}


	if(m_MouseLeftButtonHeld == false && function == 1)					//MouseState == RK_MOUSE_LB_UP && function == 1)
	{
		RKMesh::Get()->EndRotateUVs();
		function = 0;

		RKMesh::Get()->EndLiveUnwrap();
		UndoList::Get()->CopyMesh();
	}


	if(m_MouseMiddleButtonHeld == false && function == 2)					//MouseState == RK_MOUSE_MB_UP && function == 2)
	{
		function = 0;

		RKMesh::Get()->EndLiveUnwrap();
		UndoList::Get()->CopyMesh();
	}


	if(m_MouseRightButtonHeld == false && function == 3)					//MouseState == RK_MOUSE_RB_UP && function == 3)
	{
		RKMesh::Get()->EndRotateUVs();
		function = 0;

		RKMesh::Get()->EndLiveUnwrap();
		UndoList::Get()->CopyMesh();
	}


	return(UpdateViewport);
}



void ViewUV::SetMode(int ModeIn)
{
	if(ModeIn == m_CurrentMode) return;			// bastard windows re-sending keydown!

	if(ModeIn == RK_SHIFT_HELD) { m_SHIFTHeld = true; return; }
	if(ModeIn == RK_CTRL_HELD) { m_CTRLHeld = true; return; }

	if(ModeIn == RK_UV_ROTATE) RKMesh::Get()->StartUVRotate();

	if(ModeIn == RK_EDGE_SELECT) strcpy(ModeText, "Mode: Edge Select");
	if(ModeIn == RK_EDGE_TOPOLOGICAL) strcpy(ModeText, "Mode: Edge Topological");
//	if(ModeIn == RK_EDGE_LOOP) strcpy(ModeText, "Mode: Edge Loop");

	if(ModeIn == RK_UV_SELECT) strcpy(ModeText, "Mode: UV Select");
	if(ModeIn == RK_UV_ROTATE) strcpy(ModeText, "Mode: UV Move");
	if(ModeIn == RK_UV_STRETCH) strcpy(ModeText, "Mode: UV Stretch");

	if(ModeIn == RK_MOVE_CENTREPOINT) m_BackupMode = m_CurrentMode;

	m_CurrentMode = ModeIn;
}


bool ViewUV::UnSetMode(int ModeIn)
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

	if(ModeIn == RK_MOVE_CENTREPOINT)
	{
		if(m_BackupMode == RK_UV_ROTATE)
		{
			m_CurrentMode = m_BackupMode;
			strcpy(ModeText, "Mode: UV Move");
		}
		else
		{
			SetMode(m_BackupMode);
		}
//		strcpy(ModeText, "Mode: UV Move");
//		m_CurrentMode = RK_UV_ROTATE;
		return(true);
	}


//	if(ModeIn == RK_ROTATE) { m_CTRLHeld = false; return(UpdateViewport); }

	if(ModeIn == RK_EDGE_TOPOLOGICAL) 
	{
		strcpy(ModeText, "Mode: Edge Select");
		m_CurrentMode = RK_EDGE_SELECT;
		return(UpdateViewport);
	}

	m_CurrentMode = RK_NO_SELECT;
	return(UpdateViewport); 
}


void ViewUV::AllKeysReleased()
{
	m_MouseLeftButtonHeld = false;
	m_MouseMiddleButtonHeld = false;
	m_MouseRightButtonHeld = false;

	m_SHIFTHeld = false;
	m_CTRLHeld = false;

	if(m_CurrentMode == RK_EDGE_TOPOLOGICAL) m_CurrentMode = RK_EDGE_SELECT;
	if(m_CurrentMode == RK_MOVE_CENTREPOINT) SetMode(m_BackupMode);
}



void ViewUV::MoveCentrePoint(int MouseState, int MouseX, int MouseY)
{
	int DeltaX = MouseX - m_LastMouseX;
	int DeltaY = MouseY - m_LastMouseY;


	if(DeltaX == 0 && DeltaY == 0) return;

	if(MouseState == RK_MOUSE_MOVE && m_MouseLeftButtonHeld == true)
	{
		float ScreenWidth = (float)(m_ViewDimensions.right - m_ViewDimensions.left);
		float ScreenHeight = (float)(m_ViewDimensions.bottom - m_ViewDimensions.top);

		float MoveX = (float)(DeltaX) * 1.15f * m_CameraLocation.Z * 1/ScreenWidth;
		float MoveY = (float)(DeltaY) * 1.15f * m_CameraLocation.Z * 1/ScreenWidth;

		RKMesh::Get()->TransUVCentre(-MoveX, MoveY);
	}
}


bool ViewUV::TextureScaleUp()
{
	if(m_UVScale == 4) return(false);

	m_UVScale++;
	return(true);
}


bool ViewUV::TextureScaleDown()
{
	if(m_UVScale == 1) return(false);

	m_UVScale--;
	return(true);
}



bool ViewUV::NewMouseHandler()
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

