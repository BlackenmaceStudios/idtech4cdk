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


// undo and redo functions..  simple as you like!!  haha

// Undo is a single undo item, a full copy of the current mesh..  big data, but reliable undo.
// UndoList is a linked list of 40 undo items..

// The 'fun' begins when unwrapping a big object in a separate thread.  This can take so much time that the undo item may be long deleted.  eeek
// To counteract this, each Undo item had an individual ID..  when the thread begins the undo list ID is stored with it, when the unwrap is complete
// we check to see if that ID still exists, if it does we copy the unwrapped data into that undolist item and all the ones to the end of the list.

//  I hope that makes sense..  it was fiddly to get right!


#include "../LSCM/MeshBuilderStdafx.h"
#include "Undo.h"
#include "RKMesh.h"

#include "MouseDefines.h"


extern Mesh gMesh;
extern int UVBankSize;
extern float* FinalUVBank;


UndoList* UndoList::Get()
{
	static UndoList* pUndoList = NULL;

	if(pUndoList == NULL)
	{
		pUndoList = new UndoList;
	}

	return(pUndoList);
}



void UndoList::CopyMesh()
{
	int ReturnVal = m_UniqueId;

	if(p_UndoRoot == NULL)
	{
		p_UndoRoot = new Undo(m_UniqueId++);
		p_UndoRoot->CopyMesh();
		p_CurrentUndo = p_UndoRoot;
	}
	else
	{
		p_CurrentUndo->NewUndo(p_CurrentUndo->m_pPrev, m_UniqueId++);
		p_CurrentUndo = p_CurrentUndo->m_pNext;
		p_CurrentUndo->CopyMesh();
	}

	int UndoListLength = CountUndos();
	if(UndoListLength == 40)						// keep the undo list manageable
	{
		RemoveFirstUndo();
	}
}



bool UndoList::CopyMeshFromID(int IDIn)
{
	Undo* pThisUndo = p_UndoRoot;
	bool OverLatest = false;

	while(pThisUndo->m_UniqueId != IDIn)
	{
		if(pThisUndo->m_pNext == NULL) return(false);		// something has gone horribly wrong!
		pThisUndo = pThisUndo->m_pNext;
	}

	while(pThisUndo->m_pNext != NULL)
	{
		pThisUndo->CopyNewUVs();
		pThisUndo = pThisUndo->m_pNext;
		if(pThisUndo == p_CurrentUndo) OverLatest = true;
	}

	if(pThisUndo == p_CurrentUndo) OverLatest = true;
	pThisUndo->CopyNewUVs();

	return(OverLatest);
}


Face* UndoList::GetFaces(int IDIn)
{
	Undo* pThisUndo = p_UndoRoot;

	while(pThisUndo->m_UniqueId != IDIn)
	{
		if(pThisUndo->m_pNext == NULL) return(NULL);		// something has gone horribly wrong!
		pThisUndo = pThisUndo->m_pNext;
	}

	return(pThisUndo->m_pFaces);
}


RKEdge* UndoList::GetEdges(int IDIn)
{
	Undo* pThisUndo = p_UndoRoot;

	while(pThisUndo->m_UniqueId != IDIn)
	{
		if(pThisUndo->m_pNext == NULL) return(NULL);		// something has gone horribly wrong!
		pThisUndo = pThisUndo->m_pNext;
	}

	return(pThisUndo->m_pEdges);
}


RKUV* UndoList::GetUVs(int IDIn)
{
	Undo* pThisUndo = p_UndoRoot;

	while(pThisUndo->m_UniqueId != IDIn)
	{
		if(pThisUndo->m_pNext == NULL) return(NULL);		// something has gone horribly wrong!
		pThisUndo = pThisUndo->m_pNext;
	}

	return(pThisUndo->m_pUVs);
}




void UndoList::RemoveFirstUndo()
{
	Undo* pNext = p_UndoRoot->m_pNext;

	p_UndoRoot->m_pNext = NULL;
	delete p_UndoRoot;

	p_UndoRoot = pNext;
	p_UndoRoot->m_pPrev = NULL;
}



int UndoList::CountUndos()
{
	int ReturnCount = 0;
	Undo* pThisUndo = p_UndoRoot;

	while(pThisUndo->m_pNext != NULL)
	{
		ReturnCount++;
		pThisUndo = pThisUndo->m_pNext;
	}

	return(ReturnCount);
}


bool UndoList::IDStillExist(int IDIn)
{
	Undo* pThisUndo = p_UndoRoot;

	while(pThisUndo->m_pNext != NULL)
	{
		if(pThisUndo->m_UniqueId == IDIn) return(true);

		pThisUndo = pThisUndo->m_pNext;
	}

	if(pThisUndo->m_UniqueId == IDIn) return(true);

	return(false);
}



void UndoList::UndoMesh()
{
	if(p_CurrentUndo == NULL) return;

	if(p_CurrentUndo->m_pPrev != NULL)
	{
		p_CurrentUndo = p_CurrentUndo->m_pPrev;
	}

	p_CurrentUndo->RestoreMesh();
}



void UndoList::RedoMesh()
{
	if(p_CurrentUndo == NULL) return;


	if(p_CurrentUndo->m_pNext != NULL)
	{
		p_CurrentUndo = p_CurrentUndo->m_pNext;
	}

	p_CurrentUndo->RestoreMesh();
}




void UndoList::ClearList()
{
	if(p_UndoRoot != NULL) delete p_UndoRoot;
	p_UndoRoot = NULL;
	p_CurrentUndo = NULL;
}







/****************************************************************************************
*
****************************************************************************************/



void Undo::RestoreMesh()
{
	delete[] RKMesh::Get()->pUVs;
	delete[] RKMesh::Get()->pFaces;

	RKMesh::Get()->m_NumberOfUVs = m_NumberOfUVs;
	RKMesh::Get()->m_NumberOfFaces = m_NumberOfFaces;
	RKMesh::Get()->m_NumberOfEdges = m_NumberOfEdges;
	RKMesh::Get()->m_Edge1 = m_Edge1;

	RKMesh::Get()->pUVs = new RKUV[m_NumberOfUVs];
	RKMesh::Get()->pFaces = new Face[m_NumberOfFaces];
	RKMesh::Get()->pEdges = new RKEdge[m_NumberOfEdges];

	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		RKMesh::Get()->pUVs[Index] = m_pUVs[Index];
	}

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		RKMesh::Get()->pFaces[Index] = m_pFaces[Index];
	}

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		RKMesh::Get()->pEdges[Index] = m_pEdges[Index];
	}

	RKMesh::Get()->SetRegions();
	RKMesh::Get()->CalcAreas();
}



void Undo::CopyMesh()
{
	m_NumberOfFaces = RKMesh::Get()->m_NumberOfFaces;
	m_NumberOfUVs = RKMesh::Get()->m_NumberOfUVs;
	m_NumberOfEdges = RKMesh::Get()->m_NumberOfEdges;
	m_Edge1 = RKMesh::Get()->m_Edge1;

	m_pFaces = new Face[m_NumberOfFaces];
	m_pUVs = new RKUV[m_NumberOfUVs];
	m_pEdges = new RKEdge[m_NumberOfEdges];


	for(int Index = 0; Index < m_NumberOfUVs; Index++)
	{
		m_pUVs[Index] = RKMesh::Get()->pUVs[Index];
	}

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		m_pFaces[Index] = RKMesh::Get()->pFaces[Index];
	}

	for(int Index = 0; Index < m_NumberOfEdges; Index++)
	{
		m_pEdges[Index] = RKMesh::Get()->pEdges[Index];
	}
}


Undo* Undo::NewUndo(Undo* pPrev, int UniqueId)
{
	if(m_pNext != NULL) delete m_pNext;


	m_pNext = new Undo(UniqueId);
	m_pNext->m_pPrev = this;
	return(m_pNext);
}


void Undo::CopyNewUVs()
{
	m_NumberOfUVs = 0;
	delete [] m_pUVs;

	m_pUVs = new RKUV[UVBankSize+1];

	for(int Index = 0; Index < UVBankSize; Index++)
	{
		m_pUVs[m_NumberOfUVs].m_U = FinalUVBank[Index*2];
		m_pUVs[m_NumberOfUVs].m_V = FinalUVBank[(Index*2)+1];
		m_pUVs[m_NumberOfUVs].m_Selected = NOTSELECTED;

		m_NumberOfUVs++;
	}


	int OutPolyIndex = 0;

	for(int Index = 0; Index < m_NumberOfFaces; Index++)
	{
		int FaceVertexCount = m_pFaces[Index].GetNumberOfVertices();
		int NextIndex = 1;

		int UV0Index = gMesh.tface[OutPolyIndex].BankIndex[0];
		int UV1Index = gMesh.tface[OutPolyIndex].BankIndex[1];
		m_pUVs[UV0Index].m_Selected = 0;
		m_pUVs[UV1Index].m_Selected = 0;

		m_pFaces[Index].SetUV(0, UV0Index);
		if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN1) 
		{
			m_pUVs[UV0Index].m_Selected = PINNED;
		}

		m_pFaces[Index].SetUV(1, UV1Index);
		if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN2) 
		{
			m_pUVs[UV1Index].m_Selected = PINNED;
		}


		for(int Index2 = 2; Index2 < FaceVertexCount; Index2++)
		{
			int UVNIndex = gMesh.tface[OutPolyIndex].BankIndex[2];
			m_pUVs[UVNIndex].m_Selected = 0;


			m_pFaces[Index].SetUV(Index2, UVNIndex);
			if(gMesh.tface[OutPolyIndex].unwrap & TF_PIN3) 
			{
				m_pUVs[UVNIndex].m_Selected = PINNED;
			}
			OutPolyIndex++;
		}
	}
}