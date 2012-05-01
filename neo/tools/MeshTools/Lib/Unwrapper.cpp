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
#include "Unwrapper.h"
#include "OGLRender.h"
#include "Font.h"

#include "stdio.h"



/* *************************************************************************************************************************************
	Unwrapper

	Description:
		The Root of the Unwrap functions, sets up the Open GL Viewport, handles inputs, loads and saves meshes.
		Sends keys and mouse movements ont the 3D and UV window
		Calls render function to draw 3D and UV window

************************************************************************************************************************************** */


Unwrapper* Unwrapper::Get()
{
	static Unwrapper* pUnwrapper = NULL;

	if(pUnwrapper == NULL)
	{
		pUnwrapper = new Unwrapper;
	}

	return(pUnwrapper);
}


/************************************
*	Initialise the unwrap system	*
************************************/

bool Unwrapper::Initialise(char* OBJFileName)
{
	m_pOBJLoader = new OBJLoader;

	bool result = m_pOBJLoader->Load(OBJFileName);

	if(result == false) return(false);

	View3D::Get()->Initialise();
	ViewUV::Get()->Initialise();

	UndoList::Get()->CopyMesh();

	return(result);
}


void Unwrapper::Render()
{
	OGLRender::Get()->ReSizeWindow(m_ViewportDimensions.left, m_ViewportDimensions.right, m_ViewportDimensions.top, m_ViewportDimensions.bottom, 1.0f, 65535.0f);
	OGLRender::Get()->EnableContext();
	OGLRender::Get()->StartRender();

	RenderCentreBar();
	RenderMeshViewport();
	RenderUVViewport();

	OGLRender::Get()->EndRender();
	OGLRender::Get()->DisableContext();
}



void Unwrapper::ResizeViewport(RECT ViewportDimensions)
{
	m_ViewportDimensions = ViewportDimensions;

	RECT View3DDimensions;
	RECT ViewUVDimensions;

	View3DDimensions.top = ViewportDimensions.top;
	View3DDimensions.left = ViewportDimensions.left;
	View3DDimensions.bottom = ViewportDimensions.bottom;
	View3DDimensions.right = (int)((float)m_ViewportDimensions.right * m_CentreBarX);

	ViewUVDimensions.top = ViewportDimensions.top;
	ViewUVDimensions.left = (int)((float)m_ViewportDimensions.right * m_CentreBarX);
	ViewUVDimensions.bottom = ViewportDimensions.bottom;
	ViewUVDimensions.right = ViewportDimensions.right;

	View3D::Get()->SetViewDimensions(View3DDimensions);
	ViewUV::Get()->SetViewDimensions(ViewUVDimensions);
}



void Unwrapper::RenderMeshViewport()
{
	RECT MeshViewRect = View3D::Get()->GetViewDimensions();
		
	OGLRender::Get()->ReSizeWindow(MeshViewRect.left, MeshViewRect.right, MeshViewRect.top, MeshViewRect.bottom, 0.01f, 65535.0f);
	OGLRender::Get()->SetTexturing();
	RKMesh::Get()->RenderMesh(View3D::Get()->m_CurrentMode == RK_EDGE_TOPOLOGICAL);
}



void Unwrapper::RenderUVViewport()
{
	RECT UVViewRect = ViewUV::Get()->GetViewDimensions();

	OGLRender::Get()->ReSizeWindow(UVViewRect.left, UVViewRect.right, UVViewRect.top, UVViewRect.bottom, 0.01f, 65535.0f);
	OGLRender::Get()->SetFlatShaded();
	RKMesh::Get()->RenderUVs(View3D::Get()->m_CurrentMode == RK_EDGE_TOPOLOGICAL);
}



void Unwrapper::RenderCentreBar()
{
	float ScreenCentreX = (float)((m_ViewportDimensions.right - m_ViewportDimensions.left) / 2);
	float ScreenCentreY = (float)((m_ViewportDimensions.bottom - m_ViewportDimensions.top) / 2);
	float Left, Right, Top, Bottom;

	float CentreX = (m_CentreBarX - 0.5f) *2.0f * ScreenCentreX;

	Left = (float)CentreX - 4.0f;
	Right = (float)CentreX + 4.0f;
	Top = (float)-ScreenCentreY;
	Bottom = (float)ScreenCentreY;

	Left /= ScreenCentreX;
	Right /= ScreenCentreX;
	Top /= -ScreenCentreY;
	Bottom /= -ScreenCentreY;



//	OGLRender::Get()->ReSizeWindow(m_ViewportDimensions.left, m_ViewportDimensions.right, m_ViewportDimensions.top, m_ViewportDimensions.bottom, 1.0f, 65535.0f);
	OGLRender::Get()->SetFlatShaded();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_CULL_FACE);
//	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);
	glColor4f(0.836f, 0.824f, 0.805f, 1.0f);
	glVertex4f(Left, Top, 0.0f, 1.0f);
	glVertex4f(Right, Top, 0.0f, 1.0f);
	glVertex4f(Right, Bottom, 0.0f, 1.0f);
	glVertex4f(Left, Bottom, 0.0f, 1.0f);
	glEnd();

	RenderAxis(ScreenCentreX, ScreenCentreY);

	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



void Unwrapper::RenderAxis(float ScreenCentreX, float ScreenCentreY)
{
	Vertex XAxisIn, YAxisIn, ZAxisIn;
	Vertex XAxisOut, YAxisOut, ZAxisOut;
	float XPos = 30;
	float YPos = (ScreenCentreY*2)-30;
	
	XAxisIn.SetVertex(20.0f, 0.0f, 0.0f);
	YAxisIn.SetVertex(0.0f, -20.0f, 0.0f);
	ZAxisIn.SetVertex(0.0f, 0.0f, 20.0f);

	Matrix RotMatrix = View3D::Get()->GetAxisMatrix();

	RotMatrix.RotateVertex(&XAxisIn, &XAxisOut);
	RotMatrix.RotateVertex(&YAxisIn, &YAxisOut);
	RotMatrix.RotateVertex(&ZAxisIn, &ZAxisOut);

//	(XPos-ScreenCentreX)/ScreenCentreX, (YPos-ScreenCentreY)/-ScreenCentreY

	glBegin(GL_LINES);

	glColor3f(2.0f, 0.0f, 0.0f);					// X Axis
	glVertex3f((XPos-ScreenCentreX)/ScreenCentreX, (YPos-ScreenCentreY)/-ScreenCentreY, -1.0f);
	glVertex3f((XPos+XAxisOut.X-ScreenCentreX)/ScreenCentreX, (YPos+XAxisOut.Y-ScreenCentreY)/-ScreenCentreY, -1.0f);

	glColor3f(0.0f, 2.0f, 0.0f);					// Y Axis
	glVertex3f((XPos-ScreenCentreX)/ScreenCentreX, (YPos-ScreenCentreY)/-ScreenCentreY, -1.0f);
	glVertex3f((XPos+YAxisOut.X-ScreenCentreX)/ScreenCentreX, (YPos+YAxisOut.Y-ScreenCentreY)/-ScreenCentreY, -1.0f);

	glColor3f(0.0f, 0.0f, 2.0f);					// Z Axis
	glVertex3f((XPos-ScreenCentreX)/ScreenCentreX, (YPos-ScreenCentreY)/-ScreenCentreY, -1.0f);
	glVertex3f((XPos+ZAxisOut.X-ScreenCentreX)/ScreenCentreX, (YPos+ZAxisOut.Y-ScreenCentreY)/-ScreenCentreY, -1.0f);

	glEnd();


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.01f);

	if(OGLRender::Get()->m_Initialised == true)
	{
		Font::Get()->PrintString("X", XPos-4+XAxisOut.X, YPos-6+XAxisOut.Y, -1.0f, 0xff0000, ScreenCentreX, ScreenCentreY);
		Font::Get()->PrintString("Y", XPos-4+YAxisOut.X, YPos-6+YAxisOut.Y, -1.0f, 0x00ff00, ScreenCentreX, ScreenCentreY);
		Font::Get()->PrintString("Z", XPos-4+ZAxisOut.X, YPos-6+ZAxisOut.Y, -1.0f, 0x0000ff, ScreenCentreX, ScreenCentreY);
	}

	glDisable(GL_ALPHA_TEST);		//GL_BLEND);
	glDisable(GL_BLEND);
}



bool Unwrapper::HandleInput(int MouseState, int MouseX, int MouseY)
{
	static int ButtonsHeld = 0;
	bool UpdateViewport = false;
	RECT MeshViewRect = View3D::Get()->GetViewDimensions();
	RECT UVViewRect = ViewUV::Get()->GetViewDimensions();
	static int LastMouseX;
	static int LastMouseY;


	short AltKey = GetAsyncKeyState(VK_LMENU);
	View3D::Get()->SetALT(AltKey);					// windows messing me about again!
	ViewUV::Get()->SetALT(AltKey);


	if(MouseState == RK_MOUSE_LB_DOWN || MouseState == RK_MOUSE_MB_DOWN || MouseState == RK_MOUSE_RB_DOWN || MouseState == RK_MOUSE_LB_DBLCLK)
	{
		ButtonsHeld = MouseState;

		float ScreenCentreX = (float)((m_ViewportDimensions.right - m_ViewportDimensions.left) / 2);
		float CentreX = (m_CentreBarX - 0.5f) *2.0f * ScreenCentreX;
		CentreX += ScreenCentreX;

		if(MouseX > CentreX - 4 && MouseX < CentreX +4) 
		{
			WhichWindow = CENTREBAR;
			return(false);
		}

		if(MouseX > m_ViewportDimensions.left && MouseX < (m_ViewportDimensions.right) * m_CentreBarX && MouseY > m_ViewportDimensions.top && MouseY < m_ViewportDimensions.bottom)
		{
			OGLRender::Get()->ReSizeWindow(MeshViewRect.left, MeshViewRect.right, MeshViewRect.top, MeshViewRect.bottom, 1.0f, 65535.0f);
			UpdateViewport = View3D::Get()->HandleInput(ButtonsHeld, MouseX, MouseY, LastMouseX, LastMouseY);
			WhichWindow = DVIEW3D;
		}

		if(MouseX > (m_ViewportDimensions.right) * m_CentreBarX && MouseX < m_ViewportDimensions.right && MouseY > m_ViewportDimensions.top && MouseY < m_ViewportDimensions.bottom)
		{
			OGLRender::Get()->ReSizeWindow(UVViewRect.left, UVViewRect.right, UVViewRect.top, UVViewRect.bottom, 0.01f, 65535.0f);
			UpdateViewport = ViewUV::Get()->HandleInput(ButtonsHeld, MouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), MouseY, LastMouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), LastMouseY);
			WhichWindow = DVIEWUV;
		}

		return(UpdateViewport);
	}

	if(WhichWindow == DVIEW3D) OGLRender::Get()->ReSizeWindow(MeshViewRect.left, MeshViewRect.right, MeshViewRect.top, MeshViewRect.bottom, 1.0f, 65535.0f);
	if(WhichWindow == DVIEWUV) OGLRender::Get()->ReSizeWindow(UVViewRect.left, UVViewRect.right, UVViewRect.top, UVViewRect.bottom, 0.01f, 65535.0f);


	if(WhichWindow == CENTREBAR && MouseState == RK_MOUSE_MOVE) 
	{
		float ScreenWidth = (float)((m_ViewportDimensions.right - m_ViewportDimensions.left));

		float MoveX = (float)(LastMouseX - MouseX) / ScreenWidth;

		m_CentreBarX -= MoveX;

		if(m_CentreBarX < 0.1f) m_CentreBarX = 0.1f;
		if(m_CentreBarX > 0.9f) m_CentreBarX = 0.9f;

		LastMouseX = MouseX;
		LastMouseY = MouseY;

		return(true);
	}

	
	if(MouseState == RK_MOUSE_LB_UP || MouseState == RK_MOUSE_MB_UP || MouseState == RK_MOUSE_RB_UP || MouseState == CENTREBAR)
	{
		if(WhichWindow == DVIEW3D) UpdateViewport = View3D::Get()->HandleInput(MouseState, MouseX, MouseY, LastMouseX, LastMouseY);
		if(WhichWindow == DVIEWUV) UpdateViewport = ViewUV::Get()->HandleInput(MouseState, MouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), MouseY, LastMouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), LastMouseY);

		ButtonsHeld = 0;

		WhichWindow = VIEWNONE;
		return(UpdateViewport);
	}


	if(WhichWindow == DVIEW3D)
	{
		UpdateViewport = View3D::Get()->HandleInput(MouseState, MouseX, MouseY, LastMouseX, LastMouseY);
	}
	
	if(WhichWindow == DVIEWUV)
	{
		UpdateViewport = ViewUV::Get()->HandleInput(MouseState, MouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), MouseY, LastMouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), LastMouseY);
	}

	if(MouseState == RK_MOUSE_MOVE)
	{
		LastMouseX = MouseX;
		LastMouseY = MouseY;
		if(WhichWindow == DVIEW3D) View3D::Get()->HandleInput(RK_MOUSE_MOVE, MouseX, MouseY,LastMouseX, LastMouseY);
		if(WhichWindow == DVIEWUV) ViewUV::Get()->HandleInput(RK_MOUSE_MOVE, MouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), MouseY, LastMouseX - (int)((float)m_ViewportDimensions.right * m_CentreBarX), LastMouseY);
	}


	if(MouseState == RK_KEY_DOWN)
	{
		if(MouseX == 'E')
		{
			View3D::Get()->SetMode(RK_EDGE_SELECT);
			ViewUV::Get()->SetMode(RK_EDGE_SELECT);
			UpdateViewport = true;
		}

		if(MouseX == 'U')
		{
			View3D::Get()->SetMode(RK_UV_SELECT);
			ViewUV::Get()->SetMode(RK_UV_SELECT);
			UpdateViewport = true;
		}

		if(MouseX == 'P')
		{
			RKMesh::Get()->PinSelectedUVs();
			UpdateViewport = true;
		}

		if(MouseX == 'O')
		{
			RKMesh::Get()->UnPinSelectedUVs();
			UpdateViewport = true;
		}
/*
		if(MouseX == 'S')
		{
			ViewUV::Get()->SetMode(RK_UV_STRETCH);
			UpdateViewport = true;
		}
*/
		if(MouseX == 'R')
		{
			ViewUV::Get()->SetMode(RK_UV_ROTATE);
			UpdateViewport = true;
		}

		if(MouseX == VK_SHIFT)
		{
			View3D::Get()->SetMode(RK_SHIFT_HELD);
			ViewUV::Get()->SetMode(RK_SHIFT_HELD);
		}

		if(MouseX == VK_CONTROL)
		{
			View3D::Get()->SetMode(RK_CTRL_HELD);
			ViewUV::Get()->SetMode(RK_CTRL_HELD);
		}

		if(MouseX == 'Z')
		{
			UndoList::Get()->UndoMesh();
			UpdateViewport = true;
		}

		if(MouseX == 'X')
		{
			UndoList::Get()->RedoMesh();
			UpdateViewport = true;
		}

		if(MouseX == 'T')
		{
			View3D::Get()->SetMode(RK_EDGE_TOPOLOGICAL);
			ViewUV::Get()->SetMode(RK_EDGE_TOPOLOGICAL);
			UpdateViewport = true;
		}

		if(MouseX == 'A')
		{
			Vector UVLocation;
			UVLocation.SetVector(-0.5f, -0.5f, -1.0f, 1.0f);

			RKMesh::Get()->GetCentreAndRadius();
			ViewUV::Get()->SetCameraLocation(UVLocation);
			UpdateViewport = true;
		}

		if(MouseX == 'Q')
		{
			RKMesh::Get()->DeselectAll();
			UpdateViewport = true;
		}

		if(MouseX == 'C')
		{
			UpdateViewport = RKMesh::Get()->CutSelected();
		}

		if(MouseX == 'W')
		{
			UpdateViewport = RKMesh::Get()->WeldSelected();
		}

		if(MouseX == 'M')
		{
			View3D::Get()->SetMode(RK_MOVE_CENTREPOINT);
			ViewUV::Get()->SetMode(RK_MOVE_CENTREPOINT);
			UpdateViewport = true;
		}

		if(MouseX == 'N')
		{
			View3D::Get()->CentreRoundSelectedEdges();
			UpdateViewport = true;
		}

		if(MouseX == VK_LEFT)
		{
			UpdateViewport = ViewUV::Get()->TextureScaleDown();
		}

		if(MouseX == VK_RIGHT)
		{
			UpdateViewport = ViewUV::Get()->TextureScaleUp();
		}

		if(MouseX == VK_NUMPAD4)
		{
			UpdateViewport = RKMesh::Get()->MinMaxUVs(MINU);
		}

		if(MouseX == VK_NUMPAD6)
		{
			UpdateViewport = RKMesh::Get()->MinMaxUVs(MAXU);
		}

		if(MouseX == VK_NUMPAD2)
		{
			UpdateViewport = RKMesh::Get()->MinMaxUVs(MINV);
		}

		if(MouseX == VK_NUMPAD8)
		{
			UpdateViewport = RKMesh::Get()->MinMaxUVs(MAXV);
		}

		if(MouseX == 'S')
		{
			UpdateViewport = RKMesh::Get()->RelaxUVs();
		}
	}


	if(MouseState == RK_KEY_UP)
	{
		if(MouseX == VK_SHIFT)
		{
			UpdateViewport = View3D::Get()->UnSetMode(RK_SHIFT_HELD);
			UpdateViewport |= ViewUV::Get()->UnSetMode(RK_SHIFT_HELD);
		}

		if(MouseX == VK_CONTROL)
		{
			UpdateViewport = View3D::Get()->UnSetMode(RK_CTRL_HELD);
			UpdateViewport |= ViewUV::Get()->UnSetMode(RK_CTRL_HELD);
		}

		if(MouseX == 'T')
		{
			View3D::Get()->UnSetMode(RK_EDGE_TOPOLOGICAL);
			ViewUV::Get()->UnSetMode(RK_EDGE_TOPOLOGICAL);
			UpdateViewport = true;
		}

		if(MouseX == 'M')
		{
			View3D::Get()->UnSetMode(RK_MOVE_CENTREPOINT);
			ViewUV::Get()->UnSetMode(RK_MOVE_CENTREPOINT);
			UpdateViewport = true;
		}
	}

	return(UpdateViewport);
}



void Unwrapper::AllKeysReleased()
{
	View3D::Get()->AllKeysReleased();
	ViewUV::Get()->AllKeysReleased();
}


void Unwrapper::Delete()
{
	RKMesh::Get()->Delete();
	UndoList::Get()->Delete();
}