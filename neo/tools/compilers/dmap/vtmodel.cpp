// vtmodel.cpp
//

#include "precompiled.h"

#include "dmap.h"
#include "vtmodel.h"

void bmVTModel::Init( void ) {

	tris.Resize( 0 );
}

void bmVTModel::AddTris( const srfTriangles_t *uTris ) {
	

	tris.Append( *uTris );
}

void bmVTModel::WriteToFile( const char *file ) {
	idFile *objf = NULL;
	
	int lastVTArea = -1;
	int objFaceNum  = 0;
	int objIndxNum  = 0;
	
	

	for(int d = 0; d < tris.Num(); d++)
	{
		const srfTriangles_t *tri = &tris[d];
		idDrawVert *Vertexes = tri->verts;
		glIndex_t *indicies = tri->indexes;

		// Open a new file for writing if we are in a different area. 
		if(lastVTArea == -1 /*!= tris[d].vt_AreaID*/) {
			if(objf != NULL) {
				fileSystem->CloseFile( objf );
			}
			
			objf = fileSystem->OpenFileWrite(va( "%s_area.obj", file, tris[d].vt_AreaID), "fs_basepath" );
			if(!objf) {
				common->FatalError( "Failed to write obj");
			}

			objFaceNum = 0;
			objIndxNum = 0;

			lastVTArea = tris[d].vt_AreaID;
		}

		for ( int i = 0 ; i < tri->numVerts; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "v %f %f %f\n", dv->xyz.x, dv->xyz.z, -dv->xyz.y );
		}

		for ( int i = 0 ; i <  tri->numVerts ; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "vt %f %f\n", dv->st.x + tri->vt_AreaID, dv->st.y);
		}

		for ( int i = 0 ; i <  tri->numVerts ; i++ ) {
			const idDrawVert *dv;

			dv = &Vertexes[i];

			objf->WriteFloatString( "vn %f %f %f\n", dv->normal.x, dv->normal.y, dv->normal.z );
		}
	
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
		objIndxNum += tri->numIndexes;
	}
	if(objf != NULL) {
		fileSystem->CloseFile( objf );
	}
}