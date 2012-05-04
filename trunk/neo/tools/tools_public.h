/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __EDIT_PUBLIC_H__
#define __EDIT_PUBLIC_H__

/*
===============================================================================

	Editors.

===============================================================================
*/


class	idProgram;
class	idInterpreter;

//
// idToolInterface
//
class idToolInterface
{
public:
	virtual void	Frame( void ) = 0;
	// Radiant Level Editor
	
	virtual void	RadiantPrint( const char *text ) = 0;
	virtual void	InitTool( const toolFlag_t tool, const idDict *dict ) = 0;
	virtual bool	FrameSysMsg( void *msg ) = 0;
	virtual byte    *CompressImage( byte *uncompressedBuffer, int width, int height ) = 0;
	virtual void	ComputeUVAtlasForModel( class bmVTModel *model, int startTris, int numTris ) = 0;
	virtual int		GetValueFromManagedEnum(const char * enumTypeStr, const char * enumValStr) = 0;
	virtual void	ShowDebugConsole( void ) = 0;
};

typedef idToolInterface * (*ToolsAPI_Init_t)( bmEngineClassExport *engineExport, void *winVars );

extern idToolInterface *toolInterface;

#endif /* !__EDIT_PUBLIC_H__ */
