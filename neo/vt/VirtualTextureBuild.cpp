// VirtualTextureBuild.cpp
//

#include "precompiled.h"
#include "VirtualTexture.h"
#include "VirtualTextureBuild.h"


#include "../tools/compilers/dmap/vtmodel.h"

#define USE_CORRECT_UV_GENERATION				// Works now...frickin sweat...

VirtualTextureBuilder vtBuilder;
static int vertNum = 0;

int		*allocated= NULL;
int		firstTrisOnPage = 0;
int VT_CurrentNumAreas = 0;
float spacing = 1.0;

idCVar vt_compile_areasize( "vt_compile_areasize", "4096", CVAR_RENDERER | CVAR_INTEGER , "Size of the texture for each area of the map." );
idCVar vt_compile_size( "vt_compile_size", "16384", CVAR_RENDERER | CVAR_INTEGER , "Size of the virtual texture to build." );
idCVar vt_compile_bsize( "vt_compile_bsize", "128", CVAR_RENDERER | CVAR_INTEGER , "Size of the virtual texture to build." );

#define VT_Size vt_compile_bsize.GetInteger()


void PrepareNewVTArea( void ) {
	if(allocated != NULL) {
		delete allocated;
	}

	allocated = new int[VT_Size];
	memset( allocated, 0, sizeof( int ) * VT_Size );

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

	int numVTAreas = (vt_compile_size.GetInteger() / vt_compile_areasize.GetInteger()) * (vt_compile_size.GetInteger() / vt_compile_areasize.GetInteger());

	spacing = 1;

	common->Printf( "Determing UV and Virtual Texture Areas...\n" );
	common->Printf( "...VT Compile Size %i\n", vt_compile_size.GetInteger() );
	common->Printf( "...VT Area Size %i\n", vt_compile_areasize.GetInteger() );
	common->Printf( "...Number of VT areas %d\n", numVTAreas );
	int numTrisPerArea = model->tris.Num() / numVTAreas;
	

	if(numTrisPerArea == 0) {
		numVTAreas = numTrisPerArea;
		numTrisPerArea = 1;
	}
	VT_CurrentNumAreas = 0;
#ifdef USE_CORRECT_UV_GENERATION
	for(int i = 0; i < model->tris.Num(); i+=numTrisPerArea)
	{
		int triCount = numTrisPerArea;

		common->Printf( "Generating UV's for chart %d - %d\n", VT_CurrentNumAreas, triCount);
		if(i + triCount > model->tris.Num()) {
			triCount = model->tris.Num() - i;
		}

		toolInterface->ComputeUVAtlasForModel( model, i, triCount );

		for(int f = i; f < i + triCount; f++) {
			model->tris[f].vt_AreaID = VT_CurrentNumAreas;
		}

		VT_CurrentNumAreas++;
	}
#else // old stuff.
	
	while(!GenerateVTVerts_r( model, surfaceSize, numVTAreas )) {

		surfaceSize += 0.001;
		lastSpacing = spacing;
	}
#endif

	if(VT_CurrentNumAreas != numVTAreas) {
		common->Warning("Areas not divided properly!");
	}
	else {
		common->Printf( "Virtual Texture UV coordinates generated succesfully. Final blocksize %f\n", surfaceSize);
	}
}

/*
====================
VirtualTextureBuilder::GenerateVTVerts
====================
*/
bool VirtualTextureBuilder::GenerateVTVerts_r( bmVTModel *model,  float surfaceSize, int numVTAreas ) {
	idBounds bounds;
	idVec3 exactSize, delta;
	float w, h, s, t;
	int axis = 0;
	idVec3 vecs[2];
	idVec2 scaleST;


	spacing = 1.0;
	firstTrisOnPage = 0;
	VT_CurrentNumAreas = 0;

	PrepareNewVTArea();

	int wtest = 0, htest = 0;

	scaleST = idVec2(1, 1);

	for(int d = 0; d < model->tris.Num(); d++)
	{
		idVec3 size;
		idPlane plane;
		idVec3 planeNormal;
		idDrawVert *v = model->tris[d].verts;
		
		vecs[0] = idVec3(0,0,0);
		vecs[1] = idVec3(0,0,0);

		plane.FromPoints( v[0].xyz, v[1].xyz, v[2].xyz );

		// Clear the current bounds.
		bounds.Clear();

		// Get the bounds for the tris
		for(int i = 0; i < model->tris[d].numVerts; i++)
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
		if(!AllocVTBlock( w, h , &x, &y )) {


			scaleST.x = 1;
			scaleST.y = 1;
			VT_CurrentNumAreas++;

			// If we  already filled up all the texture space for all of the target pages, start over with a higher blocksize(less space).
			if(VT_CurrentNumAreas >= numVTAreas)
			{
				return false;
			}
			
			// Prepare a new VT page.
			PrepareNewVTArea();
			firstTrisOnPage = d;
		}

		if(model->tris[d].vt_AreaID != -1)
		{
			bool hasCorrectUVs = true;
			// Check to see if all the UV's are between 0 and 1.
			for ( int i = 0 ; i < model->tris[d].numVerts ; i++ ) {
				if(v[i].st.x > 1 || v[i].st.x < 0)
				{
					hasCorrectUVs = false;
					break;
				}

				if(v[i].st.y > 1 || v[i].st.y < 0)
				{
					hasCorrectUVs = false;
					break;
				}
			}

			model->tris[d].vt_AreaID = VT_CurrentNumAreas;

		
			if(!hasCorrectUVs)
			{
				for ( int i = 0 ; i < model->tris[d].numVerts ; i++ ) {
			
					delta = v[i].xyz - bounds[0];
					v[i].st.x = (DotProduct(delta, vecs[0]) + x + 0.5) / VT_Size;
					v[i].st.y = (DotProduct(delta, vecs[1]) + y + 0.5) / VT_Size;

					if(v[i].st.x > 1 || v[i].st.y > 1) {
						goto generatePage;
					}

			
				}
			}
			else
			{
				for ( int i = 0 ; i < model->tris[d].numVerts ; i++ ) {
				//	v[i].st.x = (v[i].st.x + x + 0.5) * ((float)w / (float)VT_Size);
				//	v[i].st.y = (v[i].st.y + y + 0.5) * ((float)h / (float)VT_Size);
				}
			}
		}
		else {
			model->tris[d].vt_AreaID = numVTAreas;
		}
		// Don't add the area id until we know all the verts are good.
		
		//for ( int i = 0 ; i < model->tris[d].numVerts ; i++ ) {
		//	v[i].st.x += model->tris[d].vt_AreaID;
		//}
		
	}

	

	return true;
}
/*
======================
VirtualTextureBuilder::NumVTAreas
======================
*/
int	 VirtualTextureBuilder::NumVTAreas( void ) {
	return VT_CurrentNumAreas + 1;
}
