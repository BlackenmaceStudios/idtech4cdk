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
	materials.Clear();
	areas.Clear();

	numNodeAreas = 1;
}

void bmVTModel::AllocTriangleAtPosition( int triId ) {
	srfTriangles_t	*uTri = R_AllocStaticTriSurf();

	tris.Insert( uTri, triId );
	areas.Insert( 0, triId );
	materials.Insert( "worlddefault", triId );
}

void bmVTModel::FreeTri( int triId ) { 
	
	tris.RemoveIndex( triId ); 
	materials.RemoveIndex( triId );
	areas.RemoveIndex( triId );
	R_FreeStaticTriSurf( tris[triId] ); 
}

void bmVTModel::SetVertexesForTris( srfTriangles_t	*uTri, idDrawVert *verts, int numVerts, int *indexes, int numIndexes ) {

	R_AllocStaticTriSurfIndexes( uTri, numIndexes );
	R_AllocStaticTriSurfVerts( uTri, numVerts );

	for(int i = 0; i < numIndexes; i+=3)
	{
		uTri->indexes[i+0] = indexes[i+0];
		uTri->indexes[i+1] = indexes[i+1];
		uTri->indexes[i+2] = indexes[i+2];
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

int bmVTModel::NumTrisForArea( int areaNum ) {
	int numTrisInArea = 0;
	for(int d = 0; d < tris.Num(); d++)
	{
		if(tris[d]->vt_AreaID == areaNum)
			numTrisInArea++;
	}

	return numTrisInArea;
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

		
		if(tri->vt_AreaID == -1)
			continue;

		for ( int i = 0 ; i < tri->numVerts; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "v %f %f %f\n", dv->xyz.x * 0.30, dv->xyz.z * 0.30, -dv->xyz.y * 0.30 );
		}


	}

	for(int d = 0; d < tris.Num(); d++)
	{
		const srfTriangles_t *tri = tris[d];
		idDrawVert *Vertexes = tri->verts;
		glIndex_t *indicies = tri->indexes;

		if(tri->vt_AreaID == -1)
			continue;


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
		
		if(tri->vt_AreaID == -1)
			continue;

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
	
		
		if(tri->vt_AreaID == -1)
			continue;

		for ( int i = 0 ; i < tri->numIndexes; i+=3 ) {
			objf->WriteFloatString( "f " );

			if(indicies[i+0] < 0 || indicies[i+1] < 0 || indicies[i+2] < 0) {
				common->FatalError("There was a problem with the generated model, check the source art or call a programmer\n");
			}


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