// Draw_Shadows.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"
#include "../../base/renderprogs/tr_rprogshared.h"

extern bmRenderSceneLights_t scene;
extern viewLight_t *deferredViewLights;

/*
=========================
RB_T_FillShadowBuffer
=========================
*/
void RB_T_FillShadowBuffer( const drawSurf_t *surf ) {
	const srfTriangles_t	*tri = surf->geo;

	qglDepthFunc(GL_LEQUAL);
	GL_Cull( CT_FRONT_SIDED );
	progs[PROG_SHADOW].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

	idDrawVert *ac = (idDrawVert *)vertexCache.Position( tri->ambientCache );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
	qglTexCoordPointer( 2, GL_FLOAT, sizeof( idDrawVert ), reinterpret_cast<void *>(&ac->st) );


	RB_DrawElementsWithCounters( tri );	

}



/*
=========================
RB_Deferred_ShadowPass
=========================
*/
void RB_Deferred_ShadowPass( viewLight_t *vLight, drawSurf_t **drawSurfs, int numDrawSurfs ) {
	progs[PROG_SHADOW].programHandle->Bind();

	GL_SelectTextureNoClient( 0 );
	backEnd.shadowMapCache.BindCache();
	progs[PROG_SHADOW].programHandle->BindTextureVar( PP_SHADOW  );

	progs[PROG_SHADOW].programHandle->SetVar3fv( PP_LIGHT_ORIGIN, 1, vLight->lightDef->globalLightOrigin.ToFloatPtr() );

		
	RB_RenderDrawSurfListWithFunction( drawSurfs, numDrawSurfs, RB_T_FillShadowBuffer );


	progs[PROG_SHADOW].programHandle->UnBind();

	// after all the frustums have been drawn, the surfaces that have been drawn on will get interactions
	// scissor may still be a win even with the stencil test for very fast rejects
	qglStencilFunc( GL_EQUAL, 0, 255 );
	qglStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	qglDisable( GL_STENCIL_TEST );
	qglDisable( GL_SCISSOR_TEST );
}