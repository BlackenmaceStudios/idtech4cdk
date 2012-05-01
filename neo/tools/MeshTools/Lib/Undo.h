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


#ifndef UNDO
#define UNDO

#include "../LSCM/MeshBuilderStdafx.h"
#include "Vertex.h"
#include "Face.h"


class Undo
{
public:

	Undo(int UniqueId) 
	{
		m_NumberOfFaces = 0;
		m_NumberOfUVs = 0;
		m_pFaces = NULL;
		m_pUVs = NULL;
		m_pEdges = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_UniqueId = UniqueId;
	};

	~Undo() 
	{
		if(m_pNext != NULL) delete m_pNext;

		if(m_pFaces != NULL) delete[] m_pFaces;
		if(m_pUVs != NULL) delete[] m_pUVs;
		if(m_pEdges != NULL) delete[] m_pEdges;
//		if(m_pNext != NULL) delete(m_pNext);
		
		m_NumberOfFaces = 0;
		m_NumberOfUVs = 0;
		m_pFaces = NULL;
		m_pUVs = NULL;
		m_pEdges = NULL;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_UniqueId = -1;
	};



	
	void RestoreMesh();
	void CopyMesh();
	Undo* NewUndo(Undo* pPrev, int UniqueId);
	void CopyNewUVs();

	int m_NumberOfFaces;
	int m_NumberOfUVs;
	int m_NumberOfEdges;

	int m_UniqueId;

	Face* m_pFaces;
	RKUV* m_pUVs;
	RKEdge* m_pEdges;
	int m_Edge1;

	Undo* m_pNext;
	Undo* m_pPrev;


private:
};


class UndoList
{
public:
	static UndoList* Get();							// UndoList is a singleton

	void CopyMesh();
	void UndoMesh();
	void RedoMesh();
	void ClearList();
	bool CopyMeshFromID(int IDIn);

	Face* GetFaces(int IDIn);			// get the faces from this undo ID
	RKEdge* GetEdges(int IDIn);			// get the edges from this undo ID
	RKUV* GetUVs(int IDIn);				// get the UV's from this undo ID

	int GetLatestID() { return(p_CurrentUndo->m_UniqueId); }
	bool IDStillExist(int IDIn);
    
	void Delete()
	{
		if(p_UndoRoot != NULL) delete p_UndoRoot;
		p_UndoRoot = NULL;
		p_CurrentUndo = NULL;
		m_UniqueId = 0;
	}

private:

	UndoList()
	{
		p_UndoRoot = NULL;
		p_CurrentUndo = NULL;
		m_UniqueId = 0;
	}

	~UndoList()
	{
		Delete();
	}



	int CountUndos();
	void RemoveFirstUndo();
	int m_UniqueId;

	Undo* p_UndoRoot;
	Undo* p_CurrentUndo;
};


#endif