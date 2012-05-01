#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"

idRenderEntityLocal::idRenderEntityLocal() {
	memset( &parms, 0, sizeof( parms ) );
	memset( modelMatrix, 0, sizeof( modelMatrix ) );

	world					= NULL;
	index					= 0;
	lastModifiedFrameNum	= 0;
	archived				= false;
	dynamicModel			= NULL;
	dynamicModelFrameCount	= 0;
	cachedDynamicModel		= NULL;
	referenceBounds			= bounds_zero;
	viewCount				= 0;
	viewEntity				= NULL;
	visibleCount			= 0;
	decals					= NULL;
	overlay					= NULL;
	entityRefs				= NULL;
	firstInteraction		= NULL;
	lastInteraction			= NULL;
	needsPortalSky			= false;
}

void idRenderEntityLocal::FreeRenderEntity() {
}

void idRenderEntityLocal::UpdateRenderEntity( const renderEntity_t *re, bool forceUpdate ) {
}

void idRenderEntityLocal::GetRenderEntity( renderEntity_t *re ) {
}

void idRenderEntityLocal::ForceUpdate() {
}

int idRenderEntityLocal::GetIndex() {
	return index;
}

void idRenderEntityLocal::ProjectOverlay( const idPlane localTextureAxis[2], const idMaterial *material ) {
}
void idRenderEntityLocal::RemoveDecals() {
}