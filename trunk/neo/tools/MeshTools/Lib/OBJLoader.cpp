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
#include <stdio.h>

#include "OBJLoader.h"

#include "assert.h"

/* *************************************************************************************************************************************
	.OBJ loader functions

	Description:
		Loads a mesh saved from an art package in the .OBJ format

************************************************************************************************************************************** */

//KA 30/3/07: OBJLoader:: Load() rewrite using ascii reads. How much easier is this!! :-)
//
//
// if ( LINELEN > 100 ) { get->Animator->Slap() }
#define LINELEN 100

bool OBJLoader::Load(char* FileName)	 //{{{
{

	char Line[LINELEN];
	char tmp[10];


	RKMesh::Get()->Delete();

	if(Count(FileName) == false) return(false);

	FILE* FileHandle;
	FileHandle = fopen(FileName, "r");
	if(FileHandle == NULL) return(false);

	// Line at a time read. Comments, etc stripped by Readline
	while ( ReadLine( FileHandle, Line ) ) {

		// it's a vertex
		if ( isV( Line ) ) {
			float x,y,z;
			sscanf( Line, "%s %f %f %f", tmp, &x, &y, &z );
			RKMesh::Get()->AddVertex(x, y, z);

		}
		// it's a UV
		if ( isVT( Line ) ) { // texture coordinate. may not be data here.
			float u,v;
			sscanf( Line, "%s %f %f", tmp, &u, &v );
			RKMesh::Get()->AddUV(u, v);
		}
		// it's a normal
		if ( isVN( Line ) ) {
			float x,y,z;
			sscanf( Line, "%s %f %f %f", tmp, &x, &y, &z );
			RKMesh::Get()->AddNormal(x, y, z);

		}
		//It's a face
		if ( isF( Line ) ) {

			char *p = Line;
			
			int ivList[100], ivtList[100], ivnList[100]; //if (sizeof( face ) > 100) get->animator->slap();
			int iIndices = 0;
			
			while ( p = NextToken( p ) ) {
				// ToDo: !!!!!!!!!!!!!  There's no reason there should be normals there.
				// but mostly they are,
				// it's messy if we have to cater for them not being here
				// unless I can think of a better way to do this.
				//
				// no UVs
				if ( m_NoUVsIn ) {
					sscanf( p, "%d//%d", &ivList[iIndices], &ivnList[iIndices] );
					ivtList[iIndices] = 0;
					ivnList[iIndices]--;

				// yes, uvs in the list.	
				} else {
					sscanf( p, "%d/%d/%d", &ivList[iIndices], &ivtList[iIndices], &ivnList[iIndices] );
					ivnList[iIndices]--;
					ivtList[iIndices]--;

				}

				iIndices++;
			}
			RKMesh::Get()->AddFace(iIndices, ivList, ivnList, ivtList );
			
		}
	}
	if(RKMesh::Get()->m_NumberOfUVs == 0)
	{
		RKMesh::Get()->AddUV(0.0f, 0.0f);
	}


	if(RKMesh::Get()->m_NumberOfNormals == 0)
	{
		m_NoNormalsIn = true;
		RKMesh::Get()->CreateNormals();
	}
	else
	{
		m_NoNormalsIn = false;
	}
	RKMesh::Get()->FinaliseMesh();

	return true;

} //}}}
// KA 30/3/07: Read a line of text, stripping comments and trailing new lines.
bool OBJLoader:: ReadLine ( FILE* fp, char *Line )  //{{{
{

	
	if ( fgets( Line, LINELEN, fp ) == NULL ) return false;
	char *p = Line;
	// strip comments
	//
	for ( p = Line; *p; p++ ) {
		if ( *p == '#' ) {
			*p = '\0';
			break;
		}
	}
	while( p[strlen(p)-1] == '\n' || p[strlen(p)-1] == '\r' )
		p[strlen(p)-1] = '\0';

	// if Line is zero length, get next line
	// Recursively.
	if ( *Line == '\0' )
		return ReadLine( fp, Line );

	return true;
} //}}}

// KA 30/3/07: methods to check face, texture, normal or vertex coords
// {{{
bool OBJLoader::isV( char *Line )
{
	char *p = Line;

	while ( isWhite( *p ) )
		p++;

	return (( *p == 'v' && (*(p+1)==' ' || *(p+1)=='\t') ) ? true : false );

}
bool OBJLoader::isVT( char *Line )
{
	char *p = Line;

	while ( isWhite( *p ) )
		p++;

	return (( *p == 'v' && *(p+1) == 't' ) ? true : false );
}
bool OBJLoader::isVN( char *Line )
{
	char *p = Line;

	while ( isWhite( *p ) )
		p++;

	return (( *p == 'v' && *(p+1) == 'n' ) ? true : false );
}
bool OBJLoader::isF( char *Line )
{
	char *p = Line;

	while ( isWhite( *p ) )
		p++;

	return (( *p == 'f' ) ? true : false );
}
//}}}
// KA 30/3/07: return pointer to next item in a line, whitespace delimitted
// assume some kind of token at start of line and skip it.
// so we should always be getting a string of the form n/<n>/n. 
//
char* OBJLoader::NextToken( char *Line ) //{{{
{

	char *p = Line;

	// skip any leading whitespace
	while ( *p && isWhite( *p ) )
		p++;
	// end of string?
	if ( ! *p ) 
		return (char*)NULL;

	// now skip over non-whitespace to get to the next token.
	while ( *p && !isWhite( *p ) )
		p++;

	if ( ! *p ) 
		return (char*)NULL;

	// and finally, skip the leading whitespace to bring us to the token

	while ( *p && isWhite( *p ) )
		p++;
	// end of string?
	if ( ! *p ) 
		return (char*)NULL;

	return p;

	

}
bool OBJLoader::isWhite( char c ) 
{
	if ( c == '\t' ) return true;
	if ( c == ' ' ) return true;
	return false;
} //}}}

/* // OLD code
//{{{
bool OBJLoader::Load(char* FileName)	//, RKMesh* pRKMesh)
{
	char buffer[3];
	buffer[2] = 0;

	RKMesh::Get()->Delete();

	if(Count(FileName) == false) return(false);


	bool RedVal = ReadWholeFile(FileName);

	FILE* FileHandle;
	FileHandle = fopen(FileName, "rb");
	if(FileHandle == NULL) return(false);




	fread(&buffer[0], sizeof(char), 1, FileHandle);
	

	while(true)
	{
		int Size = (int)fread(&buffer[1], sizeof(char), 1, FileHandle);
		if(Size == 0) break;

		if(strcmp(buffer, "v ") == 0) 
		{
			ReadVertex(FileHandle);
			fread(&buffer[1], sizeof(char), 1, FileHandle);
		}

		if(strcmp(buffer, "f ") == 0) 
		{
			ReadFace(FileHandle);
			fread(&buffer[1], sizeof(char), 1, FileHandle);
		}

		if(strcmp(buffer, "vt") == 0) 
		{
			ReadUV(FileHandle);
			fread(&buffer[1], sizeof(char), 1, FileHandle);
		}

		if(strcmp(buffer, "vn") == 0) 
		{
			ReadNormal(FileHandle);
			fread(&buffer[1], sizeof(char), 1, FileHandle);
		}

		buffer[0] = buffer[1];
	}

	fclose(FileHandle);


	if(RKMesh::Get()->m_NumberOfUVs == 0)
	{
		RKMesh::Get()->AddUV(0.0f, 0.0f);
	}


	if(RKMesh::Get()->m_NumberOfNormals == 0)
	{
		m_NoNormalsIn = true;
		RKMesh::Get()->CreateNormals();
	}
	else
	{
		m_NoNormalsIn = false;
	}

	RKMesh::Get()->FinaliseMesh();

	return(true);
}

*/

// KA 30/3/07: No longer needed
/* 

void OBJLoader::ReadVertex(FILE* FileHandle)			//, RKMesh* pRKMesh)
{
	float X, Y, Z;
	char Number[20];
	int Index;

	bool NumberFound = false;

	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a Vertex!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == ' ' && NumberFound == true) break;
		Index++;
	}
	Number[Index] = 0;
	X = (float)atof(Number);


	NumberFound = false;
	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a Vertex!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == ' ' && NumberFound == true) break;
		Index++;
	}
	Number[Index] = 0;
	Y = (float)atof(Number);


	NumberFound = false;
	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a Vertex!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == '\n' && NumberFound == true) break;
		Index++;
	}
	Number[Index] = 0;
	Z = (float)atof(Number);

	RKMesh::Get()->AddVertex(X, Y, Z);
}


void OBJLoader::ReadNormal(FILE* FileHandle)			//, RKMesh* pRKMesh)
{
	float X, Y, Z;
	char Number[20];
	int Index;

	bool NumberFound = false;

	fread(&Number[0], sizeof(char), 1, FileHandle);		// skip the space
	if(GoodData(Number[0]) == false) return;			// found something that's not really a Normal!

	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a Normal!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == ' ' && NumberFound == true) break;
		Index++;
	}
	Number[Index] = 0;
	X = (float)atof(Number);


	NumberFound = false;
	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a Normal!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == ' ' && NumberFound == true) break;
		Index++;
	}
	Number[Index] = 0;
	Y = (float)atof(Number);


	NumberFound = false;
	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a Normal!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == '\n' && NumberFound == true) break;
		Index++;
	}
	Number[Index] = 0;
	Z = (float)atof(Number);

	float One = (X*X) + (Y*Y) + (Z*Z);

	RKMesh::Get()->AddNormal(X, Y, Z);

}



void OBJLoader::ReadUV(FILE* FileHandle)		//, RKMesh* pRKMesh)
{
	float X, Y;
	char Number[20];
	int Index;
	bool NumberFound = false;

	fread(&Number[0], sizeof(char), 1, FileHandle);		// skip the space
	if(GoodData(Number[0]) == false) return;			// found something that's not really a UV!

	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a UV!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == ' ') break;
		Index++;
	}
	Number[Index] = 0;
	X = (float)atof(Number);


	NumberFound = false;
	Index = 0;
	Number[0] = 0;
	while(1)
	{
		fread(&Number[Index], sizeof(char), 1, FileHandle);
		if(GoodData(Number[Index]) == false) return;			// found something that's not really a UV!
		if(NumericalData(Number[Index]) == true) NumberFound = true;
		if(Number[Index] == '\n') break;
		if(Number[Index] == 0xa) break;
		if(Number[Index] == 0xd) break;
		if(Number[Index] == ' ') break;
		Index++;
	}
	Number[Index] = 0;
	Y = (float)atof(Number);


	RKMesh::Get()->AddUV(X, Y);
}



void OBJLoader::ReadFace(FILE* FileHandle)			//, RKMesh* pRKMesh)
{
	int Index;
	bool FaceDone = false;
	char Number[20];
	int NumberIndices = 0;
	int VertexIndices[100];				// more that 100 verts?  I'll come over there and slap you!   ;^)
	int NormalIndices[100];	
	int UVIndices[100];	

	int VertUVNorm;

	while(!FaceDone)
	{
		Index = 0;
		Number[0] = 0;
		VertUVNorm = 0;

		NormalIndices[NumberIndices] = 0;			//-1;
		VertexIndices[NumberIndices] = -1;
		UVIndices[NumberIndices] = 0;		//-1;

		while(1)
		{
			fread(&Number[Index], sizeof(char), 1, FileHandle);

			if(GoodData(Number[Index]) == false) return;			// found something that's not really a face!

			if(Number[Index] == ' ') break;
			if(Number[Index] == '\n') {FaceDone = true;	break;}
			if(Number[Index] == '/')
			{
				if(VertUVNorm == 0)
				{
					Number[Index] = 0;
					int Value = atoi(Number);
					VertexIndices[NumberIndices] = Value;
					VertUVNorm = 1;
					if(RKMesh::Get()->m_NumberOfNormals == 0) VertUVNorm = 2;
					Index = -1;
				}
				else
				{
					if(Index != 0)
					{
						Number[Index] = 0;
						int Value = atoi(Number);
						UVIndices[NumberIndices] = Value-1;
					}
					VertUVNorm = 2;
					Index = -1;
				}
			}
			Index++;
		}

		Number[Index] = 0;
		int Value = atoi(Number);
		if(Index == 0 || Value == 0) goto BodgeOTron;

		if(VertexIndices[NumberIndices] == -1) VertexIndices[NumberIndices] = Value;

		if(RKMesh::Get()->m_NumberOfNormals == 0)
		{
			if(m_NoUVsIn == true)
			{
				UVIndices[NumberIndices] = 0;
				NormalIndices[NumberIndices] = 0;
			}
			else
			{
				NormalIndices[NumberIndices] = 0;
				UVIndices[NumberIndices] = Value-1;
			}
		}
		else
		{
			NormalIndices[NumberIndices] = Value-1;
			if(m_NoUVsIn == true) UVIndices[NumberIndices] = 0;
		}

		if(VertexIndices[NumberIndices] == 0 || NormalIndices[NumberIndices] == -1 || UVIndices[NumberIndices] == -1)
		{
			int a = 0;
			a++;
		}
		else
		{
			NumberIndices++;
		}

		assert(NumberIndices < 100);				// face with 100 vertices!!!  Go and have a re-think!
	}

BodgeOTron:											// dear me, this is awful   :^(

	if(NumberIndices > 2)
	{
		RKMesh::Get()->AddFace(NumberIndices, VertexIndices, NormalIndices, UVIndices);
	}
}

//}}}
*/

bool OBJLoader::Count( char *FileName )  //{{{
{
	int NumberOfVertices = 0;
	int NumberOfFaces = 0;
	int NumberOfNormals = 0;
	int NumberOfUVs = 0;
	char Line[LINELEN];

	FILE* FileHandle;
	FileHandle = fopen(FileName, "r");
	if(FileHandle == NULL) return(false);

	while ( ReadLine( FileHandle, Line ) ) {

		if ( isV( Line ) ) NumberOfVertices++;
		if ( isVT( Line ) ) NumberOfUVs++;
		if ( isF( Line ) ) NumberOfFaces++;
		if ( isVN( Line ) ) NumberOfNormals++;

	}
	fclose( FileHandle );


	if(NumberOfUVs == 0)
	{
		RKMesh::Get()->Create(NumberOfVertices, NumberOfFaces, NumberOfNormals, 1);
		m_NoUVsIn = true;
	}
	else
	{
		RKMesh::Get()->Create(NumberOfVertices, NumberOfFaces, NumberOfNormals, NumberOfUVs);
		m_NoUVsIn = false;
	}

	if ( NumberOfNormals == 0 )
		m_NoNormalsIn = true;
	else
		m_NoNormalsIn = false;

	return true;


} //}}}


// KA 30/3/07:  Old obj reader guaranteed to fail if you have a comment line with a 'v', 'v', 'vn' or 'vt' in it.
// the latter two may be rare but the first two, I bet you could fail 5 out of 10 files with this.
// Rewrite:
// {{{
/*
bool OBJLoader::Count(char* FileName)			//, RKMesh* pRKMesh)
{
	int NumberOfVertices = 0;
	int NumberOfFaces = 0;
	int NumberOfNormals = 0;
	int NumberOfUVs = 0;

	char buffer[3];
	buffer[2] = 0;


	FILE* FileHandle;
	FileHandle = fopen(FileName, "rb");
	if(FileHandle == NULL) return(false);

	fread(&buffer[0], sizeof(char), 1, FileHandle);

	while(1)
	{
		int Size = (int)fread(&buffer[1], sizeof(char), 1, FileHandle);
		if(Size == 0) break;

		if(strcmp(buffer, "v ") == 0) NumberOfVertices++;
		if(strcmp(buffer, "f ") == 0) NumberOfFaces++;
		if(strcmp(buffer, "vn") == 0) NumberOfNormals++;
		if(strcmp(buffer, "vt") == 0) NumberOfUVs++;
		buffer[0] = buffer[1];
	}

	fclose(FileHandle);

	if(NumberOfUVs == 0)
	{
		RKMesh::Get()->Create(NumberOfVertices, NumberOfFaces, NumberOfNormals, 1);
		m_NoUVsIn = true;
	}
	else
	{
		RKMesh::Get()->Create(NumberOfVertices, NumberOfFaces, NumberOfNormals, NumberOfUVs);
		m_NoUVsIn = false;
	}

	return(true);
}
//}}}

*/
// KA 30/3/07: rewrite. 
// This all seemed a little complicated and liable to error. new version uses ascii writing
// to write the full set of data directly from Roadkill's RKMesh object
// It's simpler and a lot easier to see what's going on. 


bool OBJLoader::Save(char* FileOut, char* FileIn) //{{{
{
	FILE* OutFileHandle;
	bool UVsWritten = false;
	float U, V;

	if(FileIn == NULL) return(false);

	OutFileHandle = fopen( FileOut, "w");  // ASCII write !! 
	if(OutFileHandle == NULL) return(false);

	// Header data;
	fprintf( OutFileHandle, "# ###############################\n" );
	fprintf( OutFileHandle, "# Obj file written by RoadKill1_1\n" );
	fprintf( OutFileHandle, "# RoadKill by Andy Swann\n" );
	fprintf( OutFileHandle, "# http://www.pullin-shapes.co.uk/\n" );
	fprintf( OutFileHandle, "# ###############################\n" );

	fprintf( OutFileHandle, "g RoadKillOut\n" );
	fprintf( OutFileHandle, "o RoadKillOut\n" );

	// write out the vertex data
	// ToDo:
	// Something very odd going on here; m_NumberOfVertices returns nverts +1
	// and the first coord looks like garbage.
	// WHY!!!
	// so we're looping 1 to nverts for now, until we can figure out WTF.
	//
	fprintf( OutFileHandle, "# %d Vertex coords\n",RKMesh::Get()->m_NumberOfVertices-1 ); 
	for(long i = 1; i < RKMesh::Get()->m_NumberOfVertices; i++) {

		float X = RKMesh::Get()->pVertices[i].X;
		float Y = RKMesh::Get()->pVertices[i].Y;
		float Z = RKMesh::Get()->pVertices[i].Z;

		// RK Normalises the vertex data
		// so I've stored off the scale and offset values as
		// properties of the RKMesh class.
		// and undone the changes prior to writing.
		// 
		X *= RKMesh::Get()->m_Radius;
		Y *= RKMesh::Get()->m_Radius;
		Z *= RKMesh::Get()->m_Radius;

		X += RKMesh::Get()->m_CentreX;
		Y += RKMesh::Get()->m_CentreY;
		Z += RKMesh::Get()->m_CentreZ;

		fprintf( OutFileHandle, "v %f %f %f\n", X, Y, Z );
	}
	// write out the normal data
	fprintf( OutFileHandle, "\n# %d Normal vectors\n",RKMesh::Get()->m_NumberOfNormals ); 
	for(long i = 0; i < RKMesh::Get()->m_NumberOfNormals; i++) {

		float X = RKMesh::Get()->pNormals[i].X;
		float Y = RKMesh::Get()->pNormals[i].Y;
		float Z = RKMesh::Get()->pNormals[i].Z;

		fprintf( OutFileHandle, "v %f %f %f\n", X, Y, Z );
	}

	// write out the UV data
	fprintf( OutFileHandle, "\n# %d texture coords\n",RKMesh::Get()->m_NumberOfUVs ); 
	for(long Index = 0; Index < RKMesh::Get()->m_NumberOfUVs; Index++) {
		U = RKMesh::Get()->pUVs[Index].m_U;
		V = RKMesh::Get()->pUVs[Index].m_V;
		fprintf( OutFileHandle, "vt %f %f 0.00\n", U, V );
	}
	// write out the face data
	fprintf( OutFileHandle, "\n# %d Faces\n", RKMesh::Get()->m_NumberOfFaces );
	
	for( long i = 0; i < RKMesh::Get()->m_NumberOfFaces; i++ ) {

		fprintf( OutFileHandle, "f " );
		for ( long j=0; j<RKMesh::Get()->pFaces[i].GetNumberOfVertices(); j++ ) {
			unsigned long VertexIndex = RKMesh::Get()->pFaces[i].GetVertexIndex(j);
			unsigned long UVIndex = RKMesh::Get()->pFaces[i].GetUVIndex(j);
			unsigned long NormalIndex = RKMesh::Get()->pFaces[i].GetNormalIndex(j);

			fprintf( OutFileHandle, " %d/%d/%d ", VertexIndex, UVIndex+1, NormalIndex+1 );
		}
		fprintf( OutFileHandle, "\n" );
		
	}
	fclose ( OutFileHandle );


	return true;
}
//}}}
//KA 30/3/07: Original Save version
/* {{{
bool OBJLoader::Save(char* FileOut, char* FileIn) 
{
	FILE* InFileHandle;
	FILE* OutFileHandle;
	char buffer[3];
	buffer[2] = 0;
	bool UVsWritten = false;
	int FaceIndex = 0;
	char TempFileName[256];
	char DotTMP[5] = ".TMP";
	bool WriteUVs = false;

	if(FileIn == NULL) return(false);

	strcpy(TempFileName, FileOut);
	
	char* DotOBJ = strrchr(TempFileName, '.');
	strcpy(DotOBJ, DotTMP);

	m_InFileIndex = 0;


//	InFileHandle = fopen(FileIn, "rb");
//	if(InFileHandle == NULL) return(false);

	OutFileHandle = fopen(TempFileName, "wb");
	if(OutFileHandle == NULL) return(false);


//	fread(&buffer[0], sizeof(char), 1, InFileHandle);
	buffer[0] = pLoadBuffer[m_InFileIndex++];
	fwrite(&buffer[0], sizeof(char), 1, OutFileHandle);
	
	if(m_NoUVsIn == true) WriteUVs = true;


	while(true)
	{
		buffer[1] = pLoadBuffer[m_InFileIndex++];								// sort the EOF issue
		if(m_InFileIndex > m_InFileSize) break;
//		int Size = (int)fread(&buffer[1], sizeof(char), 1, InFileHandle);
//		if(Size == 0) break;
		fwrite(&buffer[1], sizeof(char), 1, OutFileHandle);

		if(strcmp(buffer, "v ") == 0) 
		{
			Copy(OutFileHandle);
			buffer[1] = pLoadBuffer[m_InFileIndex++];
//			fread(&buffer[1], sizeof(char), 1, InFileHandle);
			fwrite(&buffer[1], sizeof(char), 1, OutFileHandle);
		}

		if(strcmp(buffer, "vn") == 0) 
		{
			if(m_NoUVsIn == true && WriteUVs == true)
			{
				OverwriteUVs(OutFileHandle);
				WriteUVs = false;
				UVsWritten = true;
				fwrite(&buffer[0], sizeof(char), 2, OutFileHandle);			// write out first normal
			}

			Copy(OutFileHandle);
			buffer[1] = pLoadBuffer[m_InFileIndex++];
//			fread(&buffer[1], sizeof(char), 1, InFileHandle);
			fwrite(&buffer[1], sizeof(char), 1, OutFileHandle);
		}

		if(strcmp(buffer, "vt") == 0) 
		{
			if(UVsWritten == false) OverwriteUVs(OutFileHandle);
			UVsWritten = true;
			buffer[0] = pLoadBuffer[m_InFileIndex++];
			buffer[1] = pLoadBuffer[m_InFileIndex++];
//			fread(&buffer[0], sizeof(char), 1, InFileHandle);
//			fread(&buffer[1], sizeof(char), 1, InFileHandle);
		}

		if(strcmp(buffer, "f ") == 0) 
		{
			if(m_NoUVsIn == true && WriteUVs == true)
			{
				OverwriteUVs(OutFileHandle);
				WriteUVs = false;
				UVsWritten = true;
				fwrite(&buffer[0], sizeof(char), 2, OutFileHandle);			// write out first normal
			}

			if(CopyFaceNewUV(OutFileHandle, FaceIndex) == true) FaceIndex++;
			buffer[1] = pLoadBuffer[m_InFileIndex++];
//			fread(&buffer[1], sizeof(char), 1, InFileHandle);
			fwrite(&buffer[1], sizeof(char), 1, OutFileHandle);
		}

		buffer[0] = buffer[1];
	}

//	fclose(InFileHandle);
	fclose(OutFileHandle);

	InFileHandle = fopen(TempFileName, "rb");
	if(InFileHandle == NULL) return(false);

	OutFileHandle = fopen(FileOut, "wb");
	if(OutFileHandle == NULL) return(false);


	int Size = (int)fread(&buffer[0], sizeof(char), 1, InFileHandle);

	while(Size != 0)
	{
		fwrite(&buffer[0], sizeof(char), 1, OutFileHandle);
		Size = (int)fread(&buffer[0], sizeof(char), 1, InFileHandle);
	}


	fclose(InFileHandle);
	fclose(OutFileHandle);
	remove(TempFileName);

	return(true);
}
*/

//KA 30/3/07: Not Needed
/*
void OBJLoader::Copy(FILE* OutFileHandle)
{
	unsigned char CopyByte;

	while(1)
	{
		CopyByte = pLoadBuffer[m_InFileIndex++];
//		fread(&CopyByte, sizeof(char), 1, InFileHandle);
		fwrite(&CopyByte, sizeof(char), 1, OutFileHandle);
		if(CopyByte == '\n') return;
		if(CopyByte == 0xa) return;
	}
}

*/
//KA 30/3/07: Not Needed
/*
void OBJLoader::OverwriteUVs(FILE* OutFileHandle)
{
	unsigned char UVText[4] = "vt ";
	unsigned char Space = ' ';
	unsigned char NewLine = '\n';
	char Number[20];
	int StringLength;
	float U, V;


	fseek(OutFileHandle, -2, SEEK_CUR);

	for(int Index = 0; Index < RKMesh::Get()->m_NumberOfUVs; Index++)
	{
		U = RKMesh::Get()->pUVs[Index].m_U;
		V = RKMesh::Get()->pUVs[Index].m_V;

		fwrite(&UVText, sizeof(char), 3, OutFileHandle);
		
		gcvt(U, 6, Number);
		StringLength = (int)strlen(Number);
		fwrite(Number, sizeof(char), StringLength, OutFileHandle);
		fwrite(&Space, sizeof(char), 1, OutFileHandle);

		gcvt(V, 6, Number);
		StringLength = (int)strlen(Number);
		fwrite(Number, sizeof(char), StringLength, OutFileHandle);
		fwrite(&NewLine, sizeof(char), 1, OutFileHandle);
	}

	if(m_NoUVsIn == true) return;

	char Buffer[3];

	while(1)
	{
		Buffer[0] = pLoadBuffer[m_InFileIndex++];
//		fread(&Buffer[0], sizeof(char), 1, InFileHandle);
		if(Buffer[0] == '\n')
		{
			Buffer[0] = pLoadBuffer[m_InFileIndex++];
			Buffer[1] = pLoadBuffer[m_InFileIndex++];
//			fread(&Buffer[0], sizeof(char), 2, InFileHandle);
			Buffer[2] = 0;
			if(strcmp(Buffer, "vt") != 0)
			{
				fwrite(Buffer, sizeof(char), 2, OutFileHandle);
				m_InFileIndex -= 2;
//				fseek(InFileHandle, -2, SEEK_CUR);
				// two bytes back in file
				return;
			}
		}
	}
}
*/

//KA 30/3/07: not neaded
/*
bool OBJLoader::CopyFaceNewUV(FILE* OutFileHandle, int FaceIndex)
{
//	unsigned char CopyChar;
	int NumberIndices = 0;
	bool FaceDone = false;
	char Number[20];
	int StringLength;
	int VertUVNorm;
	char Slash = '/';
	char Space = ' ';
	char NewLine = '\n';

	char BadBuffer[512];							// if we've found some bad data, store it here to be copied.


	int Index = 0;

	char CopyChar = 0;
	while(1)
	{

//		fread(&BadBuffer[Index], sizeof(char), 1, InFileHandle);
		BadBuffer[Index] = pLoadBuffer[m_InFileIndex++];

		if(BadBuffer[Index] == '\n' || BadBuffer[Index] == 0xa) break;
		if(GoodData(BadBuffer[Index]) == false)
		{
			for(int BadCopy = 0; BadCopy < Index+1; BadCopy++) fwrite(&BadBuffer[BadCopy], sizeof(char), 1, OutFileHandle);
			return(false);
		}
		Index++;
	}



	VertUVNorm = 0;

	int NumberOfVerts = RKMesh::Get()->pFaces[FaceIndex].GetNumberOfVertices();

	for(int Index = 0; Index < NumberOfVerts; Index++)
	{
		int VertexIndex = RKMesh::Get()->pFaces[FaceIndex].GetVertexIndex(Index);
		int UVIndex = RKMesh::Get()->pFaces[FaceIndex].GetUVIndex(Index);
		int NormalIndex = RKMesh::Get()->pFaces[FaceIndex].GetNormalIndex(Index);

		itoa(VertexIndex, Number, 10);
		StringLength = (int)strlen(Number);
		fwrite(Number, sizeof(char), StringLength, OutFileHandle);
		fwrite(&Slash, sizeof(char), 1, OutFileHandle);

		itoa(UVIndex+1, Number, 10);
		StringLength = (int)strlen(Number);
		fwrite(Number, sizeof(char), StringLength, OutFileHandle);

		if(m_NoNormalsIn == false)
		{
			fwrite(&Slash, sizeof(char), 1, OutFileHandle);
			itoa(NormalIndex+1, Number, 10);
			StringLength = (int)strlen(Number);
			fwrite(Number, sizeof(char), StringLength, OutFileHandle);
		}

		if(Index != NumberOfVerts-1)
		{
			fwrite(&Space, sizeof(char), 1, OutFileHandle);
		}
		else
		{
			fwrite(&NewLine, sizeof(char), 1, OutFileHandle);
		}

	}

	return(true);
}


bool OBJLoader::GoodData(char InChar)
{
	if(InChar < '0' || InChar > '9')
	{
		if(InChar != ' ' && InChar != '/' && InChar != 0x13 && InChar != 0xd && InChar != 0xa && InChar != '-'&& InChar != '.' && InChar != 'e') return(false);			// found something that we don't want
	}

	return(true);
}

bool OBJLoader::NumericalData(char InChar)
{
	if(InChar < '0' || InChar > '9')
	{
		if(InChar != '-'&& InChar != '.' && InChar != 'e') return(false);			// found something that we don't want
	}

	return(true);
}




bool OBJLoader::ReadWholeFile(char* FileName)
{
	FILE* FileHandle;
	FileHandle = fopen(FileName, "rb");
	if(FileHandle == NULL) return(false);

	if(pLoadBuffer != NULL) delete[] pLoadBuffer;
	pLoadBuffer = NULL;

	fseek(FileHandle, 0, SEEK_END);
	fpos_t FileLength;
	fgetpos(FileHandle,&FileLength);
	pLoadBuffer = new unsigned char[FileLength+1];
	m_InFileSize = FileLength;

	fseek(FileHandle, 0, SEEK_SET);

	fread(pLoadBuffer, sizeof(char), FileLength, FileHandle);
	pLoadBuffer[FileLength] = ' ';

	fclose(FileHandle);
	return true;
}

//}}}
*/
