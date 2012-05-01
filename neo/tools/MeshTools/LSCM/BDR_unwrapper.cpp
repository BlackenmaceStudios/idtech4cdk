/**
 * $Id: unwrapper.c,v 1.17 2006/06/10 20:35:54 blendix Exp $
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "../LSCM/MeshBuilderStdafx.h"
#include "../Lib/MouseDefines.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MEM_guardedalloc.h"
#include "BKE_utildefines.h"

#include "BLI_arithb.h"
#include "BLI_edgehash.h"
#include "blendef.h"


#include "BDR_unwrapper.h"
#include "parametrizer.h"

//#include "MouseDefines.h"



Mesh gMesh;


int UVBankSize;
float* FinalUVBank = NULL;


int gNumberOfPinned;
float* gPinnedVertsProj;
int* gPinnedIndices;


extern bool gFillHoles;
extern bool gABF;




/* Set tface seams based on edge data, uses hash table to find seam edges. */

static void hash_add_face(EdgeHash *ehash, MFace *mf)
{
	BLI_edgehash_insert(ehash, mf->v1, mf->v2, NULL);
	BLI_edgehash_insert(ehash, mf->v2, mf->v3, NULL);
	if(mf->v4) {
		BLI_edgehash_insert(ehash, mf->v3, mf->v4, NULL);
		BLI_edgehash_insert(ehash, mf->v4, mf->v1, NULL);
	}
	else
		BLI_edgehash_insert(ehash, mf->v3, mf->v1, NULL);
}


//Swanny bodgery, ripped from last version..  god knows if it'll work

void set_seamtface()
{
	Mesh *me;
	EdgeHash *ehash;
	int a;
	MFace *mf;
	TFace *tf;
	MEdge *medge;

	me= &gMesh;									//get_mesh(OBACT);
	if(!me || !me->tface) return;				//  || !(G.f & G_FACESELECT)) return;		// altered by Swanny 20/05/05
	
	ehash= BLI_edgehash_new();

	for(medge= me->medge, a=me->totedge; a > 0; a--, medge++)
	{
		if(medge->flag & ME_SEAM)
		{
			BLI_edgehash_insert(ehash, medge->v1, medge->v2, NULL);
		}
	}

	mf= me->mface;
	tf= me->tface;

	for(a=me->totface; a>0; a--, mf++, tf++) 
	{
		tf->unwrap &= ~(TF_SEAM1|TF_SEAM2|TF_SEAM3|TF_SEAM4);

		if(!ehash) continue;

		if(BLI_edgehash_haskey(ehash, mf->v1, mf->v2)) tf->unwrap |= TF_SEAM1;
		if(BLI_edgehash_haskey(ehash, mf->v2, mf->v3)) tf->unwrap |= TF_SEAM2;

		if(mf->v4) 
		{
			if(BLI_edgehash_haskey(ehash, mf->v3, mf->v4)) tf->unwrap |= TF_SEAM3;
			if(BLI_edgehash_haskey(ehash, mf->v4, mf->v1)) tf->unwrap |= TF_SEAM4;
		}
		else 
		{
			if(BLI_edgehash_haskey(ehash, mf->v3, mf->v1)) tf->unwrap |= TF_SEAM3;
		}
	}

	BLI_edgehash_free(ehash, NULL);
}



void select_linked_tfaces_with_seams(int mode, unsigned int index)			// Mesh *me,
{
	TFace *tf;
	MFace *mf;
	int a, doit=1, mark=0;
	char *linkflag;
	EdgeHash *ehash, *seamhash;
	MEdge *med;
	Mesh* me = &gMesh;

	ehash= BLI_edgehash_new();
	seamhash = BLI_edgehash_new();
	linkflag= (char*)MEM_callocN(sizeof(char)*me->totface, "linkflaguv");

	for(med=me->medge, a=0; a < me->totedge; a++, med++)
		if(med->flag & ME_SEAM)
			BLI_edgehash_insert(seamhash, med->v1, med->v2, NULL);

	if (mode==0 || mode==1) {
		/* only put face under cursor in array */
		mf= ((MFace*)me->mface) + index;
		hash_add_face(ehash, mf);
		linkflag[index]= 1;
	}
	else {
		/* fill array by selection */
		tf= me->tface;
		mf= me->mface;
		for(a=0; a<me->totface; a++, tf++, mf++) {
			if(tf->flag & TF_HIDE);
			else if(tf->flag & TF_SELECT) {
				hash_add_face(ehash, mf);
				linkflag[a]= 1;
			}
		}
	}
	
	while(doit) {
		doit= 0;
		
		/* expand selection */
		tf= me->tface;
		mf= me->mface;
		for(a=0; a<me->totface; a++, tf++, mf++) {
			if(tf->flag & TF_HIDE)
				continue;

			if(!linkflag[a]) {
				mark= 0;

				if(!BLI_edgehash_haskey(seamhash, mf->v1, mf->v2))
					if(BLI_edgehash_haskey(ehash, mf->v1, mf->v2))
						mark= 1;
				if(!BLI_edgehash_haskey(seamhash, mf->v2, mf->v3))
					if(BLI_edgehash_haskey(ehash, mf->v2, mf->v3))
						mark= 1;
				if(mf->v4) {
					if(!BLI_edgehash_haskey(seamhash, mf->v3, mf->v4))
						if(BLI_edgehash_haskey(ehash, mf->v3, mf->v4))
							mark= 1;
					if(!BLI_edgehash_haskey(seamhash, mf->v4, mf->v1))
						if(BLI_edgehash_haskey(ehash, mf->v4, mf->v1))
							mark= 1;
				}
				else if(!BLI_edgehash_haskey(seamhash, mf->v3, mf->v1))
					if(BLI_edgehash_haskey(ehash, mf->v3, mf->v1))
						mark = 1;

				if(mark) {
					linkflag[a]= 1;
					hash_add_face(ehash, mf);
					doit= 1;
				}
			}
		}
		
	}

	BLI_edgehash_free(ehash, NULL);
	BLI_edgehash_free(seamhash, NULL);

	if(mode==0 || mode==2) {
		for(a=0, tf=me->tface; a<me->totface; a++, tf++)
			if(linkflag[a])
				tf->flag |= TF_SELECT;
			else
				tf->flag &= ~TF_SELECT;
	}
	else if(mode==1) {
		for(a=0, tf=me->tface; a<me->totface; a++, tf++)
			if(linkflag[a] && (tf->flag & TF_SELECT))
				break;

		if (a<me->totface) {
			for(a=0, tf=me->tface; a<me->totface; a++, tf++)
				if(linkflag[a])
					tf->flag &= ~TF_SELECT;
		}
		else {
			for(a=0, tf=me->tface; a<me->totface; a++, tf++)
				if(linkflag[a])
					tf->flag |= TF_SELECT;
		}
	}
	
	MEM_freeN(linkflag);
	
//	BIF_undo_push("Select linked UV face");
//	object_tface_flags_changed(OBACT, 0);
}

/* Parametrizer */

ParamHandle *construct_param_handle(Mesh *me, short implicit, short fill, short sel)
{
	int a;
	TFace *tf;
	MFace *mf;
	MVert *mv;
	MEdge *medge;
	ParamHandle *handle;
	
	handle = param_construct_begin();
	
	mv= me->mvert;
	mf= me->mface;
	tf= me->tface;
	for (a=0; a<me->totface; a++, mf++, tf++) {
		ParamKey key, vkeys[4];
		bool pin[4], select[4];							// was ParamBool
		float *co[4];
		float *uv[4];
		int nverts;

		if (tf->flag & TF_HIDE)
			continue;

		if (sel && !(tf->flag & TF_SELECT))
			continue;

		if (implicit && !(tf->flag & (TF_SEL1|TF_SEL2|TF_SEL3|TF_SEL4)))
			continue;

		key = (ParamKey)mf;
		vkeys[0] = (ParamKey)mf->v1;
		vkeys[1] = (ParamKey)mf->v2;
		vkeys[2] = (ParamKey)mf->v3;

		co[0] = (mv+mf->v1)->co;
		co[1] = (mv+mf->v2)->co;
		co[2] = (mv+mf->v3)->co;

		uv[0] = tf->uv[0];
		uv[1] = tf->uv[1];
		uv[2] = tf->uv[2];

		pin[0] = ((tf->unwrap & TF_PIN1) != 0);
		pin[1] = ((tf->unwrap & TF_PIN2) != 0);
		pin[2] = ((tf->unwrap & TF_PIN3) != 0);

		select[0] = ((tf->flag & TF_SEL1) != 0);
		select[1] = ((tf->flag & TF_SEL2) != 0);
		select[2] = ((tf->flag & TF_SEL3) != 0);

		if (mf->v4) {
			vkeys[3] = (ParamKey)mf->v4;
			co[3] = (mv+mf->v4)->co;
			uv[3] = tf->uv[3];
			pin[3] = ((tf->unwrap & TF_PIN4) != 0);
			select[3] = ((tf->flag & TF_SEL4) != 0);
			nverts = 4;
		}
		else
			nverts = 3;

		param_face_add(handle, key, nverts, vkeys, co, uv, pin, select);
	}

	if (!implicit) {
		for(medge=me->medge, a=me->totedge; a>0; a--, medge++) {
			if(medge->flag & ME_SEAM) {
				ParamKey vkeys[2];

				vkeys[0] = (ParamKey)medge->v1;
				vkeys[1] = (ParamKey)medge->v2;
				param_edge_set_seam(handle, vkeys);
			}
		}
	}

	param_construct_end(handle, (bool)fill, (bool)implicit);

	return handle;
}

void unwrap_lscm(short seamcut)
{
	Mesh *me;
	ParamHandle *handle;
//	short abf = G.scene->toolsettings->unwrapper == 1;
//	short fillholes = G.scene->toolsettings->uvcalc_flag & 1;
	
	me= &gMesh;									//get_mesh(OBACT);
	if(me==0 || me->tface==0) return;

	handle = construct_param_handle(me, 0, gFillHoles, seamcut == 0);		// fillholes set to false

	param_lscm_begin(handle, PARAM_FALSE, gABF);
	param_lscm_solve(handle);
	param_lscm_end(handle);

	param_pack(handle);

	param_flush(handle);

	param_delete(handle);

	CompressUVs();
}


ParamHandle *StretchHandle;

void StartStretch()
{
	Mesh *me;

//	double lasttime;
	short doit = 1, escape = 0, blend = 0;
	unsigned short event = 0;
	short fillholes = true;						//G.scene->toolsettings->uvcalc_flag & 1;
	
	me= &gMesh;									//get_mesh(OBACT);
	if(me==0 || me->tface==0) return;

//	StretchHandle = construct_param_handle(me, 0, fillholes, 1);
	StretchHandle = construct_param_handle(me, 1, fillholes, 1);

//	lasttime = PIL_check_seconds_timer();

	param_stretch_begin(StretchHandle);
//	param_stretch_iter(StretchHandle);
}



void DoStretch(int BlendVal)		// BlendVal between 0 and 10
{
//	if (blend < 10) 
//	{
//		blend++;
	param_stretch_iter(StretchHandle);
	param_stretch_blend(StretchHandle, BlendVal*0.1f);
	param_flush(StretchHandle);
	CompressUVs();
//		lasttime = 0.0f;
//	}
}



void EndStretch(bool KeepResults)
{
	if (KeepResults == false)
		param_flush_restore(StretchHandle);
	else
		param_flush(StretchHandle);

	param_stretch_end(StretchHandle);
	param_delete(StretchHandle);
	CompressUVs();
}



/*
void minimize_stretch_tface_uv(void)
{
	Mesh *me;
	ParamHandle *handle;
	double lasttime;
	short doit = 1, escape = 0, val, blend = 0;
	unsigned short event = 0;
	short fillholes = true;						//G.scene->toolsettings->uvcalc_flag & 1;
	
	me= gpMesh;									//get_mesh(OBACT);
	if(me==0 || me->tface==0) return;

	handle = construct_param_handle(me, 1, fillholes, 1);

	lasttime = PIL_check_seconds_timer();

	param_stretch_begin(handle);

	while (doit) 
	{
		param_stretch_iter(handle);

		while (qtest()) 
		{
			event= extern_qread(&val);

			if (val) 
			{
				switch (event) 
				{
					case ESCKEY:
						escape = 1;
					case RETKEY:
					case PADENTER:
						doit = 0;
						break;
					case PADPLUSKEY:
					case WHEELUPMOUSE:
						if (blend < 10) 
						{
							blend++;
							param_stretch_blend(handle, blend*0.1f);
							param_flush(handle);
							lasttime = 0.0f;
						}
						break;
					case PADMINUS:
					case WHEELDOWNMOUSE:
						if (blend > 0) 
						{
							blend--;
							param_stretch_blend(handle, blend*0.1f);
							param_flush(handle);
							lasttime = 0.0f;
						}
						break;
				}
			}
			else if ((event == LEFTMOUSE) || (event == RIGHTMOUSE)) 
			{
					escape = (event == RIGHTMOUSE);
					doit = 0;
			}
		}
		
		if (!doit)
			break;

		if (PIL_check_seconds_timer() - lasttime > 0.5) 
		{
			char str[100];

			param_flush(handle);

			sprintf(str, "Stretch minimize. Blend %.2f.", blend*0.1f);
			headerprint(str);

			lasttime = PIL_check_seconds_timer();
			object_uvs_changed(OBACT);
			if(G.sima->lock) force_draw_plus(SPACE_VIEW3D, 0);
			else force_draw(0);
		}
	}

	if (escape)
		param_flush_restore(handle);
	else
		param_flush(handle);

	param_stretch_end(handle);

	param_delete(handle);

//	BIF_undo_push("UV stretch minimize");

//	object_uvs_changed(OBACT);

//	allqueue(REDRAWVIEW3D, 0);
//	allqueue(REDRAWIMAGE, 0);
}
*/

/* LSCM live mode */

static ParamHandle *liveHandle = NULL;


void unwrap_lscm_live_begin(void)
{
//	Mesh *me;
//	short abf = G.scene->toolsettings->unwrapper == 1;
//	short fillholes = G.scene->toolsettings->uvcalc_flag & 1;

	Mesh *me;
//	ParamHandle *handle;
//	short abf = G.scene->toolsettings->unwrapper == 1;
//	short fillholes = G.scene->toolsettings->uvcalc_flag & 1;
	
	me= &gMesh;									//get_mesh(OBACT);
	if(me==0 || me->tface==0) return;

	liveHandle = construct_param_handle(me, 0, gFillHoles, 1);		// fillholes set to false

//	liveHandle = construct_param_handle(me, 0, fillholes, 1);

	param_lscm_begin(liveHandle, PARAM_TRUE, gABF);			// PARAM_TRUE
}


void unwrap_lscm_live_re_solve(void)
{
	if (liveHandle) 
	{
		param_lscm_solve(liveHandle);
		param_flush(liveHandle);
//		CompressUVs();
	}
}


void unwrap_lscm_live_end(short cancel)
{
	if (liveHandle) 
	{
		param_lscm_solve(liveHandle);
		param_flush(liveHandle);
//		CompressUVs();

		param_lscm_end(liveHandle);
		if (cancel)
			param_flush_restore(liveHandle);

//		param_pack(liveHandle);					// added by Swanny
		param_flush(liveHandle);
		param_delete(liveHandle);
		liveHandle = NULL;
	}
}






/************************************************************************/

void MeshAddVerts(int NumberVerts, float* Vertices, Mesh* pMeshIn)
{
//	CleanUp(pMeshIn);

	if(pMeshIn->medge != NULL) free(pMeshIn->medge);
	if(pMeshIn->mvert != NULL) free(pMeshIn->mvert);
	if(pMeshIn->tface != NULL) free(pMeshIn->tface);
	if(pMeshIn->mface != NULL) free(pMeshIn->mface);

	pMeshIn->medge = NULL;
	pMeshIn->mvert = NULL;
	pMeshIn->tface = NULL;
	pMeshIn->mface = NULL;

//	if(pMeshIn == NULL) pMeshIn = (Mesh*)malloc(sizeof(Mesh));
	pMeshIn->mvert = (MVert*)malloc(sizeof(MVert) * NumberVerts);
	int InIndex = 0;


	pMeshIn->totvert = NumberVerts;

	for(int Index = 0; Index < NumberVerts; Index++)
	{
		pMeshIn->mvert[Index].co[0] = Vertices[InIndex++];
		pMeshIn->mvert[Index].co[1] = Vertices[InIndex++];
		pMeshIn->mvert[Index].co[2] = Vertices[InIndex++];
	}
}


void MeshAddFaces(int NumberFaces, int* FaceIndices, int* UVIndices, float* PinnedProj, int* PinnedUVs, Mesh* pMeshIn)
{
	if(pMeshIn == NULL) pMeshIn = (Mesh*)malloc(sizeof(Mesh));

	pMeshIn->totface = NumberFaces;
	pMeshIn->mface = (MFace*)malloc(sizeof(MFace) * NumberFaces);
	pMeshIn->tface = (TFace*)malloc(sizeof(TFace) * NumberFaces);

	for(int Index = 0; Index < NumberFaces; Index++)
	{
		pMeshIn->mface[Index].v1 = FaceIndices[(Index*3)];
		pMeshIn->mface[Index].v2 = FaceIndices[(Index*3)+1];
		pMeshIn->mface[Index].v3 = FaceIndices[(Index*3)+2];
		pMeshIn->mface[Index].v4 = 0;

		pMeshIn->tface[Index].unwrap = 0;

		pMeshIn->tface[Index].flag = TF_SELECT;

		int VertIndex1 = pMeshIn->mface[Index].v1;
		int VertIndex2 = pMeshIn->mface[Index].v2;
		int VertIndex3 = pMeshIn->mface[Index].v3;
		float U0 = 0.0f;
		float V0 = 0.0f;
		float U1 = 0.0f;
		float V1 = 0.0f;
		float U2 = 0.0f;
		float V2 = 0.0f;

		if(PinnedUVs[Index*3] == 1) pMeshIn->tface[Index].unwrap |= TF_PIN1;
		if(PinnedUVs[(Index*3)+1] == 1) pMeshIn->tface[Index].unwrap |= TF_PIN2;
		if(PinnedUVs[(Index*3)+2] == 1) pMeshIn->tface[Index].unwrap |= TF_PIN3;

		int UVIndex1, UVIndex2, UVIndex3;

//		if(UVIndices[(Index*3)] != -1)
//		{
			UVIndex1 = UVIndices[(Index*3)];
			U0 = PinnedProj[(UVIndex1*3)];
			V0 = PinnedProj[(UVIndex1*3)+1];
//			gpMesh->tface[Index].unwrap |= TF_PIN1;
//		}

//		if(UVIndices[(Index*3)+1] != -1)
//		{
			UVIndex2 = UVIndices[(Index*3)+1];
			U1 = PinnedProj[(UVIndex2*3)];
			V1 = PinnedProj[(UVIndex2*3)+1];
//			gpMesh->tface[Index].unwrap |= TF_PIN2;
//		}

//		if(UVIndices[(Index*3)+2] != -1)
//		{
			UVIndex3 = UVIndices[(Index*3)+2];
			U2 = PinnedProj[(UVIndex3*3)];
			V2 = PinnedProj[(UVIndex3*3)+1];
//			gpMesh->tface[Index].unwrap |= TF_PIN3;
//		}

//		if(Index > 30 && Index < 120)
//		{
			pMeshIn->tface[Index].flag |= TF_SEL1;
//		}


		pMeshIn->tface[Index].uv[0][0] = U0;
		pMeshIn->tface[Index].uv[0][1] = V0;

		pMeshIn->tface[Index].uv[1][0] = U1;
		pMeshIn->tface[Index].uv[1][1] = V1;

		pMeshIn->tface[Index].uv[2][0] = U2;
		pMeshIn->tface[Index].uv[2][1] = V2;

		pMeshIn->tface[Index].uv[3][0] = 0;
		pMeshIn->tface[Index].uv[3][1] = 0;
	}
}

void MeshUpdateUVs(int NumberFaces, int* FaceIndices, int* UVIndices, float* PinnedProj)
{
	for(int Index = 0; Index < NumberFaces; Index++)
	{
		float U0 = 0.0f;
		float V0 = 0.0f;
		float U1 = 0.0f;
		float V1 = 0.0f;
		float U2 = 0.0f;
		float V2 = 0.0f;

		if(UVIndices[(Index*3)] != -1)
		{
			int UVIndex = UVIndices[(Index*3)];
			U0 = PinnedProj[(UVIndex*3)];
			V0 = PinnedProj[(UVIndex*3)+1];
			gMesh.tface[Index].unwrap |= TF_PIN1;
			gMesh.tface[Index].uv[0][0] = U0;
			gMesh.tface[Index].uv[0][1] = V0;
		}

		if(UVIndices[(Index*3)+1] != -1)
		{
			int UVIndex = UVIndices[(Index*3)+1];
			U1 = PinnedProj[(UVIndex*3)];
			V1 = PinnedProj[(UVIndex*3)+1];
			gMesh.tface[Index].unwrap |= TF_PIN2;
			gMesh.tface[Index].uv[1][0] = U1;
			gMesh.tface[Index].uv[1][1] = V1;
		}

		if(UVIndices[(Index*3)+2] != -1)
		{
			int UVIndex = UVIndices[(Index*3)+2];
			U2 = PinnedProj[(UVIndex*3)];
			V2 = PinnedProj[(UVIndex*3)+1];
			gMesh.tface[Index].unwrap |= TF_PIN3;
			gMesh.tface[Index].uv[2][0] = U2;
			gMesh.tface[Index].uv[2][1] = V2;
		}

/*
		gpMesh->tface[Index].uv[0][0] = U0;
		gpMesh->tface[Index].uv[0][1] = V0;

		gpMesh->tface[Index].uv[1][0] = U1;
		gpMesh->tface[Index].uv[1][1] = V1;

		gpMesh->tface[Index].uv[2][0] = U2;
		gpMesh->tface[Index].uv[2][1] = V2;

		gpMesh->tface[Index].uv[3][0] = 0;
		gpMesh->tface[Index].uv[3][1] = 0;
*/	}
}




void MeshAddEdges(int NumberEdges, RKEdge* EdgeVerts, Mesh* pMeshIn, bool NoSeams)
{
	if(pMeshIn == NULL) pMeshIn = (Mesh*)malloc(sizeof(Mesh));

	pMeshIn->totedge = NumberEdges;
	pMeshIn->medge = (MEdge*)malloc(sizeof(MEdge) * NumberEdges);

	for(int Index = 0; Index < NumberEdges; Index++)
	{
		pMeshIn->medge[Index].v1 = EdgeVerts[Index].m_VertexIndex1;		//[Index*3];
		pMeshIn->medge[Index].v2 = EdgeVerts[Index].m_VertexIndex2;		//[(Index*3)+1];
		pMeshIn->medge[Index].flag = 0;

		if(EdgeVerts[Index].m_Selected & EDGECUT && NoSeams == false) 
		{
			pMeshIn->medge[Index].flag = ME_SEAM;
		}
	}
}



void CompressUVs()
{
	if(FinalUVBank != NULL)
	{
		delete[] FinalUVBank;
	}

	FinalUVBank = new float[gMesh.totface * 6];		//		(float*)malloc(gpMesh->totface * 6);				// max UV's there can be..
	UVBankSize = 0;

	for(int Index = 0; Index < gMesh.totface; Index++)
	{
		gMesh.tface[Index].BankIndex[0] = InsertUVIntoBank(gMesh.tface[Index].uv[0]);
		gMesh.tface[Index].BankIndex[1] = InsertUVIntoBank(gMesh.tface[Index].uv[1]);
		gMesh.tface[Index].BankIndex[2] = InsertUVIntoBank(gMesh.tface[Index].uv[2]);
	}
}



int InsertUVIntoBank(float UV[2])
{
	if(UVBankSize == 0)
	{
		FinalUVBank[0] = UV[0];
		FinalUVBank[1] = UV[1];
		return(UVBankSize++);
	}

	for(int Index = 0; Index < UVBankSize; Index++)
	{
		if(FinalUVBank[Index*2] == UV[0] && FinalUVBank[Index*2+1] == UV[1])
		{
			return(Index);
		}
	}

	FinalUVBank[UVBankSize*2] = UV[0];
	FinalUVBank[(UVBankSize*2)+1] = UV[1];
	return(UVBankSize++);
}


void MeshAddPinned(int NumberOfPinned, float* PinnedProj, int* PinnedIndex)
{
	gNumberOfPinned = NumberOfPinned;
	gPinnedVertsProj = PinnedProj;
	gPinnedIndices = PinnedIndex;
}



void CleanUp(Mesh* pMesh)
{
	if(FinalUVBank != NULL)
	{
		delete[] FinalUVBank;
	}

	FinalUVBank = NULL;

	if(pMesh->medge != NULL) free(pMesh->medge);
	if(pMesh->mvert != NULL) free(pMesh->mvert);
	if(pMesh->tface != NULL) free(pMesh->tface);
	if(pMesh->mface != NULL) free(pMesh->mface);

	pMesh->medge = NULL;
	pMesh->mvert = NULL;
	pMesh->tface = NULL;
	pMesh->mface = NULL;

//	if(gpMesh != NULL) free(gpMesh);
//	gpMesh = NULL;
}
