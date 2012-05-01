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

// Not just defines for the mouse input..  think before you name Swanny!!!  :^(


#ifndef MOUSEDEFINES
#define MOUSEDEFINES

#define RK_MOUSE_MOVE 0x1
#define RK_MOUSE_LB_DOWN 0x2
#define RK_MOUSE_LB_UP 0x4
#define RK_MOUSE_MB_DOWN 0x8
#define RK_MOUSE_MB_UP 0x10
#define RK_MOUSE_RB_DOWN 0x20
#define RK_MOUSE_RB_UP 0x40

#define RK_KEY_DOWN 0x80
#define RK_KEY_UP 0x100
#define RK_MOUSE_LB_DBLCLK 0x200



// various selections

#define RK_NO_SELECT 0x0
#define RK_EDGE_SELECT 0x1
#define RK_ROTATE 0x2
#define RK_UV_SELECT 0x3
#define RK_UV_TRANSLATE 0x4
#define RK_UV_STRETCH 0x5
#define RK_EDGE_CLOSEST 0x6
#define RK_UV_ROTATE 0x7
#define RK_EDGE_LOOP 0x8
#define RK_EDGE_TOPOLOGICAL 0x9
#define RK_MOVE_CENTREPOINT 0xA
#define RK_UV_SHELL 0xB

#define RK_SHIFT_HELD 0x10
#define RK_CTRL_HELD 0x20



// the various states of an RKEdge or RKUV

#define NOTSELECTED 1
#define SELECTED 2
#define NEWSELECTED 4
#define NEWNOTSELECTED 8
#define PINNED 16
#define EDGECUT 32
#define CLEARED 64


#define MINU 1
#define MINV 2
#define MAXU 3
#define MAXV 4




#endif