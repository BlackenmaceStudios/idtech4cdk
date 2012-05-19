// draw_virtualtexture.cpp
//

#include "precompiled.h"

#include "tr_local.h"
#include "../vt/VirtualTexture.h"
#include "../vt/VirtualTexture_Backend.h"

void RB_SetupDeferredRenderer( void ) ;

/*
===============
RB_VirtualTexture_Init
===============
*/
void RB_VirtualTexture_Init( void ) {
	// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	

	// Bind the pre-interaction program.
	progs[PROG_VIRTUALTEXTURE].programHandle->Bind();



	RB_SetupDeferredRenderer();
	

	backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
	GL_State( backEnd.depthFunc );
}

/*
===============
RB_VirtualTexture_Cleanup
===============
*/
void RB_VirtualTexture_Cleanup( void ) {
	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );


	// Unbind the pre-interaction program.
	progs[PROG_VIRTUALTEXTURE].programHandle->UnBind();

		GL_SelectTextureNoClient( 4 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 3 );
	globalImages->BindNull();

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
RB_VirtualTexture_DrawSurface
===================
*/
void RB_VirtualTexture_DrawSurface( drawSurf_t *surf, int areaNum ) {
	const idMaterial *shader;
	idImage *diffuse = NULL, *bump = NULL, *spec = NULL;

	shader = surf->material;

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



		// set the vertex pointers
	idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
	qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
	qglVertexAttribPointerARB( 11, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
	qglVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
	qglVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
	qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );



	progs[PROG_VIRTUALTEXTURE].programHandle->SetVar1fi(PP_VT_VTOFFSET, areaNum);
	progs[PROG_VIRTUALTEXTURE].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

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
}

/*
===================
RB_VirtualTexture_Prepass
===================
*/
void RB_VirtualTexture_DrawUnpackBuffer( int areaNum ) {
	bmRenderProgram *program = progs[PROG_VIRTUALTEXTURE].programHandle;


	globalImages->currentRenderImageTargets->BindFBO();

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
    qglDrawBuffers(3, buffers);

	// set the window clipping
	qglViewport( tr.viewportOffset[0] + backEnd.viewDef->viewport.x1, 
		tr.viewportOffset[1] + backEnd.viewDef->viewport.y1, 
		backEnd.viewDef->viewport.x2 + 1 - backEnd.viewDef->viewport.x1,
		backEnd.viewDef->viewport.y2 + 1 - backEnd.viewDef->viewport.y1 );

	// the scissor may be smaller than the viewport for subviews
	qglScissor( tr.viewportOffset[0] + backEnd.viewDef->viewport.x1 + backEnd.viewDef->scissor.x1, 
		tr.viewportOffset[1] + backEnd.viewDef->viewport.y1 + backEnd.viewDef->scissor.y1, 
		backEnd.viewDef->scissor.x2 + 1 - backEnd.viewDef->scissor.x1,
		backEnd.viewDef->scissor.y2 + 1 - backEnd.viewDef->scissor.y1 );
	backEnd.currentScissor = backEnd.viewDef->scissor;
	
	

	// Set the current pass for this shader.
	program->SetCurrentPass( VTPASS_UNPACKTILES );

	RB_VirtualTexture_Init();

	// Bind the pre-interaction program.
	progs[PROG_VIRTUALTEXTURE].programHandle->SetVar1fi(PP_VT_VTOFFSET, areaNum);

	// Bind the diffuse buffer.
	GL_SelectTexture( 0 );
	virtualTextureManager->GetWorldPage()->Bind();
	program->BindTextureVar( PP_TEX_DIFFUSE );

	// Bind the diffuse buffer.
	GL_SelectTexture( 1 );
	globalImages->currentVTRemapImage->Bind();
	program->BindTextureVar( PP_TEX_INDEXTEX );

	GL_SelectTexture( 2 );
	globalImages->currentVTRemapImage->fboColorTargets[0]->Bind();
	program->BindTextureVar( VV_TEX_VERTEX );

	GL_SelectTexture( 3 );
	globalImages->currentVTRemapImage->fboColorTargets[1]->Bind();
	program->BindTextureVar( VV_TEX_VERTEX2 );

	GL_SelectTexture( 4 );
	globalImages->currentVTRemapImage->fboColorTargets[2]->Bind();
	program->BindTextureVar( VV_TEX_VERTEX3 );
	
	
	renderDevice->Set2DViewMatrix();
	GL_State(GLS_SRCBLEND_ONE| GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);

	renderDevice->DrawImage( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1, NULL );



	renderDevice->RestoreViewMatrix();
	RB_VirtualTexture_Cleanup();



		// UnBind the FBO.
	globalImages->currentRenderImageTargets->UnBindFBO();
	//renderDevice->ForceDeviceSync();

	GL_State( 0 );


}

/*
===================
RB_VirtualTexture_Prepass
===================
*/
void RB_VirtualTexture_Prepass( drawSurf_t	 **drawSurfs, int numDrawSurfs, int areaNum ) {
	drawSurf_t *drawSurf;
	bmRenderProgram *program;
	bool sync = vt_syncrender.GetBool();
	program = progs[PROG_VIRTUALTEXTURE].programHandle;

	// Set the current pass for this shader.
	program->SetCurrentPass( VTPASS_REMAPINDEXES );

	// if we are just doing 2D rendering, no need to fill the depth buffer
	if ( !backEnd.viewDef->viewEntitys ) {
		return;
	}

	RB_LogComment( "---------- RB_VirtualTexture_Prepass ----------\n" );
	
	
	// Bind the rendertargets FBO
	//renderDevice->BeginDeviceSync();
	globalImages->currentVTRemapImage->BindFBO();
	qglClear( GL_COLOR_BUFFER_BIT  );

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT,GL_COLOR_ATTACHMENT2_EXT,GL_COLOR_ATTACHMENT3_EXT};
    qglDrawBuffers(4, buffers);
	


	
	RB_VirtualTexture_Init();

	// Bind the virtual texture.
	if(vt_debug_atlas.GetBool()) {
		GL_SelectTextureNoClient( 0 );
		backEnd.viewDef->renderWorld->vt->BindAtlas();
		program->BindTextureVar( PP_TEX_DIFFUSE );
	} else {
		GL_SelectTextureNoClient( 0 );
		backEnd.viewDef->renderWorld->vt->BindAtlas();
		program->BindTextureVar( PP_TEX_DIFFUSE );

		GL_SelectTextureNoClient( 1 );
		vtBackEnd.Bind();
		program->BindTextureVar( PP_TEX_INDEXTEX );
	}


	int numSurfsInView = 0;
	for (int i = 0  ; i < numDrawSurfs ; i++ ) {
		drawSurf = drawSurfs[i];

		// If we aren't rendering this area don't render this surface.
		if(drawSurf->geo->vt_AreaID != areaNum) {
			continue;
		}

		numSurfsInView++;
		if(sync)
		{
			renderDevice->BeginDeviceSync();
		}
	
		

		// change the matrix if needed
		if ( drawSurf->space != backEnd.currentSpace ) {
			qglLoadMatrixf( drawSurf->space->modelViewMatrix );
		}

		if ( drawSurf->space->weaponDepthHack ) {
			RB_EnterWeaponDepthHack();
		}

		if ( drawSurf->space->modelDepthHack != 0.0f ) {
			RB_EnterModelDepthHack( drawSurf->space->modelDepthHack );
		}

		// change the scissor if needed
		
		if ( r_useScissor.GetBool() && !backEnd.currentScissor.Equals( drawSurf->scissorRect ) ) {
			backEnd.currentScissor = drawSurf->scissorRect;
			qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
				backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
				backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
				backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
		}
		
		RB_VirtualTexture_DrawSurface( drawSurf, areaNum );

		if ( drawSurf->space->weaponDepthHack || drawSurf->space->modelDepthHack != 0.0f ) {
			RB_LeaveDepthHack();
		}

		backEnd.currentSpace = drawSurf->space;

		if(sync)
		{
			renderDevice->ForceDeviceSync();
		}
	
	}

	//RB_RenderDrawSurfListWithFunction( drawSurfs, numDrawSurfs, RB_Deferred_DrawPreInteraction );
	//renderDevice->ForceDeviceSync();
	virtualTextureManager->GetWorldPage()->UnBind();
	RB_VirtualTexture_Cleanup();

		// UnBind the FBO.
	globalImages->currentVTRemapImage->UnBindFBO();
	
	if(numSurfsInView > 0)
	{
		RB_VirtualTexture_DrawUnpackBuffer( areaNum );
	}
}
