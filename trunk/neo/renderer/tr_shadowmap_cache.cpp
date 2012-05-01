// tr_shadowmap_cache.cpp
//

#include "precompiled.h"

#include "tr_local.h"

idCVar r_backend_shadowMapCacheSize( "r_backend_shadowMapCacheSize", "2048", CVAR_RENDERER | CVAR_INTEGER, "size of the shadowmap used for realtime shadows" );

//
// R_CreateCubemapShadowCacheFBO
//
static void R_CreateCubemapShadowCacheFBO( idImage *image ) {
	image->GenerateFrameBufferCubeImage( r_backend_shadowMapCacheSize.GetInteger(), r_backend_shadowMapCacheSize.GetInteger() );
}

/*
==================
bmShadowMapCache::Init
==================
*/
void bmShadowMapCache::Init( int width, int height ) {
	shadowMapCube = globalImages->ImageFromFunction( "ShadowMapCache", R_CreateCubemapShadowCacheFBO );
}

/*
==================
bmShadowMapCache::ClearScreenMapCache
==================
*/
void bmShadowMapCache::ClearScreenMapCache( void ) {
	globalImages->currentRenderShadows->BindFBO();

	

	globalImages->currentRenderShadows->UnBindFBO();

	// Disable blending of shadows for the 
	enableShadowBlend = false;
}

/*
==================
bmShadowMapCache::RenderToScreenBuffer
==================
*/
void bmShadowMapCache::RenderToScreenBuffer( struct viewLight_s *vLight ) {
	drawSurf_t	 **drawSurfs;
	int			numDrawSurfs;

	drawSurfs = (drawSurf_t **)&backEnd.viewDef->drawSurfs[0];
	numDrawSurfs = backEnd.viewDef->numDrawSurfs;

	globalImages->currentRenderShadows->BindFBO();

	
	

	if(enableShadowBlend) {
		RB_BeginDrawingView(false);
		GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK  );
	
	}
	else {
		// clear the z buffer, set the projection matrix, etc
		RB_BeginDrawingView();

		qglClearColor(0,0,0,0);
		qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	enableShadowBlend = true;

	RB_Deferred_ShadowPass( vLight, drawSurfs, numDrawSurfs );

	globalImages->currentRenderShadows->UnBindFBO();

	GL_State( GLS_DEFAULT );
}