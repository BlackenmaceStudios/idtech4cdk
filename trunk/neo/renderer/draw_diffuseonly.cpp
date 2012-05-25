// Draw_DiffuseOnly.cpp
//

#include "precompiled.h"

#include "tr_local.h"
#include "../vt/virtualtexture.h"

void RB_SetupDeferredRenderer( void ) ;

/*
===============
RB_DiffuseOnly_Init
===============
*/
void RB_DiffuseOnly_Init( void ) {
	// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	

	// Bind the pre-interaction program.
	progs[PROG_DIFFUSEONLY].programHandle->Bind();

	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

	RB_SetupDeferredRenderer();
	

	backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
	GL_State( backEnd.depthFunc );
}

/*
===============
RB_DiffuseOnly_Cleanup
===============
*/
void RB_DiffuseOnly_Cleanup( void ) {
	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );


	// Unbind the pre-interaction program.
	progs[PROG_DIFFUSEONLY].programHandle->UnBind();


	GL_SelectTextureNoClient( 2 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 1 );
	globalImages->BindNull();

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture( 0 );

	qglDisableVertexAttribArrayARB( 8 );
	qglDisableVertexAttribArrayARB( 9 );
	qglDisableVertexAttribArrayARB( 10 );
	qglDisableVertexAttribArrayARB( 11 );
}

/*
===================
RB_DiffuseOnly_DrawSurface
===================
*/
void RB_DiffuseOnly_DrawSurface( drawSurf_t *surf, idImage *image, bool noVt ) {
	const idMaterial *shader;
	idImage *diffuse = NULL, *bump = NULL, *spec = NULL;

	shader = surf->material;

	if(surf->geo->vt_AreaID == -1)
		return;

	// If the shader doesn't recieve lighting, use forward rendering instead.
	if(!shader->ReceivesLighting()) {
		return;
	}

	// translucent surfaces don't put anything in the depth buffer and don't
	// test against it, which makes them fail the mirror clip plane operation
	if ( shader->Coverage() == MC_TRANSLUCENT ) {
		return;
	}

	// set polygon offset if necessary
	if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) {
		qglEnable( GL_POLYGON_OFFSET_FILL );
		qglPolygonOffset( r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * shader->GetPolygonOffset() );
	}

	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( 0 );
	image->Bind();
	progs[PROG_DIFFUSEONLY].programHandle->BindTextureVar( PP_TEX_DIFFUSE );

	int stride = sizeof( idDrawVert );

		// set the vertex pointers
	idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
	qglColorPointer( 4, GL_UNSIGNED_BYTE, stride, ac->color );
	qglVertexAttribPointerARB( 11, 3, GL_FLOAT, false, stride, ac->normal.ToFloatPtr() );
	qglVertexAttribPointerARB( 10, 3, GL_FLOAT, false, stride, ac->tangents[1].ToFloatPtr() );
	qglVertexAttribPointerARB( 9, 3, GL_FLOAT, false, stride, ac->tangents[0].ToFloatPtr() );
	qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, stride, ac->st.ToFloatPtr() );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );


	progs[PROG_DIFFUSEONLY].programHandle->SetVar1fi(PP_VT_VTOFFSET, surf->geo->vt_AreaID);
	progs[PROG_DIFFUSEONLY].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

	

	// draw it
	RB_DrawElementsWithCounters( surf->geo );

	// reset polygon offset
	if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) {
		qglDisable( GL_POLYGON_OFFSET_FILL );
	}

	// reset blending
	if ( shader->GetSort() == SS_SUBVIEW ) {
		GL_State( GLS_DEPTHFUNC_LESS );
	}

	globalImages->BindNull();
}


/*
===============
RB_Draw_DiffuseOnly
===============
*/
void RB_Draw_DiffuseOnly( drawSurf_t **drawSurfs, int numDrawSurfs ) {
	drawSurf_t *drawSurf;

	// if we are just doing 2D rendering, no need to fill the depth buffer
	if ( !backEnd.viewDef->viewEntitys ) {
		return;
	}

	RB_LogComment( "---------- RB_DiffuseOnly_Prepass ----------\n" );
	
	
	// clear the z buffer, set the projection matrix, etc
	RB_BeginDrawingView();

	RB_DiffuseOnly_Init();

	qglClear( GL_COLOR_BUFFER_BIT );

	if( backEnd.viewDef->renderWorld->vt->vtAtlasImage2 == NULL ) {
		backEnd.viewDef->renderWorld->vt = virtualTextureManager->GetCurrentVirtualTextureFile();
	}

	for (int i = 0  ; i < numDrawSurfs ; i++ ) {
		drawSurf = drawSurfs[i];

		

		// change the matrix if needed
		if ( drawSurf->space != backEnd.currentSpace ) {
			qglLoadMatrixf( drawSurf->space->modelViewMatrix );
		}

		if ( drawSurf->space->modelDepthHack != 0.0f ) {
		//	continue;
		}

		// change the scissor if needed
		if ( r_useScissor.GetBool() && !backEnd.currentScissor.Equals( drawSurf->scissorRect ) ) {
			backEnd.currentScissor = drawSurf->scissorRect;
	//		qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
	//			backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
	//			(backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1) / 10,
	//			(backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1)  / 10 );
		}


		RB_DiffuseOnly_DrawSurface( drawSurf, backEnd.viewDef->renderWorld->vt->vtAtlasImage2, true );


		backEnd.currentSpace = drawSurf->space;
	}

	//RB_RenderDrawSurfListWithFunction( drawSurfs, numDrawSurfs, RB_Deferred_DrawPreInteraction );
	RB_DiffuseOnly_Cleanup();
}