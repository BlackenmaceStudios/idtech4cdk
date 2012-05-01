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


#ifndef FACE_INCLUDED
#define FACE_INCLUDED

#include "../LSCM/MeshBuilderStdafx.h"
#include "Vertex.h"
#include "Vector.h"
#include "Matrix.h"
#include "RKEdge.h"


class Face
{
public:
	Face()
	{
		m_NumberOfVertices = 0;
		m_pVertexIndices = NULL;
		m_pUVIndices = NULL;
		m_pNormalIndices = NULL;
		m_pEdgeIndices = NULL;
		m_pConnectedTo = NULL;
		m_Region = -1;
//		m_pEdgeSelectedList = NULL;
//		m_pUVSelectedList = NULL;
	}

	~Face()
	{
		if(m_pVertexIndices != NULL) delete[] m_pVertexIndices;
		if(m_pUVIndices != NULL) delete[] m_pUVIndices;
		if(m_pNormalIndices != NULL) delete[] m_pNormalIndices;
		if(m_pEdgeIndices != NULL) delete[] m_pEdgeIndices;
		if(m_pConnectedTo != NULL) delete [] m_pConnectedTo;

//		if(m_pEdgeSelectedList != NULL) delete[] m_pEdgeSelectedList;
//		if(m_pUVSelectedList != NULL) delete[] m_pUVSelectedList;
		m_NumberOfVertices = 0;
		m_pVertexIndices = NULL;
		m_pUVIndices = NULL;
		m_pNormalIndices = NULL;
		m_pEdgeIndices = NULL;
		m_pConnectedTo = NULL;
		m_Region = -1;
//		m_pEdgeSelectedList = NULL;
//		m_pUVSelectedList = NULL;
	};


	void SetFace(int NumberOfVertices, int* VertexIndices, int* NormalIndices, int* UVIndices);
	void SetUVs(int* UVIndices);
	void SetEdges(int* EdgeIndices);
	void SetEdgeIndex(int IndexIn, int EdgeIndex) { m_pEdgeIndices[IndexIn] = EdgeIndex; }

	void RenderFacePolygon(Vertex* pVertices, Vertex* pNormals, RKUV* pUVs, RKEdge* pEdges, Vector* LineJustify, Matrix* MatrixIn);
	void RenderFaceWireframe(Vertex* pVertices, Vertex* pNormals, RKUV* pUVs, RKEdge* pEdges, Vector* LineJustify, Matrix* MatrixIn);
	void RenderFaceUV(RKUV* pUVs, RKEdge* pEdges, int CloseV1, int CloseV2);
	void RenderUVs(Matrix* ThisMatrix, Vertex* pVertices, RKUV* pUVs);
	void RenderUVPolygon(RKUV* pUVs);

	int GetNumberOfVertices() { return(m_NumberOfVertices); }
	int GetVertexIndex(int IndexIn) { return(m_pVertexIndices[IndexIn]); }
	void SetUV(int Index, int UVIndex) { m_pUVIndices[Index] = UVIndex; }


//	void SelectEdge(int VertIndexIn1, int VertIndexIn2);

	bool CheckSelectedEdges(RECT SelectionBox, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, RKEdge* pEdges, bool SHIFTHeld, bool CTRLHeld, int* pEdge1);;		// bool OnlyClosest, float* NearestZ, int* NearestEdge);
	bool InsideOutside(float X1, float Y1, float X2, float Y2, int Boxleft, int BoxRight, int BoxTop, int BoxBottom);
//	bool FinaliseEdgeSelection();
//	bool IsEdgeSelected(int EdgeIndex, int* VertIndex1, int* VertIndex2, bool NewSelection);

	void SetEdgeColour(int EdgeSelect, bool UVWindow);

	bool CheckSelectedUVs(int BoxLeft, int BoxRight, int BoxTop, int BoxBottom, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect);
//	void FinaliseUVSelection();
//	bool GetIsPinned(int UVIndex) { if(m_pUVSelectedList[UVIndex] & PINNED) return true; return false; }
	int GetUVIndex(int UVIndex) { return(m_pUVIndices[UVIndex]); }
	int GetNormalIndex(int NormalIndex) { return(m_pNormalIndices[NormalIndex]); }
	int GetEdgeIndex(int EdgeIndex) { return(m_pEdgeIndices[EdgeIndex]); }


	void TrailingEdgeCheck(Face* pFaces, RKEdge* pEdges);
	int GetConnectedFace(int EdgeIndex, Face* pFaces, RKEdge* pEdges);

	float GetStretchVal() { return(m_StretchVal); }

	void AssignStretch(float ShrunkLimit, float StretchLimit);
	void AreaCalc(Vertex* pVertices, RKUV* pUVs);
	void SetRegion(int RegionIn) { m_Region = RegionIn; }
	int GetRegion() { return(m_Region); }
	void LinkFaces(Face* pFaceIn, RKEdge* pEdges, int FaceIn1, int FaceIn2);

	bool UVOnPolyEdge(int UVIndex, RKEdge* pEdges, RKUV* pUVs, int* NewUV1, int* NewUV2);
	void CreateNormal(Vertex* pNormals, Vertex* pVertices, int NormalIndex);

	bool HasUV(int UVIndex)
	{
		for(int Index = 0; Index < m_NumberOfVertices; Index++)
		{
			if(m_pUVIndices[Index] == UVIndex) return(true);
		}
		return(false);
	}

	void SelectUVs(RKUV* pUV);

	bool GetConnectedUVs(int UVIndexIn, int& UVConnected1, int& UVConnected2);
	bool UVOnEdge(int UVIndex, RKEdge* pEdges);


	int relativeUV[3];


	Face &operator =( Face &FaceIn )
	{
		m_NumberOfVertices = FaceIn.m_NumberOfVertices;
		m_pVertexIndices = new int[m_NumberOfVertices];
		m_pNormalIndices = new int[m_NumberOfVertices];
		m_pUVIndices = new int[m_NumberOfVertices];
		m_pEdgeIndices = new int[m_NumberOfVertices];
		m_pConnectedTo = new int[m_NumberOfVertices];
//		m_pEdgeSelectedList = new int[m_NumberOfVertices];
//		m_pUVSelectedList = new int[m_NumberOfVertices];

		m_Region = FaceIn.m_Region;
		m_StretchVal = FaceIn.m_StretchVal;
		m_StretchColouring = FaceIn.m_StretchColouring;

		for(int Index = 0; Index < m_NumberOfVertices; Index++)
		{
			m_pVertexIndices[Index] = FaceIn.m_pVertexIndices[Index];
			m_pNormalIndices[Index] = FaceIn.m_pNormalIndices[Index];
			m_pUVIndices[Index] = FaceIn.m_pUVIndices[Index];
			m_pEdgeIndices[Index] = FaceIn.m_pEdgeIndices[Index];
			m_pConnectedTo[Index] = FaceIn.m_pConnectedTo[Index];
//			m_pEdgeSelectedList[Index] = FaceIn.m_pEdgeSelectedList[Index];
//			m_pUVSelectedList[Index] = FaceIn.m_pUVSelectedList[Index];
		}

		return *this;
	}


private:
	bool IsBackFace(Matrix* MatrixIn, Vertex* pVertices);


	float m_StretchVal;							// how stretched is this face
	float m_StretchColouring;					// 0 = red / 0.5 = green / 1.0 = blue
	int m_Region;
	

	int m_NumberOfVertices;
	int* m_pVertexIndices;
	int* m_pNormalIndices;
	int* m_pUVIndices;
	int* m_pEdgeIndices;
	int* m_pConnectedTo;

//	int* m_pEdgeSelectedList;
//	int* m_pUVSelectedList;

	int m_LineWidth;
};

#endif	//FACE_INCLUDED