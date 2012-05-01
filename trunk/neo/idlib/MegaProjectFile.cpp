// MegaProjectFile.cpp
//

#include "precompiled.h"

bmMegaProjectFile::bmMegaProjectFile() {

}

bmMegaProjectFile::~bmMegaProjectFile() {

}

/*
==================
bmMegaProjectLayer::Init
==================
*/
void bmMegaProjectLayer::Init( const char *diffuseImagePath, const char *maskImagePath ) {
	diffuse = renderSystem->FindImage( diffuseImagePath );
	maskImage = renderSystem->FindImage( maskImagePath );
}

/*
========================
bmMegaProjectFile::InitDefault
========================
*/
void bmMegaProjectFile::InitDefault( void ) {
	bmMegaProjectArea *defaultArea;

	// Clear all the areas.
	_areas.Clear();

	defaultArea = AllocArea();
	defaultArea->AllocLayer( "textures/layers/default", "_white" );

}