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


#ifndef RKEDGE
#define RKEDGE

#include "../LSCM/MeshBuilderStdafx.h"


class RKEdge
{
public:
	RKEdge()
	{
		m_VertexIndex1 = 0;
		m_VertexIndex2 = 0;
		m_Selected = 0;
		m_Visited = false;
	}

	~RKEdge()
	{
		m_VertexIndex1 = 0;
		m_VertexIndex2 = 0;
		m_Selected = 0;
		m_Visited = false;
	}

	int m_VertexIndex1;
	int m_VertexIndex2;
	int m_Selected;

	bool m_FrontFacing;
	bool m_Visited;

	RKEdge &operator =( RKEdge &VIn )
	{
		m_VertexIndex1 = VIn.m_VertexIndex1;
		m_VertexIndex2 = VIn.m_VertexIndex2;
		m_Selected = VIn.m_Selected;

		return *this;
	}


	bool SameAs(int VertexIndexIn1, int VertexIndexIn2)
	{
		if(m_VertexIndex1 == VertexIndexIn1 && m_VertexIndex2 == VertexIndexIn2) return(true);
		if(m_VertexIndex1 == VertexIndexIn2 && m_VertexIndex2 == VertexIndexIn1) return(true);
		return(false);
	}
};



class RKUV
{
public:
	RKUV()
	{
		m_U = 0.0f;
		m_V = 0.0f;
		m_Selected = 0;
	}

	~RKUV()
	{
		m_U = 0.0f;
		m_V = 0.0f;
		m_Selected = 0;
	}

	RKUV &operator =( RKUV &VIn )
	{
		m_U = VIn.m_U;
		m_V = VIn.m_V;
		m_Selected = VIn.m_Selected;

		return *this;
	}

	float m_U;
	float m_V;
	int m_Selected;
};

#endif