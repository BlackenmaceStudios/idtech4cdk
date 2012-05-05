// VirtualTextureBuild.cpp
//

#include "precompiled.h"
#include "VirtualTexture.h"
#include "VirtualTextureBuild.h"


#include "../tools/compilers/dmap/vtmodel.h"
#include "../tools/compilers/dmap/dmap.h"
//#define USE_CORRECT_UV_GENERATION				// Works now...frickin sweat...

VirtualTextureBuilder vtBuilder;
static int vertNum = 0;

int		*allocated= NULL;
int		firstTrisOnPage = 0;
int VT_CurrentNumAreas = 0;
float spacing = 1.0;

idCVar vt_compile_areasize( "vt_compile_areasize", "4096", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT , "Size of the texture for each area of the map." );
idCVar vt_compile_size( "vt_compile_size", "16384", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT , "Size of the virtual texture to build." );
idCVar vt_compile_bsize( "vt_compile_bsize", "128", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT , "Size of the virtual texture to build." );

#define VT_Size vt_compile_bsize.GetInteger()

struct vtBuildChartScale_t {
	srfTriangles_t *tris;
	int x;
	int y;
	int w;
	int h;
};

idList<vtBuildChartScale_t> chartScalePool;

void AddScaleUVsToFitAreaDeferred( srfTriangles_t *tris, int x, int y, int w, int h )
{
	vtBuildChartScale_t vt;

	vt.tris = tris;
	vt.x = x;
	vt.y = y;
	vt.w = w;
	vt.h = h;
	chartScalePool.Append( vt );
}


void PrepareNewVTArea( bool fakePass ) {
	if(allocated != NULL) {
		delete allocated;
	}

	allocated = new int[VT_Size];
	memset( allocated, 0, sizeof( int ) * VT_Size );
	VT_CurrentNumAreas++;
	if(!fakePass)
	{
		common->Printf( "-------VTUVGenerate Processing %d---------\n", VT_CurrentNumAreas );
	}
}

/*
===============
AllocVTBlock

returns a texture number and the position inside it
===============
*/
bool AllocVTBlock (int w, int h, int *x, int *y)
{
	int		i, j;
	int		best, best2;

	best = VT_Size;

	*x = 0;
	*y = 0;

	for ( i=0 ; i <= VT_Size-w ; i++ ) {
		best2 = 0;

		for (j=0 ; j<w ; j++) {
			if (allocated[i+j] >= best) {
				break;
			}
			if (allocated[i+j] > best2) {
				best2 = allocated[i+j];
			}
		}
		if (j == w)	{	// this is a valid spot
			*x = i;
			*y = best = best2;
		}
	}

	if (best + h > VT_Size ) {
		return false;
	}

	for (i=0 ; i<w ; i++) {
		allocated[*x + i] = best + h;
	}

	return true;
}
/*
====================
VirtualTextureBuilder::GenerateVTVerts
====================
*/
void VirtualTextureBuilder::GenerateVTVerts( bmVTModel *model ) {
	float surfaceSize = 4;
	float lastSpacing = 1;

	int numVTAreas = (int)((float)vt_compile_size.GetInteger() / (float)vt_compile_areasize.GetInteger());
	numVTAreas = numVTAreas * numVTAreas;
	numCalcedAreas = numVTAreas;

	spacing = 1;

	common->Printf( "Determing UV and Virtual Texture Areas...\n" );
	common->Printf( "...VT Compile Size %i\n", vt_compile_size.GetInteger() );
	common->Printf( "...VT Area Size %i\n", vt_compile_areasize.GetInteger() );
	common->Printf( "...Number of VT areas %d\n", numVTAreas );
	common->Printf( "...Number of Faces in map %d\n", model->tris.Num() );

	int numTrisPerArea = (int)ceil((float)model->tris.Num() / (float)numVTAreas);

	chartScalePool.Clear();
	
	if(numTrisPerArea <= 0 || numVTAreas <= 0) {
		numVTAreas = 1;
		numTrisPerArea =model->tris.Num();
	}
	VT_CurrentNumAreas = 0;
#ifdef USE_CORRECT_UV_GENERATION

	float startTris = 0;

	 for(int i = 0, chartID = 0; i < model->tris.Num(); i+=numTrisPerArea, chartID++)
	{
		int triCount = numTrisPerArea;

		if(startTris + triCount > model->tris.Num() || (chartID == (numVTAreas))) {
			triCount = model->tris.Num() - startTris;
			if(triCount <= 0)
			{
				common->FatalError("GenerateVTVerts: invalid tricount\n");
			}
			
		}

		common->Printf( "Generating UV's for chart %d - %d\n", VT_CurrentNumAreas, triCount);

		toolInterface->ComputeUVAtlasForModel( model, startTris, triCount );

		for(int f = i; f < i + triCount; f++) {
			model->tris[f]->vt_AreaID = VT_CurrentNumAreas;
		}

		startTris = i+numTrisPerArea;
		VT_CurrentNumAreas++;
	}
#else // old stuff.
	float scaleAmt = 0.001;

	common->Printf("----- VT_CalcSpacing ------\n");
	while(!GenerateVTVerts_r( model, surfaceSize, numVTAreas, true )) {

		surfaceSize += scaleAmt;
		scaleAmt *= 1.03;
		lastSpacing = spacing;
	}
	common->Printf("...Surface Size %d\n", surfaceSize);

	GenerateVTVerts_r( model, surfaceSize, numVTAreas, false );

	common->Printf("---- VT_ScaleUVsToFitCell -----\n");

	// After we know everything fits properly scale everything to fit in the proper areas.
	for(int i = 0; i < chartScalePool.Num(); i++)
	{
		vtBuildChartScale_t *vt = &chartScalePool[i];
		
		ScaleUVsToFitArea( vt->tris, vt->x, vt->y, vt->w, vt->h );
	}

	// UV's might not have everything stretched to the chart borders.
	common->Printf("---- VT_OptimizeUVCharts -----\n");
	for(int i = 0; i < numVTAreas; i++)
	{
		idBounds pageBounds;
		
		pageBounds.Clear();

		for(int v = 0; v < model->tris.Num(); v++)
		{
			srfTriangles_t *tris;

			tris = model->tris[v];

			// If this mesh is in the current area add it to the bounds.
			if(tris->vt_AreaID == i) 
			{
				for(int k = 0; k < tris->numVerts; k++)
				{
					pageBounds.AddPoint( idVec3( tris->verts[k].st.x, tris->verts[k].st.y, 0.0f ));
				}
			}
		}

		float scale = 0.0f;
		float scale2 = 0.0f;
		// Scale the UV's to the 
		if( pageBounds[1].x < 0.97f && pageBounds[1].y < 0.97f )
		{
			float fakeScale;
			float fakeScale2;
			if(pageBounds[1].x > pageBounds[1].y) {
				fakeScale = pageBounds[1].x;
				fakeScale2 = pageBounds[1].y;
			}
			else
			{
				fakeScale2 = pageBounds[1].x;
				fakeScale = pageBounds[1].y;
			}

			scale = fakeScale;

			// I can't think of the right way to do this, yup I pulled another nate.
			while( scale * fakeScale < 1.0f ) {
				scale += fakeScale;
			}
			scale -= fakeScale;

			scale2 = fakeScale2;

			// I can't think of the right way to do this, yup I pulled another nate.
			while( scale2 * fakeScale2 < 1.0f ) {
				scale2 += fakeScale2;
			}
			scale2 -= fakeScale2;



			for(int v = 0; v < model->tris.Num(); v++)
			{
				srfTriangles_t *tris;

				tris = model->tris[v];

				// If this mesh is in the current area add it to the bounds.
				if(tris->vt_AreaID == i) 
				{
					for(int k = 0; k < tris->numVerts; k++)
					{
						tris->verts[k].st.x *= scale;
						tris->verts[k].st.y *= scale2;
					}
				}
			}
		}
	}


	chartScalePool.Clear();
#endif
	numCalcedAreas = VT_CurrentNumAreas;
	model->numAreas = numCalcedAreas;
	if(VT_CurrentNumAreas != numVTAreas) {
		common->Warning("Areas not divided properly!");
	}
	else {
		common->Printf( "Virtual Texture UV coordinates generated succesfully. Final blocksize %f\n", surfaceSize);
	}
}
/*
====================
VirtualTextureBuilder::ScaleUVsToFitArea
====================
*/
void VirtualTextureBuilder::ScaleUVsToFitArea( srfTriangles_t *tris, int x, int y, int w, int h ) {
	idDrawVert *v = tris->verts;
	idBounds uvBounds;

	float w1,h1,x1,y1;

	w1 = ((float)w / (float)VT_Size);
	h1 = ((float)h / (float)VT_Size);

	uvBounds.Clear();

	if(x != 0)
	{
		x1 = ((float)x / (float)VT_Size);
	}
	else
	{
		x1 = 0;
	}

	if(y != 0)
	{
		y1 = ((float)y / (float)VT_Size);
	}
	else
	{
		y1 = 0;
	}

	// Scale uv's.
	for(int i = 0; i < tris->numVerts; i++)
	{
		idVec2 *st = &v[i].st;
		st->x *= w1; 
		st->y *= h1;
	}

	// Find the actual width and hieght of the current uv set.
	for ( int i = 0 ; i < tris->numVerts ; i++ ) {
		uvBounds.AddPoint(idVec3(v[i].st.x, v[i].st.y, 0.0f));
	}

	// Move the UVs to the origin and than offset to the right place.
	for ( int i = 0 ; i < tris->numVerts ; i++ ) {
		v[i].st.x -= uvBounds[0].x;
		v[i].st.y -= uvBounds[0].y;

		v[i].st.x += x1;
		v[i].st.y += y1;
	}

	//		v[i].st.x = (v[i].st.x + x + 0.5) * ((float)w / (float)VT_Size);
	//		v[i].st.y = (v[i].st.y + y + 0.5) * ((float)h / (float)VT_Size);
}
/*
====================
VirtualTextureBuilder:RemapVertexFromParentToChildTri
====================
*/
idList<int> validIndexes;
int VirtualTextureBuilder::RemapVertexFromParentToChildTri( srfTriangles_t *parentTris, idList<idDrawVert> &childVerts, int index, bool appendVert ) {
	idDrawVert parentVert;

	// Check to see if the vertex is in the child vert and return its position.
	parentVert = parentTris->verts[index];
	for(int i = 0; i < childVerts.Num(); i++)
	{
		if(childVerts[i] == parentVert)
		{
			return i;
		}
	}

	if(appendVert) {
		childVerts.Append(parentVert);
	//	validIndexes.Append(childVerts.Num() - 1);
		return childVerts.Num() - 1;
	}

	return -1;
}

/*
====================
VirtualTextureBuilder:ScaleUVRegionToFitInTri
====================
*/
void VirtualTextureBuilder::ScaleUVRegionToFitInTri( bmVTModel *model, srfTriangles_t *parentTris, srfTriangles_t *tris, int triId, int pageId, int widthId, int heightId, float uvScaleW, float uvScaleH, float chartW, float chartH ) {
	idBounds uvRegion;
	idList<idDrawVert> verts;
	idList<int> indexes, burnIndexes;

	verts.SetGranularity( 3000 );

	validIndexes.Clear();
	burnIndexes.Clear();
	common->Printf("ScaleUVRegionToFitInTri: Processing %d...\n", pageId );

	// Get the region of UV's we want to use.
	uvRegion.Clear();
	uvRegion[0] = idVec3(widthId * uvScaleW, heightId * uvScaleH, 0.0f);

	// Even if there isn't anything after this chart this the end part of the region.
	uvRegion[1] = idVec3((widthId+1) * uvScaleW, (heightId+1 ) * uvScaleH, 0.0f);

	// Get and store all the vertexes and indexes that fall within this range.
	for(int i = 0; i < parentTris->numVerts; i++)
	{
		idDrawVert v;

		v = parentTris->verts[i];
		if(uvRegion.ContainsPoint( idVec3(v.st.x, v.st.y, 0.0f))) {
			verts.Append( v );
			validIndexes.Append( i );
		}
	}

	
	common->Printf("...Number of vertexes %d\n", verts.Num() );

	for(int i = 0; i < parentTris->numIndexes; i+=3)
	{
		int index = parentTris->indexes[i];
		int remapIndex[3];

		if(index == -1)
		{
			continue;
		}

		remapIndex[0] = RemapVertexFromParentToChildTri( parentTris, verts, index, false);

		if(remapIndex[0] != -1)
		{
			bool burnAllIndexes;

			// Check to see if this UV exclusively belongs to this tris.
			for(int c = 1; c < 3; c++)
			{
				index = parentTris->indexes[i + c];
				if(index == -1)
				{
					remapIndex[0] = remapIndex[1] = -1;
					break;
				}
				remapIndex[c] = RemapVertexFromParentToChildTri( parentTris, verts, index, false);
			}

			burnAllIndexes = (remapIndex[0] != -1 && remapIndex[1] != -1 && remapIndex[2] != -1);

			for(int c = 0; c < 3; c++)
			{
				index = parentTris->indexes[i + c];
				if(burnAllIndexes)
				{
					burnIndexes.Append(parentTris->indexes[i + c]);
				}
				else
				{
					remapIndex[c] = RemapVertexFromParentToChildTri( parentTris, verts, index, true);
				}
				indexes.Append( remapIndex[c] );
				
			}
		}
		else
		{
			// Test the other two verts and see if it works out.
			for(int c = 0; c < 3; c++)
			{
				index = parentTris->indexes[i + c];
				if(index == -1)
				{
					remapIndex[0] = remapIndex[1] = -1;
					break;
				}
				remapIndex[c] = RemapVertexFromParentToChildTri( parentTris, verts, index, false);
			}

			if(remapIndex[0] == remapIndex[1] || remapIndex[1] == remapIndex[2] || remapIndex[0] == remapIndex[2])
				continue;

			for(int c = 0; c < 3; c++)
			{
				index = parentTris->indexes[i + c];
				remapIndex[c] = RemapVertexFromParentToChildTri( parentTris, verts, index, true);
			}

			bool allValid = false;
			for(int v = 0; v < validIndexes.Num(); v++)
			{
				for(int c = 0; c < 3; c++)
				{
					if(validIndexes[v] == remapIndex[c])
					{
						indexes.Append( remapIndex[0] );
						indexes.Append( remapIndex[1] );
						indexes.Append( remapIndex[2] );
						allValid  = true;
						break;
					}
				}

				if(allValid) {
					break;
				}
			}
		}
	}

	for(int i = 0; i < burnIndexes.Num(); i++)
	{
		for(int f = 0; f < parentTris->numIndexes; f++)
		{
			if(parentTris->indexes[f] == burnIndexes[i])
			{
				parentTris->indexes[f] = -1;
			}
		}
	}

	common->Printf("...Number of indexes %d\n", indexes.Num() );

	// Now that we have all the verts and indexes for this section(alloc verts and indexes as well here for the tris).
	model->SetVertexesForTris( triId, &verts[0], verts.Num(), &indexes[0], indexes.Num() );

	// The UV's might be outside of the target range if the a triangle is barely within the requested bounds.
	// This will create duplicate triangles in places fixme!
	uvRegion.Clear();
	for(int i = 0; i < tris->numVerts; i++)
	{
		uvRegion.AddPoint(idVec3(tris->verts[i].st.x, tris->verts[i].st.y, 0.0f));
	}

	common->Printf("...Modified UV Range %f %f\n", uvRegion[0].x, uvRegion[1].y);

	// Finally get the UV's offset properly
	idVec2 scale;
	scale.x = 1.0f;
	scale.y = 1.0f;
	for(int i = 0; i < tris->numVerts; i++)
	{
		idVec2 st = tris->verts[i].st;

		st.x -= uvRegion[0].x;
		st.y -= uvRegion[0].y;

		st.x *= chartW;
		st.y *= chartH;

		if(abs(st.x) > scale.x) {
			scale.x = abs(st.x);
		}

		if(abs(st.y) > scale.y) {
			scale.y = abs(st.y);
		}

		tris->verts[i].st = st;
	}

	// Scale the UV's if they go beyond the 0-1 border.
	for(int i = 0; i < tris->numVerts; i++)
	{
		tris->verts[i].st /= scale;

	}
}

/*
====================
VirtualTextureBuilder::GenerateVTVerts
====================
*/
bool VirtualTextureBuilder::GenerateVTVerts_r( bmVTModel *model,  float surfaceSize, int numVTAreas, bool fakePass ) {
	idBounds bounds;
	idVec3 exactSize, delta;
	float w, h, s, t;
	int axis = 0;
	idVec3 vecs[2];
	idVec2 scaleST;


	spacing = 1.0;
	firstTrisOnPage = 0;
	VT_CurrentNumAreas = 0;

	PrepareNewVTArea(fakePass);

	int wtest = 0, htest = 0;

	scaleST = idVec2(1, 1);
	chartScalePool.Clear();
	int numTrisOnChart = 0;

	for(int d = 0; d < model->tris.Num(); d++)
	{
		idVec3 size;
		idPlane plane;
		idVec3 planeNormal;
		idDrawVert *v = model->tris[d]->verts;
		
		vecs[0] = idVec3(0,0,0);
		vecs[1] = idVec3(0,0,0);

		plane.FromPoints( v[0].xyz, v[1].xyz, v[2].xyz );

		// Clear the current bounds.
		bounds.Clear();

		// Get the bounds for the tris
		for(int i = 0; i < model->tris[d]->numVerts; i++)
		{
			bounds.AddPoint( v[i].xyz );
		}

		for ( int i = 0 ; i < 3 ; i++ ) {
			exactSize[i] = bounds[1][i] - bounds[0][i];
			bounds[0][i] = surfaceSize * floor( bounds[0][i] / surfaceSize );
			bounds[1][i] = surfaceSize * ceil( bounds[1][i] / surfaceSize );
			size[i] = ((bounds[1][i] - bounds[0][i]) / surfaceSize + 1) ;

			
		}

	

		planeNormal[0] = fabs( plane[0] );
		planeNormal[1] = fabs( plane[1] );
		planeNormal[2] = fabs( plane[2] );

		
		// Calculate the UV orientation and size.
		if ( planeNormal[0] >= planeNormal[1] && planeNormal[0] >= planeNormal[2] ) {
			w = size[1];
			h = size[2];
			axis = 0;
			vecs[0][1] = 1.0 / surfaceSize;
			vecs[1][2] = 1.0 / surfaceSize;
		} else if ( planeNormal[1] >= planeNormal[0] && planeNormal[1] >= planeNormal[2] ) {
			w = size[0];
			h = size[2];
			axis = 1;
			vecs[0][0] = 1.0 / surfaceSize;
			vecs[1][2] = 1.0 / surfaceSize;
		} else {
			w = size[0];
			h = size[1];
			axis = 2;
			vecs[0][0] = 1.0 / surfaceSize;
			vecs[1][1] = 1.0 / surfaceSize;
		}

		int x, y;
		
		wtest += w;
		htest += h;

generatePage:		
		// Check to see if this will fit on the current page.
		if(!AllocVTBlock( w, h , &x, &y ) && model->tris[d]->vt_uvGenerateType < Editor_ImportUVs_SinglePage) {
			scaleST.x = 1;
			scaleST.y = 1;


			// If we  already filled up all the texture space for all of the target pages, start over with a higher blocksize(less space).
			if(VT_CurrentNumAreas >= numVTAreas || numTrisOnChart == 0)
			{
				return false;
			}
			
			// Prepare a new VT page.
			PrepareNewVTArea(fakePass);
			firstTrisOnPage = d;
			numTrisOnChart = 0;
		}
		
		switch(model->tris[d]->vt_uvGenerateType)
		{
			
			case Editor_GenerateUVs_Q3Style:
				{
					if(!fakePass)
					{
						for ( int i = 0 ; i < model->tris[d]->numVerts ; i++ ) {
			
							delta = v[i].xyz - bounds[0];
							v[i].st.x = (DotProduct(delta, vecs[0]) + x + 0.5) / VT_Size;
							v[i].st.y = (DotProduct(delta, vecs[1]) + y + 0.5) / VT_Size;

							if(v[i].st.x > 1 || v[i].st.y > 1) {
								goto generatePage;
							}
						}

						model->tris[d]->vt_AreaID = VT_CurrentNumAreas;
					}
				}
			case Editor_GenerateUVs_Orient:
				{
					if(!fakePass)
					{
						toolInterface->ComputeUVAtlasForModel( model, d, 1 );
						AddScaleUVsToFitAreaDeferred(model->tris[d], x, y, w, h);
						model->tris[d]->vt_AreaID = VT_CurrentNumAreas;
					}
				}
				break;
			case Editor_ImportUVs_AutoSpacing:
				{
					if(!fakePass)
					{
						AddScaleUVsToFitAreaDeferred(model->tris[d], x, y, w, h);
						model->tris[d]->vt_AreaID = VT_CurrentNumAreas;
					}
				}
				break;
			case Editor_ImportUVs_SinglePage:
				{
					

					if(firstTrisOnPage != d)
					{
						// Prepare a new VT page.
						PrepareNewVTArea(fakePass);
						firstTrisOnPage = d;
					}
					
					model->tris[d]->vt_AreaID = VT_CurrentNumAreas;
					
					// Prepare a new VT page.
					PrepareNewVTArea(fakePass);
					firstTrisOnPage = d+1;
					continue;
				}
				break;
			default:
				{
					float numUVPagesForTri, numIndexesPerArea, cellsPerWidth, cellsPerHeight;
					float UVScale;

					srfTriangles_t *tris = model->tris[d];

					if(firstTrisOnPage != d)
					{
						// Prepare a new VT page.
						PrepareNewVTArea(fakePass);
						firstTrisOnPage = d;
					}
					
					numUVPagesForTri = model->tris[d]->vt_uvGenerateType - Editor_ImportUVs_SinglePage;
					
					UVScale = (numUVPagesForTri / 10.0f);
					numUVPagesForTri = numVTAreas * UVScale; 
					

					cellsPerWidth = (float)floor(sqrt(numUVPagesForTri));
					cellsPerHeight = (float)ceil(sqrt(numUVPagesForTri));

					float UVScaleW = (1.0f / cellsPerWidth);
					float UVScaleH = (1.0f / cellsPerHeight);

					int cellId = 0;
					for(int w = 0; w < cellsPerWidth; w++)
					{
						for(int h = 0; h < cellsPerHeight; h++, cellId++)
						{
							model->AllocTriangleAtPosition( d + cellId );

							srfTriangles_t *newTris = model->tris[d + cellId];
							ScaleUVRegionToFitInTri( model, tris, newTris, d + cellId, VT_CurrentNumAreas, w, h, UVScaleW, UVScaleH, cellsPerWidth, cellsPerHeight );
							newTris->vt_AreaID = VT_CurrentNumAreas;
							newTris->vt_uvGenerateType = Editor_ImportUVs_SinglePage; // Just incase we have to go through the UVs again.
							// Prepare a new VT page.
							PrepareNewVTArea(fakePass);
						}
					}

					// Remove the only triangles.
					R_FreeStaticTriSurf( tris ); 
					model->tris.Remove( tris );

					d += cellId + 1;
					
					// Prepare a new VT page.
					PrepareNewVTArea(fakePass);
					firstTrisOnPage = d+1;
					continue;
				}
		}
		
		
		numTrisOnChart++;

	}	

	return true;
}
