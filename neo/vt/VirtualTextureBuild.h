// VirtualTextureBuild.h
//

//
//  vtUVBlock_t
//
struct vtUVBlock_t {
	int			x;
	int			y;

	int			w;
	int			h;
	int			page;

	idVec3		vecs[2];
	idBounds	bounds;

	srfTriangles_t *tris;
};

struct vtPageNode_t
{
    vtPageNode_t* child[2];
    vtUVBlock_t *block;

	vtPageNode_t::vtPageNode_t() {
		child[0] = NULL;
		child[1] = NULL;
		block = NULL;
		
	}

	idBox	box;
};

 


//
// VirtualTextureBuilder
//
class VirtualTextureBuilder {
public:
	void						GenerateVTVerts( class bmVTModel *model );

	bool						GenerateVTVerts_r( class bmVTModel *model, float surfaceSize, int numVTAreas, bool fakePass );
	int							NumVTAreas( void ) {return numCalcedAreas; };
	
private:
	bool						VerifyUVForPage( bmVTModel *model, float surfaceSizes, int numVTAreas );
	void						ScaleUVsToFitArea( srfTriangles_t *tris, int x, int y, int w, int h );
	void						ScaleUVRegionToFitInTri( bmVTModel *model, srfTriangles_t *parentTris, srfTriangles_t *tris, int triId, int pageId, int widthId, int heightId, float uvScaleW, float uvScaleH, float chartW, float chartH );
	int							RemapVertexFromParentToChildTri( srfTriangles_t *parentTris, idList<idDrawVert> &childVerts,  int index, bool appendVert );

	idList< idDrawVert >		verts;
	int	numCalcedAreas;
};

ID_INLINE void VT_FixST( idVec2 &st ) {
	if(st.y >= 1) {
		st.y = 1 - st.y;
	}

	if(st.y <= 0) {
		st.y = abs(st.y);
	}

	if(st.x >= 1) {
		st.x = 1 - st.x;
	}

	if(st.x <= 0) {
		st.x = abs(st.x);
	}
}

extern VirtualTextureBuilder vtBuilder;