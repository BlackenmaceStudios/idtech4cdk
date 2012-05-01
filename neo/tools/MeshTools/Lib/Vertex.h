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


#ifndef VERTEX_INCLUDED
#define VERTEX_INCLUDED
/* *************************************************************************************************************************************
	Point in 3d space 
************************************************************************************************************************************** */
#ifdef NOISY_COMPILATION
#pragma message("Entering Vertex.h")
#endif
/* *************************************************************************************************************************************
	Headers	
************************************************************************************************************************************** */
class Vector;

/* *************************************************************************************************************************************
	Class definitions
************************************************************************************************************************************** */
class Vertex
{
public:

	float	X, Y, Z;								// Data to save


	/* Con/Destruction */
	Vertex(){};
	~Vertex(){};

	/* Read & write functions */

	void Scale(float ScaleVal);

	void SetVertex(Vertex* VertexIn);
	void SetVertex(float XIn, float YIn, float ZIn)
	{
		X = XIn;
		Y = YIn;
		Z = ZIn;
	};

	void Multiply(float Mult)
	{
		X *= Mult;
		Y *= Mult;
		Z *= Mult;
	}

	void operator +=( Vertex In )
	{
		X	+= In.X;
		Y	+= In.Y;
		Z	+= In.Z;
	}

	/* Overiden operators */

	void Sub( Vector &VIn, Vector &VOut );
	Vertex &operator -( Vector &VIn );


	Vertex &operator =( Vertex &VIn )
	{
		X = VIn.X;
		Y = VIn.Y;
		Z = VIn.Z;

//		memcpy( this, &VIn, sizeof( Vertex ) );
		return *this;
	}
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
#ifdef NOISY_COMPILATION
#pragma message("Leaving Vertex.h")
#endif
#endif //VERTEX_INCLUDED
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */	


