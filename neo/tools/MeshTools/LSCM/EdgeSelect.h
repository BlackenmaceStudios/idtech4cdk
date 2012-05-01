#ifndef EDGESELECT_H
#define EDGESELECT_H

#include "BLI_editVert.h"
#include "DNA_listBase.h"
#include "../lib/Face.h"


typedef struct PathNode
{
	int u;
	int visited;
	ListBase edges;
} PathNode;


typedef struct PathEdge
{
	struct PathEdge *next, *prev;
	int v;
	float w;
} PathEdge;


int seam_shortest_path(int source, int target);


void EditMeshAddVerts(int NumberVerts, float* Vertices);
void EditMeshAddFaces(int NumberFaces, Face* pFaces);
void EdgeRingSelect(int SelectedEdgeVert1, int SelectedEdgeVert2, int AdjEdgeVert1, int AdjEdgeVert2, Face* pFaces);

void ResetEditMesh();


#endif