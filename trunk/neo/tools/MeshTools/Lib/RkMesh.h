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


#ifndef RKMESH
#define RKMESH

#include "../LSCM/MeshBuilderStdafx.h"
#include "Vertex.h"
#include "Face.h"
#include "RKEdge.h"

#include "../LSCM/BDR_unwrapper.h"



class RKMesh
{
public:

	static RKMesh* Get();							// UV View is a singleton
	~RKMesh() 
	{
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

		m_NumberOfFaces = 0;
		m_NumberOfVertices = 0;
		m_NumberOfUVs = 0;
		m_NumberOfNormals = 0;
		m_NumberOfEdges = 0;
		m_NumberOfTriangulatedFaces = 0;
		m_NumberOfRegions = 0;

		m_Edge1 = -1;
		m_Edge2 = -1;



		m_Rotating = false;
	};

	void Create(int NumberOfVertices, int NumberOfFaces, int NumberOfNormals, int NumberOfUVs);
	void Delete();

	void AddVertex(float XIn, float YIn, float ZIn);
	void AddUV(float UIn, float VIn);
	void AddNormal(float NxIn, float NyIn, float NzIn);
	void AddFace(int NumberOfIndices, int* VertexIndices, int* NormalIndices, int* UVIndices, int *relUV);

	void CreateEdgeList();
	void AddEdgesToFaces();
	void FinaliseMesh();									// mesh is loaded, calc max number of UV's possible and create array
	int AddToEdgeList(int VertexIndex1, int VertexIndex2);
	void SetNewUVs();
	bool SelectFaceEdges(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool SingleSelect, bool DontDeselect, bool KeepEdge1);
	bool SelectUVEdges(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool SingleSelect, bool DontDeselect, bool KeepEdge1);
	bool FinaliseEdgeSelection();
	bool FinaliseUVSelection();
	bool FinaliseClosestSelection(bool ShiftHeld);
	bool FinaliseLoopSelection(bool SHIFTHeld);
	bool FinaliseShellSelection(bool SHIFTHeld);

	bool UVEdgeRing(bool SHIFTHeld);
	void IterateOverEdges(int SelectedUVIndex);

	void SelectFaceUVs(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect);
	void SelectUVUVs(Matrix* pMatrixIn, RECT SelectionBox, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect);

	void SelectEdge(int VertIndex1, int VertIndex2);

	bool ClosestFaceEdges();

	void PinSelectedUVs();
	void UnPinSelectedUVs();

	void GetCentreAndRadius();

	void RenderMesh(bool ShowEdge1);
	void RenderUVs(bool ShowEdge1);
	void SetSeams();
	int FindEdge(int VertIndex1, int VertIndex2);

	bool TranslateSelectedUVs(float MoveX, float MoveY);

	void StalledUnwrap();						// unwrap without threading

	void Unwrap(bool UseABF, bool FillHoles, int UndoID);
	void ThreadComplete();

	bool StartLiveUnwrap(bool UseABF, bool FillHoles);
	void LiveUnwrap();
	void EndLiveUnwrap();


	bool StartLiveStretch(bool UseABF, bool FillHoles);
	void LiveStretch(int BlendVal);
	void EndLiveStretch();

	bool StartUVRotate();
	void RotateUVs(int DeltaX, int DeltaY);
	void ScaleUVs(float CentreX, float CentreY, float Scale);
	void TransUVCentre(float DeltaX, float DeltaY);
	void ScaleUVs(int DeltaX, int DeltaY);
	void EndRotateUVs();

	int AddToCentre(float* CentreX, float* CentreY);
	void RotateUVs(float CentreX, float CentreY, Matrix* pRotMatrix);


	bool ClearEdgeSelection();
	bool ClearNewSelections();
	void ClearUVSelection(bool FullClear);

	bool NewSelectEdge(int VertexIndexIn1, int VertexIndexIn2, bool DontDeselect);
	bool IsEdgeSelected(int EdgeIndex, bool NewSelection);


	bool CheckSelectedEdges(RECT SelectionBox, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, RKEdge* pEdges, bool SHIFTHeld, bool CTRLHeld, bool OnlyClosest, float* NearestZ, int* NearestEdge);
	bool InsideOutside(float X1, float Y1, float X2, float Y2, int BoxLeft, int BoxRight, int BoxTop, int BoxBottom);
	bool CheckSelectedUVs(int BoxLeft, int BoxRight, int BoxTop, int BoxBottom, Matrix* MatrixIn, Vertex* pVertices, RKUV* pUVs, bool SHIFTHeld, bool CTRLHeld, bool DontDeselect);

	void FindCentreOfEdges(float* CentreX, float* CentreY, float* CentreZ);

	bool CutSelected();
	bool WeldSelected();

	void ReallyClearEdges();
	void CutEdge(int VertIndex1, int VertIndex2);

	void DeselectAll();

	void CalcAreas();
	void SetRegions();
	int FindUnlinkedFace();
	void SetFaceRegion(int FaceIn, int RegionNumber);

	void CreateNormals();

	bool MinMaxUVs(int MinOrMax_UOrV);
	bool RelaxUVs();
	bool UVOnEdge(int UVIndex);


	bool m_Rotating;
	float m_UVRotateCentreX;
	float m_UVRotateCentreY;
	float m_UVRotateAngle;
	float m_UVScale;

	int m_NumberOfFaces;
	int m_NumberOfVertices;
	int m_NumberOfUVs;
	int m_NumberOfNormals;
	int m_NumberOfEdges;
	int m_NumberOfTriangulatedFaces;

	

	Face* pFaces;
	Vertex* pVertices;
	Vertex* pNormals;
	RKUV* pUVs;
	RKEdge* pEdges;
	int m_Edge1;

	//KA 30/3/07: store scale and offset for restore on save

	float m_CentreX, m_CentreY, m_CentreZ;
	float m_Radius;


private:
	RKMesh() 
	{
		m_NumberOfFaces = 0;
		m_NumberOfVertices = 0;
		m_NumberOfUVs = 0;
		m_NumberOfNormals = 0;
		m_NumberOfEdges = 0;
		m_NumberOfTriangulatedFaces = 0;

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
	};

	bool UploadMeshToUnwrapper(Mesh* pMeshIn, int UndoID);
	void UploadClosestSearchMesh();
	void UpdateUVs();

	void CalcStretch(int RegionNumber);
	int m_NumberOfRegions;


	static void StartUnwrapThread(void* Param);
	int m_UnwrapID;										// the UndoList ID to unwrap to.

	HANDLE m_ThreadHandle;
	bool m_Unwrapping;
	bool m_UnwrapAgain;
	int m_UnwrapID2;									// the UndoList ID to unwrap to.

	RKUV* pUVBackup;

	int* m_FaceIndices;
	int* m_UVIndices;

	int m_Edge2;
	int m_AdjacentEdge;

};



#endif		// RKMESH