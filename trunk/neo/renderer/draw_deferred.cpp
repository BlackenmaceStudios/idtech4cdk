// draw_deferred.cpp
//

#include "precompiled.h"

#include "tr_local.h"
#include "../../base/renderprogs/tr_rprogshared.h"
#include "../vt/VirtualTexture.h"
#include "../vt/VirtualTexture_Backend.h"


#define DEBUG_DEFERRED //GL_CheckErrors();

void RB_STD_T_RenderShaderPasses( const drawSurf_t *surf, bool isDeferred );
void RB_T_FillDepthBuffer( const drawSurf_t *surf );

viewLight_t *deferredViewLights = NULL;
idVec3 deferredEyePos;
idMat4 deferredViewMatrix;

bmRenderSceneLights_t scene;


/*
==================
RB_Deferred_DrawPreInteraction
==================
*/
void	RB_Deferred_DrawPreInteraction( const drawSurf_t *surf ) {
	const idMaterial *shader;
	idImage *diffuse = NULL, *bump = NULL, *spec = NULL;

	shader = surf->material;

	// If this shader is part of the vt don't draw it.
	if(surf->geo->vt_AreaID != -1)
		return;

	// If the shader doesn't recieve lighting, use forward rendering instead.
	//if(!shader->ReceivesLighting()) {
	//	return;
	//}

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

	if ( surf->space->entityDef->parms.weaponDepthHack )
	{
		//GL_State( GLS_DEPTHFUNC_ALWAYS  );	
	}
	else
	{
		//GL_State( shader->GetStage( 0 )->drawStateBits );

	}


	

		// set the vertex pointers
	idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
	qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
	qglVertexAttribPointerARB( 11, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
	qglVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
	qglVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
	qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

	for ( int surfaceStageNum = 0 ; surfaceStageNum < shader->GetNumStages() ; surfaceStageNum++ ) {
			const shaderStage_t	*surfaceStage = shader->GetStage( surfaceStageNum );

			switch( surfaceStage->lighting ) {
				case SL_BUMP: {
					// ignore stage that fails the condition
					if ( !surf->shaderRegisters[ surfaceStage->conditionRegister ] ) {
						break;
					}
					// draw any previous interaction
					bump = surfaceStage->texture.image;
					break;
				}
				case SL_DIFFUSE: {
					// ignore stage that fails the condition
					if ( !surf->shaderRegisters[ surfaceStage->conditionRegister ] ) {
						break;
					}
					diffuse = surfaceStage->texture.image;
					break;
				}
				case SL_SPECULAR: {
					// ignore stage that fails the condition
					if ( !surf->shaderRegisters[ surfaceStage->conditionRegister ] ) {
						break;
					}
					spec = surfaceStage->texture.image;
				break;
			}
		}
	}

	progs[PROG_PREINTERACTION].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

	if(bump == NULL) {
		return;
	}

		
	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( 0 );
	bump->Bind();
	progs[PROG_PREINTERACTION].programHandle->BindTextureVar( PP_TEX_NORMAL );

	// texture 2 is the per-surface diffuse map
	GL_SelectTextureNoClient( 1 );
	diffuse->Bind();
	progs[PROG_PREINTERACTION].programHandle->BindTextureVar( PP_TEX_DIFFUSE );

	// texture 3 is the per-surface specular map
	GL_SelectTextureNoClient( 2 );
	if(spec == NULL)
	{
		globalImages->whiteImage->Bind();
	}
	else
	{
		spec->Bind();
	}
	progs[PROG_PREINTERACTION].programHandle->BindTextureVar( PP_TEX_SPEC );

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
=========================
RB_Deferred_PrepareSceneForDrawing
=========================
*/
void RB_Deferred_PrepareSceneForDrawing( viewLight_t *lights ) {
	viewLight_t		*vLight;
	scene.numLights = 0;

	if(backEnd.lightScale <= 0)
		backEnd.lightScale = 1.0f;

	for ( vLight = lights ; vLight ; vLight = vLight->next, scene.numLights++ ) {
		renderLight_t *parms = &vLight->lightDef->parms;

		const shaderStage_t	*lightStage = vLight->lightShader->GetStage( 0 );
		const float			*lightRegs = vLight->shaderRegisters;

		// ignore stages that fail the condition
		if ( !lightRegs[ lightStage->conditionRegister ] ) {
			continue;
		}

		scene.r_lightOrigin[scene.numLights] = vLight->globalLightOrigin;

		for(int i = 0; i < 4; i++)
		{
			scene.r_diffuseColor[scene.numLights][i] = lightRegs[ lightStage->color.registers[i] ];
			if(i != 3) {
				scene.r_diffuseColor[scene.numLights][i] *= backEnd.lightScale;
			}
		}

		scene.r_specularColor[scene.numLights] = scene.r_diffuseColor[scene.numLights];

		scene.r_lightProjectionS[scene.numLights] = vLight->lightProject[0];
		scene.r_lightProjectionT[scene.numLights] = vLight->lightProject[1];
		scene.r_lightProjectionQ[scene.numLights] = vLight->lightProject[2];
		scene.r_lightFalloff[scene.numLights] = vLight->lightProject[3];
	}
}

/*
=========================
RB_Deferred_DeferredInteractionPass
=========================
*/
void RB_Deferred_DeferredInteractionPass( drawSurf_t *surf) {
	bool next = true;
	float lightColor[4];

/*
	const shaderStage_t	*lightStage = light->lightShader->GetStage( 0 );
	const float			*lightRegs = light->shaderRegisters;

	// ignore stages that fail the condition
	if ( !lightRegs[ lightStage->conditionRegister ] ) {
		return;
	}

	// backEnd.lightScale is calculated so that lightColor[] will never exceed
	// tr.backEndRendererMaxLight
	lightColor[0] = backEnd.lightScale * lightRegs[ lightStage->color.registers[0] ];
	lightColor[1] = backEnd.lightScale * lightRegs[ lightStage->color.registers[1] ];
	lightColor[2] = backEnd.lightScale * lightRegs[ lightStage->color.registers[2] ];
	lightColor[3] = lightRegs[ lightStage->color.registers[3] ];


	// load all the vertex program parameters
	


		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_VIEW_ORIGIN, localViewOrigin.ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_BUMP_MATRIX_S, din->bumpMatrix[0].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_BUMP_MATRIX_T, din->bumpMatrix[1].ToFloatPtr() );

	// progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_DIFFUSE_MATRIX_S, din->diffuseMatrix[0].ToFloatPtr() );
	// progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_DIFFUSE_MATRIX_T, din->diffuseMatrix[1].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_SPECULAR_MATRIX_S, din->specularMatrix[0].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_SPECULAR_MATRIX_T, din->specularMatrix[1].ToFloatPtr() );

	// testing fragment based normal mapping
	if ( r_testARBProgram.GetBool() ) {
		//	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 2, din->localLightOrigin.ToFloatPtr() );
		//	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 3, din->localViewOrigin.ToFloatPtr() );
	}
*/
	

	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_VIEW_ORIGIN, deferredEyePos.ToFloatPtr() );

	static const float zero[4] = { 0, 0, 0, 0 };
	static const float one[4] = { 1, 1, 1, 1 };
	static const float negOne[4] = { -1, -1, -1, -1 };

	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_MODULATE, zero );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_ADD, one );

	//progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_DIFFUSE, lightColor );
	//progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_SPEC, lightColor );

	/*
	// set the constant colors
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 0, din->diffuseColor.ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 1, din->specularColor.ToFloatPtr() );
	*/
	// set the textures

	// texture 2 will be the light falloff texture
/*
	GL_SelectTextureNoClient( 3 );
	light->falloffImage->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_LIGHTFALLOFF );

	// texture 3 will be the light projection texture
	GL_SelectTextureNoClient( 4 );
	lightStage->texture.image->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_LIGHTPROJECTION );
*/

	progs[PROG_INTERACTION].programHandle->SetVarMatrix4fv( PP_VIEWMAT, 1, &deferredViewMatrix[0][0] );
	GL_SelectTextureNoClient( 0 );

	if(vt_debug_atlas.GetBool())
	{
		progs[PROG_INTERACTION].programHandle->SetVar1fi( PP_DIFFUSEONLY, 1);
	}
	else {
		progs[PROG_INTERACTION].programHandle->SetVar1fi( PP_DIFFUSEONLY, 0);
	}
	
	//memcpy(&surf->scissorRect, &light->scissorRect, sizeof(idScreenRect) );
	//surf->scissorRect.x1 *= 1.5;
	//surf->scissorRect.x2 *= 1.5;
	//surf->scissorRect.y1 *= 1.5;
	//surf->scissorRect.y2 *= 1.5;
	RB_STD_T_RenderShaderPasses( surf, true );
}

/*
=========================
RB_Deferred_DrawDeferredInteraction
=========================
*/
void RB_Deferred_DrawDeferredInteraction( const drawSurf_t *surf ) {
//	if(deferredViewLights == NULL)
//		return;

	// bind the vertex program
	progs[PROG_INTERACTION].programHandle->Bind();

	progs[PROG_INTERACTION].programHandle->SetVar1fi( PP_NUMLIGHTS, scene.numLights );
	progs[PROG_INTERACTION].programHandle->SetVar3fv( PP_LIGHT_ORIGIN, scene.numLights, scene.r_lightOrigin[0].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_PROJECT_S, scene.numLights, scene.r_lightProjectionS[0].ToFloatPtr());
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_PROJECT_T, scene.numLights, scene.r_lightProjectionT[0].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_PROJECT_Q, scene.numLights, scene.r_lightProjectionQ[0].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_FALLOFF_S, scene.numLights, scene.r_lightFalloff[0].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar3fv( PP_COLOR_DIFFUSE, scene.numLights, scene.r_diffuseColor[0].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar3fv( PP_COLOR_SPEC, scene.numLights, scene.r_specularColor[0].ToFloatPtr() );

	//tr.cbRenderScene->Bind();
	//tr.cbRenderScene->Blit( &scene, sizeof( bmRenderSceneLights_t ) );

	// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	qglEnableClientState( GL_COLOR_ARRAY );

	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( 0 );
	globalImages->currentRenderImageTargets->fboColorTargets[0]->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_NORMAL );

	// texture 4 is the per-surface diffuse map
	GL_SelectTextureNoClient( 1 );
	globalImages->currentRenderImageTargets->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_DIFFUSE );

	// texture 5 is the per-surface specular map
	//GL_SelectTextureNoClient( 2 );
	//globalImages->currentRenderImageTargets->fboColorTargets[2]->Bind();
	//progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_SPEC );

	// texture 5 is the per-surface specular map
	GL_SelectTextureNoClient( 3 );
	globalImages->currentRenderImageTargets->fboColorTargets[1]->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( VV_TEX_VERTEX );

	// texture 5 is the per-surface specular map
	GL_SelectTextureNoClient( 4 );

	if(r_shadows.GetBool() == false) {
		globalImages->whiteImage->Bind();
	}
	else
	{
		globalImages->currentRenderShadows->Bind();
	}
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_SHADOW );
	
	// texture 5 is the per-surface specular map
	GL_SelectTextureNoClient( 5 );
	globalImages->currentRenderUnlitTransparancy->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_UNLITTRANSPARENT );

	//if(backEnd.viewDef->renderWorld->vt != NULL)
	//{
		//GL_SelectTextureNoClient( 6 );
		//globalImages->currentRenderImageTargets->Bind();
		//progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_INDEXTEX );
	//}

	// perform setup here that will be constant for all interactions
	bool setState = true;

	idDrawVert *ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
	qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), reinterpret_cast<void *>(&ac->st) );


	
	//GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK);
	RB_Deferred_DeferredInteractionPass( (drawSurf_t *)surf );

	progs[PROG_INTERACTION].programHandle->UnBind();

	qglDisableVertexAttribArrayARB( 8 );
	qglDisableVertexAttribArrayARB( 9 );
	qglDisableVertexAttribArrayARB( 10 );
	qglDisableVertexAttribArrayARB( 11 );
	qglDisableClientState( GL_COLOR_ARRAY );

	GL_SelectTextureNoClient( 7 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 6 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 5 );
	globalImages->BindNull();


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

	//tr.cbRenderScene->UnBind();
}

/*
=========================
RB_Deferred_PreInteractionPass
=========================
*/
void RB_Deferred_PreInteractionPass( drawSurf_t	 **drawSurfs, int numDrawSurfs ) {
	

	// if we are just doing 2D rendering, no need to fill the depth buffer
	if ( !backEnd.viewDef->viewEntitys ) {
		return;
	}


	RB_LogComment( "---------- RB_Deferred_PreInteractionPass ----------\n" );
	GL_State( GLS_DEPTHFUNC_LESS );

	globalImages->currentRenderImageTargets->BindFBO();

			// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	

	DEBUG_DEFERRED
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
	DEBUG_DEFERRED

		// Bind the pre-interaction program.
	progs[PROG_PREINTERACTION].programHandle->Bind();
	DEBUG_DEFERRED

	backEnd.glState.faceCulling = -1;		// force face culling to set next time
	GL_Cull( CT_FRONT_SIDED );
	qglClear( GL_DEPTH_BUFFER_BIT);
	RB_RenderDrawSurfListWithFunction( drawSurfs, numDrawSurfs, RB_Deferred_DrawPreInteraction );
	
	DEBUG_DEFERRED
	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	DEBUG_DEFERRED

	// Unbind the pre-interaction program.
	progs[PROG_PREINTERACTION].programHandle->UnBind();
	DEBUG_DEFERRED

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
	DEBUG_DEFERRED

	globalImages->currentRenderImageTargets->UnBindFBO();
}

/*
==================
RB_SetupDeferredRenderer
==================
*/
void RB_SetupDeferredRenderer( void ) {
	deferredViewLights = backEnd.viewDef->viewLights;
	deferredEyePos = backEnd.viewDef->renderView.vieworg;
	deferredViewMatrix = idMat4( backEnd.viewDef->renderView.viewaxis, deferredEyePos );
	deferredViewMatrix = deferredViewMatrix.Transpose();

	// Preparse the scene for drawing.
	RB_Deferred_PrepareSceneForDrawing( backEnd.viewDef->viewLights );
}