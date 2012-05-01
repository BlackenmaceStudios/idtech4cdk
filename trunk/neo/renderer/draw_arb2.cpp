/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"

#include "cg_explicit.h"
#include "./renderprogs/tr_rprogshared.h"

CGcontext cg_context;

static void cg_error_callback( void ) {
	CGerror i = cgGetError();
	common->Printf( "Cg error (%d): %s\n", i, cgGetErrorString(i) );
}

/*
=========================================================================================

GENERAL INTERACTION RENDERING

=========================================================================================
*/

/*
====================
GL_SelectTextureNoClient
====================
*/
void GL_SelectTextureNoClient( int unit ) {
	backEnd.glState.currenttmu = unit;
	qglActiveTextureARB( GL_TEXTURE0_ARB + unit );
	RB_LogComment( "glActiveTextureARB( %i )\n", unit );
}

/*
==================
RB_ARB2_DrawInteraction
==================
*/
void	RB_ARB2_DrawInteraction( const drawInteraction_t *din ) {
	// load all the vertex program parameters
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_ORIGIN, din->localLightOrigin.ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_VIEW_ORIGIN, din->localViewOrigin.ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_PROJECT_S, din->lightProjection[0].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_PROJECT_T, din->lightProjection[1].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_PROJECT_Q, din->lightProjection[2].ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_LIGHT_FALLOFF_S, din->lightProjection[3].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_BUMP_MATRIX_S, din->bumpMatrix[0].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_BUMP_MATRIX_T, din->bumpMatrix[1].ToFloatPtr() );

	 progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_DIFFUSE_MATRIX_S, din->diffuseMatrix[0].ToFloatPtr() );
	 progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_DIFFUSE_MATRIX_T, din->diffuseMatrix[1].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_SPECULAR_MATRIX_S, din->specularMatrix[0].ToFloatPtr() );
	// progs[PROG_INTERACTION]->SetVar4fv( PP_SPECULAR_MATRIX_T, din->specularMatrix[1].ToFloatPtr() );

	// testing fragment based normal mapping
	if ( r_testARBProgram.GetBool() ) {
	//	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 2, din->localLightOrigin.ToFloatPtr() );
	//	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 3, din->localViewOrigin.ToFloatPtr() );
	}

	static const float zero[4] = { 0, 0, 0, 0 };
	static const float one[4] = { 1, 1, 1, 1 };
	static const float negOne[4] = { -1, -1, -1, -1 };

	switch ( din->vertexColor ) {
	case SVC_IGNORE:
		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_MODULATE, zero );
		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_ADD, one );
		break;
	case SVC_MODULATE:
		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_MODULATE, one );
		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_ADD, zero );
		break;
	case SVC_INVERSE_MODULATE:
		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_MODULATE, negOne );
		progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_ADD, one );
		break;
	}

	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_DIFFUSE, din->diffuseColor.ToFloatPtr() );
	progs[PROG_INTERACTION].programHandle->SetVar4fv( PP_COLOR_SPEC, din->specularColor.ToFloatPtr() );
/*
	// set the constant colors
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 0, din->diffuseColor.ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 1, din->specularColor.ToFloatPtr() );
*/
	// set the textures

	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( 0 );
	din->bumpImage->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_NORMAL );

	// texture 2 will be the light falloff texture
	GL_SelectTextureNoClient( 1 );
	din->lightFalloffImage->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_LIGHTFALLOFF );

	// texture 3 will be the light projection texture
	GL_SelectTextureNoClient( 2 );
	din->lightImage->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_LIGHTPROJECTION );

	// texture 4 is the per-surface diffuse map
	GL_SelectTextureNoClient( 3 );
	din->diffuseImage->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_DIFFUSE );

	// texture 5 is the per-surface specular map
	GL_SelectTextureNoClient( 4 );
	din->specularImage->Bind();
	progs[PROG_INTERACTION].programHandle->BindTextureVar( PP_TEX_SPEC );

	// draw it
	RB_DrawElementsWithCounters( din->surf->geo );
}


/*
=============
RB_ARB2_CreateDrawInteractions

=============
*/
void RB_ARB2_CreateDrawInteractions( const drawSurf_t *surf ) {
	if ( !surf ) {
		return;
	}

	// perform setup here that will be constant for all interactions
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc );

	// bind the vertex program
	progs[PROG_INTERACTION].programHandle->Bind();

	// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	qglEnableClientState( GL_COLOR_ARRAY );
/*
	// texture 0 is the normalization cube map for the vector towards the light
	GL_SelectTextureNoClient( 0 );
	if ( backEnd.vLight->lightShader->IsAmbientLight() ) {
		globalImages->ambientNormalMap->Bind();
	} else {
		globalImages->normalCubeMapImage->Bind();
	}

	// texture 6 is the specular lookup table
	GL_SelectTextureNoClient( 6 );
	if ( r_testARBProgram.GetBool() ) {
		globalImages->specular2DTableImage->Bind();	// variable specularity in alpha channel
	} else {
		globalImages->specularTableImage->Bind();
	}
*/

	for ( ; surf ; surf=surf->nextOnLight ) {
		// perform setup here that will not change over multiple interaction passes

		// set the vertex pointers
		idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
		qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
		qglVertexAttribPointerARB( 11, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
		qglVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
		qglVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
		qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
		qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

		// this may cause RB_ARB2_DrawInteraction to be exacuted multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_CreateSingleDrawInteractions( surf, RB_ARB2_DrawInteraction );
	}

	progs[PROG_INTERACTION].programHandle->UnBind();

	qglDisableVertexAttribArrayARB( 8 );
	qglDisableVertexAttribArrayARB( 9 );
	qglDisableVertexAttribArrayARB( 10 );
	qglDisableVertexAttribArrayARB( 11 );
	qglDisableClientState( GL_COLOR_ARRAY );

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
}


/*
==================
RB_ARB2_DrawInteractions
==================
*/
void RB_ARB2_DrawInteractions( void ) {
	viewLight_t		*vLight;
	const idMaterial	*lightShader;

	GL_SelectTexture( 0 );
	qglDisableClientState( GL_TEXTURE_COORD_ARRAY );

	//
	// for each light, perform adding and shadowing
	//
	for ( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next ) {
		backEnd.vLight = vLight;

		// do fogging later
		if ( vLight->lightShader->IsFogLight() ) {
			continue;
		}
		if ( vLight->lightShader->IsBlendLight() ) {
			continue;
		}

		if ( !vLight->localInteractions && !vLight->globalInteractions
			&& !vLight->translucentInteractions ) {
			continue;
		}

		lightShader = vLight->lightShader;

		// clear the stencil buffer if needed
		if ( vLight->globalShadows || vLight->localShadows ) {
			backEnd.currentScissor = vLight->scissorRect;
			if ( r_useScissor.GetBool() ) {
				qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
					backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
					backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
					backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}
			qglClear( GL_STENCIL_BUFFER_BIT );
		} else {
			// no shadows, so no need to read or write the stencil buffer
			// we might in theory want to use GL_ALWAYS instead of disabling
			// completely, to satisfy the invarience rules
			qglStencilFunc( GL_ALWAYS, 128, 255 );
		}

		if ( r_useShadowVertexProgram.GetBool() ) {
			progs[PROG_SHADOW].programHandle->Bind();
			RB_StencilShadowPass( vLight->globalShadows );
			progs[PROG_SHADOW].programHandle->UnBind();

			RB_ARB2_CreateDrawInteractions( vLight->localInteractions );

			//qglEnable( GL_VERTEX_PROGRAM_ARB );
//			qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_STENCIL_SHADOW );
			progs[PROG_SHADOW].programHandle->Bind();

			RB_StencilShadowPass( vLight->localShadows );
			progs[PROG_SHADOW].programHandle->UnBind();

			RB_ARB2_CreateDrawInteractions( vLight->globalInteractions );
			//qglDisable( GL_VERTEX_PROGRAM_ARB );	// if there weren't any globalInteractions, it would have stayed on

			
		} else {
			RB_StencilShadowPass( vLight->globalShadows );
			RB_ARB2_CreateDrawInteractions( vLight->localInteractions );
			RB_StencilShadowPass( vLight->localShadows );
			RB_ARB2_CreateDrawInteractions( vLight->globalInteractions );
		}

		// translucent surfaces never get stencil shadowed
		if ( r_skipTranslucent.GetBool() ) {
			continue;
		}

		qglStencilFunc( GL_ALWAYS, 128, 255 );

		backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
		RB_ARB2_CreateDrawInteractions( vLight->translucentInteractions );

		backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
	}

	// disable stencil shadow test
	qglStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
	qglEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

//===================================================================================

const int	MAX_GLPROGS = 200;

// a single file can have both a vertex program and a fragment program
progDef_t	progs[MAX_GLPROGS] = {
	{ "pre-interaction.crp", 1, NULL },
	{ "interaction.crp",1,  NULL },
	{ "shadow.crp", 1, NULL },
	{ "depth.crp", 1, NULL },
	{ "transparancy-unlit.crp", 1, NULL },
	{ "virtualtexture.crp", 2, NULL },
	{ "diffuseonly.crp", 1, NULL },
	{ "layers.crp", 1, NULL }
};

/*
==================
R_LoadProgramVars
==================
*/
void R_LoadProgramVars( progHandle_t handle ) {
	//progs[handle].programHandle->Bind();

	switch(handle)
	{
		case PROG_INTERACTION:
				tr.cbRenderScene = progs[handle].programHandle->BindConstantBuffer("bmRenderSceneLights_t", NULL, sizeof(bmRenderSceneLights_t) );
				if(tr.cbRenderScene == NULL) {
				//	common->FatalError( "Failed to bind bmRenderSceneLights_t");
				}
			break;
	}

	//progs[handle].programHandle->UnBind();
}

/*
==================
R_FindARBProgram

Returns a GL identifier that can be bound to the given target, parsing
a text file if it hasn't already been loaded.
==================
*/
bmRenderProgram	*R_FindARBProgram( const char *program, bool force ) {
	int		i;

	common->Printf( "...%s\n", program );

	// see if it is already loaded
	for ( i = 0 ; progs[i].name[0] ; i++ ) {
		if ( !idStr::Icmp( program, progs[i].name ) ) {
			if(progs[i].programHandle != NULL) {
				if(force) {
					progs[i].programHandle->Reload(progs[i].name, progs[i].numPasses);
					R_LoadProgramVars( (progHandle_t)i );
				}
				return progs[i].programHandle;
			}
			else
			{
				break;
			}
		}
	}

	if ( i == MAX_GLPROGS ) {
		common->Error( "R_FindARBProgram: MAX_GLPROGS" );
	}

	// add it to the list and load it
	strncpy( progs[i].name, program, sizeof( progs[i].name ) - 1 );
	progs[i].programHandle = new bmRenderProgram( progs[i].name, progs[i].numPasses );

	R_LoadProgramVars( (progHandle_t)i );

	return progs[i].programHandle;
}

/*
==================
R_ReloadARBPrograms_f
==================
*/
void R_ReloadARBPrograms_f( const idCmdArgs &args ) {
	int		i;

	common->Printf( "----- R_ReloadARBPrograms -----\n" );
	for ( i = 0 ; progs[i].name[0] ; i++ ) {
		R_FindARBProgram( progs[i].name, true );
	//	R_LoadARBProgram( i );
	}
	common->Printf( "-------------------------------\n" );
}

/*
==================
R_CvarDeferredValueChanged_f
==================
*/
void R_CvarDeferredValueChanged_f(const idCmdArgs &args, void(*callback)( const char *s )) {
	 R_ReloadARBPrograms_f( args );
}

/*
==================
R_ARB2_Init

==================
*/
void R_ARB2_Init( void ) {
	glConfig.allowARB2Path = false;

	common->Printf( "---------- R_ARB2_Init ----------\n" );
	common->Printf( "Available.\n" );

	common->Printf( "---------------------------------\n" );

	glConfig.allowARB2Path = true;
}

