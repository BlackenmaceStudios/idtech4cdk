// UVUnwrap.cpp : Defines the entry point for the console application.
//

#include "../LSCM/MeshBuilderStdafx.h"
#include "BDR_unwrapper.h"


float CubeVerts[9*3] = {  0.0f,  0.0f,  0.0f,
						 -1.0f, -1.0f, -1.0f,
						  1.0f, -1.0f, -1.0f,
						  1.0f,  1.0f, -1.0f,
						 -1.0f,  1.0f, -1.0f,

						 -1.0f, -1.0f,  1.0f,
						  1.0f, -1.0f,  1.0f,
						  1.0f,  1.0f,  1.0f,
						 -1.0f,  1.0f,  1.0f
						};


int CubeFaces[12*3] = { 1, 2, 3,
						1, 3, 4,
						  
						5, 1, 4,
						5, 4, 8,

						5, 6, 2,
						5, 2, 1,

						2, 6, 7,
						2, 7, 3,

						4, 3, 7,
						4, 7, 8,

						6, 5, 8,
						6, 8, 7
					};



int CubeEdges[12*2] = { 1, 2,
						2, 3,
						3, 4,
						4, 1,

						6, 5,
						5, 8,
						8, 7,
						7, 6,

						5, 1,
						4, 8,

						2, 6,
						3, 7
					};


short CubeEdgeFlags[12] = { 0,0,0,0, 0, ME_SEAM, ME_SEAM, ME_SEAM, ME_SEAM, ME_SEAM, ME_SEAM, ME_SEAM};



int _tmain(int argc, _TCHAR* argv[])
{
//	MeshAddVerts(9, CubeVerts);
//	MeshAddFaces(12, CubeFaces);
//	MeshAddEdges(12, CubeEdges, CubeEdgeFlags);


	set_seamtface();									// tested, working fine
//	select_linked_tfaces_with_seams(0, NULL, 0);
	unwrap_lscm();

	return 0;
}

