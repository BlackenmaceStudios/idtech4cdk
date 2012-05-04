// MeshTools_UVAtlas.cpp
//

#include "MeshTools_UVAtlas.h"
#include "../compilers/dmap/vtmodel.h"

#include "Lib\Undo.h"
/*
=================
idToolInterfaceLocal::BuildUVAtlasInternalModel
=================
*/
void idToolInterfaceLocal::BuildUVAtlasInternalModel( class bmVTModel *model, int startTris, int numTri ) {
	int startVertex = 0;
	int numVertexes = 0;
	int numIndexes = 0;
	int numTris = 0;
	int numTris2 = 0;
	for(int i = startTris; i < startTris + numTri; i++)
	{
		srfTriangles_t *tri = model->tris[i];

		numVertexes += model->tris[i]->numVerts;
		numIndexes += model->tris[i]->numIndexes;

		for(int ind = 0; ind < tri->numIndexes; ind+=3, numTris++) {

		}
	}


	RKMesh::Get()->Create( numVertexes, numTris, numVertexes, numVertexes );
	int indexes[7], indexes2[7], indexes3[7];
	startVertex = 0;
	for(int i = startTris; i <startTris + numTri; i++)
	{
		srfTriangles_t *tri = model->tris[i];

		for(int v = 0; v < tri->numVerts; v++)
		{
			idVec3 vert = tri->verts[v].xyz;

			RKMesh::Get()->AddVertex(vert.x, vert.y, vert.z);

		}

		for(int v = 0; v < tri->numVerts; v++)
		{
			idVec3 normal = tri->verts[v].normal;
			RKMesh::Get()->AddNormal(normal.x, normal.y, normal.z);
		}

		for(int v = 0; v < tri->numVerts; v++)
		{
			idVec2 st = tri->verts[v].st;
			RKMesh::Get()->AddUV(st.x, st.y);
		}

		for(int ind = 0; ind < tri->numIndexes; ind+=3, numTris2++)
		{
			
			indexes[0] = indexes2[0] = tri->indexes[ind] + startVertex;
			indexes[1] = indexes2[1] = tri->indexes[ind+1]  + startVertex;
			indexes[2] = indexes2[2] = tri->indexes[ind+2]  + startVertex;

			indexes[0] += 1;
			indexes[1] += 1;
			indexes[2] += 1;

			indexes3[0] = i;
			indexes3[1] = ind;
			indexes3[2] = startVertex; //tri->indexes[ind+2];

			RKMesh::Get()->AddFace( 3, &indexes[0], &indexes2[0], &indexes2[0], &indexes3[0] );
		}

		startVertex += tri->numVerts;
	}

	RKMesh::Get()->CreateNormals();
}

/*
=================
idToolInterfaceLocal::SetUVsFromInternalUVAtlas
=================
*/
void idToolInterfaceLocal::SetUVsFromInternalUVAtlas( class bmVTModel *model ) {
	int startVertex = 0;
	int triNum = 0;
	int highUVNum = 0;
	for(int i = 0; i < RKMesh::Get()->m_NumberOfFaces; i++)
	{
		Face *face = &RKMesh::Get()->pFaces[i];

		int triNum = face->relativeUV[0];
		srfTriangles_t *tri = model->tris[triNum];

		int *indexes = &tri->indexes[face->relativeUV[1]];
		idVec2 stupidHack;
		for(int t = 0; t < face->GetNumberOfVertices(); t++,indexes++)
		{
			int vertIndex = face->GetVertexIndex(t);
			int uvIndex = face->GetUVIndex(t);
			if(highUVNum < uvIndex) {
				highUVNum = uvIndex;
			}

			

			float X = RKMesh::Get()->pVertices[vertIndex].X;
			float Y = RKMesh::Get()->pVertices[vertIndex].Y;
			float Z = RKMesh::Get()->pVertices[vertIndex].Z;

			// RK Normalises the vertex data
			// so I've stored off the scale and offset values as
			// properties of the RKMesh class.
			// and undone the changes prior to writing.
			// 
			X *= RKMesh::Get()->m_Radius;
			Y *= RKMesh::Get()->m_Radius;
			Z *= RKMesh::Get()->m_Radius;

			X += RKMesh::Get()->m_CentreX;
			Y += RKMesh::Get()->m_CentreY;
			Z += RKMesh::Get()->m_CentreZ;
			
			//tri->verts[vertIndex-1-face->relativeUV[2]].xyz = idVec3(X,Y,Z);
			tri->verts[vertIndex-1-face->relativeUV[2]].st.x = RKMesh::Get()->pUVs[uvIndex].m_U;
			tri->verts[vertIndex-1-face->relativeUV[2]].st.y = RKMesh::Get()->pUVs[uvIndex].m_V;

		}
	}

	
}

/*
=================
 idToolInterfaceLocal::ComputeUVAtlasForModel
=================
*/
void  idToolInterfaceLocal::ComputeUVAtlasForModel( bmVTModel *model, int startTris, int numTris ) {
	RKMesh::Get()->Delete();

	// Build the internal model were going to use to build everything.
	BuildUVAtlasInternalModel( model, startTris, numTris );


	// Add a default UV
	//RKMesh::Get()->AddUV(0.0f, 0.0f);

	// Build the mesh.
	RKMesh::Get()->FinaliseMesh();
	UndoList::Get()->CopyMesh();
	

	// Unwrap the mesh.
	RKMesh::Get()->Unwrap( 0, 0, UndoList::Get()->GetLatestID()); 


	// Copy the new uvs to the mesh.
	SetUVsFromInternalUVAtlas( model );


	// Free the mesh.
	UndoList::Get()->ClearList();
	RKMesh::Get()->Delete();
}