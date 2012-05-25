// tr_shadowmap.cpp
//

#include "precompiled.h"

#include "tr_local.h"

idCVar r_sb_polyOfsFactor( "r_sb_polyOfsFactor", "2", CVAR_RENDERER | CVAR_FLOAT, "polygonOffset factor for drawing shadow buffer" );
idCVar r_sb_polyOfsUnits( "r_sb_polyOfsUnits", "3000", CVAR_RENDERER | CVAR_FLOAT, "polygonOffset units for drawing shadow buffer" );
idCVar r_sb_useCulling( "r_sb_useCulling", "1", CVAR_RENDERER | CVAR_BOOL, "cull geometry to individual side frustums" );

idCVar r_sb_frustomFOV( "r_sb_frustomFOV", "90", CVAR_RENDERER | CVAR_FLOAT, "oversize FOV for point light side matching" );

float lightProjectionMatrix[16];
float lightMatrix[16];
idVec3 lightOrigin;

// frustum planes
idPlane	globalFrustum[6];

void RB_Shadow_SetModelViewMatrixForLight( idRenderLightLocal *light, int side );
idRenderLightLocal *_currentLight;
const idMaterial *defaultMaterial = NULL;
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
	zNear	= 1;

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
==================
R_Shadow_CalcLightAxialSize

all light side projections must currently match, so non-centered
and non-cubic lights must take the largest length
==================
*/
float	R_Shadow_CalcLightAxialSize( viewLight_t *vLight ) {
	float	max = 0;

	if ( !vLight->lightDef->parms.pointLight ) {
		idVec3	dir = vLight->lightDef->parms.target - vLight->lightDef->parms.origin;
		max = dir.Length();
		return max;
	}

	for ( int i = 0 ; i < 3 ; i++ ) {
		float	dist = fabs(vLight->lightDef->parms.lightCenter[i] );
		dist += vLight->lightDef->parms.lightRadius[i];
		if ( dist > max ) {
			max = dist;
		}
	}
	return max;
}


/*
==================
R_CreateShadowMapPrivateShadowCache

This is used only for a specific light
==================
*/


void R_CreateShadowMapPrivateShadowCache( srfTriangles_t *tri ) {
	if ( !tri->verts ) {
		return;
	}

	vertexCache.Alloc( tri->verts, tri->numVerts * sizeof( *tri->verts ), &tri->shadowCache );
}

/*
================
R_CalculateShadowsForModelLight
================
*/
void R_CalculateShadowsForModelLight( idRenderModel *model, viewEntity_t *vEntity,idRenderLightLocal *light, viewLight_t *vLight ) {
	

	
	
	for(int s = 0; s < model->NumSurfaces(); s++) {
		bool isAffectingLight = false;

		int shadowId = -1;

		srfTriangles_t	*tri = model->Surface( s )->geometry;
		if(tri->numVerts <= 0) // why?
			continue; 

		for(int i = 0; i < MAX_SHADOWS_PER_TRIS; i++)
		{
			if(tri->shadowMapLights[i] == NULL && shadowId == -1)
				shadowId = i;

			if(tri->shadowMapLights[i] == light) {
				for(int side = 0; side < 6; side++)
				{
					if(tri->shadowMapVisibleSides[i][side]) {
						if(vEntity)
						{
							if(tri->shadowMapMeshDist[i] == vEntity->entityDef->GetParms()->origin - light->GetParms()->origin)
							{
								isAffectingLight = true;
							}
						}
						else
						{
							if(tri->shadowMapMeshDist[i] == light->GetParms()->origin)
							{
								isAffectingLight = true;
							}
						}
						break;
					}
				}
				shadowId = i;
				break;
			}
		}

		// If there are too many lights affecting this mesh.
		if(shadowId == -1) {
			continue;
		}

		// This surface is already in the light.
		if(isAffectingLight) {
			continue; 
		}

		for(int side = 0; side < 6; side++)
		{
			if(tri->shadowMapVisibleSides[shadowId][side]) {
				if(defaultMaterial == NULL) {
					defaultMaterial = declManager->FindMaterial("worlddefault");
				}

				R_LinkLightSurf( &vLight->globalShadows, tri, vEntity, light, defaultMaterial, vLight->scissorRect, true /* FIXME? */ );

				continue;
			}
			// these shadows will all have valid bounds, and can be culled normally
			if ( r_useShadowCulling.GetBool() ) {
				RB_Shadow_SetModelViewMatrixForLight( light, side );



				if ( R_CullLocalBox( tri->bounds, tr.viewDef->worldSpace.modelMatrix, 5, globalFrustum) ) {
					tri->shadowMapVisibleSides[shadowId][side] = false;
					continue;
				}
			}

			// if we have been purged, re-upload the shadowVertexes
			if ( !tri->shadowCache ) {
				R_CreateShadowMapPrivateShadowCache( tri );
				if ( !tri->shadowCache ) {
					tri->shadowMapVisibleSides[shadowId][side] = false;
					continue;
				}
			}

			// touch the shadow surface so it won't get purged
			vertexCache.Touch( tri->shadowCache );

			if ( !tri->indexCache && r_useIndexBuffers.GetBool() ) {
				vertexCache.Alloc( tri->indexes, tri->numIndexes * sizeof( tri->indexes[0] ), &tri->indexCache, true );
			}
			if ( tri->indexCache ) {
				vertexCache.Touch( tri->indexCache );
			}


			tri->shadowMapVisibleSides[shadowId][side] = true;

			if(defaultMaterial == NULL) {
				defaultMaterial = declManager->FindMaterial("worlddefault");
			}

			R_LinkLightSurf( &vLight->globalShadows, tri, vEntity, light, defaultMaterial, vLight->scissorRect, true /* FIXME? */ );
			isAffectingLight = true;
			break;
		}

		if(isAffectingLight) {
			if(vEntity)
			{
				tri->shadowMapMeshDist[shadowId] = vEntity->entityDef->GetParms()->origin - light->GetParms()->origin;
			}
			else
			{
				tri->shadowMapMeshDist[shadowId] = light->GetParms()->origin;
			}
			tri->shadowMapLights[shadowId] = light;
		}
	}
}

/*
==================
RB_Shadow_CullSurface

Determines if the surface is visible or not.


==================
*/
void R_CalculateShadowsForLight( idRenderLightLocal *light, viewLight_t *vLight ) {

	idRenderWorldLocal *world = tr.viewDef->renderWorld;
	tr.viewDef->worldSpace.modelMatrix[12] = light->globalLightOrigin.x;
	tr.viewDef->worldSpace.modelMatrix[13] = light->globalLightOrigin.y;
	tr.viewDef->worldSpace.modelMatrix[14] = light->globalLightOrigin.z;
	tr.viewDef->worldSpace.modelMatrix[15] = 1.0f;

	// Add the world surfaces for shadow.
	for(int i = 0; i < world->localModels.Num(); i++) {
		idRenderModel *model = world->localModels[i];

		R_CalculateShadowsForModelLight( model, NULL, light, vLight );
	}

	// Add all the entities for shadowing.
	for(viewEntity_t *vEntity = tr.viewDef->viewEntitys; vEntity != NULL ; vEntity = vEntity->next) {
		idRenderModel *model;

		if(vEntity->entityDef == NULL)
			continue;

		model = vEntity->entityDef->parms.hModel;

		if(vEntity && (vEntity->scissorRect.IsEmpty() < 0 || vEntity->scissorRect.x2 <= 0 || vEntity->scissorRect.y2 <= 0 || vEntity->scissorRect.x2 >= SCREEN_WIDTH || vEntity->scissorRect.y2 >= SCREEN_HEIGHT))
			continue;

		if(vEntity && vEntity->entityDef && vEntity->entityDef->parms.weaponDepthHack)
			continue;

		// If we have a cached dynamic model there is no reason to redue the skeletal verts again.
		if(vEntity->entityDef->dynamicModel)
		{
			model = vEntity->entityDef->dynamicModel;
		}
		else
		{
			model = R_EntityDefDynamicModel( vEntity->entityDef );
		}

		R_CalculateShadowsForModelLight( model,  vEntity->entityDef->viewEntity, light, vLight );
	}
	tr.viewDef->worldSpace.modelMatrix[12] = 0;
	tr.viewDef->worldSpace.modelMatrix[13] = 0;
	tr.viewDef->worldSpace.modelMatrix[14] = 0;

}

/*
==================
RB_Shadow_CullSurface

Determines if the surface is visible or not.
==================
*/
bool RB_Shadow_CullSurface( const drawSurf_t *surf, idPlane frustumPlanes[6] ) {


	int	culled = 0;

	if ( r_sb_useCulling.GetBool() ) {
		// transform light frustum into object space, positive side points outside the light
		idPlane	localPlanes[6];
		int		plane;
		for ( plane = 0 ; plane < 6 ; plane++ ) {
			R_GlobalPlaneToLocal( surf->space->modelMatrix, frustumPlanes[plane], localPlanes[plane] );
		}

		// cull the entire entity bounding box
		// has referenceBounds been tightened to the actual model bounds?
		idVec3	corners[8];
		for ( int i = 0 ; i < 8 ; i++ ) {
			corners[i][0] = surf->geo->bounds[i&1][0];
			corners[i][1] = surf->geo->bounds[(i>>1)&1][1];
			corners[i][2] = surf->geo->bounds[(i>>2)&1][2];
		}

		for ( plane = 0 ; plane < 6 ; plane++ ) {
			int		j;
			for ( j = 0 ; j < 8 ; j++ ) {
				// if a corner is on the negative side (inside) of the frustum, the surface is not culled
				// by this plane
				if ( corners[j] * localPlanes[plane].ToVec4().ToVec3() + localPlanes[plane][3] < 0 ) {
					break;
				}
			}
			if ( j == 8 ) {
				break;			// all points outside the light
			}
		}
		if ( plane < 6 ) {
			return true;
		}
	}

	return false;
}

/*
=========================
RB_Shadow_SetModelViewMatrixForLight
=========================
*/
void RB_Shadow_SetModelViewMatrixForLight( idRenderLightLocal *light, int side )
{
	float viewMatrix[16];

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

	
	// side of a point light
	memset( viewMatrix, 0, sizeof( viewMatrix ) );
	switch ( side ) {
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
	viewMatrix[12] = -origin[0] * viewMatrix[0] + -origin[1] * viewMatrix[4] + -origin[2] * viewMatrix[8];
	viewMatrix[13] = -origin[0] * viewMatrix[1] + -origin[1] * viewMatrix[5] + -origin[2] * viewMatrix[9];
	viewMatrix[14] = -origin[0] * viewMatrix[2] + -origin[1] * viewMatrix[6] + -origin[2] * viewMatrix[10];

	viewMatrix[3] = 0;
	viewMatrix[7] = 0;
	viewMatrix[11] = 0;
	viewMatrix[15] = 1;
	
	myGlMultMatrix( viewMatrix, s_flipMatrix,lightMatrix);



	// near clip
	globalFrustum[0][0] = -viewMatrix[0];
	globalFrustum[0][1] = -viewMatrix[4];
	globalFrustum[0][2] = -viewMatrix[8];
	globalFrustum[0][3] = -(origin[0] * globalFrustum[0][0] + origin[1] * globalFrustum[0][1] + origin[2] * globalFrustum[0][2]);

	// far clip
	globalFrustum[1][0] = viewMatrix[0];
	globalFrustum[1][1] = viewMatrix[4];
	globalFrustum[1][2] = viewMatrix[8];
	globalFrustum[1][3] = -globalFrustum[0][3] - R_Shadow_CalcLightAxialSize( light->viewLight );

	// side clips
	globalFrustum[2][0] = -viewMatrix[0] + viewMatrix[1];
	globalFrustum[2][1] = -viewMatrix[4] + viewMatrix[5];
	globalFrustum[2][2] = -viewMatrix[8] + viewMatrix[9];

	globalFrustum[3][0] = -viewMatrix[0] - viewMatrix[1];
	globalFrustum[3][1] = -viewMatrix[4] - viewMatrix[5];
	globalFrustum[3][2] = -viewMatrix[8] - viewMatrix[9];

	globalFrustum[4][0] = -viewMatrix[0] + viewMatrix[2];
	globalFrustum[4][1] = -viewMatrix[4] + viewMatrix[6];
	globalFrustum[4][2] = -viewMatrix[8] + viewMatrix[10];

	globalFrustum[5][0] = -viewMatrix[0] - viewMatrix[2];
	globalFrustum[5][1] = -viewMatrix[4] - viewMatrix[6];
	globalFrustum[5][2] = -viewMatrix[8] - viewMatrix[10];

	// is this nromalization necessary?
	for ( int i = 0 ; i < 6 ; i++ ) {
		globalFrustum[i].ToVec4().ToVec3().Normalize();
	}

	for ( int i = 2 ; i < 6 ; i++ ) {
		globalFrustum[i][3] = - (origin * globalFrustum[i].ToVec4().ToVec3() );
	}

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
	RB_LogComment( "---------- RB_RenderLightView ----------\n" );

	if(light->viewLight == NULL)
		return;

	if(r_shadows.GetBool() == false) {
		return;
	}
	
	backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;

	_currentLight = light;

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
		

		// Set the modelviewmatrix from the lights perspective.
		RB_Shadow_SetModelViewMatrixForLight( light, i );
		
		// Render the shadow map.
		RenderShadowMap( (drawSurf_t *)light->viewLight->globalShadows,  i );
	}

	qglMatrixMode( GL_PROJECTION );
	qglLoadMatrixf( backEnd.viewDef->projectionMatrix );
	qglMatrixMode( GL_MODELVIEW );

	qglLoadMatrixf( backEnd.viewDef->worldSpace.modelViewMatrix );

	GL_State( GLS_DEFAULT );
}

/*
==================
RB_T_ShadowMapBuffer
==================
*/
void RB_T_FillShadowMapBuffer( const drawSurf_t *surf, srfCullInfo_t *cullInfo ) {
	int			stage;
	const idMaterial	*shader;
	const shaderStage_t *pStage;
	const float	*regs;
	float		color[4];
	const srfTriangles_t	*tri;

	tri = surf->geo;
	shader = surf->material;

	progs[PROG_DEPTH].programHandle->SetVarMatrix4fv( VV_TEX_MATVIEW, 1, &surf->space->modelMatrix[0] );

	idDrawVert *ac = (idDrawVert *)vertexCache.Position( tri->shadowCache );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
	qglTexCoordPointer( 2, GL_FLOAT, sizeof( idDrawVert ), reinterpret_cast<void *>(&ac->st) );

	//if(useFrontCulling)
	//{
	//	GL_Cull( CT_FRONT_SIDED );
	//}
	//else
	//{
		GL_Cull( CT_BACK_SIDED );
	//}
	qglDepthFunc(GL_LESS);
	
	//qglPolygonOffset( -r_sb_polyOfsFactor.GetFloat(), -r_sb_polyOfsUnits.GetFloat() );
	//qglEnable( GL_POLYGON_OFFSET_FILL );
	
	RB_DrawElementsWithCounters( tri );
	//qglDisable( GL_POLYGON_OFFSET_FILL );
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
void RB_RenderDrawShadowMappedSurfList( drawSurf_t *drawSurfs, int side ) {
	int				i;
	const drawSurf_t		*drawSurf;

	backEnd.currentSpace = NULL;
	qglMatrixMode( GL_MODELVIEW );

	for (const drawSurf_t *drawSurf = drawSurfs; drawSurf; drawSurf = drawSurf->nextOnLight ) {
		bool useFrontCulling = false;
		bool isAffectingLight = false;

		for(int i = 0; i < MAX_SHADOWS_PER_TRIS; i++)
		{
			if(drawSurf->geo->shadowMapVisibleSides[i][side]) {
				isAffectingLight = true;
				break;
			}
		}

		if(!isAffectingLight) {
			continue;
		}

		if(RB_Shadow_CullSurface( drawSurf, globalFrustum )) {
			continue;
		}



		
		srfCullInfo_t cullInfo;
		cullInfo.facing = NULL;
		
		//R_CalcInteractionFacing(NULL, drawSurf->geo, _currentLight, cullInfo, (float *)&drawSurf->space->modelMatrix[0]);

		
		

		float	matrix[16];
		myGlMultMatrix( drawSurf->space->modelMatrix, lightMatrix, matrix );
		qglLoadMatrixf( matrix );

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

		RB_T_FillShadowMapBuffer( drawSurf, NULL );

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
void bmRenderShadowMap::RenderShadowMap( drawSurf_t	 *drawSurfs,  int side  ) {
	int			lightBufferSize = r_backend_shadowMapCacheSize.GetInteger();

	backEnd.shadowMapCache.Bind();
	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + side );

	// ensures that depth writes are enabled for the depth clear
	qglViewport( 0, 0, lightBufferSize, lightBufferSize );
	qglScissor( 0, 0, lightBufferSize, lightBufferSize );

	GL_State( GLS_DEFAULT );
	backEnd.glState.faceCulling = -1;		// force face culling to set next time
	GL_Cull( CT_FRONT_SIDED );

	qglClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	progs[PROG_DEPTH].programHandle->Bind();
	progs[PROG_DEPTH].programHandle->SetVar3fv( PP_LIGHT_ORIGIN, 1, lightOrigin.ToFloatPtr() );	
			
	// fill the depth buffer and clear color buffer to black except on
	// subviews
	
	RB_RenderDrawShadowMappedSurfList( drawSurfs, side );
	

	qglDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	
	progs[PROG_DEPTH].programHandle->UnBind();
	
	backEnd.shadowMapCache.UnBind();


	GL_State( GLS_DEFAULT );
}


