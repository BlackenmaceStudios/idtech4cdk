// vtmodel.cpp
//

#include "precompiled.h"

#include "dmap.h"
#include "vtmodel.h"

void bmVTModel::Init( void ) {
	for(int i = 0; i < tris.Num(); i++)
	{
		if(!tris[i]->nextDeferredFree)
		{
			R_FreeStaticTriSurf( tris[i] );
		}
	}
	tris.Resize( 0 );
}

void bmVTModel::AllocTriangleAtPosition( int triId ) {
	srfTriangles_t	*uTri = R_AllocStaticTriSurf();

	tris.Insert( uTri, triId );
}

void bmVTModel::FreeTri( int triId ) { 
	R_FreeStaticTriSurf( tris[triId] ); 
	tris.Remove( tris[triId] ); 
}

void bmVTModel::SetVertexesForTris( int triId, idDrawVert *verts, int numVerts, int *indexes, int numIndexes ) {
	srfTriangles_t	*uTri = tris[triId];

	R_AllocStaticTriSurfIndexes( uTri, numIndexes );
	R_AllocStaticTriSurfVerts( uTri, numVerts );

	for(int i = 0; i < numIndexes; i++)
	{
		uTri->indexes[i] = indexes[i];
	}

	for(int i = 0; i < numVerts; i++)
	{
		uTri->verts[i] = verts[i];
	}

	uTri->numVerts = numVerts;
	uTri->numIndexes = numIndexes;
}

void bmVTModel::AddTris( const srfTriangles_t *uTris ) {
	

	tris.Append( (srfTriangles_t *)uTris );
}

void bmVTModel::WriteToFile( const char *file ) {
	idFile *objf = NULL;
	

	int objFaceNum  = 0;

	
	objf = fileSystem->OpenFileWrite(va( "%s.obj", file, tris[0]->vt_AreaID), "fs_basepath" );
	if(!objf) {
		common->FatalError( "Failed to write obj");
	}

	for(int d = 0; d < tris.Num(); d++)
	{
		const srfTriangles_t *tri = tris[d];
		idDrawVert *Vertexes = tri->verts;
		glIndex_t *indicies = tri->indexes;



		for ( int i = 0 ; i < tri->numVerts; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "v %f %f %f\n", dv->xyz.x, dv->xyz.z, -dv->xyz.y );
		}


	}

	for(int d = 0; d < tris.Num(); d++)
	{
		const srfTriangles_t *tri = tris[d];
		idDrawVert *Vertexes = tri->verts;
		glIndex_t *indicies = tri->indexes;


		for ( int i = 0 ; i <  tri->numVerts ; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "vt %f %f\n", dv->st.x + tri->vt_AreaID,  dv->st.y);
		}
	
	}

	for(int d = 0; d < tris.Num(); d++)
	{
		const srfTriangles_t *tri = tris[d];
		idDrawVert *Vertexes = tri->verts;
		glIndex_t *indicies = tri->indexes;


		for ( int i = 0 ; i <  tri->numVerts ; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "vn %f %f %f\n", dv->normal.x, dv->normal.y, dv->normal.z );
		}

	}

	for(int d = 0; d < tris.Num(); d++)
	{
		const srfTriangles_t *tri = tris[d];
		idDrawVert *Vertexes = tri->verts;
		glIndex_t *indicies = tri->indexes;
	
		for ( int i = 0 ; i < tri->numIndexes; i+=3 ) {
			objf->WriteFloatString( "f " );


			objf->WriteFloatString( "%i", objFaceNum + indicies[i+0] + 1 );
			objf->WriteFloatString( "/%i", objFaceNum + indicies[i+0] + 1 );
			objf->WriteFloatString( "/%i", objFaceNum + indicies[i+0] + 1 );

			objf->WriteFloatString( " ", objFaceNum  );

			objf->WriteFloatString( "%i", objFaceNum + indicies[i+1] + 1 );
			objf->WriteFloatString( "/%i", objFaceNum + indicies[i+1] + 1 );
			objf->WriteFloatString( "/%i", objFaceNum + indicies[i+1] + 1 );

			objf->WriteFloatString( " ", objFaceNum  );

			objf->WriteFloatString( "%i", objFaceNum + indicies[i+2] + 1 );
			objf->WriteFloatString( "/%i", objFaceNum + indicies[i+2] + 1 );
			objf->WriteFloatString( "/%i", objFaceNum + indicies[i+2] + 1 );
			objf->WriteFloatString( "\n" );
		}

		objFaceNum += tri->numVerts;
	
	}

	if(objf != NULL) {
		fileSystem->CloseFile( objf );
	}
}