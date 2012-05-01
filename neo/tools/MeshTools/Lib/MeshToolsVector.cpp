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


#include "../LSCM/MeshBuilderStdafx.h"

#include "Vector.h"
#include "Vertex.h"

/* *************************************************************************************************************************************
	Vector type

	Description:
		Point in 3d space + manipulation functions


	More terrible 10 year old code..  so sorry.

************************************************************************************************************************************** */



/********************************
*	Cross product				*
********************************/

void Vector::CrossProduct( Vector &In, Vector &Out )
{
	Out.X	= Y*In.Z - In.Y*Z;
	Out.Y	= X*In.Z - In.X*Z;
	Out.Z	= X*In.Y - In.X*Y;
}

/********************************
*	Dot product					*
********************************/

float Vector::DotProduct( Vector &In )
{
	float	Answer;
	Answer	= ( ( X * In.X ) + ( Y * In.Y ) + ( Z * In.Z ) );
	return( Answer );
}

/********************************
*	Dot product					*
********************************/
float Vector::DotProduct( float x, float y, float z )
{
	float	Answer;
	Answer	= ( ( X * x ) + ( Y * y ) + ( Z * z ) );
	return( Answer );
}


/********************************
*	Dot product					*
********************************/
float Vector::DotProduct( Vertex &In )
{
	float	Answer;
	Answer	= ( ( X * In.X ) + ( Y * In.Y ) + ( Z * In.Z ) );
	return( Answer );
}

/********************************
*	Setup vector information	*
********************************/

void Vector::SetVector(Vector* VectorIn)
{
	X = VectorIn->X;
	Y = VectorIn->Y;
	Z = VectorIn->Z;
	W = VectorIn->W;
}

/********************************
*	Setup vector information	*
********************************/
void Vector::SetVector(Vector& VectorIn)
{
	X = VectorIn.X;
	Y = VectorIn.Y;
	Z = VectorIn.Z;
	W = VectorIn.W;
}


/********************************
*	Setup vector information	*
********************************/
void Vector::SetVector( float x, float y, float z, float w )
{

	X	= x;
	Y	= y;
	Z	= z;
	W	= w;
}


float Vector::Magnitude()
{
	float Length;

	Length = (X * X) + (Y * Y) + (Z * Z);			//  Only work on 3d displacement
	Length = (float)sqrt(Length);

	return(Length);
}


/****************************
*	Normalise the Vector	*
****************************/
void Vector::Normalise( void )
{
	float Length;

	Length = (X * X) + (Y * Y) + (Z * Z);			//  Only work on 3d displacement
	Length = (float)sqrt(Length);

	X /= Length;
	Y /= Length;
	Z /= Length;

	/* For now */

	W	= 1;
}

void Vector::InvertZ( void )
{
	Z	= -Z;
}

/****************************
*	 V = -V					*
****************************/
void Vector::Negate( void )
{
	X	= -X;
	Y	= -Y;
	Z	= -Z;

	/* Leave W alone, for now */
}


/****************************
*	 Compare the Vector		*
****************************/
bool Vector::Compare(Vector* VectorIn)
{
	if(VectorIn->X != X) return false;
	if(VectorIn->Y != Y) return false;
	if(VectorIn->Z != Z) return false;

	return true;
}


/****************************
*	 Compare the Vector		*
****************************/
bool Vector::Compare(Vector& VectorIn)
{
	if(VectorIn.X != X) return false;
	if(VectorIn.Y != Y) return false;
	if(VectorIn.Z != Z) return false;

	return true;
}