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
	const idMaterial *shader;
	shader = surf->material;

	// If the shader doesn't recieve lighting, use forward rendering instead.
	if(!shader->ReceivesLighting()) {
	//	return;
	}

	// translucent surfaces don't put anything in the depth buffer and don't
	// test against it, which makes them fail the mirror clip plane operation
	if ( shader->Coverage() == MC_TRANSLUCENT ) {
		return;
	}

	if(!shader->SurfaceCastsShadow()) {
//		return;
	}

	GL_Cull( CT_FRONT_SIDED );
	progs[PROG_SHADOW].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

	GL_SelectTextureNoClient( 0 );
	backEnd.shadowMapCache.BindCache();
	progs[PROG_SHADOW].programHandle->BindTextureVar( PP_SHADOW  );

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

	
	

	progs[PROG_SHADOW].programHandle->SetVar3fv( PP_LIGHT_ORIGIN, 1, vLight->lightDef->globalLightOrigin.ToFloatPtr() );

	progs[PROG_SHADOW].programHandle->SetVar1fi( PP_LIGHTNUM,  backEnd.vLightNum );
	RB_RenderDrawSurfListWithFunction( drawSurfs, numDrawSurfs, RB_T_FillShadowBuffer );


	progs[PROG_SHADOW].programHandle->UnBind();
}