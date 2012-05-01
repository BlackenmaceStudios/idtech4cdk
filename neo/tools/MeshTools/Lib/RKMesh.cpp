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


// the main mesh functions..  far too many functions all in one file..
// if you're trying to convert this code and it goes wrong please email me and I'll try my best to explain what it does..

// SwannOTron@googlemail.com


// Real quick overview.
// A Mesh consists of Vertices, Normals, UV's and Faces all loaded from the OBJ file.
// Edges are calculated at load time.

// Blenders unwrapper code holds it's own copy of the mesh to do it's magic with.
// This is just my representation for rendering and edge / uv selection purposes.

// One final thing, int Blenders code, vertices start from number 1.  Vert 0 is used to other things.  In RK the old version this caused me headaches..
// This time I just used the same countinng method myself



#include "../LSCM/MeshBuilderStdafx.h"
#include <stdio.h>
#include <math.h>
#include "RKMesh.h"
//#include "OGLRender.h"
//#include "View3d.h"
#include "ViewUV.h"
#include "../LSCM/EdgeSelect.h"
#include "Undo.h"
#include "Matrix.h"

#include <process.h>



extern Mesh gMesh;					// the mesh blender uses to unwrap.
extern Mesh gCloseMesh;				// the mesh used to do loop and topological edge selection on..

extern int UVBankSize;				// the size of the UV bank returned from the unwrapper
extern float* FinalUVBank;			// and those UVs

extern HWND ghWnd;					// handle to window..  used for threading.  HWND also stands for Hardcore Will Never Die.. best not to ask about that!

extern bool gBackFaces;				// couple of global switches
extern bool gShowStretch;


/* *************************************************************************************************************************************
	RoadKill Mesh

	Description:
		A Road Kill Mesh

************************************************************************************************************************************** */


RKMesh* RKMesh::Get()
{
	static RKMesh* pRKMesh = NULL;

	if(pRKMesh == NULL)
	{
		pRKMesh = new RKMesh;
	}

	return(pRKMesh);
}


void RKMesh::Create(int NumberOfVertices, int NumberOfFaces, int NumberOfNormals, int NumberOfUVs)
{
	m_NumberOfFaces = 0;
	m_NumberOfVertices = 1;
	m_NumberOfNormals = 0;
	m_NumberOfUVs = 0;
	m_Edge1 = -1;

	pFaces = new Face[NumberOfFaces+1];
	pVertices = new Vertex[NumberOfVertices+1];
	pNormals = new Vertex[NumberOfNormals+1];
	pUVs = new RKUV[NumberOfUVs+1];
}


void RKMesh::Delete()
{
	m_NumberOfFaces = 0;
	m_NumberOfVertices = 1;			// but why?
	m_NumberOfNormals = 0;
	m_NumberOfUVs = 0;

	if(pFaces != NULL) delete[] pFaces;
	if(pVertices != NULL) delete[] pVertices;
	if(pNormals != NULL) delete[] pNormals;
	if(pUVs != NULL) delete[] pUVs;
	if(pEdges != NULL) delete[] pEdges;
	if(pUVBackup != NULL) delete[] pUVBackup;
	
	pFaces = NULL;
	pVertices = NULL;
	pNormals = NULL;
	pUVs = NULL;
	pEdges = NULL;
	pUVBackup = NULL;

	m_Edge1 = -1;
	m_Edge2 = -1;

	m_AdjacentEdge = -1;
	m_Unwrapping = false;

	ResetEditMesh();

	CleanUp(&gMesh);
	CleanUp(&gCloseMesh);
}



void RKMesh::AddVertex(float XIn, float YIn, float ZIn)
{
	
	pVertices[m_NumberOfVertices++].SetVertex(XIn, YIn, ZIn);
}


void RKMesh::AddNormal(float XIn, float YIn, float ZIn)
{
	pNormals[m_NumberOfNormals++].SetVertex(XIn, YIn, ZIn);
}


void RKMesh::AddUV(float UIn, float VIn)
{
	pUVs[m_NumberOfUVs].m_U = UIn;
	pUVs[m_NumberOfUVs].m_V = VIn;
	pUVs[m_NumberOfUVs].m_Selected = NOTSELECTED;

	m_NumberOfUVs++;
}


void RKMesh::AddFace(int NumberOfIndices, int* VertexIndices, int* NormalIndices, int* UVIndices, int *relUV)
{
	pFaces[m_NumberOfFaces].SetFace(NumberOfIndices, VertexIndices, NormalIndices, UVIndices);


	for(int i = 0; i < 3; i++)
		pFaces[m_NumberOfFaces].relativeUV[i] = relUV[i];

	m_NumberOfFaces++;
}


void RKMesh::AddEdgesToFaces()
{
	int* pEdgeIndices;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int NumberOfEdges = pFaces[Index].GetNumberOfVertices();

		pEdgeIndices = new int[NumberOfEdges];

		int EdgeIndex2 = NumberOfEdges-1;

		for(int EdgeIndex = 0; EdgeIndex < NumberOfEdges; EdgeIndex++)
		{
			int VertIndex1, VertIndex2;

			VertIndex1 = pFaces[Index].GetVertexIndex(EdgeIndex);
			VertIndex2 = pFaces[Index].GetVertexIndex(EdgeIndex2);

			EdgeIndex2 = EdgeIndex;
			for(int FindEdge = 0; FindEdge < m_NumberOfEdges; FindEdge++)
			{
				if(pEdges[FindEdge].SameAs(VertIndex1, VertIndex2) == true) pEdgeIndices[EdgeIndex] = FindEdge;
			}
		}

		pFaces[Index].SetEdges(pEdgeIndices);
		delete[] pEdgeIndices;
	}
}


void RKMesh::FinaliseMesh()
{
	GetCentreAndRadius();
	CreateEdgeList();								// very slow function
	//AddEdgesToFaces();								// very, very, slow function

	CleanUp(&gMesh);
	CleanUp(&gCloseMesh);

	ResetEditMesh();
	EditMeshAddVerts(m_NumberOfVertices, (float*)pVertices);
	EditMeshAddFaces(m_NumberOfFaces, pFaces);


	for(int Index = 0; Index < m_NumberOfFaces; Index++)				// very, very, very slow function
	{
		for(int Index2 = Index; Index2 < m_NumberOfFaces; Index2++)
		{
			pFaces[Index].LinkFaces(&pFaces[Index2], pEdges, Index, Index2);
		}
	}

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		pFaces[Index].TrailingEdgeCheck(pFaces, pEdges);
	}

	SetRegions();
	CalcAreas();

	UploadClosestSearchMesh();
}


void RKMesh::GetCentreAndRadius()
{
	float CentreX = 0.0f;
	float CentreY = 0.0f;
	float CentreZ = 0.0f;
	float Radius = 0.0f;

	for(int Index = 1; Index < m_NumberOfVertices; Index++)
	{
		CentreX += pVertices[Index].X;
		CentreY += pVertices[Index].Y;
		CentreZ += pVertices[Index].Z;
	}

	CentreX /= (float)m_NumberOfVertices;
	CentreY /= (float)m_NumberOfVertices;
	CentreZ /= (float)m_NumberOfVertices;


	for(int Index = 1; Index < m_NumberOfVertices; Index++)
	{
		float DistX, DistY, DistZ;
		float Result;

		DistX = CentreX - pVertices[Index].X;
		DistY = CentreY - pVertices[Index].Y;
		DistZ = CentreZ - pVertices[Index].Z;

		Result = DistX * DistX + DistY * DistY + DistZ * DistZ;
		Result = sqrt(Result);

		if(Result > Radius) Radius = Result;
	}

	//KA 30/3/07: store off centre and radius for restore on save

	m_CentreX = CentreX;
	m_CentreY = CentreY;
	m_CentreZ = CentreZ;
	m_Radius = Radius;

	for(int Index = 1; Index < m_NumberOfVertices; Index++)
	{
		pVertices[Index].X -= CentreX;					// centralise
		pVertices[Index].Y -= CentreY;
		pVertices[Index].Z -= CentreZ;

		pVertices[Index].X /= Radius;					// normalise
		pVertices[Index].Y /= Radius;
		pVertices[Index].Z /= Radius;
	}

	Radius = 1.0f;

	Vector CameraLocation;
	CameraLocation.SetVector(0, 0,  -Radius-(Radius*0.8f), 1.0f);
//	View3D::Get()->SetCameraLocation(CameraLocation);
}


void RKMesh::CreateEdgeList()
{
	int MaxNumberOfEdges = 0;
	m_NumberOfEdges = 0;
	m_NumberOfTriangulatedFaces = 0;


	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int NumberOfVerts = pFaces[Index].GetNumberOfVertices();
		MaxNumberOfEdges += NumberOfVerts;			// Number of vertices is same as number of edges
		m_NumberOfTriangulatedFaces += NumberOfVerts - 2;
	}

	pEdges = new RKEdge[MaxNumberOfEdges];

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int NumberOfVertices = pFaces[Index].GetNumberOfVertices();
		int VertexIndex1, VertexIndex2;
		int Index2 = NumberOfVertices-1;

		for(int VIndex = 0; VIndex < NumberOfVertices; VIndex++)
		{
			VertexIndex1 = pFaces[Index].GetVertexIndex(Index2);
			VertexIndex2 = pFaces[Index].GetVertexIndex(VIndex);
			int EdgeIndex = AddToEdgeList(VertexIndex1, VertexIndex2);
			pFaces[Index].SetEdgeIndex(VIndex, EdgeIndex);
			Index2 = VIndex;
		}
	}
}


int RKMesh::AddToEdgeList(int VertexIndex1, int VertexIndex2)
{
	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].SameAs(VertexIndex1, VertexIndex2) == true)
		{
			return(Index);
		}
	}

	pEdges[m_NumberOfEdges].m_VertexIndex1 = VertexIndex1;
	pEdges[m_NumberOfEdges].m_VertexIndex2 = VertexIndex2;
	pEdges[m_NumberOfEdges].m_Selected = NOTSELECTED;
	return(m_NumberOfEdges++);
}



void RKMesh::RenderMesh(bool ShowEdge1)
{
	
}



void RKMesh::RenderUVs(bool ShowEdge1)
{
	
}


bool RKMesh::ClosestFaceEdges()
{
	UploadClosestSearchMesh();
	seam_shortest_path(m_Edge1, m_Edge2);

	for(int Index = 0; Index < gCloseMesh.totedge; Index++)
	{
		if(gCloseMesh.medge[Index].flag == ME_SEAM)
		{
			int VertIndex1 = gCloseMesh.medge[Index].v1;
			int VertIndex2 = gCloseMesh.medge[Index].v2;

			SelectEdge(VertIndex1, VertIndex2);
		}
	}

	return(true);
}


// function to select edges in 3D window

bool RKMesh::SelectFaceEdges(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool SingleSelect, bool DontDeselect, bool KeepEdge1)
{
	bool EdgeSelectionChanged = false;
	float ClosestZ = 100000.0f;
	int ClosestEdge = -1;
	int BackupEdge1 = m_Edge1;


	if(SHIFTHeld == false && CTRLHeld == false) EdgeSelectionChanged |= ClearEdgeSelection();
//	if(SHIFTHeld == true && CTRLHeld == true) DontDeselect = true;

	bool Selection = false;
	float LastClosest = ClosestZ;

	CheckSelectedEdges(SelectionBox, pMatrixIn, pVertices, NULL, pEdges, SHIFTHeld, CTRLHeld, SingleSelect, &ClosestZ, &ClosestEdge);
	if(KeepEdge1 == true) m_Edge1 = BackupEdge1;

	if(SingleSelect == false) return(EdgeSelectionChanged);

	int EdgeVert1, EdgeVert2;
	EdgeVert1 = EdgeVert2 = -1;
	if(ClosestEdge != -1) 
	{
		EdgeVert1 = pEdges[ClosestEdge].m_VertexIndex1;
		EdgeVert2 = pEdges[ClosestEdge].m_VertexIndex2;

		NewSelectEdge(EdgeVert1, EdgeVert2, DontDeselect);
		m_Edge1 = ClosestEdge;
	}

	if(KeepEdge1 == true) m_Edge1 = BackupEdge1;

	return(EdgeSelectionChanged);
}


// select edges in UV window

bool RKMesh::SelectUVEdges(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool SingleSelect, bool DontDeselect, bool KeepEdge1)
{
	bool EdgeSelectionChanged = false;
	float ClosestZ = 100000.0f;
	int ClosestEdge = -1;
	int ClosestFace = -1;
	int BackupEdge1 = m_Edge1;


	for(int Index = 0; Index < m_NumberOfEdges; Index++) pEdges[Index].m_Visited = false;

	if(SHIFTHeld == false && CTRLHeld == false) EdgeSelectionChanged |= ClearEdgeSelection();

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		bool Selection = false;
		float LastClosest = ClosestZ;

		EdgeSelectionChanged |= pFaces[Index].CheckSelectedEdges(SelectionBox, pMatrixIn, NULL, pUVs, pEdges, SHIFTHeld, CTRLHeld, &m_Edge1);		// SingleSelect, &ClosestZ, &ClosestEdge);
		if(ClosestZ < LastClosest) ClosestFace = Index; 
	}

	if(KeepEdge1 == true) m_Edge1 = BackupEdge1;

	if(SingleSelect == false) return(EdgeSelectionChanged);

	int EdgeVert1, EdgeVert2;
	EdgeVert1 = EdgeVert2 = -1;
	if(ClosestEdge != -1) 
	{
		EdgeVert1 = pEdges[ClosestEdge].m_VertexIndex1;
		EdgeVert2 = pEdges[ClosestEdge].m_VertexIndex2;

		NewSelectEdge(EdgeVert1, EdgeVert2, DontDeselect);
		m_Edge1 = ClosestEdge;
	}

	if(KeepEdge1 == true) m_Edge1 = BackupEdge1;

	return(EdgeSelectionChanged);
}



// select UVs in 3D window

void RKMesh::SelectFaceUVs(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect)
{
	if(SHIFTHeld == false && CTRLHeld == false) ClearUVSelection(false);				//pFaces[Index].ClearUVSelection(false);

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		pFaces[Index].CheckSelectedUVs(SelectionBox.left, SelectionBox.right, SelectionBox.top, SelectionBox.bottom, pMatrixIn, pVertices, pUVs, SHIFTHeld, CTRLHeld, DontDeselect);
	}
}


// select UVs in UV window

void RKMesh::SelectUVUVs(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect)
{
	if(SHIFTHeld == false && CTRLHeld == false) ClearUVSelection(false);					//pFaces[Index].ClearUVSelection(false);

//	for(int Index = 0; Index < m_NumberOfFaces; Index++)
//	{
		CheckSelectedUVs(SelectionBox.left, SelectionBox.right, SelectionBox.top, SelectionBox.bottom, pMatrixIn, NULL, pUVs, SHIFTHeld, CTRLHeld, DontDeselect);
//	}
}


bool RKMesh::FinaliseClosestSelection(bool SHIFTHeld)
{
	bool AnyChanges = false;
	bool done = false;
	bool SetOne = false;


	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int VertCount = pFaces[Index].GetNumberOfVertices();

		for(int Index2 = 0; Index2 < VertCount; Index2++)
		{
			int EdgeIndex = pFaces[Index].GetEdgeIndex(Index2);
			bool Result = IsEdgeSelected(EdgeIndex, true);

			if(Result == true) 
			{
				if(done == false)
				{
					if(m_Edge1 == EdgeIndex)
					{
						m_Edge1 = -1;
						m_Edge2 = -1;
						done = true;
						SetOne = true;
					}
					else
					{
						if(SHIFTHeld == false || m_Edge1 == -1) 
						{
							m_Edge1 = EdgeIndex;
							int AdjIndex = Index2+1;
							if(AdjIndex >= pFaces[Index].GetNumberOfVertices()) AdjIndex = Index2-1;

							m_AdjacentEdge = pFaces[Index].GetEdgeIndex(AdjIndex);			//FindEdge(VertIndex1, VertIndex2);

							m_Edge2 = -1;
							done = true;
							SetOne = true;
						}
					}

					if(m_Edge2 == -1 && m_Edge1 != EdgeIndex)
					{
						if(m_Edge2 != EdgeIndex) m_Edge2 = EdgeIndex;
					}
					break;
				}
			}
			else
			{
				if(SHIFTHeld == false && m_Edge1 == EdgeIndex)
				{
					m_Edge1 = -1;
				}
			}
		}
	}

	AnyChanges |= ClearNewSelections();


	if(m_Edge1 != -1 && m_Edge2 != -1)
	{
		ClosestFaceEdges();
		m_Edge1 = m_Edge2;
		m_Edge2 = -1;
		AnyChanges = true;
	}

	if(SetOne == true) return(false);				// no need to update till we get the second edge

	return(AnyChanges);
}



bool RKMesh::FinaliseLoopSelection(bool SHIFTHeld)
{
	bool AnyChanges = false;
	bool done = false;
	bool SetOne = false;

	int WalkingEdge, AdjacentEdge;


	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int VertCount = pFaces[Index].GetNumberOfVertices();

		for(int Index2 = 0; Index2 < VertCount; Index2++)
		{
			int EdgeIndex = pFaces[Index].GetEdgeIndex(Index2);
			bool Result = IsEdgeSelected(EdgeIndex, true);

			if(Result == true) 
			{
				WalkingEdge = EdgeIndex;
				int AdjIndex = Index2+1;
				if(AdjIndex >= pFaces[Index].GetNumberOfVertices()) AdjIndex = Index2-1;

				AdjacentEdge = pFaces[Index].GetEdgeIndex(AdjIndex);
				SetOne = true;
			}
		}
	}

	AnyChanges |= ClearNewSelections();


	if(SetOne == true)
	{
		int SelectedVertIndex1, SelectedVertIndex2;
		int AdjVertIndex1, AdjVertIndex2;

		SelectedVertIndex1 = pEdges[WalkingEdge].m_VertexIndex1;
		SelectedVertIndex2 = pEdges[WalkingEdge].m_VertexIndex2;

		AdjVertIndex1 = pEdges[AdjacentEdge].m_VertexIndex1;
		AdjVertIndex2 = pEdges[AdjacentEdge].m_VertexIndex2;

		EdgeRingSelect(SelectedVertIndex1, SelectedVertIndex2, AdjVertIndex1, AdjVertIndex2, pFaces);
		AnyChanges = true;
	}

	return(AnyChanges);
}



bool RKMesh::FinaliseShellSelection(bool SHIFTHeld)
{
	if(UVEdgeRing(SHIFTHeld) == true) return(true);						// clicked on a UV on a trailing edge polygon?


	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & 0xc)
		{
			for(int FaceIndex = 0; FaceIndex < m_NumberOfFaces; FaceIndex++)
			{
				bool Result = pFaces[FaceIndex].HasUV(Index);
				if(Result == true)
				{
					int Region = pFaces[FaceIndex].GetRegion();

					for(int FaceIndex2 = 0; FaceIndex2 < m_NumberOfFaces; FaceIndex2++)
					{
						if(pFaces[FaceIndex2].GetRegion() == Region) pFaces[FaceIndex2].SelectUVs(pUVs);
					}
					return(true);
				}
			}
		}
	}

	return(false);		//AnyChanges);
}




bool RKMesh::UVEdgeRing(bool SHIFTHeld)
{
	int SelectedUVIndex;
	int StartFace = -1;
	int NewUV1 = -1;
	int NewUV2 = -1;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & 0xc)									// selected UV
		{
			SelectedUVIndex = Index;

			for(int FaceIndex = 0; FaceIndex < m_NumberOfFaces; FaceIndex++)
			{
				bool Result = pFaces[FaceIndex].HasUV(Index);
				if(Result == true)
				{
					if(pFaces[FaceIndex].UVOnPolyEdge(SelectedUVIndex, pEdges, pUVs, &NewUV1, &NewUV2) == true)
					{
						StartFace = FaceIndex;
						if(NewUV1 != -1) IterateOverEdges(NewUV1);
						if(NewUV2 != -1) IterateOverEdges(NewUV2);
						FinaliseUVSelection();
						return(true);
					}
				}
			}
		}
	}

	if(StartFace == -1) return(false);

	return(true);
}



void RKMesh::IterateOverEdges(int SelectedUVIndex)
{
	int NewUV1;
	int NewUV2;

	for(int FaceIndex = 0; FaceIndex < m_NumberOfFaces; FaceIndex++)
	{
		bool Result = pFaces[FaceIndex].HasUV(SelectedUVIndex);
		if(Result == true)
		{
			if(pFaces[FaceIndex].UVOnPolyEdge(SelectedUVIndex, pEdges, pUVs, &NewUV1, &NewUV2) == true)
			{
				if(NewUV1 != -1) IterateOverEdges(NewUV1);
				if(NewUV2 != -1) IterateOverEdges(NewUV2);
			}
		}
	}
}





void RKMesh::StalledUnwrap()
{
	if(m_NumberOfFaces == 0) return;
	m_UnwrapID = UndoList::Get()->GetLatestID();
	UploadMeshToUnwrapper(&gMesh, m_UnwrapID);

	select_linked_tfaces_with_seams(2, 0);
	unwrap_lscm(0);

	bool Result = UndoList::Get()->IDStillExist(m_UnwrapID);			// has this Undo item been deleted?

	if(Result == true)
	{
		bool OverLatest = UndoList::Get()->CopyMeshFromID(m_UnwrapID);
		SetNewUVs();

		delete[] m_FaceIndices;
		delete[] m_UVIndices;
	}
	
	SetRegions();
}


void RKMesh::Unwrap(bool UseABF, bool FillHoles, int UndoID)
{
	if(m_NumberOfFaces == 0) return;

	if(m_Unwrapping == true) 
	{
		m_UnwrapAgain = true;		// another call to unwrap while the second thread is still busy
		m_UnwrapID2 = UndoID;
		return;
	}

	m_UnwrapID = UndoID;			//UndoList::Get()->GetLatestID();
	m_Unwrapping = true;
	UploadMeshToUnwrapper(&gMesh, m_UnwrapID);		//UndoList::Get()->GetLatestID());

	//m_ThreadHandle = CreateThread(NULL, 0, (unsigned long (__stdcall *)(void *))this->StartUnwrapThread, (void *)this, 0, NULL);
	select_linked_tfaces_with_seams(2, 0);
	unwrap_lscm(0);

	RKMesh::Get()->ThreadComplete();
}


void RKMesh::StartUnwrapThread(void* Param)
{
	
//	PostMessage(ghWnd, ID_THREAD_FINISHED, 0, 0);
}




void RKMesh::ThreadComplete()
{
//	CloseHandle(m_ThreadHandle);

	bool Result = UndoList::Get()->IDStillExist(m_UnwrapID);			// has this Undo item been deleted?

	if(Result == true)
	{
		bool OverLatest = UndoList::Get()->CopyMeshFromID(m_UnwrapID);
		SetNewUVs();

		//if(UndoList::Get()->GetLatestID() == m_UnwrapID) SetNewUVs();
		delete[] m_FaceIndices;
		delete[] m_UVIndices;
	}

	SetRegions();
	CalcAreas();

	m_Unwrapping = false;
	if(m_UnwrapAgain == true)
	{
		m_UnwrapAgain = false;
		Unwrap(0,0, m_UnwrapID2);
	}
}



bool RKMesh::StartLiveUnwrap(bool UseABF, bool FillHoles)
{
	if(m_Unwrapping == true) return(false);

	UploadMeshToUnwrapper(&gMesh, UndoList::Get()->GetLatestID());
	m_UVRotateAngle = 0;
	m_UVScale = 1.0f;

	select_linked_tfaces_with_seams(2, 0);
	unwrap_lscm_live_begin();
	return(true);
}



void RKMesh::LiveUnwrap()
{
	MeshUpdateUVs(m_NumberOfTriangulatedFaces, m_FaceIndices, m_UVIndices, (float*)pUVs);
	unwrap_lscm_live_re_solve();

	UpdateUVs();
}


void RKMesh::EndLiveUnwrap()
{
	unwrap_lscm_live_end(false);

	UpdateUVs();

	delete[] m_FaceIndices;
	delete[] m_UVIndices;
}



bool RKMesh::StartLiveStretch(bool UseABF, bool FillHoles)
{
	if(m_Unwrapping == true) return(false);

	UploadMeshToUnwrapper(&gMesh, UndoList::Get()->GetLatestID());
	select_linked_tfaces_with_seams(2, 0);
	StartStretch();
	return(true);
}


void RKMesh::LiveStretch(int BlendVal)
{
	DoStretch(BlendVal);
	UpdateUVs();
}


void RKMesh::EndLiveStretch()
{
	EndStretch(true);
	UpdateUVs();
	delete[] m_FaceIndices;
	delete[] m_UVIndices;
}


void RKMesh::UploadClosestSearchMesh()
{
	MeshAddVerts(m_NumberOfVertices, (float*)pVertices, &gCloseMesh);			// vert 0 is not used
	MeshAddEdges(m_NumberOfEdges, pEdges, &gCloseMesh, true);

	int* pFaceIndices = new int[m_NumberOfTriangulatedFaces*3];
	int* pUVIndices = new int[m_NumberOfTriangulatedFaces*3];			// send any pinned UV's indices in here.  Not pinned = -1
	int* pPinnedUVs = new int[m_NumberOfTriangulatedFaces*3];
	int OutIndex = 0;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int FaceVertexCount = pFaces[Index].GetNumberOfVertices();
		int NextIndex = 1;

		for(int Index2 = 0; Index2 < FaceVertexCount-2; Index2++)
		{
			pFaceIndices[OutIndex] = pFaces[Index].GetVertexIndex(0);				//vertList[0] + 1;						// triangulate the polygons
			pFaceIndices[OutIndex+1] = pFaces[Index].GetVertexIndex(NextIndex);		//vertList[NextIndex] + 1;
			pFaceIndices[OutIndex+2] = pFaces[Index].GetVertexIndex(NextIndex+1);	//vertList[NextIndex+1] + 1;

			int UVIndex1, UVIndex2, UVIndex3;
			UVIndex1 = UVIndex2 = UVIndex3 = -1;
			pPinnedUVs[OutIndex] = 0;
			pPinnedUVs[OutIndex+1] = 0;
			pPinnedUVs[OutIndex+2] = 0;

			UVIndex1 = pFaces[Index].GetUVIndex(0);
			UVIndex2 = pFaces[Index].GetUVIndex(NextIndex);
			UVIndex3 = pFaces[Index].GetUVIndex(NextIndex+1);

			if(pUVs[UVIndex1].m_Selected & PINNED) pPinnedUVs[OutIndex] = 1;
			if(pUVs[UVIndex2].m_Selected & PINNED) pPinnedUVs[OutIndex+1] = 1;
			if(pUVs[UVIndex3].m_Selected & PINNED) pPinnedUVs[OutIndex+2] = 1;

			pUVIndices[OutIndex++] = UVIndex1;
			pUVIndices[OutIndex++] = UVIndex2;
			pUVIndices[OutIndex++] = UVIndex3;

			NextIndex++;
		}
	}

	MeshAddFaces(m_NumberOfTriangulatedFaces, pFaceIndices, pUVIndices, (float*)pUVs, pPinnedUVs, &gCloseMesh);

	delete[] pFaceIndices;
	delete[] pUVIndices;
	delete[] pPinnedUVs;
}



bool RKMesh::UploadMeshToUnwrapper(Mesh* pMeshIn, int UndoID)
{

	if(UndoList::Get()->IDStillExist(UndoID) == false) return(false);
	RKEdge* pEdgeList = UndoList::Get()->GetEdges(UndoID);
	Face* pFaceList = UndoList::Get()->GetFaces(UndoID);
	RKUV* pUVList = UndoList::Get()->GetUVs(UndoID);


	MeshAddVerts(m_NumberOfVertices, (float*)pVertices, pMeshIn);			// vert 0 is not used
	MeshAddEdges(m_NumberOfEdges, pEdgeList, pMeshIn, false);


	m_FaceIndices = new int[m_NumberOfTriangulatedFaces*3];
	m_UVIndices = new int[m_NumberOfTriangulatedFaces*3];			// send any pinned UV's indices in here.  Not pinned = -1
	int* PinnedUVs = new int[m_NumberOfTriangulatedFaces*3];
	int OutIndex = 0;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int FaceVertexCount = pFaceList[Index].GetNumberOfVertices();
		int NextIndex = 1;

		for(int Index2 = 0; Index2 < FaceVertexCount-2; Index2++)
		{
			m_FaceIndices[OutIndex] = pFaceList[Index].GetVertexIndex(0);				//vertList[0] + 1;						// triangulate the polygons
			m_FaceIndices[OutIndex+1] = pFaceList[Index].GetVertexIndex(NextIndex);		//vertList[NextIndex] + 1;
			m_FaceIndices[OutIndex+2] = pFaceList[Index].GetVertexIndex(NextIndex+1);	//vertList[NextIndex+1] + 1;

			int UVIndex1, UVIndex2, UVIndex3;
			UVIndex1 = UVIndex2 = UVIndex3 = -1;
			PinnedUVs[OutIndex] = 0;
			PinnedUVs[OutIndex+1] = 0;
			PinnedUVs[OutIndex+2] = 0;

			UVIndex1 = pFaceList[Index].GetUVIndex(0);
			UVIndex2 = pFaceList[Index].GetUVIndex(NextIndex);
			UVIndex3 = pFaceList[Index].GetUVIndex(NextIndex+1);

			if(pUVList[UVIndex1].m_Selected & PINNED) PinnedUVs[OutIndex] = 1;
			if(pUVList[UVIndex2].m_Selected & PINNED) PinnedUVs[OutIndex+1] = 1;
			if(pUVList[UVIndex3].m_Selected & PINNED) PinnedUVs[OutIndex+2] = 1;

			if(pUVList[UVIndex1].m_Selected & SELECTED) PinnedUVs[OutIndex] = 1;
			if(pUVList[UVIndex2].m_Selected & SELECTED) PinnedUVs[OutIndex+1] = 1;
			if(pUVList[UVIndex3].m_Selected & SELECTED) PinnedUVs[OutIndex+2] = 1;

			m_UVIndices[OutIndex++] = UVIndex1;
			m_UVIndices[OutIndex++] = UVIndex2;
			m_UVIndices[OutIndex++] = UVIndex3;

			NextIndex++;
		}
	}

	MeshAddFaces(m_NumberOfTriangulatedFaces, m_FaceIndices, m_UVIndices, (float*)pUVList, PinnedUVs, pMeshIn);
	delete[] PinnedUVs;

	return(true);
}


void RKMesh::UpdateUVs()
{
	int OutPolyIndex = 0;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int FaceVertexCount = pFaces[Index].GetNumberOfVertices();
		int NextIndex = 1;

		int UVIndex1 = pFaces[Index].GetUVIndex(0);
		int UVIndex2 = pFaces[Index].GetUVIndex(1);

		pUVs[UVIndex1].m_U = gMesh.tface[OutPolyIndex].uv[0][0];
		pUVs[UVIndex1].m_V = gMesh.tface[OutPolyIndex].uv[0][1];

		pUVs[UVIndex2].m_U = gMesh.tface[OutPolyIndex].uv[1][0];
		pUVs[UVIndex2].m_V = gMesh.tface[OutPolyIndex].uv[1][1];
//		if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN2) pUVs[UVIndex2].m_Selected = PINNED;

		for(int Index2 = 2; Index2 < FaceVertexCount; Index2++)
		{
			int UVIndexN = pFaces[Index].GetUVIndex(Index2);

			pUVs[UVIndexN].m_U = gMesh.tface[OutPolyIndex].uv[2][0];
			pUVs[UVIndexN].m_V = gMesh.tface[OutPolyIndex].uv[2][1];
//			if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN3) pUVs[UVIndexN].m_Selected = PINNED;

			OutPolyIndex++;
		}
	}
}


void RKMesh::SetNewUVs()
{
	m_NumberOfUVs = 0;
	delete [] pUVs;

	pUVs = new RKUV[UVBankSize+1];

	for(int Index = 0; Index < UVBankSize; Index++)
	{
		AddUV(FinalUVBank[Index*2], FinalUVBank[(Index*2)+1]);
	}

	int OutPolyIndex = 0;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int FaceVertexCount = pFaces[Index].GetNumberOfVertices();
		int NextIndex = 1;

		int UV0Index = gMesh.tface[OutPolyIndex].BankIndex[0];
		int UV1Index = gMesh.tface[OutPolyIndex].BankIndex[1];
		pUVs[UV0Index].m_Selected = NOTSELECTED;
		pUVs[UV1Index].m_Selected = NOTSELECTED;

		pFaces[Index].SetUV(0, UV0Index);
		if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN1) pUVs[UV0Index].m_Selected |= PINNED;

		pFaces[Index].SetUV(1, UV1Index);
		if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN2) pUVs[UV1Index].m_Selected |= PINNED;


		for(int Index2 = 2; Index2 < FaceVertexCount; Index2++)
		{
			int UVNIndex = gMesh.tface[OutPolyIndex].BankIndex[2];
			pUVs[UVNIndex].m_Selected = NOTSELECTED;

			pFaces[Index].SetUV(Index2, UVNIndex);
			if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN3) pUVs[UVNIndex].m_Selected |= PINNED;
			OutPolyIndex++;
		}
	}
}



int RKMesh::FindEdge(int VertIndex1, int VertIndex2)
{
	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_VertexIndex1 == VertIndex1 && pEdges[Index].m_VertexIndex2 == VertIndex2) return(Index);
		if(pEdges[Index].m_VertexIndex1 == VertIndex2 && pEdges[Index].m_VertexIndex2 == VertIndex1) return(Index);
	}

	return(-1);
}



void RKMesh::SelectEdge(int VertIndex1, int VertIndex2)
{
	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].SameAs(VertIndex1, VertIndex2) == true)
		{
			pEdges[Index].m_Selected &= 0xf0;
			pEdges[Index].m_Selected |= SELECTED;
		}
	}
}


bool RKMesh::StartUVRotate()
{
	int NumberSelected = 0;
	float CentreX, CentreY;
	CentreX = CentreY = 0.0f;
	m_Rotating = false;

	NumberSelected = AddToCentre(&CentreX, &CentreY);
	if(NumberSelected == 0) return(false);


	CentreX /= (float)NumberSelected;
	CentreY /= (float)NumberSelected;

	m_UVRotateCentreX = CentreX;
	m_UVRotateCentreY = CentreY;
	m_UVRotateAngle = 0;
	m_Rotating = true;

	return(true);
}


void RKMesh::RotateUVs(int DeltaX, int DeltaY)
{
	Matrix RotMatrix;

	if(pUVBackup == NULL)
	{
		pUVBackup = new RKUV[m_NumberOfUVs];

		for(int Index = 0; Index < m_NumberOfUVs; Index++)
		{
			pUVBackup[Index] = pUVs[Index];
		}
	}

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		pUVs[Index] = pUVBackup[Index];
	}

	if(m_Rotating == true)
	{
		int absDeltaX, absDeltaY;

		absDeltaX = abs(DeltaX);
		absDeltaY = abs(DeltaY);

		if(absDeltaX > absDeltaY)
		{
			m_UVRotateAngle += (float)(DeltaX)*0.33f;
		}
		else
		{
			m_UVRotateAngle += (float)(DeltaY)*0.33f;
		}
	}

	RotMatrix.SetZRotation(m_UVRotateAngle);
	RotateUVs(m_UVRotateCentreX, m_UVRotateCentreY, &RotMatrix);

	LiveUnwrap();
}


void RKMesh::EndRotateUVs()
{
	delete[] pUVBackup;
	pUVBackup = NULL;
}



void RKMesh::ScaleUVs(int DeltaX, int DeltaY)
{
	Matrix RotMatrix;

	if(pUVBackup == NULL)
	{
		pUVBackup = new RKUV[m_NumberOfUVs];

		for(int Index = 0; Index < m_NumberOfUVs; Index++)
		{
			pUVBackup[Index] = pUVs[Index];
		}
	}

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		pUVs[Index] = pUVBackup[Index];
	}

	if(m_Rotating == true)
	{
		int absDeltaX, absDeltaY;

		absDeltaX = abs(DeltaX);
		absDeltaY = abs(DeltaY);

		if(absDeltaX > absDeltaY)
		{
			m_UVScale += (float)(DeltaX)*0.005f;
		}
		else
		{
			m_UVScale += (float)(DeltaY)*0.005f;
		}
	}

	ScaleUVs(m_UVRotateCentreX, m_UVRotateCentreY, m_UVScale);

	LiveUnwrap();
}




void RKMesh::TransUVCentre(float DeltaX, float DeltaY)
{
	m_UVRotateCentreX += DeltaX;
	m_UVRotateCentreY += DeltaY;
}


void RKMesh::DeselectAll()
{
	ClearEdgeSelection();
	ClearUVSelection(true);
	FinaliseEdgeSelection();
	FinaliseUVSelection();

	m_Edge1 = -1;
	m_Edge2 = -1;
}





void RKMesh::PinSelectedUVs()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)
		{
			pUVs[Index].m_Selected |= PINNED;
			AnyChanges = true;
		}
	}

	if(AnyChanges == true) UndoList::Get()->CopyMesh();
}



void RKMesh::UnPinSelectedUVs()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED && pUVs[Index].m_Selected & PINNED)
		{
			pUVs[Index].m_Selected &= 0xf;
			AnyChanges = true;
		}
	}

	if(AnyChanges == true) UndoList::Get()->CopyMesh();
}



bool RKMesh::ClearEdgeSelection()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_Selected & SELECTED) 
		{
			AnyChanges = true;
			pEdges[Index].m_Selected |= CLEARED;
		}

		pEdges[Index].m_Selected &= 0xf0;
		pEdges[Index].m_Selected |= NOTSELECTED;
	}

	return(AnyChanges);
}




bool RKMesh::ClearNewSelections()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if((pEdges[Index].m_Selected & 0xc) != 0) AnyChanges = true;
		pEdges[Index].m_Selected &= 0xf3;
	}

	return(AnyChanges);
}



void RKMesh::ClearUVSelection(bool FullClear)
{
	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED) pUVs[Index].m_Selected |= CLEARED;

		if(FullClear == true)
		{
			pUVs[Index].m_Selected &= 0xffe0;
			pUVs[Index].m_Selected |= NOTSELECTED;
		}
		else
		{
			pUVs[Index].m_Selected &= 0xfff0;
			pUVs[Index].m_Selected |= NOTSELECTED;
		}
	}
}






bool RKMesh::TranslateSelectedUVs(float MoveX, float MoveY)
{
	bool ReturnVal = false;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)			// && pUVs[Index].m_Selected & PINNED)
		{
			if(!(pUVs[Index].m_Selected & PINNED))
			{
				pUVs[Index].m_U += MoveX;
				pUVs[Index].m_V += MoveY;
				ReturnVal = true;
			}
		}
	}
	
	if(ReturnVal == true) LiveUnwrap();		//(true, true);

	return(ReturnVal);
}




int RKMesh::AddToCentre(float* CentreX, float* CentreY)
{
	int SelectedCount = 0;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)		// && pUVs[Index].m_Selected & PINNED)
		{
			if(!(pUVs[Index].m_Selected & PINNED))
			{
				*CentreX += pUVs[Index].m_U;
				*CentreY += pUVs[Index].m_V;
				SelectedCount++;
			}
		}
	}
	
	return(SelectedCount);
}




void RKMesh::RotateUVs(float CentreX, float CentreY, Matrix* pRotMatrix)
{
	Vertex VertIn, VertOut;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)			// && pUVs[Index].m_Selected & PINNED)
		{
			if(!(pUVs[Index].m_Selected & PINNED))
			{
				VertIn.X = pUVs[Index].m_U - CentreX;
				VertIn.Y = pUVs[Index].m_V - CentreY;
				VertIn.Z = 0.0f;

				pRotMatrix->RotateVertex(&VertIn, &VertOut);

				pUVs[Index].m_U = VertOut.X + CentreX;
				pUVs[Index].m_V = VertOut.Y + CentreY;
			}
		}
	}
}



void RKMesh::ScaleUVs(float CentreX, float CentreY, float Scale)
{
	Vertex VertIn, VertOut;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)			// && pUVs[Index].m_Selected & PINNED)
		{
			if(!(pUVs[Index].m_Selected & PINNED))
			{
				VertIn.X = pUVs[Index].m_U - CentreX;
				VertIn.Y = pUVs[Index].m_V - CentreY;
				VertIn.Z = 0.0f;

				VertIn.X *= Scale;
				VertIn.Y *= Scale;

				pUVs[Index].m_U = VertIn.X + CentreX;
				pUVs[Index].m_V = VertIn.Y + CentreY;
			}
		}
	}
}





bool RKMesh::FinaliseEdgeSelection()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_Selected > 2)
		{
			if(pEdges[Index].m_Selected & NEWSELECTED)
			{
				if(pEdges[Index].m_Selected & NOTSELECTED)
				{
					pEdges[Index].m_Selected &= 0xf0;
					pEdges[Index].m_Selected |= SELECTED;
					if((pEdges[Index].m_Selected & CLEARED) == 0) AnyChanges = true;
					pEdges[Index].m_Selected &= 0x3f;
				}
				else
				{
					pEdges[Index].m_Selected &= 0xf0;
					pEdges[Index].m_Selected |= NOTSELECTED;
					if((pEdges[Index].m_Selected & CLEARED) != 0) AnyChanges = true;
					pEdges[Index].m_Selected &= 0x3f;
				}
			}

			if(pEdges[Index].m_Selected & NEWNOTSELECTED)
			{
				if(pEdges[Index].m_Selected & NOTSELECTED)
				{
					pEdges[Index].m_Selected &= 0xf0;
					pEdges[Index].m_Selected |= SELECTED;
					if((pEdges[Index].m_Selected & CLEARED) == 0)
					{
						AnyChanges = true;
						pEdges[Index].m_Selected &= 0x3f;
					}

				}
				else
				{
					pEdges[Index].m_Selected &= 0xf0;
					pEdges[Index].m_Selected |= NOTSELECTED;
					if((pEdges[Index].m_Selected & CLEARED) != 0)
					{
						AnyChanges = true;
						pEdges[Index].m_Selected &= 0x3f;
					}
				}
			}
		}

		if((pEdges[Index].m_Selected & CLEARED) != 0)
		{
			AnyChanges = true;
			pEdges[Index].m_Selected &= 0x3f;
		}
	}

	return(AnyChanges);
}



bool RKMesh::FinaliseUVSelection()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & NEWSELECTED)
		{
			if(pUVs[Index].m_Selected & NOTSELECTED)
			{
				pUVs[Index].m_Selected &= 0xfffffff0;
				pUVs[Index].m_Selected |= SELECTED;
				if((pUVs[Index].m_Selected & CLEARED) == 0) AnyChanges = true;
				pUVs[Index].m_Selected &= 0x3f;
			}
			else
			{
				pUVs[Index].m_Selected &= 0xfffffff0;
				pUVs[Index].m_Selected |= NOTSELECTED;
				if((pUVs[Index].m_Selected & CLEARED) != 0) AnyChanges = true;
				pUVs[Index].m_Selected &= 0x3f;
			}
		}

		if(pUVs[Index].m_Selected & NEWNOTSELECTED)
		{
			if(pUVs[Index].m_Selected & NOTSELECTED)
			{
				pUVs[Index].m_Selected &= 0xfffffff0;
				pUVs[Index].m_Selected |= SELECTED;
				if((pUVs[Index].m_Selected & CLEARED) == 0)
				{
					AnyChanges = true;
					pUVs[Index].m_Selected &= 0x3f;
				}
			}
			else
			{
				pUVs[Index].m_Selected &= 0xfffffff0;
				pUVs[Index].m_Selected |= NOTSELECTED;
				if((pUVs[Index].m_Selected & CLEARED) != 0)
				{
					AnyChanges = true;
					pUVs[Index].m_Selected &= 0x3f;
				}
			}
		}
		if((pUVs[Index].m_Selected & CLEARED) != 0)
		{
			AnyChanges = true;
			pUVs[Index].m_Selected &= 0x3f;
		}
	}

	return(AnyChanges);
}



bool RKMesh::NewSelectEdge(int VertexIndexIn1, int VertexIndexIn2, bool DontDeselect)
{
	bool AnyChanges = false;


	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].SameAs(VertexIndexIn1, VertexIndexIn2) == true)
		{
			pEdges[Index].m_Selected &= 0xf3;
			pEdges[Index].m_Selected |= NEWSELECTED;
//			pEdges[Index].m_Selected &= ~NEWNOTSELECTED;
			AnyChanges |= true;

			if(DontDeselect == true)
			{
				pEdges[Index].m_Selected &= 0xf0;
				pEdges[Index].m_Selected |= NOTSELECTED | NEWSELECTED;
//				pEdges[Index].m_Selected |= NEWNOTSELECTED;
			}

		}
		else
		{
			if(pEdges[Index].m_Selected & NEWSELECTED)
			{
				pEdges[Index].m_Selected &= 0xf3;
				pEdges[Index].m_Selected |= NEWNOTSELECTED;
//				pEdges[Index].m_Selected &= ~NEWSELECTED;
				AnyChanges |= true;
			}
		}
	}

	return(AnyChanges);
}



bool RKMesh::IsEdgeSelected(int EdgeIndex, bool NewSelection)
{
	bool Selection = false;


	if(NewSelection == true)
	{
		if((pEdges[EdgeIndex].m_Selected & NEWSELECTED) && (pEdges[EdgeIndex].m_Selected & NOTSELECTED)) Selection = true;
		if((pEdges[EdgeIndex].m_Selected & NEWNOTSELECTED) && (pEdges[EdgeIndex].m_Selected & SELECTED)) Selection = true;
		if(Selection == false) return(false);
	}
	else
	{
		if(pEdges[EdgeIndex].m_Selected != SELECTED) return(false);
	}

	return(true);
}







bool RKMesh::CheckSelectedEdges(RECT SelectionBox, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, RKEdge* pEdges, bool SHIFTHeld, bool CTRLHeld, bool OnlyClosest, float* NearestZ, int* NearestEdge)
{
	

	return(false);
}




bool RKMesh::InsideOutside(float X1, float Y1, float X2, float Y2, int BoxLeft, int BoxRight, int BoxTop, int BoxBottom)
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



bool RKMesh::CheckSelectedUVs(int BoxLeft, int BoxRight, int BoxTop, int BoxBottom, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect)
{
	

	return(false);
}



bool RKMesh::CutSelected()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_Selected & SELECTED && ((pEdges[Index].m_Selected & EDGECUT) == 0))
		{
			pEdges[Index].m_Selected |= EDGECUT;
			AnyChanges = true;
		}
	}


	if(AnyChanges == true)
	{
		UndoList::Get()->CopyMesh();
		Unwrap(0,0, UndoList::Get()->GetLatestID());
	}

	return(AnyChanges);
}



bool RKMesh::WeldSelected()
{
	bool AnyChanges = false;

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_Selected & SELECTED && pEdges[Index].m_Selected & EDGECUT)
		{
			pEdges[Index].m_Selected &= 0xf;
			AnyChanges = true;
		}
	}

	if(AnyChanges == true)
	{
		UndoList::Get()->CopyMesh();
		Unwrap(0,0, UndoList::Get()->GetLatestID());
	}

	return(AnyChanges);
}



void RKMesh::CalcStretch(int RegionNumber)
{
	float AverageRatio = 0.0f;
	float ShrunkLimit, StretchLimit;
	int Index;
	int FaceCount = 0;

	for(Index = 0; Index < m_NumberOfFaces; Index++)
	{
		if(pFaces[Index].GetRegion() == RegionNumber)
		{
			AverageRatio += pFaces[Index].GetStretchVal();
			FaceCount++;
		}
	}

	AverageRatio /= (float)FaceCount;
	ShrunkLimit = AverageRatio - AverageRatio * 0.8f;
	StretchLimit = AverageRatio + AverageRatio * 0.8f;

	for(Index = 0; Index < m_NumberOfFaces; Index++)
	{
		if(pFaces[Index].GetRegion() == RegionNumber)
		{
			pFaces[Index].AssignStretch(ShrunkLimit, StretchLimit);
		}
	}
}


void RKMesh::CalcAreas()
{
	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		pFaces[Index].AreaCalc(pVertices, pUVs);
	}

	for(int Index = 0; Index < m_NumberOfRegions; Index++)
	{
        CalcStretch(Index);
	}
}




void RKMesh::SetRegions()
{
	m_NumberOfRegions = 0;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		pFaces[Index].SetRegion(-1);
	}

	int StartFace = FindUnlinkedFace();

	while(StartFace != -1)
	{
		SetFaceRegion(StartFace, m_NumberOfRegions);

		m_NumberOfRegions++;
		StartFace = FindUnlinkedFace();
	}
}




int RKMesh::FindUnlinkedFace()
{
	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		if(pFaces[Index].GetRegion() == -1) return(Index);
	}

	return(-1);
}



void RKMesh::SetFaceRegion(int FaceIn, int RegionNumber)
{
	pFaces[FaceIn].SetRegion(RegionNumber);

	for(int EdgeIndex = 0; EdgeIndex < pFaces[FaceIn].GetNumberOfVertices(); EdgeIndex++)
	{
		int ConnectedFace = pFaces[FaceIn].GetConnectedFace(EdgeIndex, pFaces, pEdges);
		if(ConnectedFace != -1) SetFaceRegion(ConnectedFace, RegionNumber);
	}
}



void RKMesh::FindCentreOfEdges(float* CentreX, float* CentreY, float* CentreZ)
{
	int Count = 0;
	float XAcc, YAcc, ZAcc;
	XAcc = YAcc = ZAcc = 0.0f;

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_Selected & SELECTED)
		{
			int VertIndex1 = pEdges[Index].m_VertexIndex1;
			int VertIndex2 = pEdges[Index].m_VertexIndex2;

			XAcc += pVertices[VertIndex1].X;
			YAcc += pVertices[VertIndex1].Y;
			ZAcc += pVertices[VertIndex1].Z;

			XAcc += pVertices[VertIndex2].X;
			YAcc += pVertices[VertIndex2].Y;
			ZAcc += pVertices[VertIndex2].Z;

			Count += 2;
		}
	}

	*CentreX = XAcc / (float)Count;
	*CentreY = YAcc / (float)Count;
	*CentreZ = ZAcc / (float)Count;
}



void RKMesh::ReallyClearEdges()
{
	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		pEdges[Index].m_Selected = NOTSELECTED;
	}
}


void RKMesh::CutEdge(int VertIndex1, int VertIndex2)
{
	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		if(pEdges[Index].m_VertexIndex1 == VertIndex1 && pEdges[Index].m_VertexIndex2 == VertIndex2)
		{
			pEdges[Index].m_Selected = EDGECUT | NOTSELECTED;
		}

		if(pEdges[Index].m_VertexIndex1 == VertIndex2 && pEdges[Index].m_VertexIndex2 == VertIndex1)
		{
			pEdges[Index].m_Selected = EDGECUT | NOTSELECTED;
		}
	}
}


void RKMesh::CreateNormals()
{
	m_NumberOfNormals = m_NumberOfFaces;
	if(pNormals != NULL) delete[] pNormals;
	pNormals = new Vertex[m_NumberOfNormals+1];

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		pFaces[Index].CreateNormal(pNormals, pVertices, Index);
	}
}



bool RKMesh::MinMaxUVs(int MinOrMax_UOrV)
{
	float MinU, MinV, MaxU, MaxV;
	MinU = MinV = 10000.0f;
	MaxU = MaxV = -10000.0f;

	bool Found = false;

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)
		{
			if(!(pUVs[Index].m_Selected & PINNED))
			{
				Found = true;

				if(pUVs[Index].m_U < MinU) MinU = pUVs[Index].m_U;
				if(pUVs[Index].m_V < MinV) MinV = pUVs[Index].m_V;

				if(pUVs[Index].m_U > MaxU) MaxU = pUVs[Index].m_U;
				if(pUVs[Index].m_V > MaxV) MaxV = pUVs[Index].m_V;
			}
		}
	}

	if(Found == false) return(false);


	StartLiveUnwrap(false, false);

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		if(pUVs[Index].m_Selected & SELECTED)
		{
			if(!(pUVs[Index].m_Selected & PINNED))
			{
				if(MinOrMax_UOrV == MINU) pUVs[Index].m_U = MinU;
				if(MinOrMax_UOrV == MAXU) pUVs[Index].m_U = MaxU;
				if(MinOrMax_UOrV == MINV) pUVs[Index].m_V = MinV;
				if(MinOrMax_UOrV == MAXV) pUVs[Index].m_V = MaxV;
			}
		}
	}

	LiveUnwrap();
	EndLiveUnwrap();
	UndoList::Get()->CopyMesh();

	return(true);
}




bool RKMesh::RelaxUVs()
{
	int ConnectedUVs[50];					  // more than 50 connected UVs..  kill all artists!
	int ConnectedCount = 0;

	int NewUVCount = 0;
	int* NewUVList;
	float* NewUList;
	float* NewVList;


	NewUVList = new int[m_NumberOfUVs];
	NewUList = new float[m_NumberOfUVs];
	NewVList = new float[m_NumberOfUVs];

	for(int UVIndex = 0; UVIndex < m_NumberOfUVs; UVIndex++)
	{
		if(pUVs[UVIndex].m_Selected & SELECTED)
		{
			if(!(pUVs[UVIndex].m_Selected & PINNED))
			{
				if(UVOnEdge(UVIndex) == false)
				{
					ConnectedCount = 0;

					for(int FaceIndex = 0; FaceIndex < m_NumberOfFaces; FaceIndex++)
					{
						int Connected1, Connected2;
						bool RetVal;

						RetVal = pFaces[FaceIndex].GetConnectedUVs(UVIndex, Connected1, Connected2);

						if(RetVal == true)
						{
							for(int NewUVIndex = 0; NewUVIndex < ConnectedCount; NewUVIndex++)
							{
								if(ConnectedUVs[NewUVIndex] == Connected1) goto Sodit1;
							}
							ConnectedUVs[ConnectedCount++] = Connected1;
	Sodit1:

							for(int NewUVIndex = 0; NewUVIndex < ConnectedCount; NewUVIndex++)
							{
								if(ConnectedUVs[NewUVIndex] == Connected2) goto Sodit2;
							}
							ConnectedUVs[ConnectedCount++] = Connected2;
	Sodit2:
							int a = 0;
							a++;
						}
					}

					NewUVList[NewUVCount] = UVIndex;

					float AccU, AccV;
					AccU = AccV = 0.0f;
					for(int Count = 0; Count < ConnectedCount; Count++)
					{
						AccU += pUVs[ConnectedUVs[Count]].m_U;
						AccV += pUVs[ConnectedUVs[Count]].m_V;
					}
					AccU /= (float)ConnectedCount;
					AccV /= (float)ConnectedCount;

					float DeltaU = AccU - pUVs[UVIndex].m_U;
					float DeltaV = AccV - pUVs[UVIndex].m_V;

					NewUList[NewUVCount] = pUVs[UVIndex].m_U += DeltaU * 0.6f;			// 10%
					NewVList[NewUVCount] = pUVs[UVIndex].m_V += DeltaV * 0.6f;			// 10%

					NewUVCount++;
				}
			}
		}
	}

	for(int Index = 0; Index < NewUVCount; Index++)
	{
		int UVIndex = NewUVList[Index];

		pUVs[UVIndex].m_U = NewUList[Index];
		pUVs[UVIndex].m_V = NewVList[Index];
	}


	delete[] NewUVList;
	delete[] NewUList;
	delete[] NewVList;

	return(true);
}




bool RKMesh::UVOnEdge(int UVIndex)
{
	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		if(pFaces[Index].UVOnEdge(UVIndex, pEdges) == true) return(true);
	}

	return(false);
}

