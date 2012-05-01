// Model_md5static.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"
#include "Model_local.h"
#include "Model_md5static.h"

//
// MD5Static_LoadModel
//
bool MD5Static_LoadModel( const char *name, bmMD5StaticModel &model ) {
	int version;
	idToken		token;
	idLexer		parser( LEXFL_ALLOWPATHNAMES | LEXFL_NOSTRINGESCAPECHARS );

	if ( !parser.LoadFile( name ) ) {
		common->Warning( "MD5Static_LoadModel: %s not found\n", name );
		return false;
	}

	// Check the version string.
	parser.ExpectTokenString( MD5_VERSION_STRING );
	version = parser.ParseInt();

	if ( version != MD5_VERSION ) {
		parser.Error( "Invalid version %d.  Should be version %d\n", version, MD5_VERSION );
	}

	// Load in the model header.
	parser.ExpectTokenString( "numMeshes" );
	model.meshes.SetNum( parser.ParseInt() );

	parser.ExpectTokenString( "numVertexes" );
	model.verts.SetNum( parser.ParseInt() );

	parser.ExpectTokenString( "numIndexes" );
	model.indexes.SetNum( parser.ParseInt() );

	// Model lists make sure they were allocated correctly.
	if(model.meshes.Num() <= 0 || model.verts.Num() <= 0 || model.indexes.Num() <= 0) {
		parser.Error( "Header list error, model lists with size <= 0\n");
	}

	// Load the vertexes
	parser.ExpectTokenString( "vertexes" );
	parser.ExpectTokenString( "{" );

	for(int i = 0; i < model.verts.Num(); i++ ) {
		idVec2 st;
		idVec3 xyz, normal;

		parser.ExpectTokenString( "vert" );

		xyz.x = parser.ParseFloat();
		xyz.y = parser.ParseFloat();
		xyz.z = parser.ParseFloat();

		parser.ExpectTokenString( "st" );
		st.x = parser.ParseFloat();
		st.y = parser.ParseFloat();

		parser.ExpectTokenString( "normal" );

		normal.x = parser.ParseFloat();
		normal.y = parser.ParseFloat();
		normal.z = parser.ParseFloat();

		model.verts[i].xyz = xyz;
		model.verts[i].st = st;
		model.verts[i].normal = normal;
	}

	// Check end close brace.
	parser.ExpectTokenString( "}" );

	// Load the indexes.
	parser.ExpectTokenString( "tris" );
	parser.ExpectTokenString( "{" );

	for(int i = 0; i < model.indexes.Num(); i+=3 ) {
		parser.ExpectTokenString( "tri" );
		model.indexes[i + 0] = parser.ParseInt();
		model.indexes[i + 1] = parser.ParseInt();
		model.indexes[i + 2] = parser.ParseInt();
	}

	// Check end close brace.
	parser.ExpectTokenString( "}" );

	// Load in all the meshes.
	for(int i = 0; i < model.meshes.Num(); i++ ) {
		parser.ExpectTokenString( "mesh" );
		parser.ExpectTokenString( va( "%d", i) );
		parser.ExpectTokenString( "{" );

		model.meshes[i].name = va( "mesh_%d", i );

		parser.ExpectTokenString( "shader" );
		parser.ReadToken( &token );
		model.meshes[i].material = token;

		parser.ExpectTokenString( "startVertex" );
		model.meshes[i].startVertex = parser.ParseInt();

		parser.ExpectTokenString( "numVertexes" );
		model.meshes[i].numVertexes = parser.ParseInt();

		parser.ExpectTokenString( "startIndex" );
		model.meshes[i].startIndex = parser.ParseInt();

		parser.ExpectTokenString( "numIndexes" );
		model.meshes[i].numIndexes = parser.ParseInt();

		parser.ExpectTokenString( "}" );
	}

	return true;
}