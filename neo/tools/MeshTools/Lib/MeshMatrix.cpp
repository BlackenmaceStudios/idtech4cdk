/*  
	Roadkill 1.1 UV Unwrap utility
    Copyright (C) 2007  Andy Swann

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// Big apologies for this..  old, old, code..  my first crack at C++ from almost 10 years ago, that's my excuse anyway..
//
// I would never write this code this way again..  just trust it works..


#include "../LSCM/MeshBuilderStdafx.h"

#include "Matrix.h"
#include "Math.h"

#define GLfloat float

/********************************
*	 Set Matrix to Identitiy	*
********************************/

void Matrix::SetIdentity(void)
{
	M[0] = 1.0f;
	M[1] = 0.0f;
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = 1.0f;
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[8] = 0.0f;
	M[9] = 0.0f;
	M[10] = 1.0f;
	M[11] = 0.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = 0.0f;
	M[15] = 1.0f;
}



void Matrix::SetMatrix(Matrix* MatrixIn)
{
	M[0] = MatrixIn->M[0];
	M[1] = MatrixIn->M[1];
	M[2] = MatrixIn->M[2];
	M[3] = MatrixIn->M[3];

	M[4] = MatrixIn->M[4];
	M[5] = MatrixIn->M[5];
	M[6] = MatrixIn->M[6];
	M[7] = MatrixIn->M[7];

	M[8] = MatrixIn->M[8];
	M[9] = MatrixIn->M[9];
	M[10] = MatrixIn->M[10];
	M[11] = MatrixIn->M[11];

	M[12] = MatrixIn->M[12];
	M[13] = MatrixIn->M[13];
	M[14] = MatrixIn->M[14];
	M[15] = MatrixIn->M[15];
}



/********************************
*	Set Up X Rotation Matrix	*
********************************/

void Matrix::SetXRotation(float Angle)
{
	Angle *= 0.017460317f;				// convert degrees to radians

	float Sine = (float)sin(Angle);
	float Cosine = (float)cos(Angle);

	M[0] = 1.0f;
	M[4] = 0.0f;
	M[8] = 0.0f;
	M[12] = 0.0f;

	M[1] = 0.0f;
	M[5] = Cosine;
	M[9] = -Sine;
	M[13] = 0.0f;

	M[2] = 0.0f;
	M[6] = Sine;
	M[10] = Cosine;
	M[14] = 0.0f;

	M[3] = 0.0f;
	M[7] = 0.0f;
	M[11] = 0.0f;
	M[15] = 1.0f;
}



/********************************
*	Set Up Y Rotation Matrix	*
********************************/

void Matrix::SetYRotation(float Angle)
{
	Angle *= 0.017460317f;				// convert degrees to radians

	float Sine = (float)sin(Angle);
	float Cosine = (float)cos(Angle);

	M[0] = Cosine;
	M[4] = 0.0f;
	M[8] = Sine;
	M[12] = 0.0f;

	M[1] = 0.0f;
	M[5] = 1.0f;
	M[9] = 0.0f;
	M[13] = 0.0f;

	M[2] = -Sine;
	M[6] = 0.0f;
	M[10] = Cosine;
	M[14] = 0.0f;

	M[3] = 0.0f;
	M[7] = 0.0f;
	M[11] = 0.0f;
	M[15] = 1.0f;
}




/********************************
*	Set Up Z Rotation Matrix	*
********************************/

void Matrix::SetZRotation(float Angle)
{
	Angle *= 0.01746031746f;				// convert degrees to radians

	float Sine = (float)sin(Angle);
	float Cosine = (float)cos(Angle);

	M[0] = Cosine;
	M[4] = -Sine;
	M[8] = 0.0f;
	M[12] = 0.0f;

	M[1] = Sine;
	M[5] = Cosine;
	M[9] = 0.0f;
	M[13] = 0.0f;

	M[2] = 0.0f;
	M[6] = 0.0f;
	M[10] = 1.0f;
	M[14] = 0.0f;

	M[3] = 0.0f;
	M[7] = 0.0f;
	M[11] = 0.0f;
	M[15] = 1.0f;
}


/*************************************************************************************************
	Determinant of a 4x4 matrix 
		Det4x4 require calculation of of the 4 3x3 determinants too, which in turn require the
			2x2's to be calculated.  Endless joy
***************************************************************************************************/
float Matrix::Det4x4()
{
    float ans;
    float a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    a1 = M[0]; b1 = M[1];
    c1 = M[2]; d1 = M[3];

    a2 = M[4]; b2 = M[5];
    c2 = M[6]; d2 = M[7];

    a3 = M[8]; b3 = M[9];
    c3 = M[10]; d3 = M[11];

    a4 = M[12]; b4 = M[13];
    c4 = M[14]; d4 = M[15];

    ans = a1 * det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4) -
		  b1 * det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4) +
	  	  c1 * det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4) -
	  	  d1 * det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    return (ans);
}

/*************************************************************************************************
	Adjoint of a 4x4
		Matrix of co-factors
***************************************************************************************************/
void Matrix::Adjoint()
{
    float a1, a2, a3, a4, b1, b2, b3, b4;
    float c1, c2, c3, c4, d1, d2, d3, d4;

    a1 = M[0]; b1 = M[1];
    c1 = M[2]; d1 = M[3];

    a2 = M[4]; b2 = M[5];
    c2 = M[6]; d2 = M[7];

    a3 = M[8]; b3 = M[9];
    c3 = M[10]; d3 = M[11];

    a4 = M[12]; b4 = M[13];
    c4 = M[14]; d4 = M[15];

    /* row column labeling reversed since we transpose rows & coluMns */
    M[0] =  det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
    M[4] = -det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
    M[8] =  det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
    M[12] = -det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    M[1] = -det3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
    M[5] =  det3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
    M[9] = -det3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
    M[13] =  det3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

    M[2] =  det3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
    M[6] = -det3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
    M[10] =  det3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
    M[14] = -det3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

    M[3] = -det3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
    M[7] =  det3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
    M[11] = -det3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
    M[15] =  det3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

/*************************************************************************************************
	Inverse of a 4x4
***************************************************************************************************/
void Matrix::Inv( Matrix *Mat1 )
{
    float det;
    int i;

	*Mat1	= *this;
    det = Mat1->Det4x4();

    Mat1->Adjoint();

    for (i = 0; i < 16; i++)
	{
		Mat1->M[i] /= det;
	}

}

/********************************
*	   Set Up Translation		*
********************************/

void Matrix::SetTranslation(Vector* Translation)
{
	M[12] = Translation->X;
	M[13] = Translation->Y;
	M[14] = Translation->Z;
	M[15] = 1.0f;			//Translation->W;
}



/********************************
*	  Set Up Scale Factor		*
********************************/

void Matrix::SetScale(Vector* Scale)
{
	SetIdentity();

	M[0] = Scale->X;
	M[5] = Scale->Y;
	M[10] = Scale->Z;
	M[15] = Scale->W;
}



/********************************
*	    Rotate a Vertex			*
********************************/

void Matrix::RotateVertex(Vertex* VertexIn, Vertex* VertexOut)
{
	VertexOut->X = (VertexIn->X * M[0]) + (VertexIn->Y * M[4]) + (VertexIn->Z * M[8]);
	VertexOut->Y = (VertexIn->X * M[1]) + (VertexIn->Y * M[5]) + (VertexIn->Z * M[9]);
	VertexOut->Z = (VertexIn->X * M[2]) + (VertexIn->Y * M[6]) + (VertexIn->Z * M[10]);
}


/********************************
* Rotate and Translate a Vertex *
********************************/

void Matrix::RotTransVertex(Vertex* VertexIn, Vertex* VertexOut)
{
	VertexOut->X = (VertexIn->X * M[0]) + (VertexIn->Y * M[4]) + (VertexIn->Z * M[8]) + M[12];
	VertexOut->Y = (VertexIn->X * M[1]) + (VertexIn->Y * M[5]) + (VertexIn->Z * M[9]) + M[13];
	VertexOut->Z = (VertexIn->X * M[2]) + (VertexIn->Y * M[6]) + (VertexIn->Z * M[10]) + M[14];
}



/********************************
*		Rotate a Vector			*
********************************/

void Matrix::RotateVector(Vector* VectorIn, Vector* VectorOut)
{
	VectorOut->X = (VectorIn->X * M[0]) + (VectorIn->Y * M[4]) + (VectorIn->Z * M[8]);
	VectorOut->Y = (VectorIn->X * M[1]) + (VectorIn->Y * M[5]) + (VectorIn->Z * M[9]);
	VectorOut->Z = (VectorIn->X * M[2]) + (VectorIn->Y * M[6]) + (VectorIn->Z * M[10]);
	VectorOut->W = (VectorIn->X * M[3]) + (VectorIn->Y * M[7]) + (VectorIn->Z * M[11]);
}



/********************************
* Rotate and Translate a Vector *
********************************/

void Matrix::RotTransVector(Vector* VectorIn, Vector* VectorOut)
{
	VectorOut->X = (VectorIn->X * M[0]) + (VectorIn->Y * M[4]) + (VectorIn->Z * M[8]) + (VectorIn->W * M[12]);
	VectorOut->Y = (VectorIn->X * M[1]) + (VectorIn->Y * M[5]) + (VectorIn->Z * M[9]) + (VectorIn->W * M[13]);
	VectorOut->Z = (VectorIn->X * M[2]) + (VectorIn->Y * M[6]) + (VectorIn->Z * M[10]) + (VectorIn->W * M[14]);
	VectorOut->W = (VectorIn->X * M[3]) + (VectorIn->Y * M[7]) + (VectorIn->Z * M[11]) + (VectorIn->W * M[15]);
}



/****************************************
* Multiply 'this' Matrix with Matrix In *
****************************************/

void Matrix::MultiplyMatrix(Matrix* MatrixIn, Matrix* MatrixIn2)
{
	M[0] = (MatrixIn->M[0] * MatrixIn2->M[0]) + (MatrixIn->M[1] * MatrixIn2->M[4]) + (MatrixIn->M[2] * MatrixIn2->M[8]) + (MatrixIn->M[3] * MatrixIn2->M[12]);
	M[1] = (MatrixIn->M[0] * MatrixIn2->M[1]) + (MatrixIn->M[1] * MatrixIn2->M[5]) + (MatrixIn->M[2] * MatrixIn2->M[9]) + (MatrixIn->M[3] * MatrixIn2->M[13]);
	M[2] = (MatrixIn->M[0] * MatrixIn2->M[2]) + (MatrixIn->M[1] * MatrixIn2->M[6]) + (MatrixIn->M[2] * MatrixIn2->M[10]) + (MatrixIn->M[3] * MatrixIn2->M[14]);
	M[3] = (MatrixIn->M[0] * MatrixIn2->M[3]) + (MatrixIn->M[1] * MatrixIn2->M[7]) + (MatrixIn->M[2] * MatrixIn2->M[11]) + (MatrixIn->M[3] * MatrixIn2->M[15]);

	M[4] = (MatrixIn->M[4] * MatrixIn2->M[0]) + (MatrixIn->M[5] * MatrixIn2->M[4]) + (MatrixIn->M[6] * MatrixIn2->M[8]) + (MatrixIn->M[7] * MatrixIn2->M[12]);
	M[5] = (MatrixIn->M[4] * MatrixIn2->M[1]) + (MatrixIn->M[5] * MatrixIn2->M[5]) + (MatrixIn->M[6] * MatrixIn2->M[9]) + (MatrixIn->M[7] * MatrixIn2->M[13]);
	M[6] = (MatrixIn->M[4] * MatrixIn2->M[2]) + (MatrixIn->M[5] * MatrixIn2->M[6]) + (MatrixIn->M[6] * MatrixIn2->M[10]) + (MatrixIn->M[7] * MatrixIn2->M[14]);
	M[7] = (MatrixIn->M[4] * MatrixIn2->M[3]) + (MatrixIn->M[5] * MatrixIn2->M[7]) + (MatrixIn->M[6] * MatrixIn2->M[11]) + (MatrixIn->M[7] * MatrixIn2->M[15]);

	M[8] = (MatrixIn->M[8] * MatrixIn2->M[0]) + (MatrixIn->M[9] * MatrixIn2->M[4]) + (MatrixIn->M[10] * MatrixIn2->M[8]) + (MatrixIn->M[11] * MatrixIn2->M[12]);
	M[9] = (MatrixIn->M[8] * MatrixIn2->M[1]) + (MatrixIn->M[9] * MatrixIn2->M[5]) + (MatrixIn->M[10] * MatrixIn2->M[9]) + (MatrixIn->M[11] * MatrixIn2->M[13]);
	M[10] = (MatrixIn->M[8] * MatrixIn2->M[2]) + (MatrixIn->M[9] * MatrixIn2->M[6]) + (MatrixIn->M[10] * MatrixIn2->M[10]) + (MatrixIn->M[11] * MatrixIn2->M[14]);
	M[11] = (MatrixIn->M[8] * MatrixIn2->M[3]) + (MatrixIn->M[9] * MatrixIn2->M[7]) + (MatrixIn->M[10] * MatrixIn2->M[11]) + (MatrixIn->M[11] * MatrixIn2->M[15]);

	M[12] = (MatrixIn->M[12] * MatrixIn2->M[0]) + (MatrixIn->M[13] * MatrixIn2->M[4]) + (MatrixIn->M[14] * MatrixIn2->M[8]) + (MatrixIn->M[15] * MatrixIn2->M[12]);
	M[13] = (MatrixIn->M[12] * MatrixIn2->M[1]) + (MatrixIn->M[13] * MatrixIn2->M[5]) + (MatrixIn->M[14] * MatrixIn2->M[9]) + (MatrixIn->M[15] * MatrixIn2->M[13]);
	M[14] = (MatrixIn->M[12] * MatrixIn2->M[2]) + (MatrixIn->M[13] * MatrixIn2->M[6]) + (MatrixIn->M[14] * MatrixIn2->M[10]) + (MatrixIn->M[15] * MatrixIn2->M[14]);
	M[15] = (MatrixIn->M[12] * MatrixIn2->M[3]) + (MatrixIn->M[13] * MatrixIn2->M[7]) + (MatrixIn->M[14] * MatrixIn2->M[11]) + (MatrixIn->M[15] * MatrixIn2->M[15]);
}



/*****************************
*	Transpose this Matrix	 *
*****************************/

void Matrix::Transpose(Matrix* MatrixOut)
{
	MatrixOut->M[0] = M[0];
	MatrixOut->M[4] = M[1];
	MatrixOut->M[8] = M[2];
	MatrixOut->M[12] = M[3];

	MatrixOut->M[1] = M[4];
	MatrixOut->M[5] = M[5];
	MatrixOut->M[9] = M[6];
	MatrixOut->M[13] = M[7];

	MatrixOut->M[2] = M[8];
	MatrixOut->M[6] = M[9];
	MatrixOut->M[10] = M[10];
	MatrixOut->M[14] = M[11];

	MatrixOut->M[3] = M[12];
	MatrixOut->M[7] = M[13];
	MatrixOut->M[11] = M[14];
	MatrixOut->M[15] = M[15];
}


/*****************************************
*	Set up Rotation, Location and Scale	 *
*****************************************/

void Matrix::SetRotLocSca(Vector* RotationIn, Vector* LocationIn, Vector* ScaleIn)
{
	Matrix TeMpMatrix;
	Matrix TeMpMatrix2;

	Matrix MatrixX;
	Matrix MatrixY;
	Matrix MatrixZ;
	Matrix Scale;

	MatrixX.SetXRotation(RotationIn->X);
	MatrixY.SetYRotation(RotationIn->Y);
	MatrixZ.SetZRotation(RotationIn->Z);
	Scale.SetScale(ScaleIn);

	SetIdentity();									// set this Matrix to be identity
	TeMpMatrix.MultiplyMatrix(&MatrixX, &MatrixY);
	TeMpMatrix2.MultiplyMatrix(&TeMpMatrix, &MatrixZ);

	MultiplyMatrix(&TeMpMatrix2, &Scale);				// result goes into 'this' Matrix
	SetTranslation(LocationIn);
}




/*****************************************
*	Set up Rotation, Location and Scale	 *
*****************************************/

void Matrix::SetRollingMatrix(Vector* RotationIn, Vector* LocationIn)
{
	Matrix TempMatrix;
	Matrix MatrixX;
	Matrix MatrixY;

	MatrixX.SetXRotation(RotationIn->X);
	MatrixY.SetYRotation(RotationIn->Y);

	SetIdentity();											// set this Matrix to be identity
	MultiplyMatrix(&MatrixY, &MatrixX);
	SetTranslation(LocationIn);
}




/****************************************************************************
	Projection Matrix...
	Cheers Jase!!!
**************************************************************************** */

void Matrix::SetProjMatrix( int w, int h, float Near, float Far, float FOV )
{
	float cotangent, rcp;
	float Aspect;


	Aspect = (GLfloat)h/(GLfloat)w;

//	float FOV = 1.0f / Tan(m_fovy / m_zoom * 0.5f);

	float cot = 1.0f / (float)tan(FOV * 3.14159265359f / 360.0f);		//(FOV * 0.5f);
	cotangent = 1.0f / (float)tan(FOV * 0.5f * 0.01746031746f);		//(FOV * 3.14159265359f / 360.0f);		//(FOV * 0.5f);
	rcp = 1.0f / (Far - Near);

	M[0] = cotangent;					//aspect_ratio;
	M[1] = 0.0f;
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = cotangent / Aspect;
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[8] = 0.0f;
	M[9] = 0.0f;
	M[10] = rcp * (Far + Near);
	M[11] = -1.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = rcp * 2.0f * Far * Near;
	M[15] = 0.0f;
}


void Matrix::OGLProjMatrix( int w, int h, float Near, float Far, float FOV )
{
	float cotangent, rcp;
	float Aspect;

	Aspect = (GLfloat)h/(GLfloat)w;

	float cot = 1.0f / (float)tan(FOV * 3.14159265359f / 360.0f);		//(FOV * 0.5f);
	cotangent = 1.0f / (float)tan(FOV * 0.5f * 0.01746031746f);		//(FOV * 3.14159265359f / 360.0f);		//(FOV * 0.5f);
	rcp = 1.0f / (Near - Far);

	M[0] = cotangent;					//aspect_ratio;
	M[1] = 0.0f;
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = cotangent / Aspect;
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[8] = 0.0f;
	M[9] = 0.0f;
	M[10] = rcp * (Far + Near);
	M[11] = -1.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = -rcp * 2.0f * Far * Near;
	M[15] = 0.0f;
}


void Matrix::SetRotate( Vector *Axis, float angle )
{
float half;
float hsin,hcos;
float qx,qy,qz,qw;
float xs, ys, zs;
float wx, wy, wz;
float xx, xy, xz;
float yy, yz, zz;

	SetIdentity();

	half = angle * 0.5f;
	hsin = (float)sin(half);
	hcos = (float)cos(half);

	qx=Axis->X*hsin;
	qy=Axis->Y*hsin;
	qz=Axis->Z*hsin;
	qw=hcos;

	xs = qx + qx;
	ys = qy + qy;
	zs = qz + qz;

	wx = qw * xs;
	wy = qw * ys;
	wz = qw * zs;

	xx = qx * xs;
	xy = qx * ys;
	xz = qx * zs;

	yy = qy * ys;
	yz = qy * zs;
	zz = qz * zs;

	M[0] = 1.0f - (yy + zz);
	M[1] = xy + wz;
	M[2] = xz - wy;

	M[4] = xy - wz;
	M[5] = 1.0f - (xx + zz);
	M[6] = yz + wx;

	M[8] = xz + wy;
	M[9] = yz - wx;
	M[10] = 1.0f - (xx + yy);
}








