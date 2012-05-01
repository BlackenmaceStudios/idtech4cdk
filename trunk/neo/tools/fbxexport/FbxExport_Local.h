// FbxExport_Local.h
//

#pragma once

// ---- Stuff to avoid conflict with fbx sdk ----
#undef DotProduct
#undef snprintf
#undef stricmp
#undef strnicmp
#define use_idStr_Icmp stricmp
#include <fbxsdk.h>
// -----------------------------------------------

#include "utility\Common.h"
#include "utility\AnimationUtility.h"
#include "utility\GeometryUtility.h"

// -----------------------------------------------

struct bmFbxMesh {
	idStr			name;

	int				startVertex;
	int				numVertexes;

	int				startIndex;
	int				numIndexes;
	idStr			material;
};

//
// FbxInterface
//
class FbxInterface {
public:
					FbxInterface();
	void			Init( void );
	bool			LoadFBX( const char *fileName );
	bool			IsSdkStarted() { return lSdkManager != NULL && lScene != NULL; }
	bool			ImportError() { return importError; }
	void			ImportError( const char *fmt, ... );
	idList<bmFbxMesh> meshes;
	idList<idDrawVert> verts;
	idList<glIndex_t> indexes;
private:
	bmFbxMesh		*AllocMesh( const char *name );

	void			ParseMeshGeometry(KFbxMesh* pMesh, bmFbxMesh *Mesh);
	void			ParseMeshTexture( KFbxGeometry* pGeometry, bmFbxMesh *bmMesh );
	void			ParseMesh( KFbxNode* pNode );
	void			ParseNode( KFbxNode* pNode );
	void  FbxInterface::LoadUVInformation(KFbxMesh* pMesh,  bmFbxMesh *Mesh);

	KFbxSdkManager* lSdkManager;
    KFbxScene* lScene;

	bool importError;
};


/*
================
FbxInterface::AllocMesh
================
*/
ID_INLINE bmFbxMesh	*FbxInterface::AllocMesh( const char *name ) {
	int meshHandle;
	bmFbxMesh	mesh;

	meshHandle = meshes.Num();
	mesh.name = name;

	mesh.startVertex = verts.Num();
	mesh.startIndex = indexes.Num();

	meshes.Append( mesh );
	return &meshes[ meshHandle ];
}

extern FbxInterface fbxInterface;