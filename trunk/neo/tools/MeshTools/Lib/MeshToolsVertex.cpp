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

#include "Vertex.h"
#include "Vector.h"


/* *************************************************************************************************************************************
	Vertex type

	Description:
		Point in 3d space + manipulation functions

		More terrible 10 year old code..  so sorry.

************************************************************************************************************************************** */


Vertex &Vertex::operator -( Vector &VIn )
{

	X	-= VIn.X;
	Y	-= VIn.Y;
	Z	-= VIn.Z;

	return *this;
}

void Vertex::Sub( Vector &VIn, Vector &VOut )
{
	VOut.X	= X - VIn.X;
	VOut.Y	= Y - VIn.Y;
	VOut.Z	= Z - VIn.Z;
}



/********************************
*	Setup vertex information	*
********************************/

void Vertex::SetVertex(Vertex* VertexIn)
{
	X = VertexIn->X;
	Y = VertexIn->Y;
	Z = VertexIn->Z;
}



void Vertex::Scale(float ScaleVal)
{
	X *= ScaleVal;
	Y *= ScaleVal;
	Z *= ScaleVal;
}
