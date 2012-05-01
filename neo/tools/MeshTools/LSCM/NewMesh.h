#ifndef NEWMESH_H
#define NEWMESH_H


typedef struct TFace
{

	float uv[4][2];		/* when you change this: also do function set_correct_uv in editmesh.c, and there are more locations that use the size of this part */
	int BankIndex[4];
	char flag, pad;
	short unwrap;
} TFace;


typedef struct MFace
{
	unsigned int v1, v2, v3, v4;

	unsigned int map1, map2, map3;
} MFace;


typedef struct MEdge
{
	unsigned int v1, v2;
	short flag, pad;
} MEdge;


typedef struct MVert
{
	float	co[3];
	short	no[3];
	char flag, mat_nr;
} MVert;


typedef struct Mesh {

	struct MFace *mface;
	struct TFace *tface;
	struct MVert *mvert;
	struct MEdge *medge;

	int totedge, totface, totvert;

} Mesh;



/* **************** MESH ********************* */

/* texflag */
#define AUTOSPACE	1

/* me->flag */
#define ME_ISDONE		1
#define ME_NOPUNOFLIP	2
#define ME_TWOSIDED		4
#define ME_UVEFFECT		8
#define ME_VCOLEFFECT	16
#define ME_AUTOSMOOTH	32
#define ME_SMESH		64
#define ME_SUBSURF		128
#define ME_OPT_EDGES	256

/* Subsurf Type */
#define ME_CC_SUBSURF 		0
#define ME_SIMPLE_SUBSURF 	1

#define TF_DYNAMIC		1
/* #define TF_INVISIBLE	2 */
#define TF_TEX			4
#define TF_SHAREDVERT	8
#define TF_LIGHT		16

#define TF_SHAREDCOL	64
#define TF_TILES		128
#define TF_BILLBOARD	256
#define TF_TWOSIDE		512
#define TF_INVISIBLE	1024

#define TF_OBCOL		2048
#define TF_BILLBOARD2		4096	/* with Z axis constraint */
#define TF_SHADOW		8192
#define TF_BMFONT		16384

/* tface->flag: 1=select 2=active*/
#define TF_SELECT	1
#define TF_ACTIVE	2
#define TF_SEL1		4
#define TF_SEL2		8
#define TF_SEL3		16
#define TF_SEL4		32
#define TF_HIDE		64

/* tface->transp */
#define TF_SOLID	0
#define TF_ADD		1
#define TF_ALPHA	2
#define TF_SUB		3

/* tface->unwrap */
#define TF_SEAM1	1
#define TF_SEAM2	2
#define TF_SEAM3	4
#define TF_SEAM4	8
#define TF_PIN1	    16
#define TF_PIN2	    32
#define TF_PIN3	    64
#define TF_PIN4	    128

#define MESH_MAX_VERTS 2000000000L



/* medge->flag (1=SELECT)*/
#define ME_EDGEDRAW			(1<<1)
#define ME_SEAM				(1<<2)
#define ME_FGON				(1<<3)
						// reserve 16 for ME_HIDE
#define ME_EDGERENDER		(1<<5)
#define ME_LOOSEEDGE		(1<<7)
#define ME_SEAM_LAST		(1<<8)
#define ME_EDGE_STEPINDEX	(1<<15)




#endif