// tr_shadowmap.cpp
//

#include "precompiled.h"

#include "tr_local.h"

idCVar r_sb_polyOfsFactor( "r_sb_polyOfsFactor", "2", CVAR_RENDERER | CVAR_FLOAT, "polygonOffset factor for drawing shadow buffer" );
idCVar r_sb_polyOfsUnits( "r_sb_polyOfsUnits", "3000", CVAR_RENDERER | CVAR_FLOAT, "polygonOffset units for drawing shadow buffer" );
idCVar r_sb_useCulling( "r_sb_useCulling", "1", CVAR_RENDERER | CVAR_BOOL, "cull geometry to individual side frustums" );

idCVar r_sb_frustomFOV( "r_sb_frustomFOV", "92", CVAR_RENDERER | CVAR_FLOAT, "oversize FOV for point light side matching" );

float lightProjectionMatrix[16];
float lightMatrix[16];
idVec3 lightOrigin;

/*
=========================
RB_SetShadowMapProjectionMatrix
=========================
*/
void RB_SetShadowMapProjectionMatrix( void ) {
	float	xmin, xmax, ymin, ymax;
	float	width, height;
	float	zNear;

	float	fov = r_sb_frustomFOV.GetFloat();

	//
	// set up 90 degree projection matrix
	//
	zNear	= 4;

	ymax = zNear * tan( fov * idMath::PI / 360.0f );
	ymin = -ymax;

	xmax = zNear * tan( fov * idMath::PI / 360.0f );
	xmin = -xmax;

	width = xmax - xmin;
	height = ymax - ymin;

	lightProjectionMatrix[0] = 2 * zNear / width;
	lightProjectionMatrix[4] = 0;
	lightProjectionMatrix[8] = 0;
	lightProjectionMatrix[12] = 0;

	lightProjectionMatrix[1] = 0;
	lightProjectionMatrix[5] = 2 * zNear / height;
	lightProjectionMatrix[9] = 0;
	lightProjectionMatrix[13] = 0;

	// this is the far-plane-at-infinity formulation, and
	// crunches the Z range slightly so w=0 vertexes do not
	// rasterize right at the wraparound point
	lightProjectionMatrix[2] = 0;
	lightProjectionMatrix[6] = 0;
	lightProjectionMatrix[10] = -0.999f;
	lightProjectionMatrix[14] = -2.0f * zNear;

	lightProjectionMatrix[3] = 0;
	lightProjectionMatrix[7] = 0;
	lightProjectionMatrix[11] = -1;
	lightProjectionMatrix[15] = 0;

	qglMatrixMode( GL_PROJECTION );
	qglLoadMatrixf( lightProjectionMatrix );
	qglMatrixMode( GL_MODELVIEW );
}
/*
=========================
RB_Shadow_SetModelViewMatrixForLight
=========================
*/
void RB_Shadow_SetModelViewMatrixForLight( idRenderLightLocal *light, float *viewMatrix )
{
	

	static float	s_flipMatrix[16] = {
		// convert from our coordinate system (looking down X)
		// to OpenGL's coordinate system (looking down -Z)
		0, 0, -1, 0,
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	};

	backEnd.currentSpace = NULL;

	idVec3	origin = light->globalLightOrigin;
	lightOrigin = origin;

	viewMatrix[12] = -origin[0] * viewMatrix[0] + -origin[1] * viewMatrix[4] + -origin[2] * viewMatrix[8];
	viewMatrix[13] = -origin[0] * viewMatrix[1] + -origin[1] * viewMatrix[5] + -origin[2] * viewMatrix[9];
	viewMatrix[14] = -origin[0] * viewMatrix[2] + -origin[1] * viewMatrix[6] + -origin[2] * viewMatrix[10];

	viewMatrix[3] = 0;
	viewMatrix[7] = 0;
	viewMatrix[11] = 0;
	viewMatrix[15] = 1;

	
	myGlMultMatrix( viewMatrix, s_flipMatrix,lightMatrix);
}

/*
=========================
bmRenderShadowMap::bmRenderShadowMap
=========================
*/
void bmRenderShadowMap::Init( const char *lightName, idRenderLightLocal *light ) {

	//depthCubeMap = globalImages->CreateCubemapFBOImage( va( "_%s_sm_cubemap", lightName ) );
	//RenderLightView(light);
	needsUpdate = true;
}

/*
=========================
bmRenderShadowMap::RenderLightView
=========================
*/
void bmRenderShadowMap::RenderLightView( idRenderLightLocal *light ) {
	drawSurf_t	 **drawSurfs;
	int			numDrawSurfs;
	

	RB_LogComment( "---------- RB_RenderLightView ----------\n" );
	
	backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;

	drawSurfs = (drawSurf_t **)&backEnd.viewDef->drawSurfs[0];
	numDrawSurfs = backEnd.viewDef->numDrawSurfs;

	

	// Set the shadowmap projection Matrix.
	RB_SetShadowMapProjectionMatrix();

	if(needsUpdate) {
		needsUpdate = false;
	}
	else
	{
		//return;
	}
	
	for ( int i = 0 ; i < 6 ; i++ ) {
		renderView_t	ref;
		float viewMatrix[16];
		// side of a point light
		memset( viewMatrix, 0, sizeof( viewMatrix ) );
		switch ( i ) {
		case 0:
			viewMatrix[0] = 1;
			viewMatrix[9] = 1;
			viewMatrix[6] = -1;
			break;
		case 1:
			viewMatrix[0] = -1;
			viewMatrix[9] = -1;
			viewMatrix[6] = -1;
			break;
		case 2:
			viewMatrix[4] = 1;
			viewMatrix[1] = -1;
			viewMatrix[10] = 1;
			break;
		case 3:
			viewMatrix[4] = -1;
			viewMatrix[1] = -1;
			viewMatrix[10] = -1;
			break;
		case 4:
			viewMatrix[8] = 1;
			viewMatrix[1] = -1;
			viewMatrix[6] = -1;
			break;
		case 5:
			viewMatrix[8] = -1;
			viewMatrix[1] = 1;
			viewMatrix[6] = -1;
			break;
		}

		// Set the modelviewmatrix from the lights perspective.
		RB_Shadow_SetModelViewMatrixForLight( light, &viewMatrix[0] );
		
		// Render the shadow map.
		RenderShadowMap( drawSurfs, numDrawSurfs, i );
	}

	qglMatrixMode( GL_PROJECTION );
	qglLoadMatrixf( backEnd.viewDef->projectionMatrix );
	qglMatrixMode( GL_MODELVIEW );

	qglLoadMatrixf( backEnd.viewDef->worldSpace.modelViewMatrix );
}

/*
==================
RB_T_ShadowMapBuffer
==================
*/
void RB_T_FillShadowMapBuffer( const drawSurf_t *surf, bool useFrontCulling ) {
	int			stage;
	const idMaterial	*shader;
	const shaderStage_t *pStage;
	const float	*regs;
	float		color[4];
	const srfTriangles_t	*tri;

	tri = surf->geo;
	shader = surf->material;

	if ( !surf->geo->ambientCache ) {
		R_CreateAmbientCache( const_cast<srfTriangles_t *>(surf->geo), false ); 
	}

	progs[PROG_DEPTH].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

	idDrawVert *ac = (idDrawVert *)vertexCache.Position( tri->ambientCache );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
	qglTexCoordPointer( 2, GL_FLOAT, sizeof( idDrawVert ), reinterpret_cast<void *>(&ac->st) );

	if(useFrontCulling)
	{
		GL_Cull( CT_FRONT_SIDED );
	}
	else
	{
		GL_Cull( CT_BACK_SIDED );
	}
	qglDepthFunc(GL_LESS);
	
	qglPolygonOffset( -r_sb_polyOfsFactor.GetFloat(), -r_sb_polyOfsUnits.GetFloat() );
	qglEnable( GL_POLYGON_OFFSET_FILL );
	
	RB_DrawElementsWithCounters( tri );
	qglDisable( GL_POLYGON_OFFSET_FILL );
}


/*
====================
RB_RenderDrawSurfListWithFunction

The triangle functions can check backEnd.currentSpace != surf->space
to see if they need to perform any new matrix setup.  The modelview
matrix will already have been loaded, and backEnd.currentSpace will
be updated after the triangle function completes.
====================
*/
void RB_RenderDrawShadowMappedSurfList( drawSurf_t **drawSurfs, int numDrawSurfs ) {
	int				i;
	const drawSurf_t		*drawSurf;

	backEnd.currentSpace = NULL;
	qglMatrixMode( GL_MODELVIEW );

	for (i = 0  ; i < numDrawSurfs ; i++ ) {
		bool useFrontCulling = false;
		drawSurf = drawSurfs[i];
		
		float	matrix[16];
		myGlMultMatrix( drawSurf->space->modelMatrix, lightMatrix, matrix );
		qglLoadMatrixf( matrix );

		if ( drawSurf->space->weaponDepthHack ) {
			//RB_EnterWeaponDepthHack();
			continue;
		}

		if ( drawSurf->space->modelDepthHack != 0.0f ) {
			RB_EnterModelDepthHack( drawSurf->space->modelDepthHack );
		}

		

		// change the scissor if needed
		/*
		if ( r_useScissor.GetBool() && !backEnd.currentScissor.Equals( drawSurf->scissorRect ) ) {
			backEnd.currentScissor = drawSurf->scissorRect;
			qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
				backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
				backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
				backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
		}
		*/

		

		// render it
		RB_T_FillShadowMapBuffer( drawSurf, useFrontCulling );

		if ( drawSurf->space->weaponDepthHack || drawSurf->space->modelDepthHack != 0.0f ) {
			//RB_LeaveDepthHack();
		}

		backEnd.currentSpace = drawSurf->space;
	}
}

/*
=========================
bmRenderShadowMap::RenderShadowMap
=========================
*/
void bmRenderShadowMap::RenderShadowMap( drawSurf_t	 **drawSurfs, int numDrawSurfs, int side  ) {
	int			lightBufferSize = r_backend_shadowMapCacheSize.GetInteger();
	
	backEnd.shadowMapCache.Bind();

	// ensures that depth writes are enabled for the depth clear
	qglViewport( 0, 0, lightBufferSize, lightBufferSize );
	qglScissor( 0, 0, lightBufferSize, lightBufferSize );

	GL_State( GLS_DEFAULT );
	backEnd.glState.faceCulling = -1;		// force face culling to set next time
	GL_Cull( CT_FRONT_SIDED );

	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + side );
	qglClearColor(0,0,0,0);
	qglClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		

	progs[PROG_DEPTH].programHandle->Bind();
	progs[PROG_DEPTH].programHandle->SetVar3fv( PP_LIGHT_ORIGIN, 1, lightOrigin.ToFloatPtr() );
			
			
	// fill the depth buffer and clear color buffer to black except on
	// subviews
	
	RB_RenderDrawShadowMappedSurfList( drawSurfs, numDrawSurfs );
	

	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	
	progs[PROG_DEPTH].programHandle->UnBind();
	
	backEnd.shadowMapCache.UnBind();
}


