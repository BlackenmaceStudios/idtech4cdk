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


#ifndef FONT
#define FONT



typedef struct 
{
	float U1, V1;
	float U2, V2;
	float CharSizeX, CharSizeY;
	float PixelWidth;
} FontChar;



typedef struct
{
	GLuint TextureHandle;
	FontChar* CharInfo;
	int CharHigh;
	int CharWide;
	int SpaceWide;
	float LetterAddon;

} FontSet;




class Font
{
public:

	static Font* Get();							// Font is a singleton


	~Font()
	{
		if(m_pTexturePalette != NULL) delete[] m_pTexturePalette;
		if(m_pTexturePixels != NULL) delete[] m_pTexturePixels;
		if(m_pGL_Pixels != NULL) delete[] m_pGL_Pixels;

		m_pTexturePalette = NULL;
		m_pTexturePixels = NULL;
		m_pGL_Pixels = NULL;
	};

	void Load(int NumberOfChars, int XSpacing, int YSpacing, int NumCharsInRow, int SpacePixelsWide, int HasOutline);
	void PrintString(char* StringIn, float XPos, float YPos, float ZPos, unsigned int RGBA, float ScreenCentreX, float ScreenCentreY);


private:

	Font()
	{
		m_pTexturePalette = NULL;
		m_pTexturePixels = NULL;
		m_pGL_Pixels = NULL;

		Load(211, 10, 18, 25, 6, FALSE);

	};

	FontSet FontData;

	int m_TextureWidth, m_TextureHeight;
	unsigned int* m_pTexturePalette;
	unsigned char* m_pTexturePixels;
	unsigned int* m_pGL_Pixels;


	int FindWidth(unsigned char* Pixels, int XPos, int YPos, int CHARWIDE, int CHARHIGH, int FontSizeX);
	void GetFontSizes(int CHARWIDE, int CHARHIGH, int CHARSINROW, int NumberOfChars);

	bool LoadTexture();
};


#endif