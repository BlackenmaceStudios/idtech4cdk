#include "../LSCM/MeshBuilderStdafx.h"

#include "BLI_edgehash.h"
#include "BLI_heap.h"
#include "MEM_guardedalloc.h"
#include "BLI_arithb.h"
#include "BLI_editVert.h"
#include "BLI_linklist.h"
#include "blendef.h"
#include "BKE_utildefines.h"

#include "NewMesh.h"
#include "EdgeSelect.h"
#include "math.h"
#include "../lib/Face.h"
#include "../lib/RKMesh.h"


Mesh gCloseMesh;
EditMesh gEditMesh;

#define EDHASHSIZE		(512*512)
#define EDHASH(a, b)	(a % EDHASHSIZE)


#define ME_SEAM_DONE ME_SEAM_LAST		/* reuse this flag */

static float seam_cut_cost(Mesh *me, int e1, int e2, int vert, int target)
{
	MVert *v = me->mvert + vert;
	MEdge *med1 = me->medge + e1, *med2 = me->medge + e2;
	MEdge *tar = me->medge + target;
	MVert *v1 = me->mvert + ((med1->v1 == vert)? med1->v2: med1->v1);
	MVert *v2 = me->mvert + ((med2->v1 == vert)? med2->v2: med2->v1);
	MVert *tarvert1 = me->mvert + tar->v1;
	MVert *tarvert2 = me->mvert + tar->v2;
	float cost, d1[3], d2[3], EdgeVector[3];		// Heading[3],
	float TargetVector1[3], TargetVector2[3], TargetVector3[3], TargetVector4[3];
	cost = VecLenf(v1->co, v->co);
	cost += VecLenf(v->co, v2->co);

	VecSubf(d1, v1->co, v->co);
	VecSubf(d2, v->co, v2->co);

	VecSubf(TargetVector1, tarvert1->co, v1->co);
	VecSubf(TargetVector2, tarvert1->co, v2->co);
	VecSubf(TargetVector3, tarvert2->co, v1->co);
	VecSubf(TargetVector4, tarvert2->co, v2->co);

	float DistToTarget1 = TargetVector1[0] * TargetVector1[0] + TargetVector1[1] * TargetVector1[1] + TargetVector1[2] * TargetVector1[2];
	DistToTarget1 = sqrt(DistToTarget1);

	float DistToTarget2 = TargetVector2[0] * TargetVector2[0] + TargetVector2[1] * TargetVector2[1] + TargetVector2[2] * TargetVector2[2];
	DistToTarget2 = sqrt(DistToTarget2);

	float DistToTarget3 = TargetVector3[0] * TargetVector3[0] + TargetVector3[1] * TargetVector3[1] + TargetVector3[2] * TargetVector3[2];
	DistToTarget3 = sqrt(DistToTarget3);

	float DistToTarget4 = TargetVector4[0] * TargetVector4[0] + TargetVector4[1] * TargetVector4[1] + TargetVector4[2] * TargetVector4[2];
	DistToTarget4 = sqrt(DistToTarget4);

	float MinDist = DistToTarget1;
	if(DistToTarget2 < MinDist) MinDist = DistToTarget2;
	if(DistToTarget3 < MinDist) MinDist = DistToTarget3;
	if(DistToTarget4 < MinDist) MinDist = DistToTarget4;


	MVert *edgev1 = me->mvert + med2->v1;
	MVert *edgev2 = me->mvert + med2->v2;
	VecSubf(EdgeVector, edgev1->co, edgev2->co);
/*
	Normalise(TargetVector);
	Normalise(EdgeVector);

	Heading[0] = TargetVector[0] * EdgeVector[0];
	Heading[1] = TargetVector[1] * EdgeVector[1];
	Heading[2] = TargetVector[2] * EdgeVector[2];
*/
//	Normalise(Heading);

/*
	float VecX = d1[0]*d2[0];
	float VecY = d1[1]*d2[1];
	float VecZ = d1[2]*d2[2];

	float Length = (VecX * VecX + VecY * VecY + VecZ * VecZ);
	Length = (float)sqrt(Length);

	if(Length != 0)
	{
		VecX /= Length;
		VecY /= Length;
		VecZ /= Length;
	}

//	float Angle = acos(VecX + VecY + VecZ);

	float HeadingLength = Heading[0] * Heading[0] + Heading[1] * Heading[1] + Heading[2] * Heading[2];
	HeadingLength = sqrt(HeadingLength);


	if(HeadingLength != 0)
	{
		Heading[0] /= HeadingLength;
		Heading[1] /= HeadingLength;
		Heading[2] /= HeadingLength;
	}
*/
//	float Angle = acos(fabs(Heading[0] + Heading[1] + Heading[2]));	// heading towards target


//	cost = cost + 0.5f*cost*(2.0f + fabs(Angle));		//fabs(d1[0]*d2[0] + d1[1]*d2[1] + d1[2]*d2[2]));		// dodgy bastards!  Fixed that up for ya's
	cost = MinDist;		/// 50.0f + Angle;	// + fabs(Angle);
	//	cost = cost + 0.5f*cost*(2.0f - fabs(d1[0]*d2[0] + d1[1]*d2[1] + d1[2]*d2[2]));

	return cost;
}



static void seam_add_adjacent(Mesh *me, Heap *heap, int mednum, int vertnum, int *nedges, int *edges, int *prevedge, float *cost, int target)
{
	int startadj, endadj = nedges[vertnum+1];

	for (startadj = nedges[vertnum]; startadj < endadj; startadj++) 
	{
		int adjnum = edges[startadj];
		MEdge *medadj = me->medge + adjnum;
		float newcost;

		if (medadj->flag & ME_SEAM_DONE)
			continue;

		newcost = cost[mednum] + seam_cut_cost(me, mednum, adjnum, vertnum, target);

		if (cost[adjnum] > newcost) 
		{
			cost[adjnum] = newcost;
			prevedge[adjnum] = mednum;
			BLI_heap_insert(heap, newcost, (void*)adjnum);
		}
	}
}




int seam_shortest_path(int source, int target)
{
	Heap *heap;
	EdgeHash *ehash;
	float *cost;
	MEdge *med;
	int a, *nedges, *edges, *prevedge, mednum = -1, nedgeswap = 0;
	TFace *tf;
	MFace *mf;
	Mesh* me = &gCloseMesh;

	/* mark hidden edges as done, so we don't use them */
	ehash = BLI_edgehash_new();

	for (a=0, mf=me->mface, tf=me->tface; a<me->totface; a++, tf++, mf++) 
	{
		if (!(tf->flag & TF_HIDE)) 
		{
			BLI_edgehash_insert(ehash, mf->v1, mf->v2, NULL);
			BLI_edgehash_insert(ehash, mf->v2, mf->v3, NULL);
			if (mf->v4) 
			{
				BLI_edgehash_insert(ehash, mf->v3, mf->v4, NULL);
				BLI_edgehash_insert(ehash, mf->v4, mf->v1, NULL);
			}
			else
				BLI_edgehash_insert(ehash, mf->v3, mf->v1, NULL);
		}
	}

	for (a=0, med=me->medge; a<me->totedge; a++, med++)
	{
		if (!BLI_edgehash_haskey(ehash, med->v1, med->v2))
		{
			med->flag |= ME_SEAM_DONE;
		}
	}

	BLI_edgehash_free(ehash, NULL);

	/* alloc */
	nedges = (int*)MEM_callocN(sizeof(*nedges)*me->totvert+1, "SeamPathNEdges");
	edges = (int*)MEM_mallocN(sizeof(*edges)*me->totedge*2, "SeamPathEdges");
	prevedge = (int*)MEM_mallocN(sizeof(*prevedge)*me->totedge, "SeamPathPrevious");
	cost = (float*)MEM_mallocN(sizeof(*cost)*me->totedge, "SeamPathCost");

	/* count edges, compute adjacent edges offsets and fill adjacent edges */
	for (a=0, med=me->medge; a<me->totedge; a++, med++) 
	{
		nedges[med->v1+1]++;
		nedges[med->v2+1]++;
	}

	for (a=1; a<me->totvert; a++) 
	{
		int newswap = nedges[a+1];
		nedges[a+1] = nedgeswap + nedges[a];
		nedgeswap = newswap;
	}
	nedges[0] = nedges[1] = 0;

	for (a=0, med=me->medge; a<me->totedge; a++, med++) 
	{
		edges[nedges[med->v1+1]++] = a;
		edges[nedges[med->v2+1]++] = a;

		cost[a] = 1e20f;
		prevedge[a] = -1;
	}

	/* regular dijkstra shortest path, but over edges instead of vertices */
	heap = BLI_heap_new();
	BLI_heap_insert(heap, 0.0f, (void*)source);
	cost[source] = 0.0f;

	while (!BLI_heap_empty(heap)) 
	{
		mednum = (int)BLI_heap_popmin(heap);
		med = me->medge + mednum;

		if (mednum == target)
			break;

		if (med->flag & ME_SEAM_DONE)
			continue;

		med->flag |= ME_SEAM_DONE;

		seam_add_adjacent(me, heap, mednum, med->v1, nedges, edges, prevedge, cost, target);
		seam_add_adjacent(me, heap, mednum, med->v2, nedges, edges, prevedge, cost, target);
	}
	
	MEM_freeN(nedges);
	MEM_freeN(edges);
	MEM_freeN(cost);
	BLI_heap_free(heap, NULL);

	for (a=0, med=me->medge; a<me->totedge; a++, med++)
	{
		med->flag &= ~ME_SEAM_DONE;
	}

	if (mednum != target) 
	{
		MEM_freeN(prevedge);
		return 0;
	}

	/* follow path back to source and mark as seam */
	if (mednum == target) 
	{
		short allseams = 1;

		mednum = target;
		do {
			med = me->medge + mednum;
			if (!(med->flag & ME_SEAM)) 
			{
				allseams = 0;
				break;
			}
			mednum = prevedge[mednum];
		} while (mednum != source);

		mednum = target;
		do {
			med = me->medge + mednum;
			if (allseams)
				med->flag &= ~ME_SEAM;
			else
				med->flag |= ME_SEAM;
			mednum = prevedge[mednum];
		} while (mednum != -1);
	}

	MEM_freeN(prevedge);
	return 1;
}














/*

void pathselect(EditMesh* pEditMesh)
{
	EditVert *eve, *s, *t;
	EditEdge *eed;
	EditSelection *ese;
	PathEdge *newpe, *currpe;
	PathNode *currpn;
	PathNode *Q;
	int v, *previous, pathvert, pnindex; //pnindex redundant?
 	int unbalanced, totnodes;
	short physical;
	float *cost;
	Heap *heap; //binary heap for sorting pointers to PathNodes based upon a 'cost'
	
	s = t = NULL;
	
//	countall(); //paranoid?			// safe to remove Swanny?
	
	ese = ((EditSelection*)pEditMesh->selected.last);
	if(ese && ese->type == EDITVERT && ese->prev && ese->prev->type == EDITVERT)
	{
//		physical= pupmenu("Distance Method? %t|Edge Length%x1|Topological%x0");
		
		t = (EditVert*)ese->data;
		s = (EditVert*)ese->prev->data;
		
		//need to find out if t is actually reachable by s....
		for(eve=(EditVert*)pEditMesh->verts.first; eve; eve=eve->next)
		{ 
			eve->f1 = 0;
		}
		
		s->f1 = 1;
		
		unbalanced = 1;
		totnodes = 1;
		while(unbalanced)
		{
			unbalanced = 0;
			for(eed=(EditEdge*)pEditMesh->edges.first; eed; eed=eed->next)
			{
				if(!eed->h)
				{
					if(eed->v1->f1 && !eed->v2->f1)
					{ 
							eed->v2->f1 = 1;
							totnodes++;
							unbalanced = 1;
					}
					else if(eed->v2->f1 && !eed->v1->f1)
					{
							eed->v1->f1 = 1;
							totnodes++;
							unbalanced = 1;
					}
				}
			}
		}
		
		
		
		if(s->f1 && t->f1)
		{ //t can be reached by s
			Q = (PathNode*)MEM_callocN(sizeof(PathNode)*totnodes, "Path Select Nodes");
			totnodes = 0;
			for(eve=(EditVert*)pEditMesh->verts.first; eve; eve=eve->next)
			{
				if(eve->f1)
				{
					Q[totnodes].u = totnodes;
					Q[totnodes].edges.first = 0;
					Q[totnodes].edges.last = 0;
					Q[totnodes].visited = 0;
					eve->tmp.p = &(Q[totnodes]);
					totnodes++;
				}
				else eve->tmp.p = NULL;
			}
			
			for(eed=(EditEdge*)pEditMesh->edges.first; eed; eed=eed->next)
			{
				if(!eed->h)
				{
					if(eed->v1->f1)
					{
						currpn = ((PathNode*)eed->v1->tmp.p);
						
						newpe = (PathEdge*)MEM_mallocN(sizeof(PathEdge), "Path Edge");
						newpe->v = ((PathNode*)eed->v2->tmp.p)->u;
						if(physical)
						{
								newpe->w = VecLenf(eed->v1->co, eed->v2->co);
						}
						else newpe->w = 1;
						newpe->next = 0;
						newpe->prev = 0;
						BLI_addtail(&(currpn->edges), newpe);
					} 
					if(eed->v2->f1)
					{
						currpn = ((PathNode*)eed->v2->tmp.p); 
						newpe = (PathEdge*)MEM_mallocN(sizeof(PathEdge), "Path Edge");
						newpe->v = ((PathNode*)eed->v1->tmp.p)->u;
						if(physical)
						{
							newpe->w = VecLenf(eed->v1->co, eed->v2->co);
						}
						else newpe->w = 1;
						newpe->next = 0;
						newpe->prev = 0;
						BLI_addtail(&(currpn->edges), newpe);
					}
				}
			}
			
			heap = BLI_heap_new();
			cost = (float*)MEM_callocN(sizeof(float)*totnodes, "Path Select Costs");
			previous = (int*)MEM_callocN(sizeof(int)*totnodes, "PathNode indices");
			
			for(v=0; v < totnodes; v++)
			{
				cost[v] = 1000000;
				previous[v] = -1; //array of indices
			}
			
			pnindex = ((PathNode*)s->tmp.p)->u;
			cost[pnindex] = 0;
			BLI_heap_insert(heap,  0.0f, (void*)pnindex);
						
			while( !BLI_heap_empty(heap) )
			{
				
				pnindex = (int)BLI_heap_popmin(heap);
				currpn = &(Q[pnindex]);
				
				if(currpn == (PathNode*)t->tmp.p) //target has been reached....
					break;
				
				for(currpe=(PathEdge*)currpn->edges.first; currpe; currpe=currpe->next)
				{
					if(!Q[currpe->v].visited)
					{
						if( cost[currpe->v] > (cost[currpn->u ] + currpe->w) )
						{
							cost[currpe->v] = cost[currpn->u] + currpe->w;
							previous[currpe->v] = currpn->u;
							Q[currpe->v].visited = 1;
							BLI_heap_insert(heap, cost[currpe->v], (void*)currpe->v);
						}
					}
				}
			}
			
			pathvert = ((PathNode*)t->tmp.p)->u;
			while(pathvert != -1)
			{
				for(eve=(EditVert*)pEditMesh->verts.first; eve; eve=eve->next)
				{
					if(eve->f1)
					{
						if( ((PathNode*)eve->tmp.p)->u == pathvert) eve->f |= SELECT;
					}
				}
				pathvert = previous[pathvert];
			}
			
			for(v=0; v < totnodes; v++) BLI_freelistN(&(Q[v].edges));
			MEM_freeN(Q);
			MEM_freeN(cost);
			MEM_freeN(previous);
			BLI_heap_free(heap, NULL);
//			EM_select_flush();
//			countall();									// safe to remove Swanny?
//			DAG_object_flush_update(G.scene, G.obedit, OB_RECALC_DATA);
//			allqueue(REDRAWVIEW3D, 0);
		}
	}
	else{
//		error("Path Selection requires that exactly two vertices be selected");
		return;
	}
}
*/




void FindAdjacentFace(EditEdge* pWalkingEdge)			//, EditEdge* pSelectedEdge)
{
	EditFace* efa;
	EditEdge* eed;

	for(efa= (EditFace*)gEditMesh.faces.first; efa; efa= efa->next)
	{
		bool ThisFace = false;

		if(efa->e1 == pWalkingEdge) ThisFace = true;
		if(efa->e2 == pWalkingEdge) ThisFace = true;
		if(efa->e3 == pWalkingEdge) ThisFace = true;
		if(efa->e4 == pWalkingEdge) ThisFace = true;

		int WalkedCount = 0;

		if(efa->e1->f2 == 1) WalkedCount++;
		if(efa->e2->f2 == 1) WalkedCount++;
		if(efa->e3->f2 == 1) WalkedCount++;
		if(efa->e4 != NULL)
		{
			if(efa->e4->f2 == 1) WalkedCount++;
		}

		if(ThisFace == true && WalkedCount == 2)
		{
			bool Result;

			if(efa->e4 != NULL)
			{
				Result = efa->e1->f3 == false && efa->e2->f3 == false && efa->e3->f3 == false && efa->e4->f3 == false;
			}
			else
			{
				Result = efa->e1->f3 == false && efa->e2->f3 == false && efa->e3->f3 == false;
			}

			if(Result)			// face with no selections but with shared edge
			{
// go find a joining selected edge!

				if(efa->e1->f2 == 0)
				{
					for(eed = (EditEdge*)gEditMesh.edges.first; eed; eed= eed->next) 
					{
						if(eed->f3 == 1)
						{
							if(efa->e1->v1 == eed->v1 || efa->e1->v1 == eed->v2 || efa->e1->v2 == eed->v1 || efa->e1->v2 == eed->v2)
							{
								efa->e1->f3 = 1;
							}
						}
					}
				}

				if(efa->e2->f2 == 0)
				{
					for(eed = (EditEdge*)gEditMesh.edges.first; eed; eed= eed->next) 
					{
						if(eed->f3 == 1)
						{
							if(efa->e2->v1 == eed->v1 || efa->e2->v1 == eed->v2 || efa->e2->v2 == eed->v1 || efa->e2->v2 == eed->v2)
							{
								efa->e2->f3 = 1;
							}
						}
					}
				}

				if(efa->e3->f2 == 0)
				{
					for(eed = (EditEdge*)gEditMesh.edges.first; eed; eed= eed->next) 
					{
						if(eed->f3 == 1)
						{
							if(efa->e3->v1 == eed->v1 || efa->e3->v1 == eed->v2 || efa->e3->v2 == eed->v1 || efa->e3->v2 == eed->v2)
							{
								efa->e3->f3 = 1;
							}
						}
					}
				}

				if(efa->e4 != NULL)
				{
					if(efa->e4->f2 == 0)
					{
						for(eed = (EditEdge*)gEditMesh.edges.first; eed; eed= eed->next) 
						{
							if(eed->f3 == 1)
							{
								if(efa->e4->v1 == eed->v1 || efa->e4->v1 == eed->v2 || efa->e4->v2 == eed->v1 || efa->e4->v2 == eed->v2)
								{
									efa->e4->f3 = 1;
								}
							}
						}
					}
				}
			}
		}
	}
}



/* New LoopCut */

static void edgering_sel(EditEdge *startedge, EditEdge* selectedEdge, int select, int previewlines)
{
	EditMesh *em = &gEditMesh;
	EditEdge *eed;
	EditFace *efa;
//	EditVert *v[2][2];
//	float co[2][3];
	int looking= 1;		//,i;
	
	// in eed->f1 we put the valence (amount of faces in edge)
	// in eed->f2 we put tagged flag as correct loop
	// in efa->f1 we put tagged flag as correct to select

	for(eed = (EditEdge*)em->edges.first; eed; eed= eed->next) 
	{
		eed->f1= 0;
		eed->f2= 0;
		eed->f3= 0;
	}

	for(efa = (EditFace*)em->faces.first; efa; efa= efa->next) 
	{
		efa->f1= 0;
		if(efa->h==0) 
		{
			efa->e1->f1++;
			efa->e2->f1++;
			efa->e3->f1++;
			if(efa->e4) efa->e4->f1++;
		}
	}
	
	// tag startedge OK
	startedge->f2 = 1;
	selectedEdge->f3 = 1;
	
	while(looking) 
	{
		looking= 0;
		
		for(efa= (EditFace*)gEditMesh.faces.first; efa; efa= efa->next) 
		{
			if(efa->e4 && efa->f1==0 && efa->h == 0) 
			{	// not done quad
				EditEdge* pWalkingEdge = NULL;

				if(efa->e1->f1 <= 2 && efa->e2->f1 <= 2 && efa->e3->f1 <= 2 && efa->e4->f1 <= 2) 
				{ // valence ok
					// if edge tagged, select opposing edge and mark face ok
					if(efa->e1->f2) 
					{
						efa->e3->f2= 1;
						efa->f1= 1;
						pWalkingEdge = efa->e1;
						FindAdjacentFace(pWalkingEdge);
						looking= 1;

					}
					else if(efa->e2->f2) 
					{
						efa->e4->f2= 1;
						efa->f1= 1;
						pWalkingEdge = efa->e2;
						FindAdjacentFace(pWalkingEdge);
						looking= 1;
					}

					if(efa->e3->f2) 
					{
						efa->e1->f2= 1;
						efa->f1= 1;
						pWalkingEdge = efa->e3;
						FindAdjacentFace(pWalkingEdge);
						looking= 1;
					}

					if(efa->e4->f2) 
					{
						efa->e2->f2= 1;
						efa->f1= 1;
						pWalkingEdge = efa->e4;
						FindAdjacentFace(pWalkingEdge);
						looking= 1;
					}
				}
/*
				EditEdge* pSelectedEdge = NULL;

				if(efa->e1->f3 == 1) pSelectedEdge = efa->e1;
				if(efa->e2->f3 == 1) pSelectedEdge = efa->e2;
				if(efa->e3->f3 == 1) pSelectedEdge = efa->e3;
				if(efa->e4->f3 == 1) pSelectedEdge = efa->e4;

				if(pWalkingEdge != NULL && pSelectedEdge != NULL) FindAdjacentFace(pWalkingEdge, pSelectedEdge);
*/			}
		}
	}
/*	
	if(previewlines > 0 && select == 0){
			persp(PERSP_VIEW);
			glPushMatrix();
			mymultmatrix(G.obedit->obmat);
			//glColor3ub(0, 255, 255);
			//glBegin(GL_LINES);			
			//glVertex3f(nearest->v1->co[0],nearest->v1->co[1],nearest->v1->co[2]);
			//glVertex3f(nearest->v2->co[0],nearest->v2->co[1],nearest->v2->co[2]);
			//glEnd();
			for(efa= em->faces.first; efa; efa= efa->next) {
				if(efa->v4 == NULL) {  continue; }
				if(efa->h == 0){
					if(efa->e1->f2 == 1){
						if(efa->e1->h == 1 || efa->e3->h == 1 )
							continue;
						
						v[0][0] = efa->v1;
						v[0][1] = efa->v2;
						v[1][0] = efa->v4;
						v[1][1] = efa->v3;
					} else if(efa->e2->f2 == 1){
						if(efa->e2->h == 1 || efa->e4->h == 1)
							continue;
						v[0][0] = efa->v2;
						v[0][1] = efa->v3;
						v[1][0] = efa->v1;
						v[1][1] = efa->v4;					
					} else { continue; }
										  
					for(i=1;i<=previewlines;i++){
						co[0][0] = (v[0][1]->co[0] - v[0][0]->co[0])*(i/((float)previewlines+1))+v[0][0]->co[0];
						co[0][1] = (v[0][1]->co[1] - v[0][0]->co[1])*(i/((float)previewlines+1))+v[0][0]->co[1];
						co[0][2] = (v[0][1]->co[2] - v[0][0]->co[2])*(i/((float)previewlines+1))+v[0][0]->co[2];

						co[1][0] = (v[1][1]->co[0] - v[1][0]->co[0])*(i/((float)previewlines+1))+v[1][0]->co[0];
						co[1][1] = (v[1][1]->co[1] - v[1][0]->co[1])*(i/((float)previewlines+1))+v[1][0]->co[1];
						co[1][2] = (v[1][1]->co[2] - v[1][0]->co[2])*(i/((float)previewlines+1))+v[1][0]->co[2];					
						glColor3ub(255, 0, 255);
						glBegin(GL_LINES);	
						glVertex3f(co[0][0],co[0][1],co[0][2]);
						glVertex3f(co[1][0],co[1][1],co[1][2]);
						glEnd();
					}
				}
			}
			glPopMatrix();   
	} else {	
	
	// (de)select the edges 
		for(eed= em->edges.first; eed; eed= eed->next) {
		if(eed->f2) EM_select_edge(eed, select);
		}
	}
*/
}





/***************************************************************************************************************
****************************************************************************************************************
***************************************************************************************************************/

// Add a list of vertices

EditVert* addvertlist(float *vec, int VertexIndex)
{
	EditMesh *em = &gEditMesh;
	EditVert *eve;
	static int hashnr= 0;

	int Sizeof = sizeof(EditVert);
	eve= (EditVert*)calloc(sizeof(EditVert), 1);
	BLI_addtail(&em->verts, eve);
	
	if(vec) VECCOPY(eve->co, vec);
	eve->VertexIndex = VertexIndex;

	eve->hash= hashnr++;
	if( hashnr>=EDHASHSIZE) hashnr= 0;

	// new verts get keyindex of -1 since they did not
	// have a pre-editmode vertex order
	
	eve->keyindex = -1;
	return eve;
}


void free_editvert (EditVert *eve)
{
//	if(eve->dw) MEM_freeN(eve->dw);
//	EM_remove_selection(eve, EDITVERT);
	if(eve->fast==0){ 
		free(eve);
	}
}

void free_vertlist(ListBase *edve) 
{
	EditVert *eve, *next;

	if (!edve) return;

	eve= (EditVert*)edve->first;
	while(eve) {
		next= eve->next;
		free_editvert(eve);
		eve= next;
	}
	edve->first= edve->last= NULL;
}






EditEdge* findedgelist(EditVert *v1, EditVert *v2)
{
	EditVert *v3;
	struct HashEdge *he;

	/* swap ? */
	if( v1 > v2) {
		v3= v2; 
		v2= v1; 
		v1= v3;
	}
	
	if(gEditMesh.hashedgetab==NULL)
		gEditMesh.hashedgetab= (HashEdge*)MEM_callocN(EDHASHSIZE*sizeof(struct HashEdge), "hashedgetab");

	he= gEditMesh.hashedgetab + EDHASH(v1->hash, v2->hash);
	
	while(he) {
		
		if(he->eed && he->eed->v1==v1 && he->eed->v2==v2) return he->eed;
		
		he= he->next;
	}
	return 0;
}

static void insert_hashedge(EditEdge *eed)
{
	/* assuming that eed is not in the list yet, and that a find has been done before */
	
	struct HashEdge *first, *he;

	first= gEditMesh.hashedgetab + EDHASH(eed->v1->hash, eed->v2->hash);

	if( first->eed==0 ) {
		first->eed= eed;
	}
	else {
		he= &eed->hash; 
		he->eed= eed;
		he->next= first->next;
		first->next= he;
	}
}

static void remove_hashedge(EditEdge *eed)
{
	/* assuming eed is in the list */
	
	struct HashEdge *first, *he, *prev=NULL;

	he=first= gEditMesh.hashedgetab + EDHASH(eed->v1->hash, eed->v2->hash);

	while(he) {
		if(he->eed == eed) {
			/* remove from list */
			if(he==first) {
				if(first->next) {
					he= first->next;
					first->eed= he->eed;
					first->next= he->next;
				}
				else he->eed= 0;
			}
			else {
				prev->next= he->next;
			}
			return;
		}
		prev= he;
		he= he->next;
	}
}


// Add an edge to the list

EditEdge *addedgelist(EditVert *v1, EditVert *v2)		//, EditEdge *example)
{
	EditMesh *em = &gEditMesh;
	EditVert *v3;
	EditEdge *eed;
	int swap= 0;
	
	if(v1==v2) return NULL;
	if(v1==NULL || v2==NULL) return NULL;

	/* swap ? */
	if(v1>v2) {
		v3= v2; 
		v2= v1; 
		v1= v3;
		swap= 1;
	}
	
	/* find in hashlist */
	eed= findedgelist(v1, v2);

	if(eed==NULL) {
	
		eed= (EditEdge *)calloc(sizeof(EditEdge), 1);
		eed->v1= v1;
		eed->v2= v2;
		BLI_addtail(&em->edges, eed);
		eed->dir= swap;
		insert_hashedge(eed);
		
		/* copy edge data:
		   rule is to do this with addedgelist call, before addfacelist */
//		if(example) {
//			eed->crease= example->crease;
//			eed->seam = example->seam;
//			eed->h |= (example->h & EM_FGON);
//		}
	}

	return eed;
}



void free_editedge(EditEdge *eed)
{
//	EM_remove_selection(eed, EDITEDGE);
	if(eed->fast==0){ 
		free(eed);
	}
}


void free_edgelist(ListBase *lb)
{
	EditEdge *eed, *next;
	
	eed= (EditEdge*)lb->first;
	while(eed) {
		next= eed->next;
		free_editedge(eed);
		eed= next;
	}
	lb->first= lb->last= NULL;
}


void free_editface(EditFace *efa)
{
//	EM_remove_selection(efa, EDITFACE);
	if(efa->fast==0){ 
		free(efa);
	}
}

void free_facelist(ListBase *lb)
{
	EditFace *efa, *next;
	
	efa= (EditFace*)lb->first;
	while(efa) {
		next= efa->next;
		free_editface(efa);
		efa= next;
	}
	lb->first= lb->last= NULL;
}



EditFace *addfacelist(EditVert *v1, EditVert *v2, EditVert *v3, EditVert *v4)		//, EditFace *example, EditFace *exampleEdges)
{
	EditMesh *em = &gEditMesh;
	EditFace *efa;
	EditEdge *e1, *e2=0, *e3=0, *e4=0;

	/* add face to list and do the edges */
//	if(exampleEdges) {
//		e1= addedgelist(v1, v2, exampleEdges->e1);
//		e2= addedgelist(v2, v3, exampleEdges->e2);
//		if(v4) e3= addedgelist(v3, v4, exampleEdges->e3); 
//		else e3= addedgelist(v3, v1, exampleEdges->e3);
//		if(v4) e4= addedgelist(v4, v1, exampleEdges->e4);
//	}
//	else {

/*		if(v4)
		{
			e1 = addedgelist(v4, v1);		//, NULL);
			e2 = addedgelist(v1, v2);		//, NULL);
			e3 = addedgelist(v2, v3);		//, NULL);
			e4 = addedgelist(v3, v4);		//, NULL);
		}
		else
		{
			e1 = addedgelist(v3, v1);		//, NULL);
			e2 = addedgelist(v1, v2);		//, NULL);
			e3 = addedgelist(v2, v3);		//, NULL);
		}
*/

		e1= addedgelist(v1, v2);		//, NULL);
		e2= addedgelist(v2, v3);		//, NULL);
		if(v4) e3= addedgelist(v3, v4);		//, NULL); 
		else e3= addedgelist(v3, v1);		//, NULL);
		if(v4) e4= addedgelist(v4, v1);		//, NULL);

//	}
	
	if(v1==v2 || v2==v3 || v1==v3) return NULL;
	if(e2==0) return NULL;

	efa= (EditFace *)calloc(sizeof(EditFace), 1);
	efa->v1= v1;
	efa->v2= v2;
	efa->v3= v3;
	efa->v4= v4;

	efa->e1= e1;
	efa->e2= e2;
	efa->e3= e3;
	efa->e4= e4;

//	if(example) {
//		efa->mat_nr= example->mat_nr;
//		efa->tf= example->tf;
//		efa->tf.flag &= ~TF_ACTIVE;
//		efa->flag= example->flag;
//	}
//	else {
//		if (G.obedit && G.obedit->actcol)
//			efa->mat_nr= G.obedit->actcol-1;
//		default_uv(efa->tf.uv, 1.0);

		/* Initialize colors */
//		efa->tf.col[0]= efa->tf.col[1]= efa->tf.col[2]= efa->tf.col[3]= vpaint_get_current_col();
//	}

	BLI_addtail(&em->faces, efa);

	if(efa->v4) {
		CalcNormFloat4(efa->v1->co, efa->v2->co, efa->v3->co, efa->v4->co, efa->n);
		CalcCent4f(efa->cent, efa->v1->co, efa->v2->co, efa->v3->co, efa->v4->co);
	}
	else {
		CalcNormFloat(efa->v1->co, efa->v2->co, efa->v3->co, efa->n);
		CalcCent3f(efa->cent, efa->v1->co, efa->v2->co, efa->v3->co);
	}

	return efa;
}



EditVert** VertsList;


void EditMeshAddVerts(int NumberVerts, float* Vertices)
{
	VertsList = new EditVert*[NumberVerts];

	for(int Index = 0; Index < NumberVerts; Index++)
	{
		VertsList[Index] = addvertlist(&Vertices[Index*3], Index);
	}
}



void EditMeshAddFaces(int NumberFaces, Face* pFaces)
{
	for(int Index = 0; Index < NumberFaces; Index++)
	{
		int VertIndex0, VertIndex1, VertIndex2, VertIndex3;
		if(pFaces[Index].GetNumberOfVertices() > 4) return;

		VertIndex0 = pFaces[Index].GetVertexIndex(0);
		VertIndex1 = pFaces[Index].GetVertexIndex(1);
		VertIndex2 = pFaces[Index].GetVertexIndex(2);

		if(pFaces[Index].GetNumberOfVertices() == 4)
		{
			VertIndex3 = pFaces[Index].GetVertexIndex(3);
			addfacelist(VertsList[VertIndex0], VertsList[VertIndex1], VertsList[VertIndex2], VertsList[VertIndex3]);
		}
		else
		{
            addfacelist(VertsList[VertIndex0], VertsList[VertIndex1], VertsList[VertIndex2], NULL);
		}
	}
}


void EdgeRingSelect(int SelectedEdgeVert1, int SelectedEdgeVert2, int AdjEdgeVert1, int AdjEdgeVert2, Face* pFaces)
{
	EditEdge* SelectedEdge = findedgelist(VertsList[SelectedEdgeVert1], VertsList[SelectedEdgeVert2]);
	EditEdge* StartEdge = findedgelist(VertsList[AdjEdgeVert1], VertsList[AdjEdgeVert2]);

	if(SelectedEdge == NULL || StartEdge == NULL) return;
	edgering_sel(StartEdge, SelectedEdge, 0, 0);

	EditFace *efa;

	for(efa= (EditFace*)gEditMesh.faces.first; efa; efa= efa->next) 
	{
//		int VertIndex1, VertIndex2;

		if(efa->e1->f3 == 1)				// efa->f1
		{
			RKMesh::Get()->SelectEdge(efa->e1->v1->VertexIndex, efa->e1->v2->VertexIndex);
//			RKMesh::Get()->SelectAllEdges(efa->e1->v1->VertexIndex, efa->e1->v2->VertexIndex);
		}

		if(efa->e2->f3 == 1)
		{
			RKMesh::Get()->SelectEdge(efa->e2->v1->VertexIndex, efa->e2->v2->VertexIndex);
//			RKMesh::Get()->SelectAllEdges(efa->e2->v1->VertexIndex, efa->e2->v2->VertexIndex);
		}

		if(efa->e3->f3 == 1)
		{
			RKMesh::Get()->SelectEdge(efa->e3->v1->VertexIndex, efa->e3->v2->VertexIndex);
//			RKMesh::Get()->SelectAllEdges(efa->e3->v1->VertexIndex, efa->e3->v2->VertexIndex);
		}


		if(efa->e4 != NULL && efa->e4->f3 == 1)
		{
			RKMesh::Get()->SelectEdge(efa->e4->v1->VertexIndex, efa->e4->v2->VertexIndex);
//			RKMesh::Get()->SelectAllEdges(efa->e4->v1->VertexIndex, efa->e4->v2->VertexIndex);
		}

		pFaces++;
	}
}


void ResetEditMesh()
{
	free_vertlist(&gEditMesh.verts);
	free_edgelist(&gEditMesh.edges);
	free_facelist(&gEditMesh.faces);

	if(gEditMesh.hashedgetab != NULL)
	{
		MEM_freeN(gEditMesh.hashedgetab);
		gEditMesh.hashedgetab = NULL;
	}

	if(VertsList != NULL) delete[] VertsList;
	VertsList = NULL;

}
