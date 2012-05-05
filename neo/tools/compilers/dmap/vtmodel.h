// outexporter.h
//

//
// bmVTModel
//
class bmVTModel {
public:
							bmVTModel() { Init(); }
	void					Init( void );
	void					AddTris( const srfTriangles_t *uTris );
	void					AllocTriangleAtPosition( int triId );
	void					SetVertexesForTris( int triId, idDrawVert *verts, int numVerts, int *indexes, int numIndexes );
	void					FreeTri( int triId );
	void WriteToFile( const char *file ) ;
	int						NumTrisForArea( int areaNum );

	idList<srfTriangles_t*> tris;
	int						numAreas;
};

