// FbxStaticImport.cpp
//

#include "precompiled.h"

#include "FbxExport_Local.h"

/*
==================
WriteMD5StaticMesh
==================
*/
bool WriteMD5StaticMesh( const char *meshpath ) {
	idFile *file;

	// Open the mesh for writing
	common->Printf("Writing %s\n", meshpath );
	file = fileSystem->OpenFileWrite( meshpath, "fs_devpath" );
	if(!file) {
		common->Warning("Failed to open %s for writing.\n", meshpath );
		return  false;
	}

	file->WriteFloatString( MD5_VERSION_STRING " %d\n", MD5_VERSION );
	file->WriteFloatString( "numMeshes %d\n", fbxInterface.meshes.Num() );
	file->WriteFloatString( "numVertexes %d\n", fbxInterface.verts.Num() );
	file->WriteFloatString( "numIndexes %d\n\n", fbxInterface.indexes.Num() );

	// Write the vertexes
	file->WriteFloatString( "vertexes {\n" );
		for( int i = 0; i < fbxInterface.verts.Num(); i++ ) {
			file->WriteFloatString( "\tvert %f %f %f\n", fbxInterface.verts[i].xyz[0], fbxInterface.verts[i].xyz[1], fbxInterface.verts[i].xyz[2] );
			file->WriteFloatString( "\tst %f %f\n", fbxInterface.verts[i].st[0], fbxInterface.verts[i].st[1] );

			if(fbxInterface.verts[i].normal.IsInfinite()) {
				common->Warning("Invalid normal detected...defaulting\n");
				file->WriteFloatString( "\tnormal %f %f %f\n", 0, 0, 0 );
			}
			else
			{
				file->WriteFloatString( "\tnormal %f %f %f\n", fbxInterface.verts[i].normal[0], fbxInterface.verts[i].normal[1], fbxInterface.verts[i].normal[2] );
			}
		}
	file->WriteFloatString( "}\n\n" );

	// Write the indexes
	file->WriteFloatString( "tris {\n" );
		for( int i = 0; i < fbxInterface.indexes.Num(); i+=3 ) {
			file->WriteFloatString( "\ttri %d %d %d\n", fbxInterface.indexes[i + 0], fbxInterface.indexes[i + 1], fbxInterface.indexes[i + 2] );
		}
	file->WriteFloatString( "}\n\n" );

	// Write the meshes
	
	for( int i = 0; i < fbxInterface.meshes.Num(); i++ ) {
		file->WriteFloatString( "mesh %d {\n", i );

		file->WriteFloatString( "\tshader %s\n", /*fbxInterface.meshes[i].material.c_str()*/ "worlddefault" );
		file->WriteFloatString( "\tstartVertex %d\n", fbxInterface.meshes[i].startVertex );
		file->WriteFloatString( "\tnumVertexes %d\n", fbxInterface.meshes[i].numVertexes );

		file->WriteFloatString( "\tstartIndex %d\n", fbxInterface.meshes[i].startIndex );
		file->WriteFloatString( "\tnumIndexes %d\n", fbxInterface.meshes[i].numIndexes );

		file->WriteFloatString( "}\n\n" );
	}
	

	// Close the file
	fileSystem->CloseFile( file );

	return true;
}

/*
==================
FbxExportToStaticMesh
==================
*/
void FbxExportToStaticMesh( const char *meshpath, idStr &outFilePath ) {
	// Refresh all the console stuff immediatly
	common->SetRefreshOnPrint( true );

	// Init the fbk sdk.
	fbxInterface.Init();
	if(!fbxInterface.IsSdkStarted()) {
		common->SetRefreshOnPrint( false );
		return;
	}

	// Load in the FBX into the scene.
	if(!fbxInterface.LoadFBX( meshpath)) {
		common->SetRefreshOnPrint( false );
		return;
	}
	
	outFilePath = fileSystem->OSPathToRelativePath( meshpath );
	outFilePath.SetFileExtension( MD5_STATIC_EXT );


	if(fbxInterface.ImportError()) {
		common->SetRefreshOnPrint( false );
		return;
	}

	if(!WriteMD5StaticMesh( outFilePath.c_str() )) {
		common->SetRefreshOnPrint( false );
		return;
	}

	

	// Reload the mesh.
	idRenderModel *model = renderModelManager->FindModel( outFilePath.c_str() );
	if(model == NULL)
	{
		common->Warning("There was a error with the mesh import, check the log\n");
	}
	else
	{
		common->Printf("Updating Model Cache...\n");
		model->LoadModel();
		renderModelManager->ReloadModels();
		common->Printf("Mesh imported successfully\n");
	}

	// Disable refresh on print
	common->SetRefreshOnPrint( false );
}