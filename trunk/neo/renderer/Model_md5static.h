// Model_md5static.h
//

//
// bmMD5StaticMesh
//
struct bmMD5StaticMesh {
	idStr			name;

	int				startVertex;
	int				numVertexes;

	int				startIndex;
	int				numIndexes;
	idStr			material;
};

//
// bmMD5StaticModel
//
struct bmMD5StaticModel {
	idList<bmMD5StaticMesh> meshes;
	idList<idDrawVert> verts;
	idList<glIndex_t> indexes;

	int NumMeshes() { return meshes.Num(); }
};

//
// MD5Static_LoadModel
//
bool MD5Static_LoadModel( const char *name, bmMD5StaticModel &model );