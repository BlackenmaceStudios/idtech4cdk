// FbxStaticImport.cpp
//

#include "precompiled.h"

#include "FbxExport_Local.h"

//
// OBJParseFace
//
bool OBJParseFace( idParser &parser, idList<int> &indexes, idList<int> &uvindexes, idList<int> &normalindexes )
{
	idToken token;
	int index=0, uvindex=0, normalindex=0;

	index = parser.ParseInt();
	if(!parser.ExpectTokenString( "/" ))
	{
		common->Warning("Failed to parse face, more than likely this means your mesh isn't triangulated\n");
	}
	uvindex = parser.ParseInt();
	if(!parser.ExpectTokenString( "/" ))
	{
		common->Warning("Failed to parse face, more than likely this means your mesh isn't triangulated\n");
	}
	normalindex = parser.ParseInt();

	if(index <= 0 || uvindex <= 0 || normalindex <= 0) {
		common->Warning("Face with illegal  index\n");
		return false;
	}

	indexes.Append( index - 1 );
	uvindexes.Append( uvindex - 1 );
	normalindexes.Append( normalindex - 1 );

	return true;
}

/*
==================
ImportOBJStaticMesh

This is nasty needs to be fixed and moved to its own home.
==================
*/
bool ImportOBJStaticMesh( const char *meshpath ) {
	idToken token;
	bmFbxMesh mesh;
	idList<idVec3> verts;
	idList<idVec2> uvs;
	idList<idVec3> normals;
	idList<int>    uvindexes;
	idList<int>    normalindexes;
	idParser parser( LEXFL_NOSTRINGESCAPECHARS | LEXFL_IGNOREDIRECTIVES );

	fbxInterface.meshes.Clear();
	fbxInterface.verts.Clear();
	fbxInterface.indexes.Clear();

	// Load the obj into the parser.
	parser.LoadFile( meshpath );
	if(!parser.IsLoaded()) {
		common->Warning("Failed to load %s for reading\n", meshpath);
		return false;
	}

	// Parse the file
	while(parser.ReadToken( &token ))
	{
		if(!token.Icmp( "mtllib" ) || !token.Icmp( "usemtl")) {
			parser.SkipRestOfLine();
		}
		else if(!token.Icmp( "g" )) {
			parser.SkipRestOfLine();
		}
		else if(!token.Icmp( "s" )) {
			parser.SkipRestOfLine();
		}
		else if(!token.Icmp( "v" )) {
			idVec3 xyz;

			xyz.x = parser.ParseFloat();
			xyz.z = parser.ParseFloat();
			xyz.y = -parser.ParseFloat();

			verts.Append( xyz );
		}
		else if(!token.Icmp( "vt" )) {
			idVec2 st;

			st.x = parser.ParseFloat();
			st.y = 1.0f - parser.ParseFloat();
			

			uvs.Append( st );
		}
		else if(!token.Icmp( "vn" )) {
			idVec3 normal;

			normal.x = parser.ParseFloat();
			normal.y = parser.ParseFloat();
			normal.z = parser.ParseFloat();

			normals.Append( normal );
		}
		else if(!token.Icmp( "f" )) {
			for(int i = 0; i < 3; i++)
			{
				if(!OBJParseFace(parser, fbxInterface.indexes, uvindexes, normalindexes )) {
					
					return false;
				}
			}
		}
		else {
			common->Warning("Illegal or unexpected token %s\n", token.c_str() );
			return false;
		}
	}

	for(int i = 0; i < verts.Num(); i++)
	{
		idDrawVert v;
		fbxInterface.verts.Append( v );
	}

	for(int i = 0; i < fbxInterface.indexes.Num(); i++)
	{
		int ind = fbxInterface.indexes[i];
		int uvi = uvindexes[i];
		int normi = normalindexes[i];

		idDrawVert *v = &fbxInterface.verts[ind];

		v->xyz = verts[ind];
		v->st = uvs[uvi];
		v->normal = normals[normi];
	}

	

	mesh.startVertex = 0;
	mesh.numVertexes = fbxInterface.verts.Num();

	mesh.startIndex = 0;
	mesh.numIndexes = fbxInterface.indexes.Num();

	fbxInterface.meshes.Append( mesh );

	return true;
}

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
			file->WriteFloatString( "\ttri %d %d %d\n", fbxInterface.indexes[i + 2], fbxInterface.indexes[i + 1], fbxInterface.indexes[i + 0] );
		}
	file->WriteFloatString( "}\n\n" );

	// Write the meshes
	
	for( int i = 0; i < fbxInterface.meshes.Num(); i++ ) {
		file->WriteFloatString( "mesh %d {\n", i );

		if(strstr(fbxInterface.meshes[i].material.c_str(), "modeldefault"))
		{
			file->WriteFloatString( "\tshader %s\n", "modeldefault" );
		}
		else
		{
			if(strlen(fbxInterface.meshes[i].material.c_str()) <= 0)
			{
				common->Warning("No default material used - using default...\n");
				file->WriteFloatString( "\tshader %s\n", "modeldefault" );
			}
			else
			{
				file->WriteFloatString( "\tshader %s\n", fbxInterface.meshes[i].material.c_str() );
			}
		}
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
RefreshCachedModel
==================
*/
void RefreshCachedModel( idStr &outFilePath ) {
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
		common->Printf("Mesh imported successfully\n");
	}
}

/*
==================
ObjExportToStaticMesh
==================
*/
bool ObjExportToStaticMesh( const char *meshpath, idStr &outFilePath ) {
	// Refresh all the console stuff immediatly
	common->SetRefreshOnPrint( true );

	// Import the obj.
	if(!ImportOBJStaticMesh( fileSystem->OSPathToRelativePath(meshpath) )) {
		common->SetRefreshOnPrint( false );
		return false;
	}

	// Build the output path.
	outFilePath = fileSystem->OSPathToRelativePath( meshpath );
	outFilePath.SetFileExtension( MD5_STATIC_EXT );

	// Write out the mesh.
	if(!WriteMD5StaticMesh( outFilePath.c_str() )) {
		common->SetRefreshOnPrint( false );
		return false;
	}

	// Refresh the cache.
	RefreshCachedModel( outFilePath );

	// Disable refresh on print
	common->SetRefreshOnPrint( false );

	return true;
}

/*
==================
FbxExportToStaticMesh
==================
*/
bool FbxExportToStaticMesh( const char *meshpath, idStr &outFilePath ) {
	// Refresh all the console stuff immediatly
	common->SetRefreshOnPrint( true );

	// Init the fbk sdk.
	fbxInterface.Init();
	if(!fbxInterface.IsSdkStarted()) {
		common->SetRefreshOnPrint( false );
		return false;
	}

	// Load in the FBX into the scene.
	if(!fbxInterface.LoadFBX( meshpath)) {
		common->SetRefreshOnPrint( false );
		return false;
	}
	
	outFilePath = fileSystem->OSPathToRelativePath( meshpath );
	outFilePath.SetFileExtension( MD5_STATIC_EXT );


	if(fbxInterface.ImportError()) {
		common->SetRefreshOnPrint( false );
		return false;
	}

	if(!WriteMD5StaticMesh( outFilePath.c_str() )) {
		common->SetRefreshOnPrint( false );
		return false;
	}

	// Refresh the model in the importer.
	RefreshCachedModel( outFilePath );
	

	// Disable refresh on print
	common->SetRefreshOnPrint( false );

	return true;
}