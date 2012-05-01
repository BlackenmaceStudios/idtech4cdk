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


#ifndef VECTOR_INCLUDED
#define VECTOR_INCLUDED
/* *************************************************************************************************************************************
	Basic 4x1 Matrix definition (a vector)
************************************************************************************************************************************** */
#ifdef NOISY_COMPILATION
#pragma message("Entering Vector.h")
#endif
/* *************************************************************************************************************************************
	Headers	
************************************************************************************************************************************** */

#include <math.h>
#include "Vertex.h"


/* *************************************************************************************************************************************
	Defines
************************************************************************************************************************************** */

class Vertex;

/* *************************************************************************************************************************************
	Class definitions
************************************************************************************************************************************** */
class Vector
{
	friend class Matrix;												// Don't fuck about with set, get when we're doing heavy duty maths

public:

	float	X, Y, Z, W;													// Data to save


	/* Con/Destruction */
	Vector(){};
	Vector(float XIn, float YIn, float ZIn, float WIn)
	{
		X = XIn; Y = YIn; Z = ZIn; W = WIn;
	}
	~Vector(){};


	/* Overiden new and delete */

	inline Vector operator -( Vector &VIn )
	{
//		Vector Ret;
//		Ret.X	= X - VIn.X;
//		Ret.Y	= Y - VIn.Y;
//		Ret.Z	= Z - VIn.Z;
		return (Vector(X - VIn.X, Y - VIn.Y, Z - VIn.Z, 1.0f));
	}


	/* General functions */
	void SetVector(Vector* VectorIn);
	void SetVector( Vector& VectorIn );
	void SetVector( float x, float y, float z, float w );

	float Magnitude();
	void Normalise( void );
	void Negate( void );
	bool Compare(Vector* VectorIn);
	bool Compare( Vector& VectorIn );

	void	CrossProduct( Vector &In, Vector &Out );
	float	DotProduct( Vector &In );
	float	DotProduct( Vertex &In );
	float	DotProduct( float x, float y, float z );
	void	InvertZ( void );

	/* Overiden operators */

	void operator -=( Vector In )
	{
		X	-= In.X;
		Y	-= In.Y;
		Z	-= In.Z;
	}

	void operator +=( Vector In )
	{
		X	+= In.X;
		Y	+= In.Y;
		Z	+= In.Z;
	}


	Vector operator =( Vector In )
	{
		Vector Ret;
		Ret.X	= In.X;
		Ret.Y	= In.Y;
		Ret.Z	= In.Z;
		return Ret;
	}



	Vector operator-( Vertex *In )
	{
		Vector Ret;
		Ret.X	= X - In->X;
		Ret.Y	= Y - In->Y;
		Ret.Z	= Z - In->Z;
		return Ret;
	}

	Vector operator-( Vertex In )
	{
		Vector Ret;
		Ret.X	= X - In.X;
		Ret.Y	= Y - In.Y;
		Ret.Z	= Z - In.Z;
		return Ret;
	}

	Vector operator-( Vector *In )
	{
		Vector Ret;
		Ret.X	= X - In->X;
		Ret.Y	= Y - In->Y;
		Ret.Z	= Z - In->Z;
		return Ret;
	}



	/* Access functions */
	float x( void )
	{
		return X;
	}

	float y( void )
	{
		return Y;
	}

	float z( void )
	{
		return Z;
	}
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
#ifdef NOISY_COMPILATION
#pragma message("Leaving Vector.h")
#endif
#endif //VECTOR_INCLUDED
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */	
