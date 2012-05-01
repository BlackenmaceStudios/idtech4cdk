// PTex.cpp
//

#include "precompiled.h"
#include "tr_local.h"

#include "Model_local.h"
#include "ptex_local.h"

/*
==================
bmPTexMesh::InitFromFile
==================
*/
void bmPTexMesh::InitFromFile( const char *fileName ) {
	Ptex::String err;

	// Load the ptex file.
	ptex = PtexTexture::open( fileName, err );
	if(!ptex) {
		common->Error( "PTexMesh %s failed to load %s\n", err.c_str());
	}


}