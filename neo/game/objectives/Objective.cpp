// Objective.cpp
//

#include "precompiled.h"

/*
=================
bmObjectiveDecl::Size
=================
*/
size_t bmObjectiveDecl::Size( void ) const {
	return sizeof( idDecl );
}

/*
=================
bmObjectiveDecl::DefaultDefinition
=================
*/
const char *bmObjectiveDecl::DefaultDefinition( void ) const {
	return
		"{\n"
	
		"}"; 
}
/*
=================
bmObjectiveDecl::Parse
=================
*/
bool bmObjectiveDecl::Parse( const char *text, const int textLength ) {
	return true;
}

/*
=================
bmObjectiveDecl::FreeData
=================
*/
void bmObjectiveDecl::FreeData( void ) {

}