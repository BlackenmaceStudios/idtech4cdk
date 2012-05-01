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

// Simple font routine..  Font stored as binary data.


#include "../LSCM/MeshBuilderStdafx.h"

#include "stdio.h"
#include "gl\gl.h"
#include "Font.h"

#include "FontBitMap.h"			// the font bianry data




Font* Font::Get()
{
	static Font* pFont = NULL;

	if(pFont == NULL)
	{
		pFont = new Font;
	}

	return(pFont);
}



/****************************************************
*	Load in a font texture map and setup arrays		*
****************************************************/

void Font::Load(int NumberOfChars, int XSpacing, int YSpacing, int NumCharsInRow, int SpacePixelsWide, int HasOutline)
{
	LoadTexture();
	
	FontData.SpaceWide = SpacePixelsWide;
	FontData.CharWide = XSpacing;
	FontData.CharHigh = YSpacing;
	FontData.LetterAddon = 0;

	FontData.CharInfo = (FontChar*)malloc(sizeof(FontChar) * 224);
	GetFontSizes(XSpacing, YSpacing, NumCharsInRow, NumberOfChars);

	delete[] m_pGL_Pixels;
	delete[] m_pTexturePixels;
}




/****************************************************
*		   Print a string using required font		*
****************************************************/

void Font::PrintString(char* StringIn, float XPos, float YPos, float ZPos, unsigned int RGBA, float ScreenCentreX, float ScreenCentreY)
{
	int CharIndex = 0;
	int SpaceWide = FontData.SpaceWide;
	unsigned char Letter;


	if(StringIn[0] == 0) return;

	glBindTexture(GL_TEXTURE_2D, FontData.TextureHandle);
	glBegin(GL_QUADS);

	float Red = (float)(RGBA >> 16);
	float Green = (float)(RGBA >> 8 & 0xff);
	float Blue = (float)(RGBA & 0xff);

	
	while(StringIn[CharIndex] != 0)
	{
		Letter = StringIn[CharIndex];

		if(Letter == 32) XPos += (float)SpaceWide;			// No space character, just leave a gap 
		else
		{
			float CharSizeX, CharSizeY;
			CharSizeX = FontData.CharInfo[Letter-33].CharSizeX;
			CharSizeY = FontData.CharInfo[Letter-33].CharSizeY;


			glColor4f(Red, Green, Blue, 2.0f);
			glTexCoord2f(FontData.CharInfo[Letter-33].U1, FontData.CharInfo[Letter-33].V1);
			glVertex3f((XPos-ScreenCentreX)/ScreenCentreX, (YPos-ScreenCentreY)/-ScreenCentreY, 0.0f);

			glTexCoord2f(FontData.CharInfo[Letter-33].U2, FontData.CharInfo[Letter-33].V1);
			glVertex3f((XPos + CharSizeX-ScreenCentreX)/ScreenCentreX, (YPos-ScreenCentreY)/-ScreenCentreY, 0.0f);

			glTexCoord2f(FontData.CharInfo[Letter-33].U2, FontData.CharInfo[Letter-33].V2);
			glVertex3f((XPos + CharSizeX-ScreenCentreX)/ScreenCentreX, (YPos + CharSizeY-ScreenCentreY)/-ScreenCentreY, 0.0f);

			glTexCoord2f(FontData.CharInfo[Letter-33].U1, FontData.CharInfo[Letter-33].V2);
			glVertex3f((XPos-ScreenCentreX)/ScreenCentreX, (YPos + CharSizeY-ScreenCentreY)/-ScreenCentreY, 0.0f);

			XPos += FontData.CharInfo[Letter-33].PixelWidth;
		}
	
		CharIndex++;
	}

	glEnd();
}




/****************************************************
*		    Calculate the sizes of the font			*
****************************************************/

void Font::GetFontSizes(int CHARWIDE, int CHARHIGH, int CHARSINROW, int NumberOfChars)
{
	int Character = 0;
	int UpperY, LowerY;
	int Index1, Index2;
	int Width;
	int Letter, LetterOut;
	unsigned char* pPixels = m_pTexturePixels;
	int LetterAddon = (int)FontData.LetterAddon;

	UpperY = 0;
	Index1 = 0;

	while(1)
	{
		LowerY = UpperY + CHARHIGH;
			
		for(Index2 = 0; Index2 < CHARSINROW; Index2++)
		{
			Letter = Character+33;
			LetterOut = Letter;
			if(Letter > 126 && Letter <= 191) 
			{	
				LetterOut += 64;		// Foreign text chars
			}
			if(Letter > 206 && Letter < 255)			//255)
			{
				LetterOut -= 79;		// Playstation keys icons
				if(LetterOut == 161) LetterOut = 160;				// leave upside down ! alone..  what a bodge!
			}
			
			if(Letter > 191 && Letter <= 206) LetterOut = 189;
			
			if(Letter == 192) LetterOut = 169;							// copyright symbol
			if(Letter == 193) LetterOut = 161;							// Upside down Exclamation mark
			if(Letter == 194) LetterOut = 171;							// Open French Quote
			if(Letter == 195) LetterOut = 187;							// Close French Quote
			if(Letter == 196) LetterOut = 176;							// Degree symbol

			
			LetterOut -= 33;

			if(Character < 206-32)
			{
				Width = FindWidth(pPixels, Index2 * CHARWIDE, Index1 * CHARHIGH, CHARWIDE, CHARHIGH, m_TextureWidth);
				Width++;			//+= LetterAddon;

				FontData.CharInfo[LetterOut].U1 = (float)((Index2 * CHARWIDE)) / m_TextureWidth;
				FontData.CharInfo[LetterOut].V1 = (float)((Index1 * CHARHIGH)) / m_TextureWidth;

				FontData.CharInfo[LetterOut].U2 = (float)(Width) / m_TextureHeight;
				FontData.CharInfo[LetterOut].V2 = (float)(((Index1 * CHARHIGH) + CHARHIGH-2)) / m_TextureHeight;
				
				FontData.CharInfo[LetterOut].CharSizeX = (FontData.CharInfo[LetterOut].U2 - FontData.CharInfo[LetterOut].U1) * m_TextureWidth;
				FontData.CharInfo[LetterOut].CharSizeY = (FontData.CharInfo[LetterOut].V2 - FontData.CharInfo[LetterOut].V1) * m_TextureHeight;
			}
			else
			{
				Width = FindWidth(pPixels, Index2 * CHARWIDE, Index1 * CHARHIGH, CHARWIDE, CHARHIGH, m_TextureWidth);
				if(Width > ((Index2 * CHARWIDE) + 10)) Width = (Index2 * CHARWIDE) + 10;
				else
				Width++;
				
				FontData.CharInfo[LetterOut].U1 = (float)((Index2 * CHARWIDE)) / m_TextureWidth;
				FontData.CharInfo[LetterOut].V1 = (float)((Index1 * CHARHIGH)) / m_TextureWidth;
				
				FontData.CharInfo[LetterOut].U2 = (float)(Width) / m_TextureHeight;
				FontData.CharInfo[LetterOut].V2 = (float)(((Index1 * CHARHIGH) + CHARHIGH)) / m_TextureHeight;
	
				FontData.CharInfo[LetterOut].CharSizeX = (FontData.CharInfo[LetterOut].U2 - FontData.CharInfo[LetterOut].U1) * m_TextureWidth;
				FontData.CharInfo[LetterOut].CharSizeY = (FontData.CharInfo[LetterOut].V2 - FontData.CharInfo[LetterOut].V1) * m_TextureWidth;
			}
		

			FontData.CharInfo[LetterOut].PixelWidth = (float)(Width - (Index2*CHARWIDE));	// + LetterAddon*3;
			
			Character++;
			if(Character == NumberOfChars) return;		//break;
		}
			
		UpperY += CHARHIGH;
		Index1++;
	}		
}





/****************************************************
*		Measure the width of the current char		*
****************************************************/

int Font::FindWidth(unsigned char* Pixels, int XPos, int YPos, int CHARWIDE, int CHARHIGH, int FontSizeX)
{
	int ScanX, ScanY;
	unsigned char PixelIndex;
	unsigned char FindIt = Pixels[0];
	unsigned char FindIt2 = Pixels[126 * FontSizeX];
	
	if(FontSizeX < 128) FindIt2 = FindIt;
	
	XPos++;

	for(ScanX = XPos; ScanX < (XPos + CHARWIDE); ScanX++)
	{
		for(ScanY = YPos; ScanY < (YPos + CHARHIGH-1); ScanY++)
		{
			PixelIndex = Pixels[(ScanY*FontSizeX) + ScanX];
			if(!(PixelIndex == FindIt || PixelIndex == FindIt2)) break;
//			if(((FontPalette[PixelIndex]) & 0xff000000) != 0) break;
		}
		if(ScanY == (YPos + CHARHIGH-1)) return(ScanX);
	}
	
	if(FontSizeX == 64) return(XPos +6);					// bodgity, bodgity bodge!!!!  
	
	return(XPos + CHARWIDE+1);
} 



// never ever ask me about this file format..

bool Font::LoadTexture()
{
	int BPP, Mips;

	unsigned char* pCFontBitmap = (unsigned char*)&FontBitmapData;
	unsigned int* pIFontBitmap = (unsigned int*)pCFontBitmap;

	m_TextureWidth = pIFontBitmap[0];
	m_TextureHeight = pIFontBitmap[1];
	BPP = pIFontBitmap[2];
	Mips = pIFontBitmap[3];
	pIFontBitmap += 4;
	pCFontBitmap += 16;


	if(BPP == 8)
	{
		m_pTexturePalette = new unsigned int[256];
		for(int Index = 0; Index < 256; Index++) 
		{ 
			m_pTexturePalette[Index] = *pIFontBitmap;
			pIFontBitmap++;
			pCFontBitmap+=4; 
		}


	int PaletteIndex;
	int* pPalette = (int*)m_pTexturePalette;

	for(int Index = 0; Index < 8; Index++)
	{
		PaletteIndex = (Index * 32) + 8;

		for(int Index2 = 0; Index2 < 8; Index2++)
		{
			int Temp;
			Temp = pPalette[PaletteIndex];
			pPalette[PaletteIndex] = pPalette[PaletteIndex + 8];
			pPalette[PaletteIndex + 8] = Temp;
			PaletteIndex++;
		}
	}


		m_pTexturePixels = new unsigned char[m_TextureWidth * m_TextureHeight];
		for(int Index = 0; Index < m_TextureWidth * m_TextureHeight; Index++) 
		{ 
			m_pTexturePixels[Index] = *pCFontBitmap; 
			pCFontBitmap++;
		}


		m_pGL_Pixels = new unsigned int[m_TextureWidth * m_TextureHeight];

		for(int Index = 0; Index < m_TextureWidth * m_TextureHeight; Index++)
		{
			unsigned char Pixel = m_pTexturePixels[Index];

			unsigned int Colour = m_pTexturePalette[Pixel];
			unsigned int Alpha = (Colour) & 0xff000000;

			m_pGL_Pixels[Index] = 0xffffff | Alpha;
		}

	}

	glGenTextures(1, &FontData.TextureHandle);
	glBindTexture(GL_TEXTURE_2D, FontData.TextureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pGL_Pixels);


	delete[] m_pTexturePalette;

	return(true);
}
