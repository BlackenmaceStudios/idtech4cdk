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


#ifndef MATRIX_INCLUDED
#define MATRIX_INCLUDED
/* *************************************************************************************************************************************
	Basic 4x4 Matrix definition

	Comments
		OGL uses column major matricies e.g:
			r1 r5 r9  t13
			r2 r6 r10 t14
			r3 r7 r11 t15
			w4 w8 r12 t16

		Class is currently wrapper for OGL functionality, requires all maths functions defining in software so we can
		port more eaily to PS II

		OGL uses an axis of rotation and an angle to specify it's rotations (angular displacement).  Quaternions......

		Thought::  Easy way to compare our functions is to multiply an identity matrix with correct matrix that OGL has at 
		top of stack
************************************************************************************************************************************** */

/* *************************************************************************************************************************************
	Headers	
************************************************************************************************************************************** */
#include "../LSCM/MeshBuilderStdafx.h"

#include <math.h>
//#include "gl\gl.h"
#include "Vector.h"
#include "Vertex.h"

/* *************************************************************************************************************************************
	Debugging stuff
************************************************************************************************************************************** */
#ifdef NOISY_COMPILATION
#pragma message("Entering Matrix.h")
#endif


/* *************************************************************************************************************************************
	Class definitions
************************************************************************************************************************************** */
class Matrix
{

public:

	float	M[16];

	/* Con/Destruction */
	Matrix(){};
	~Matrix(){};


	/* General functions */
	void SetIdentity(void);
	void SetMatrix(Matrix* MatrixIn);
	void SetXRotation(float Angle);
	void SetYRotation(float Angle);
	void SetZRotation(float Angle);
	void SetRotate( Vector *Axis, float angle );
	void SetTranslation(Vector* Translation );

	void SetTranslation(float XIn, float YIn, float ZIn)
	{
		M[12] = XIn;
		M[13] = YIn;
		M[14] = ZIn;
	}


	void RotateVertex(Vertex* VertexIn, Vertex* VertexOut);
	void RotTransVertex(Vertex* VertexIn, Vertex* VertexOut);
	void RotateVector(Vector* VectorIn, Vector* VectorOut);
	void RotTransVector(Vector* VectorIn, Vector* VectorOut);
	void MultiplyMatrix(Matrix* MatrixIn1, Matrix* MatrixIn2);
	void Transpose(Matrix* MatrixOut);
	void SetScale(Vector* Scale);
	void Specular(Matrix* MatrixOut);

	void SetRotLocSca(Vector* RotationIn, Vector* LocationIn, Vector* ScaleIn);
	void SetRollingMatrix(Vector* RotationIn, Vector* LocationIn);

	void SetProjMatrix( int w, int h, float Near, float Far, float FOV );

	void OGLProjMatrix( int w, int h, float Near, float Far, float FOV );



	void Matrix::Inv( Matrix *Mat1 );
	float	Matrix::Det4x4( void );
	void	Matrix::Adjoint( void );
	float	Matrix::det2x2(float a, float b, float c, float d);
	float	Matrix::det3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);

	//void AngularDisplacement( Vector Axis, float angle );
};

inline float Matrix::det2x2(float a, float b, float c, float d)
{
	return (a * d - b * c);
}

inline float Matrix::det3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3)
{
	return ( (a1 * det2x2(b2, b3, c2, c3)) - (b1 * det2x2(a2, a3, c2, c3)) + (c1 * det2x2(a2, a3, b2, b3)) );
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
#ifdef NOISY_COMPILATION
#pragma message("Exiting Matrix.h")
#endif
#endif //MATRIX_INCLUDED
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */	

