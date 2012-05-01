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


// Render a face of a mesh.

// all kinds of crazyness in here..  once again, anything not obvious..

// SwannOTron@googlemail.com

// Got some work to do here..  must optimise the rendering.. 


#include "../LSCM/MeshBuilderStdafx.h"
#include "Face.h"
#include "View3D.h"
#include "ViewUV.h"
//#include "OGLRender.h"

#include "gl\gl.h"




bool gABF = true;
bool gFillHoles = true;


extern bool gWireframe;
extern bool gShowStretch;



void Face::SetFace(int NumberOfVertices, int* VertexIndices, int* NormalIndices, int* UVIndices)
{
	m_NumberOfVertices = NumberOfVertices;
	m_pVertexIndices = new int[NumberOfVertices];
	m_pNormalIndices = new int[NumberOfVertices];
	m_pUVIndices = new int[NumberOfVertices];
	m_pEdgeIndices = new int[NumberOfVertices];
	m_pConnectedTo = new int[NumberOfVertices];


	for(int Index = 0; Index < NumberOfVertices; Index++)
	{
		m_pVertexIndices[Index] = VertexIndices[Index];
		m_pNormalIndices[Index] = NormalIndices[Index];
		m_pUVIndices[Index] = UVIndices[Index];
		m_pConnectedTo[Index] = -1;
	}
}


void Face::SetEdges(int* EdgeIndices)
{
	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		m_pEdgeIndices[Index] = EdgeIndices[Index];
	}
}



void Face::SetUVs(int* UVIndices)
{
	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		m_pUVIndices[Index] = UVIndices[Index];
	}
}



void Face::RenderFacePolygon(Vertex* pVertices, Vertex* pNormals, RKUV* pUVs, RKEdge* pEdges, Vector* LineJustify, Matrix* MatrixIn)
{
	
}



void Face::RenderFaceWireframe(Vertex* pVertices, Vertex* pNormals, RKUV* pUVs, RKEdge* pEdges, Vector* LineJustify, Matrix* MatrixIn)
{
	m_LineWidth = 1;

	
}



void Face::RenderFaceUV(RKUV* pUVs, RKEdge* pEdges, int CloseV1, int CloseV2)
{
	
}



void Face::RenderUVPolygon(RKUV* pUVs)
{
//	Matrix ThisMatrix;
//	ThisMatrix.MultiplyMatrix(MatrixIn, &(OGLRender::Get()->InternalProjMatrix));


	
}




void Face::SetEdgeColour(int EdgeSelected, bool UVWindow)
{
	float MeshEdge[3] = { 0.0f, 1.3f, 1.3f };
	float SelectedEdge[3] = { 1.5f, 0.64f, 0.0f };
	float CutEdge[3] = { 1.0f, 0.0f, 0.0f };
	float CutAndSelectedEdge[3] = { 1.0f, 0.0f, 1.0f };
	bool Selected = false;

	
	if(UVWindow == true) MeshEdge[0] = 1.3f;

	if((EdgeSelected & 0xf ) > 2)
	{
		if(EdgeSelected & NEWSELECTED && EdgeSelected & NOTSELECTED) Selected = true;
		if(EdgeSelected & NEWNOTSELECTED && EdgeSelected & NOTSELECTED) Selected = true;
	}
	else
	{
		if(EdgeSelected & SELECTED)	Selected = true;
	}

	if(Selected == false && ((EdgeSelected & EDGECUT) == 0)) 
	{
		if(m_LineWidth != 1)
		{
			m_LineWidth = 1;
			glEnd();
			glLineWidth((GLfloat)m_LineWidth);
			glBegin(GL_LINES);
		}

		glColor3f(MeshEdge[0], MeshEdge[1], MeshEdge[2]);
		return;
	}

	if(m_LineWidth != 2)
	{
		m_LineWidth = 2;
		glEnd();
		glLineWidth((GLfloat)m_LineWidth);
		glBegin(GL_LINES);
	}

	if(Selected == true && ((EdgeSelected & EDGECUT) == 0)) glColor3f(SelectedEdge[0], SelectedEdge[1], SelectedEdge[2]);
	if(Selected == false && EdgeSelected & EDGECUT) glColor3f(CutEdge[0], CutEdge[1], CutEdge[2]);
	if(Selected == true && EdgeSelected & EDGECUT) glColor3f(CutAndSelectedEdge[0], CutAndSelectedEdge[1], CutAndSelectedEdge[2]);
}



void Face::RenderUVs(Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs)
{
	
}



bool Face::IsBackFace(Matrix* ThisMatrix, Vertex* pVertices)
{
	

	return(true);
}




bool Face::CheckSelectedEdges(RECT SelectionBox, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, RKEdge* pEdges, bool SHIFTHeld, bool CTRLHeld, int* pEdge1)		//, bool OnlyClosest, float* NearestZ, int* NearestEdge)
{
	

	return(false);
}




bool Face::InsideOutside(float X1, float Y1, float X2, float Y2, int BoxLeft, int BoxRight, int BoxTop, int BoxBottom)
{
	if(X1 > BoxLeft && X1 < BoxRight && Y1 > BoxTop && Y1 < BoxBottom) 
	{	
		return(true);			// vertex 1 is in the box
	}

	if(X2 > BoxLeft && X2 < BoxRight && Y2 > BoxTop && Y2 < BoxBottom) 
	{
		return(true);			// vertex 2 is in the box
	}


	if(X1 < BoxLeft && X2 > BoxLeft || X2 < BoxLeft && X1 > BoxLeft)			// clip line against BoxLeft
	{
		float DeltaX = (X2 - X1);
		float DeltaY = (Y2 - Y1) / DeltaX;
		float CrossPointY = (DeltaY * (BoxLeft - X1)) + Y1;
		if(CrossPointY > BoxTop && CrossPointY < BoxBottom) return(true);
	}
		
	if(X1 < BoxRight && X2 > BoxRight || X2 < BoxRight && X1 > BoxRight)			// clip line against BoxRight
	{
		float DeltaX = (X2 - X1);
		float DeltaY = (Y2 - Y1) / DeltaX;
		float CrossPointY = (DeltaY * (BoxRight - X1)) + Y1;
		if(CrossPointY > BoxTop && CrossPointY < BoxBottom) return(true);
	}

	if(Y1 < BoxTop && Y2 > BoxTop || Y2 < BoxTop && Y1 > BoxTop)				// clip line against BoxTop
	{
		float DeltaY = (Y2 - Y1);
		float DeltaX = (X2 - X1) / DeltaY;
		float CrossPointY = (DeltaX * (BoxTop - Y1)) + X1;
		if(CrossPointY > BoxLeft && CrossPointY < BoxRight) return(true);
	}

	if(Y1 < BoxBottom && Y2 > BoxBottom || Y2 < BoxBottom && Y1 > BoxBottom)		// clip line against BoxBottom
	{
		float DeltaY = (Y2 - Y1);
		float DeltaX = (X2 - X1) / DeltaY;
		float CrossPointY = (DeltaX * (BoxBottom - Y1)) + X1;
		if(CrossPointY > BoxLeft && CrossPointY < BoxRight) return(true);
	}

	return(false);
}



bool Face::CheckSelectedUVs(int BoxLeft, int BoxRight, int BoxTop, int BoxBottom, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect)
{
	

	return(false);
}



void Face::LinkFaces(Face* pFaceIn, RKEdge* pEdges, int FaceIn1, int FaceIn2)
{
	if(FaceIn1 == FaceIn2) return;

	for(int Face1Index = 0; Face1Index < m_NumberOfVertices; Face1Index++)
	{
		for(int Face2Index = 0; Face2Index < pFaceIn->GetNumberOfVertices(); Face2Index++)
		{
			if(m_pEdgeIndices[Face1Index] == pFaceIn->GetEdgeIndex(Face2Index))
			{
				m_pConnectedTo[Face1Index] = FaceIn2;
				pFaceIn->m_pConnectedTo[Face2Index] = FaceIn1;
				return;
			}
		}
	}
}


void Face::TrailingEdgeCheck(Face* pFaces, RKEdge* pEdges)
{
	for(int Face1Index = 0; Face1Index < m_NumberOfVertices; Face1Index++)
	{
		if(m_pConnectedTo[Face1Index] != -1)
		{
			int Face2Index = m_pConnectedTo[Face1Index];
			Face* pFaceIn = &pFaces[Face2Index];


			for(int Face2Index = 0; Face2Index < pFaceIn->GetNumberOfVertices(); Face2Index++)
			{
				if(m_pEdgeIndices[Face1Index] == pFaceIn->GetEdgeIndex(Face2Index))
				{
					int Face1UV1, Face1UV2;
					int Face2UV1, Face2UV2;
					
					if(Face1Index == 0) Face1UV1 = m_pUVIndices[m_NumberOfVertices-1];
					else
						Face1UV1 = m_pUVIndices[Face1Index-1];
					
					Face1UV2 = m_pUVIndices[Face1Index];

					if(Face2Index == 0) Face2UV1 = pFaceIn->m_pUVIndices[pFaceIn->m_NumberOfVertices-1];
					else
						Face2UV1 = pFaceIn->m_pUVIndices[Face2Index-1];

					Face2UV2 = pFaceIn->m_pUVIndices[Face2Index];


					if(!((Face1UV1 == Face2UV1 && Face1UV2 == Face2UV2) || (Face1UV2 == Face2UV1 && Face1UV1 == Face2UV2)))
					{
						pEdges[m_pEdgeIndices[Face1Index]].m_Selected = EDGECUT | NOTSELECTED;
						pEdges[pFaceIn->m_pEdgeIndices[Face2Index]].m_Selected = EDGECUT | NOTSELECTED;
					}
				}
			}
		}
	}
}



int Face::GetConnectedFace(int EdgeIndex, Face* pFaces, RKEdge* pEdges)
{
	if(m_pConnectedTo[EdgeIndex] != -1)
	{
		int FaceIndex2 = m_pConnectedTo[EdgeIndex];
		Face* pFaceIn = &pFaces[FaceIndex2];


		for(int Face2Index = 0; Face2Index < pFaceIn->GetNumberOfVertices(); Face2Index++)
		{
			if(m_pEdgeIndices[EdgeIndex] == pFaceIn->GetEdgeIndex(Face2Index))
			{
				int Face1UV1, Face1UV2;
				int Face2UV1, Face2UV2;
					
				if(EdgeIndex == 0) Face1UV1 = m_pUVIndices[m_NumberOfVertices-1];
				else
					Face1UV1 = m_pUVIndices[EdgeIndex-1];
				
				Face1UV2 = m_pUVIndices[EdgeIndex];

				if(Face2Index == 0) Face2UV1 = pFaceIn->m_pUVIndices[pFaceIn->m_NumberOfVertices-1];
				else
					Face2UV1 = pFaceIn->m_pUVIndices[Face2Index-1];

				Face2UV2 = pFaceIn->m_pUVIndices[Face2Index];


				if(((Face1UV1 == Face2UV1 && Face1UV2 == Face2UV2) || (Face1UV2 == Face2UV1 && Face1UV1 == Face2UV2)))
				{
					if(pFaces[FaceIndex2].m_Region != -1) return(-1);
					return(FaceIndex2);
				}
			}
		}
	}

	return(-1);
}



/*
void Face::TrailingEdgeCheck(Face* pFaceIn, RKEdge* pEdges)
{
	for(int Face1Index = 0; Face1Index < m_NumberOfVertices; Face1Index++)
	{
		for(int Face2Index = 0; Face2Index < pFaceIn->GetNumberOfVertices(); Face2Index++)
		{
			if(m_pEdgeIndices[Face1Index] == pFaceIn->GetEdgeIndex(Face2Index))
			{
				int Face1UV1, Face1UV2;
				int Face2UV1, Face2UV2;
				
				if(Face1Index == 0) Face1UV1 = m_pUVIndices[m_NumberOfVertices-1];
				else
					Face1UV1 = m_pUVIndices[Face1Index-1];
				
				Face1UV2 = m_pUVIndices[Face1Index];

				if(Face2Index == 0) Face2UV1 = pFaceIn->m_pUVIndices[pFaceIn->m_NumberOfVertices-1];
				else
					Face2UV1 = pFaceIn->m_pUVIndices[Face2Index-1];

				Face2UV2 = pFaceIn->m_pUVIndices[Face2Index];


				if(!((Face1UV1 == Face2UV1 && Face1UV2 == Face2UV2) || (Face1UV2 == Face2UV1 && Face1UV1 == Face2UV2)))
				{
					pEdges[m_pEdgeIndices[Face1Index]].m_Selected = EDGECUT | NOTSELECTED;
					pEdges[pFaceIn->m_pEdgeIndices[Face2Index]].m_Selected = EDGECUT | NOTSELECTED;
				}
			}
		}
	}
}
*/



void Face::AreaCalc(Vertex* pVertices, RKUV* pUVs)
{
	float FaceArea = 0.0f;
	float UVArea = 0.0f;


	int Index2 = m_NumberOfVertices - 1;

	for(int Index = 0; Index < m_NumberOfVertices-2; Index++)
	{
		Vector CrossResult;
		Vector Vert1, Vert2, Vert3;
		Vector DeltaX, DeltaY;
		int VertIndex1, VertIndex2, VertIndex3;

		VertIndex1 = m_pVertexIndices[Index2];
		VertIndex2 = m_pVertexIndices[Index];
		VertIndex3 = m_pVertexIndices[Index+1];

		DeltaX.SetVector(pVertices[VertIndex2].X - pVertices[VertIndex1].X, pVertices[VertIndex2].Y - pVertices[VertIndex1].Y, pVertices[VertIndex2].Z - pVertices[VertIndex1].Z, 1.0f);
		DeltaY.SetVector(pVertices[VertIndex3].X - pVertices[VertIndex1].X, pVertices[VertIndex3].Y - pVertices[VertIndex1].Y, pVertices[VertIndex3].Z - pVertices[VertIndex1].Z, 1.0f);

		DeltaX.CrossProduct(DeltaY, CrossResult);
		float Mag = CrossResult.Magnitude();
		FaceArea += Mag * 0.5f;


		VertIndex1 = m_pUVIndices[Index2];
		VertIndex2 = m_pUVIndices[Index];
		VertIndex3 = m_pUVIndices[Index+1];

		DeltaX.SetVector(pUVs[VertIndex2].m_U - pUVs[VertIndex1].m_U, pUVs[VertIndex2].m_V - pUVs[VertIndex1].m_V, 0.0f, 1.0f);
		DeltaY.SetVector(pUVs[VertIndex3].m_U - pUVs[VertIndex1].m_U, pUVs[VertIndex3].m_V - pUVs[VertIndex1].m_V, 0.0f, 1.0f);

		DeltaX.CrossProduct(DeltaY, CrossResult);
		Mag = CrossResult.Magnitude();
		UVArea += Mag * 0.5f;
	}

	m_StretchVal = UVArea / FaceArea;
}



void Face::AssignStretch(float ShrunkLimit, float StretchLimit)
{
	if(m_StretchVal < ShrunkLimit) { m_StretchColouring = 0.0f; return; }
	if(m_StretchVal > StretchLimit) { m_StretchColouring = 1.0f; return; }

	float DeltaLimits = StretchLimit - ShrunkLimit;
	float Point = m_StretchVal - ShrunkLimit;

	m_StretchColouring = Point / DeltaLimits;
}



void Face::SelectUVs(RKUV* pUVs)
{
	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		pUVs[m_pUVIndices[Index]].m_Selected &= 0xfffffff0;
		pUVs[m_pUVIndices[Index]].m_Selected |= SELECTED;
	}
}


bool Face::UVOnPolyEdge(int UVIndex, RKEdge* pEdges, RKUV* pUVs, int* NewUV1, int* NewUV2)
{
	int ThisUVIndex;
	bool RetVal = false;
	*NewUV1 = -1;
	*NewUV2 = -1;

	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		if(m_pUVIndices[Index] == UVIndex)  
		{
			ThisUVIndex = Index;
		}
	}

//	pUVs[UVIndex].m_Selected &= 0xf;
	pUVs[UVIndex].m_Selected |= NEWSELECTED;


	if(m_pConnectedTo[ThisUVIndex] == -1 || (pEdges[m_pEdgeIndices[ThisUVIndex]].m_Selected & EDGECUT) != 0)
	{
		int NextUVIndex = ThisUVIndex -1;
		if(NextUVIndex == -1) NextUVIndex = m_NumberOfVertices-1;

		if((pUVs[m_pUVIndices[NextUVIndex]].m_Selected & NEWSELECTED) == false)
		{
//			pUVs[m_pUVIndices[NextUVIndex]].m_Selected &= 0xfffffff0;
			pUVs[m_pUVIndices[NextUVIndex]].m_Selected |= NEWSELECTED;
			*NewUV1 = m_pUVIndices[NextUVIndex];
			RetVal = true;
		}
	}


	ThisUVIndex += 1;
	if(ThisUVIndex == m_NumberOfVertices) ThisUVIndex = 0;


	if(m_pConnectedTo[ThisUVIndex] == -1 || (pEdges[m_pEdgeIndices[ThisUVIndex]].m_Selected & EDGECUT) != 0)
	{
		int NextUVIndex = ThisUVIndex + 1;
		if(NextUVIndex == m_NumberOfVertices) NextUVIndex = 0;

		if((pUVs[m_pUVIndices[ThisUVIndex]].m_Selected & NEWSELECTED) == false)
		{
//			pUVs[m_pUVIndices[ThisUVIndex]].m_Selected &= 0xf;
			pUVs[m_pUVIndices[ThisUVIndex]].m_Selected |= NEWSELECTED;
			*NewUV2 = m_pUVIndices[ThisUVIndex];
			RetVal = true;
		}
	}

	return(RetVal);
}



bool Face::GetConnectedUVs(int UVIndexIn, int& UVConnected1, int& UVConnected2)
{
	bool ReturnVal = false;
	

	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		if(m_pUVIndices[Index] == UVIndexIn)
		{
			int NextIndex = Index + 1;
			int PrevIndex = Index - 1;

			if(NextIndex > m_NumberOfVertices - 1) NextIndex = 0;
			if(PrevIndex < 0) PrevIndex = m_NumberOfVertices - 1;

			UVConnected1 = m_pUVIndices[NextIndex];
			UVConnected2 = m_pUVIndices[PrevIndex];
			ReturnVal = true;
		}
	}

	return(ReturnVal);
}



void Face::CreateNormal(Vertex* pNormals, Vertex* pVertices, int NormalIndex)
{
	Vector DeltaVertex1;
	Vector DeltaVertex2;
	Vector Normal;
	int VertIndex1 = m_pVertexIndices[0];
	int VertIndex2 = m_pVertexIndices[1];
	int VertIndex3 = m_pVertexIndices[2];

	DeltaVertex1.SetVector(pVertices[VertIndex2].X - pVertices[VertIndex1].X, pVertices[VertIndex2].Y - pVertices[VertIndex1].Y, pVertices[VertIndex2].Z - pVertices[VertIndex1].Z, 1.0f);
	DeltaVertex2.SetVector(pVertices[VertIndex2].X - pVertices[VertIndex3].X, pVertices[VertIndex2].Y - pVertices[VertIndex3].Y, pVertices[VertIndex2].Z - pVertices[VertIndex3].Z, 1.0f);

	DeltaVertex1.Normalise();
	DeltaVertex2.Normalise();

	DeltaVertex1.CrossProduct(DeltaVertex2, Normal);

	pNormals[NormalIndex].SetVertex(-Normal.X, Normal.Y, -Normal.Z);

	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		m_pNormalIndices[Index] = NormalIndex;
	}
}



bool Face::UVOnEdge(int UVIndex, RKEdge* pEdges)
{
	bool RetVal = false;

	for(int Index = 0; Index < m_NumberOfVertices; Index++)
	{
		if(m_pUVIndices[Index] == UVIndex)
		{
			int PrevIndex = Index - 1;
			if(PrevIndex < 0) PrevIndex = m_NumberOfVertices - 1;

			if(m_pConnectedTo[Index] == -1 || (pEdges[m_pEdgeIndices[Index]].m_Selected & EDGECUT) != 0) RetVal= true;
			if(m_pConnectedTo[PrevIndex] == -1 || (pEdges[m_pEdgeIndices[PrevIndex]].m_Selected & EDGECUT) != 0) RetVal= true;

		}
	}

	return(RetVal);
}
