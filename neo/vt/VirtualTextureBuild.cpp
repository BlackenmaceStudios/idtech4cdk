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
idCVar vt_compile_size( "vt_compile_size", "32768", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT , "Size of the virtual texture to build." );
idCVar vt_compile_bsize( "vt_compile_bsize", "128", CVAR_RENDERER | CVAR_INTEGER | CVAR_CHEAT , "Size of the virtual texture to build." );

idCVar vt_debug_tiles( "vt_debug_tiles", "0", CVAR_RENDERER | CVAR_BOOL , "Write out each tile as a dds texture." );

byte *compressedBuffer = NULL;
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
		common->Printf( "Processing VT Chart %d...\n", VT_CurrentNumAreas );
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
	common->Printf("...Surface Size %f\n", surfaceSize);

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
int VirtualTextureBuilder::RemapVertexFromParentToChildTri( srfTriangles_t *parentTris, idList<idDrawVert> &childVerts, int index, bool appendVert, bool allowNewNeighbor ) {
	idDrawVert parentVert;

	// Check to see if the vertex is in the child vert and return its position.
	if(index != -1)
	{
		parentVert = parentTris->verts[index];
	}

	if(appendVert == false && index == -1) {
		return -1;
	}
	for(int i = 0; i < childVerts.Num(); i++)
	{
		if(index == -1)
		{
			parentVert = childVerts[i];
		}
		if(childVerts[i].xyz == parentVert.xyz && childVerts[i].st == parentVert.st)
		{
			int indexNum = i;

			// If we aren't appending a vertex only use validIndexes, if appendVert use any and all if present.
			if(childVerts[i].color[0] >= 1 || appendVert) {
				return indexNum;
			}



			break; // Either was added later or isn't part of the section and was append on.
		}
	}

	if(appendVert) {
		if(allowNewNeighbor) {
			//validIndexes.Append(childVerts.Num());
			parentVert.color[0]=1;
		}
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
#define VT_EPSILON			0.000f

idList<int> burnIndexes;
int burnIndexCnt = 0;
bool VirtualTextureBuilder::ScaleUVRegionToFitInTri( bmVTModel *model, srfTriangles_t *parentTris, srfTriangles_t *tris, int triId, int pageId, int widthId, int heightId, float uvScaleW, float uvScaleH, float chartW, float chartH ) {
	
	idList<idDrawVert> verts;
	idList<int> indexes, realIndexes, edgeTris;
	idVec3 epsilon = idVec3(VT_EPSILON, VT_EPSILON, 0.0f);

	verts.SetGranularity( 3000 );
	indexes.Clear();
	edgeTris.Clear();
	validIndexes.Clear();
	
	common->Printf("ScaleUVRegionToFitInTri: Processing %d...\n", pageId );

	// Get the region of UV's we want to use.
	idBounds uvRegion(idVec3(widthId * uvScaleW, heightId * uvScaleH, 0.0f) - epsilon, 
					  idVec3((widthId+1) * uvScaleW, (heightId+1 ) * uvScaleH, 0.0f) + epsilon);


	common->Printf("...(%f %f) -> (%f %f)\n", uvRegion[0].x, uvRegion[0].y, uvRegion[1].x, uvRegion[1].y);

	// Get and store all the vertexes and indexes that fall within this range.

	int dupVerts = 0;

	for(int c = 0; c < parentTris->numIndexes; c+=6)
	{
		idDrawVert v;
		float appendVert = 0;

		const int trisIndexQuadTbl[4] = {2, 1, 3, 0};
		int trisIndexQuadTblTris[6] = {0, 1, 3, 1, 2, 3};


		int trisIndexQuad[4];

		for(int d = 0; d < 4; d++)
		{
			trisIndexQuad[d] = parentTris->indexes[trisIndexQuadTbl[d] + c];

		}



		for(int d = 0; d < 4; d++)
		{
			int i = trisIndexQuad[d];
			v = parentTris->verts[i];
			v.color[0] = 2;
			parentTris->verts[i].color[0] = 0;
			if(uvRegion.ContainsPoint( idVec3(v.st.x, v.st.y, 0.0f))) {
				appendVert++;
			}
		}

		

		if(appendVert) {
			int ind[6];
			int numDupIndexes = 0;

		


			for(int d = c, t = 0; d < c + 4; d++, t++)
			{
				int i = trisIndexQuad[t];
				v = parentTris->verts[i];
				v.color[0] = 2;
				parentTris->verts[i].color[0] = 0;

				ind[t] = -1;

				for(int f = 0; f < verts.Num(); f++) {
					if(verts[f].xyz == v.xyz) {
						ind[t] = f;
						break;
					}
				}
			}

			int dupIndx[6];
			for(int g = 0; g < burnIndexes.Num(); g+=4) {
				numDupIndexes = 0;
				for(int d = 0; d < 4; d++) {
					int i = trisIndexQuad[d];
					int i2 = burnIndexes[g+d];
					if(i2 == i) {
						dupIndx[numDupIndexes++] = i;
					}
				}


				if(numDupIndexes > 2)
					break;
			}

			if(numDupIndexes > 2) {
				burnIndexCnt++;
				continue;
			}

			for(int d = 0; d < 6; d++) {
				int i = trisIndexQuad[trisIndexQuadTblTris[d]];
				v = parentTris->verts[i];
				if(ind[trisIndexQuadTblTris[d]] == -1)
				{
					validIndexes.Append( verts.Num() );
					verts.Append( v );
				}
				else
				{
					validIndexes.Append( ind[trisIndexQuadTblTris[d]] );
				}
				
			}

			for(int d = 0; d < 4; d++) {
				int i = trisIndexQuad[d];
				burnIndexes.Append( i );
			}
		}
	}

	realIndexes = validIndexes;
	if(realIndexes.Num() <= 0) {
		return false;
	}

	common->Printf("...Number of vertexes %d\n", verts.Num() );
	common->Printf("...Number of indexes %d-%d\n", realIndexes.Num(), indexes.Num() );

	// Now that we have all the verts and indexes for this section(alloc verts and indexes as well here for the tris).
	
	model->SetVertexesForTris( tris, &verts[0], verts.Num(), &realIndexes[0], realIndexes.Num() );
	// The UV's might be outside of the target range if the a triangle is barely within the requested bounds.
	// This will create duplicate triangles in places fixme!
	uvRegion.Clear();
	for(int i = 0; i < tris->numVerts; i++)
	{
		uvRegion.AddPoint(idVec3(tris->verts[i].st.x, tris->verts[i].st.y, 0.0f));
	}

	

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

		if(st.x < 0) {
			common->Warning("Texture Coord S < 0 setting to 0\n");
			st.x = 0;
		}

		if(st.y < 0) {
			common->Warning("Texture Coord T < 0 setting to 0\n");
			st.y = 0;
		}

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

		if(tris->verts[i].st.x > 1 || tris->verts[i].st.y > 1) {
			common->Warning("ScaleUVRegionToFitInTri: Failed\n");
		}
	}

	return true;
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
	VT_CurrentNumAreas = -1;

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

		if(model->tris[d]->vt_uvGenerateType == Editor_NotPartOfVirtualTexture) {
			continue;
		}

		if(model->tris[d]->vt_AreaID == -1) {
			continue;
		}
		
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

			// Prepare a new VT page.
			PrepareNewVTArea(fakePass);

			// If we  already filled up all the texture space for all of the target pages, start over with a higher blocksize(less space).
			if(VT_CurrentNumAreas >= numVTAreas || numTrisOnChart == 0)
			{
				return false;
			}
			
			
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
				break;
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
					numTrisOnChart = 0;
					continue;
				}
				break;
			default:
				{
					float numUVPagesForTri, numIndexesPerArea, cellsPerWidth, cellsPerHeight;
					float UVScale;

					storedTris.Clear();

					srfTriangles_t *tris = model->tris[d];
					
					burnIndexes.Clear();
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
					bool allocNewTris = true;
					for(int h = 0; h < cellsPerHeight; h++)
					{
						for(int w = 0; w < cellsPerWidth; w++, cellId++)	
						{
							model->AllocTriangleAtPosition( d + cellId );

							srfTriangles_t *newTris = model->tris[d + cellId];
							if(!ScaleUVRegionToFitInTri( model, tris, newTris, d + cellId, VT_CurrentNumAreas, w, h, UVScaleW, UVScaleH, cellsPerWidth, cellsPerHeight )) {
								common->Warning("!!!!!!! UV Section has no UV's!!!!!!!\n");
								model->FreeTri( d + cellId );
								cellId--;
								allocNewTris = false;
								continue;
							}
							newTris->vt_AreaID = VT_CurrentNumAreas;
							newTris->vt_uvGenerateType = Editor_ImportUVs_SinglePage; // Just incase we have to go through the UVs again.
							// Prepare a new VT page.
							PrepareNewVTArea(fakePass);
						}
					}

					// Double check the mesh.
					int numUntouchedIndexes = 0;
					for(int r = 0; r < tris->numIndexes; r++)
					{
						if(tris->indexes[r] != -1) {
							numUntouchedIndexes++;
						}
					}

					common->Warning("%d burned tris in mesh\n", burnIndexCnt);

					// Remove the old surface, it isn't needed anymore.
					model->FreeTri( model->tris.FindIndex( tris ) );

					d += cellId;
					
					// Prepare a new VT page.
					PrepareNewVTArea(fakePass);
					firstTrisOnPage = d;
					numTrisOnChart = 0;

					storedTris.Clear();
					continue;
				}
		}
		
		
		numTrisOnChart++;

	}	

	return true;
}

/*
===========================
bmVirtualTextureFile::InitNewVirtualTextureFile
===========================
*/
bool bmVirtualTextureFile::InitNewVirtualTextureFile( const char *path, int numAreas ) {
	header.Reset();

	common->Printf( "----------- VT_InitNewVirtualTextureFile ----------\n" );

	

	// Open the virtual texture for writing.
	header.numCharts = numAreas;
	for(int i = 0; i < VIRTUALTEXTURE_NUMLEVELS; i++)
	{
		vtpath[i] = va( "%s_level%d%s", path, i, VIRTUALTEXTURE_EXTEN );
		f[i] = fileSystem->OpenFileWrite( va( "%s_level%d%s_temp", path, i, VIRTUALTEXTURE_EXTEN ), "fs_devpath" );
		if(f[i] == NULL) {
			common->Warning("Failed to open %s for writing\n", path );
			return false;
		}

		// Write the header out first so everything gets positioned correctly and we can write in real time.
		header.WriteToFile( f[i] );
	}

	

	imglist.Clear();
	

	
	byte *buffer;
	int imgWidth = 0;
	int imgHeight = 0;
	ID_TIME_T timeStamp;

	// Allocate these for resuse inorder not to fragment up memory.
	buffer = (byte *)R_StaticAlloc( VIRTUALTEXTURE_PAGESIZE * VIRTUALTEXTURE_PAGESIZE * 4 );
	byte *mippedLevel = (byte *)R_StaticAlloc( 2048 * 2048 * 4 );
	byte *mippedLevel2 = (byte *)R_StaticAlloc( 1024 * 1024 * 4 );
	for(int i = 0; i < numAreas; i++)
	{
		idStr targaPath = path + idStr(va( "_u%d_v1", i + 1));
		targaPath.SetFileExtension( ".tga" );

		//R_LoadImage( targaPath.c_str(), &buffer, &imgWidth, &imgHeight, &timeStamp, true );
		LoadTGA(targaPath.c_str(), &buffer, &imgWidth, &imgHeight, &timeStamp, false );

		// Check the image stats.
		if(imgWidth != VIRTUALTEXTURE_PAGESIZE || imgHeight != VIRTUALTEXTURE_PAGESIZE) {
			common->FatalError("InitNewVirtualTextureFile: Chart image dimensions aren't %d\n", VIRTUALTEXTURE_PAGESIZE);
		}

		if(imgWidth <= 0 || imgHeight <= 0) {
			common->Warning("Area texture for VT not found...\n");
			return false;
		}
		else
		{
			common->Printf( "------- VT_BuildArea(%d/%d) ----------\n", i + 1, numAreas);
		}

		int tileSize = VIRTUALTEXTURE_TILESIZE;
		
		common->Printf("...Mipmap(level 0) (%dx%d)\n", imgWidth, imgHeight); 

		int maxTiles = (imgWidth / tileSize) * (imgWidth / tileSize);
		int currentTile = 0;

		for(int h = 0; h < imgWidth / tileSize; h++) {
			for(int w = 0; w < imgWidth / tileSize; w++, header.numTiles++, currentTile++) {

				//common->Printf( "...Tile %d/%d", currentTile, maxTiles);
				WriteTile( buffer, w * tileSize, h * tileSize, imgWidth, tileSize, 0 );
			}
		}

		

		// Write the next mipmap level.
		R_MipMap( buffer, mippedLevel, imgWidth, imgHeight, false );
		imgWidth = imgWidth >> 1;
		imgHeight = imgHeight >> 1;
		tileSize = tileSize >> 1;

		common->Printf("...Mipmap (%dx%d) %d\n", imgWidth, imgHeight, tileSize );
		for(int h = 0; h < imgWidth / tileSize; h++) {
			for(int w = 0; w < imgWidth / tileSize; w++) {

				//common->Printf( "...Tile %d/%d", currentTile, maxTiles);
				WriteTile( mippedLevel, w * tileSize, h * tileSize, imgWidth, tileSize, 1 );
			}
		}
		

		// Write the next mipmap level.
		R_MipMap( mippedLevel, mippedLevel2, imgWidth, imgHeight, false );
		imgWidth = imgWidth >> 1;
		imgHeight = imgHeight >> 1;
		tileSize = tileSize >> 1;

		common->Printf("...Mipmap (%dx%d) %d\n", imgWidth, imgHeight, tileSize );
		for(int h = 0; h < imgWidth / tileSize; h++) {
			for(int w = 0; w < imgWidth / tileSize; w++) {

				//common->Printf( "...Tile %d/%d", currentTile, maxTiles);
				WriteTile( mippedLevel2, w * tileSize, h * tileSize, imgWidth, tileSize, 2 );
			}
		}
		
	}
	R_StaticFree( buffer );
	R_StaticFree( mippedLevel2 );
	R_StaticFree( mippedLevel );

	delete[] compressedBuffer;

	common->Printf("Virtual Texture written successfully\n");
	return true;
}

/*
===========================
bmVirtualTextureFile::WriteTile
===========================
*/
void R_WriteDDS( const char *path, byte *data, int uploadWidth, int uploadHeight );
void bmVirtualTextureFile::WriteTile( byte *buffer, int DestX, int DestY, int DiemWidth, int vtTileSize, int level ) {
	unsigned int  		x, y, z, ConvBps, ConvSizePlane;
	byte 	*Converted;
	int Depth = 1;
	unsigned int 		c;
	unsigned int 		StartX, StartY, StartZ;
	byte 	*SrcTemp;
	float 	Back;
	int DestZ = 0;
	unsigned int Width = vtTileSize;
	unsigned int Height = vtTileSize;

	if(compressedBuffer == NULL) {
		compressedBuffer = new byte[vtTileSize * vtTileSize];
	}
	memset(&compressedBuffer[0], 0, vtTileSize * vtTileSize);
	
	ConvBps 	  = 4 * Width;
	ConvSizePlane = ConvBps   * Height;
	
	//@NEXT in next version this would have to be removed since Dest* will be unsigned
	StartX = DestX >= 0 ? 0 : -DestX;
	StartY = DestY >= 0 ? 0 : -DestY;
	
	// Limit the copy of data inside of the destination image
	if (Width  + DestX > DiemWidth)  Width  = DiemWidth  - DestX;
	if (Height + DestY > DiemWidth) Height = DiemWidth - DestY;
	if (Depth  + DestZ > DiemWidth)  Depth  = 1;
	
	byte *tempBuffer = new byte[vtTileSize * vtTileSize * 4];
	memset(&tempBuffer[0], 0, vtTileSize * vtTileSize);
	const unsigned int  bpp_without_alpha = 4 - 1;
		for (z = 0; z < Depth; z++) {
			for (y = 0; y < Height; y++) {
				for (x = 0; x < Width; x++) {
					const unsigned int   SrcIndex  = (z+0)*ConvSizePlane + (y+0)*ConvBps + (x+0)*4;
					const unsigned int   DestIndex = (z+DestZ)*(DiemWidth * DiemWidth) + (y+DestY)*(DiemWidth * 4) + (x+DestX)*4;
					
					tempBuffer[SrcIndex + 0] = buffer[DestIndex + 0];
					tempBuffer[SrcIndex + 1] = buffer[DestIndex + 1];
					tempBuffer[SrcIndex + 2] = buffer[DestIndex + 2];
					tempBuffer[SrcIndex + 3] = buffer[DestIndex + 3];
					
					
					
				}

			}
		}

	int numBytes = 0;
	byte border[4] = { 0, 0, 0, 0 };
	//R_SetBorderTexels( tempBuffer, vtTileSize, vtTileSize, border );
	CompressImageDXT5YCoCg( tempBuffer, &compressedBuffer[0], vtTileSize, vtTileSize, numBytes ); 

	//byte *cbuf = toolInterface->CompressImage( tempBuffer, vtTileSize, vtTileSize );

	
	f[level]->Write( &compressedBuffer[0], vtTileSize * vtTileSize );

	if(vt_debug_tiles.GetBool()) {
		R_WriteDDS(va("vt/tiles/%d.dds", header.numTiles), &compressedBuffer[0], vtTileSize, vtTileSize );
	}

	delete[] tempBuffer;
}

/*
===========================
bmVirtualTextureFile::WriteSurfaceTileToVT
===========================
*/
int bmVirtualTextureFile::WriteSurfaceTileToVT( const char *path ) {
/*
	const idMaterial *mat;

	// Load in the material, we do this before searching if the image is already loaded because we need to test
	// if the material is valid or not.
	mat = declManager->FindMaterial( path, false );
	if(mat == NULL) {
		path = DEFAULT_TEXTUREPATH;
	}
	else {
		idImage *img = mat->GetDiffuseImage();

		if(img == NULL)
		{
			path = DEFAULT_TEXTUREPATH;
		}
		else
		{
			path = img->imgName.c_str();
		}
	}

	for(int i = 0; i < imglist.Num(); i++) {
		if(!imglist[i].Cmp( path )) {
			return i + 1;
		}
	}

	byte border[4];
	border[0] = 0;
	border[1] = 0;
	border[2] = 0;
	border[3] = 0;

	byte *buffer;
	int imgWidth = 0;
	int imgHeight = 0;
	ID_TIME_T timeStamp;

	R_LoadImage( path, &buffer, &imgWidth, &imgHeight, &timeStamp, true );

	R_SetBorderTexels( buffer, imgWidth, imgHeight, border );

//	f->WriteInt( imgWidth );
//	f->WriteInt( imgHeight );
	f->Write( buffer, imgWidth * imgHeight * 4 );

	imglist.Append( path );
	return imglist.Num();
*/
	return -1;
}

/*
===========================
bmVirtualTextureFile::FinishVirtualTextureWrite
===========================
*/
void bmVirtualTextureFile::FinishVirtualTextureWrite( void ) {
	idStr temppath;


	//header.numTiles = imglist.Num();

	common->Printf("-------VT_FinishTextureWrite------\n");
	for(int i = 0; i < VIRTUALTEXTURE_NUMLEVELS; i++)
	{
		common->Printf( "Level %d - %d tiles - %d bytes\n", i, header.numTiles, f[i]->Length());

		temppath = f[i]->GetFullPath();

		// Re-write the header and close the file.
		header.WriteToFile( f[i] );
		f[i]->Flush();

		fileSystem->CloseFile( f[i] );
		f[i] = NULL;

		// Rename the temp file to the real path.
		fileSystem->RenameFile( fileSystem->RelativePathToOSPath( vtpath[i].c_str() ), temppath.c_str() );
	}

	
}