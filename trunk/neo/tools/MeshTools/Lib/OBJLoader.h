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


#ifndef OBJLOADER
#define OBJLOADER

#include "../LSCM/MeshBuilderStdafx.h"
#include "RKMesh.h"



class OBJLoader
{
	friend class RKMesh;

public:
	OBJLoader() 
	{
		//KA 30/3/07: No longer needed
		//pLoadBuffer = NULL;
	};

	~OBJLoader() 
	{
		//KA 30/3/07: No longer needed
		//if(pLoadBuffer != NULL) delete[] pLoadBuffer;
		//pLoadBuffer = NULL;
	};

	bool Load(char* FileName);
	bool Save(char* FileOut, char* FileIn);

private:
	bool Count(char* FileName);

	// KA 30/3/07: No longer needed
	//bool ReadWholeFile(char* FileName);

	//void ReadFace(FILE* FileHandle);
	//void ReadVertex(FILE* FileHandle);
	//void ReadNormal(FILE* FileHandle);
	//void ReadUV(FILE* FileHandle);

	// KA 30/3/07:
	// utility methods for Load rewrite
	bool ReadLine( FILE *, char* );
	bool isV( char * );
	bool isVT( char* );
	bool isVN( char* );
	bool isF( char* );
	char *NextToken( char* );
	bool isWhite( char );

// KA 30/3/07: No longer needed
//	void Copy(FILE* OutFileHandle);
//	void OverwriteUVs(FILE* InFileHandle);
//	bool CopyFaceNewUV(FILE* OutFileHandle, int FaceIndex);

	//bool GoodData(char InChar);
	//bool NumericalData(char InChar);

	//unsigned char* pLoadBuffer;
	int m_InFileIndex;
	int m_InFileSize;


	bool m_NoNormalsIn;
	bool m_NoUVsIn;
};



#endif		// OBJLOADER