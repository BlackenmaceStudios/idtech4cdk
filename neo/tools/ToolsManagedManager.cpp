// ToolsManagedManager.cpp
//

#include "precompiled.h"

#include "radiant/QERTYPES.H"
#include "radiant/editorEntity.h"
#include "radiant/EditorMap.h"

// jmarshall
#include "tools_managed.h"
#include "../renderer/Image.h"
// jmarshall end

using namespace ToolsManaged;
extern bool g_editorAlive;

IToolsManaged *toolsManaged;

TOOLS_EXPORTFUNC(cmdSystem, BufferCommandText, (const char *str), (CMD_EXEC_NOW, str) )
TOOLS_EXPORTFUNC_RET(fileSystem, OSPathToRelativePath, (const char *str), ( str) )
TOOLS_EXPORTFUNC_RET(fileSystem, RelativePathToOSPath, (const char *str), ( str) )
TOOLS_EXPORTFUNC_RET(fileSystem, OpenFileRead, (const char *relativePath, bool allowCopyFiles, const char* gamedir), ( relativePath, allowCopyFiles, gamedir ) )
TOOLS_EXPORTFUNC_RET(fileSystem, OpenFileWrite, (const char *relativePath,  const char* gamedir), ( relativePath,  gamedir ) )
TOOLS_EXPORTFUNC(fileSystem, CloseFile, (idFile *file), ( file ) )
TOOLS_EXPORTFUNC_RET(declManager, FindType, (declType_t type, const char *name, bool makeDefault), ( type, name, makeDefault ) )

// IdDict
TOOLS_EXPORTFUNC_NOOBJ( idDict, Set, (idDict *obj, const char *str, const char *str1), (str, str1) )
TOOLS_EXPORTFUNC_NOOBJ_RET( idDict, GetString, (idDict *obj, const char *key), (key) )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, idVec3, GetVector, (idDict *obj, const char *key), (key) )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, int, GetNumKeyVals, (idDict *obj), () )
TOOLS_EXPORTFUNC_NOOBJ_RETTYPE( idDict, idKeyValueInstance, GetKeyValInstance, (idDict *obj, int index), (index) )

float *GetCurrentViewPos();
renderView_t view;
idAngles viewAngle;

void DrawRenderSurface( srfTriangles_t *surf, idImage *image, idVec3 origin, idAngles angle, bool cameraView, bool usingVertexCache );

extern "C" __declspec(dllexport) void TOOLAPI_Editor_ExecuteCmd( const char *cmd ) 
{
	cmdSystem->BufferCommandText( CMD_EXEC_NOW, cmd );
}

extern "C" __declspec(dllexport) void TOOLAPI_RenderSystem_WriteTGA( const char *filename, const byte *data, int width, int height, bool flipVertical ) 
{
	renderSystem->WriteTGA( filename, data, width, height, flipVertical );
}

extern "C" __declspec(dllexport) void TOOLAPI_Image_CopyImageToImageBufferRegion(  byte *Dest, byte *color,  int DestX, int DestY, int Width, int Height, int DiemWidth, bool replace, bool oldReplace)
{
unsigned int  		x, y, z, ConvBps, ConvSizePlane;
	byte 	*Converted;
	int Depth = 1;
	unsigned int 		c;
	unsigned int 		StartX, StartY, StartZ;
	byte 	*SrcTemp;
	float 	Back;
	int DestZ = 0;

	
	ConvBps 	  = 4 * Width;
	ConvSizePlane = ConvBps   * Height;
	
	//@NEXT in next version this would have to be removed since Dest* will be unsigned
	StartX = DestX >= 0 ? 0 : -DestX;
	StartY = DestY >= 0 ? 0 : -DestY;
	
	// Limit the copy of data inside of the destination image
	bool forceSolidAlpha = false;

	int srcX = 0, srcY = 0;

	if(DestX < 0)
	{
		srcX = -DestX;
		Width = Width - srcX;
		DestX = 0;
	}

	if(DestY < 0)
	{
		srcY = -DestY;
		Height = Height - srcY;
		DestY = 0;
	}

	// Limit the copy of data inside of the destination image
	if (Width  + DestX > DiemWidth) 
	{
		float newWidth = (Width + DestX);
		newWidth -= DiemWidth;

		Width = Width - newWidth;
		srcY = newWidth;
	}
	if (Height + DestY > DiemWidth) 
	{
		float newHeight = (Height + DestY);
		newHeight -= DiemWidth;

		Height = Height - newHeight;
		srcY = newHeight;
	}



	if (Depth  + DestZ > DiemWidth) 
		Depth  = 1;


	#define CLAMPTOBYTE(color) if((color) & (~255)) {color = (BYTE)((-(color)) >> 31);}else{color = (BYTE)(color);}
	const unsigned int  bpp_without_alpha = 4 - 1;
		for (z = 0; z < Depth; z++) {
			for (y = 0; y < Height; y++) {
				for (x = 0; x < Width; x++) {
					const unsigned int   SrcIndex  = (z+0)*ConvSizePlane + (y+srcY)*ConvBps + (x+srcX)*4;
					const unsigned int   DestIndex = (z+DestZ)*(DiemWidth * DiemWidth) + (y+DestY)*(DiemWidth * 4) + (x+DestX)*4;
					
					float Front = 0;
					int r,g,b,a;
					if(replace)
					{
						r = (byte)color[SrcIndex + 0];
						g = (byte)color[SrcIndex + 1];
						b = (byte)color[SrcIndex + 2];
						a = (byte)color[SrcIndex + 3];
					}
					else if(oldReplace)
					{
						r = Dest[DestIndex + 0] + (byte)color[SrcIndex + 0];
						g = Dest[DestIndex + 1] + (byte)color[SrcIndex + 1];
						b = Dest[DestIndex + 2] + (byte)color[SrcIndex + 2];
						a = Dest[DestIndex + 3] + (byte)color[SrcIndex + 3];

						if(r > 255)
							r = 255;

						if(g > 255)
							g = 255;

						if(b > 255)
							b = 255;

						if(a > 255)
							a = 255;
					}
					else
					{
						a = (byte)color[SrcIndex + 3]; 
						r = (color[SrcIndex + 0] * a + Dest[DestIndex + 0] * (255 - a)) / 255; 
						g = (color[SrcIndex + 1] * a + Dest[DestIndex + 1] * (255 - a)) / 255; 
						b = (color[SrcIndex + 2] * a + Dest[DestIndex + 2] * (255 - a)) / 255; 
						CLAMPTOBYTE(r); 
						CLAMPTOBYTE(g); 
						CLAMPTOBYTE(b); 
		

						
					}

					if(forceSolidAlpha)
						a = 255.0f;
	
					Dest[DestIndex + 0] = (byte)r;
					Dest[DestIndex + 1] = (byte)g;
					Dest[DestIndex + 2] = (byte)b;
					Dest[DestIndex + 3] = (byte)a;
				}
			}
		}
}

extern "C" __declspec(dllexport) void TOOLAPI_Image_CopyImageToImageBufferRegionWithAlpha(  byte *Dest, byte *color, byte *alpha, int DestX, int DestY, int Width, int Height, int DiemWidth )
{
unsigned int  		x, y, z, ConvBps, ConvSizePlane;
	byte 	*Converted;
	int Depth = 1;
	unsigned int 		c;
	unsigned int 		StartX, StartY, StartZ;
	byte 	*SrcTemp;
	float 	Back;
	int DestZ = 0;

	
	ConvBps 	  = 4 * Width;
	ConvSizePlane = ConvBps   * Height;
	
	//@NEXT in next version this would have to be removed since Dest* will be unsigned
	StartX = DestX >= 0 ? 0 : -DestX;
	StartY = DestY >= 0 ? 0 : -DestY;

	bool forceSolidAlpha = false;
	
	// Limit the copy of data inside of the destination image
	if (Width  + DestX > DiemWidth) 
	{
		forceSolidAlpha = true;
		Width  = DiemWidth  - DestX;
	}
	if (Height + DestY > DiemWidth) 
	{
		forceSolidAlpha = true;
		Height = DiemWidth - DestY;
	}
	if (Depth  + DestZ > DiemWidth) 
		Depth  = 1;
	
	const unsigned int  bpp_without_alpha = 4 - 1;
		for (z = 0; z < Depth; z++) {
			for (y = 0; y < Height; y++) {
				for (x = 0; x < Width; x++) {
					const unsigned int   SrcIndex  = (z+0)*ConvSizePlane + (y+0)*ConvBps + (x+0)*4;
					const unsigned int   DestIndex = (z+DestZ)*(DiemWidth * DiemWidth) + (y+DestY)*(DiemWidth * 4) + (x+DestX)*4;
					
					float Front = 0;
					float r,g,b,a;

					if(forceSolidAlpha)
					{
						a = 1.0f;
					}
					else
					{
						a = alpha[DestIndex + 0] / 255.0f;
					}


					r = color[SrcIndex + 0]  / 255.0f ;
					g = color[SrcIndex + 1]  / 255.0f;
					b = color[SrcIndex + 2]  / 255.0f;
				
					r = (r * a) * 255.0f;
					g = (b * a) * 255.0f;
					b = (b * a) * 255.0f;
					a = a * 255.0f;

					if(r > 255)
						r = 255;
					if(g > 255)
						g = 255;

					if(b > 255)
						b = 255;
					if(a > 255)
						a = 255;

					Dest[DestIndex + 0] = (byte)r;
					Dest[DestIndex + 1] = (byte)g;
					Dest[DestIndex + 2] = (byte)b;
					Dest[DestIndex + 3] = (byte)a;
				}
			}
		}
}

extern "C" __declspec(dllexport) byte *TOOLAPI_Image_ReadDriverPixels( idImage *image ) 
{
	return image->ReadDriverPixels( 0, 0 );
}

extern "C" __declspec(dllexport) void TOOLAPI_Image_CopyUncompressedBufferIntoRegion( idImage *image,  void *buffer, int mipLevel, int x, int y, int width, int height ) {
	//image->Bind();
	image->CopyUncompressedBufferIntoRegion( buffer, mipLevel, x, y, width, height );
}

extern "C" __declspec(dllexport) void TOOLAPI_Image_FreeTextureBuffer( void *in ) {
	allocator->Free( in );
}

extern "C" __declspec(dllexport) byte *TOOLAPI_Image_ResampleTextureBuffer( const byte *in, int inwidth, int inheight, int outwidth, int outheight ) {
	return R_ResampleTexture( in, inwidth, inheight, outwidth, outheight );
}

extern "C" __declspec(dllexport) void TOOLAPI_Image_GetWidthHeight( idImage *image, int &width, int &height )
{
	width = image->uploadWidth;
	height = image->uploadHeight;
}

extern "C" __declspec(dllexport) void TOOLAPI_RendererDevice_BindImageToUnit( idImage *image, int unit )
{
	renderDevice->SelectTextureNoClient( unit );
	if(image == NULL) {
		globalImages->BindNull();

		
		return;
	}

	image->Bind();
}

extern "C" __declspec(dllexport) bmRenderProgram *TOOLAPI_RendererDevice_LoadRenderProgram( const char *path, int numPasses )
{
	return renderDevice->LoadRenderProgram( path, numPasses );
}

extern "C" __declspec(dllexport) void TOOLAPI_RendererSystem_DrawPlane( float size, idImage *image, float x, float y, float z, float yaw, float pitch, float roll )
{
	idVec3 origin = idVec3(x,y,z);
	idMat3 axis = idAngles(pitch, yaw, roll).ToMat3();
//	qglPushMatrix();
	qglDisable(GL_DEPTH_TEST);
	
// jmarshall end
	if(image != NULL)
	{
		image->Bind();
	}
// jmarshall

	qglBegin( GL_QUADS );
		idVec3 v[4];

		v[0] = idVec3(-1.0f * size, 1.0f, 1.0f * size) * axis + origin;
		v[1] = idVec3(1.0f * size, 1.0f, 1.0f * size) * axis + origin;
		v[2] = idVec3(1.0f * size, 1.0f,-1.0f * size) * axis + origin;
		v[3] = idVec3(-1.0f * size, 1.0f,-1.0f * size) * axis + origin;

		qglTexCoord2f(1.0f,1.0f); qglVertex3f(v[0].x, v[0].y, v[0].z);
		qglTexCoord2f(1.0f,0.0f); qglVertex3f(v[1].x, v[1].y, v[1].z);
		qglTexCoord2f(0.0f,0.0f); qglVertex3f(v[2].x, v[2].y, v[2].z);
		qglTexCoord2f(0.0f,1.0f); qglVertex3f(v[3].x, v[3].y, v[3].z);

	qglEnd();
	qglEnable(GL_DEPTH_TEST);

	globalImages->BindNull();

	// Reset the transform.
	//qglPopMatrix();

}
extern "C" __declspec(dllexport) void TOOLAPI_RendererSystem_UnProject( HWND wndHandle, float &x, float &y, float &z )
{
	POINT mouse;                        // Stores The X And Y Coords For The Current Mouse Position

	GetCursorPos(&mouse);                   // Gets The Current Cursor Coordinates (Mouse Coordinates)
	ScreenToClient(wndHandle, &mouse);

	idVec3 worldxyz;
	idVec2 m_xy;

	m_xy.x = mouse.x;
	m_xy.y = mouse.y;

	renderSystem->ProjectMouseToWorldCoord( m_xy, worldxyz );

	x = worldxyz.x;
	y = worldxyz.y;
	z = worldxyz.z;
}

extern "C" __declspec(dllexport) idUserInterface *TOOLAPI_UserInterface_LoadGui( const char *mapName )
{
	return uiManager->FindGui( mapName, true, false, true );
}


extern "C" __declspec(dllexport) void TOOLAPI_Editor_DrawRenderSurf( srfTriangles_t *surf, idImage *image, float x, float y, float z, float yaw, float pitch, float roll, bool cameraView )
{
	DrawRenderSurface( surf, image, idVec3(x,y,z), idAngles(yaw, pitch, roll ), cameraView, false );
}

extern "C" __declspec(dllexport) idImage *TOOLAPI_Editor_FindImage(const char *path, bool clampToEdge)
{
	return renderSystem->FindImage( path, clampToEdge );
}

extern "C" __declspec(dllexport) float *TOOLAPI_Editor_GetViewPosition()
{
	return GetCurrentViewPos();
}

extern "C" __declspec(dllexport) srfTriangles_t *TOOLAPI_RenderWorld_GetVisibleVirtualTextureAreaSurface( idRenderWorld *world, int surfId )
{
	return world->GetVisibleVirtualTextureAreaSurface(surfId);
}

void SetProjectionMatrix(float width2, float height2) {
	float xfov = 90;
	float yfov = 2 * atan((float)height2 / width2) * idMath::M_RAD2DEG;
#if 0
	float screenaspect = (float)m_Camera.width / m_Camera.height;
	qglLoadIdentity();
	gluPerspective(yfov, screenaspect, 2, 8192);
#else
	float	xmin, xmax, ymin, ymax;
	float	width, height;
	float	zNear;
	float   zFar = 3000000;
	float	projectionMatrix[16];

	//
	// set up projection matrix
	//
	zNear	= 1;

	ymax = zNear * tan( yfov * idMath::PI / 360.0f );
	ymin = -ymax;

	xmax = zNear * tan( xfov * idMath::PI / 360.0f );
	xmin = -xmax;

	width = xmax - xmin;
	height = ymax - ymin;
	float depth = zFar - zNear;

	projectionMatrix[0] = 2 * zNear / width;
	projectionMatrix[4] = 0;
	projectionMatrix[8] = ( xmax + xmin ) / width;	// normally 0
	projectionMatrix[12] = 0;

	projectionMatrix[1] = 0;
	projectionMatrix[5] = 2 * zNear / height;
	projectionMatrix[9] = ( ymax + ymin ) / height;	// normally 0
	projectionMatrix[13] = 0;

	projectionMatrix[2] = 0;
	projectionMatrix[6] = 0;
	projectionMatrix[10] = -( zFar + zNear ) / depth;
	projectionMatrix[14] = -2 * zFar * zNear / depth;

	projectionMatrix[3] = 0;
	projectionMatrix[7] = 0;
	projectionMatrix[11] = -1;
	projectionMatrix[15] = 0;

	qglLoadMatrixf( projectionMatrix );
#endif
}

extern "C" __declspec(dllexport) int *TOOLAPI_RenderWorld_GetVisibleVirtualTextureArea( idRenderWorld *world, int &numSurfaces, float width, float height, float x, float y, float z, float yaw, float pitch, float roll )
{


	viewAngle.yaw = yaw;
	viewAngle.pitch = pitch;
	viewAngle.roll = roll;

	view.x = 0;
	view.y = 0;
	view.width = width;
	view.height = height;
	view.fov_x = 90.0f;
	view.fov_y = 2 * atan((float)height / width) * idMath::M_RAD2DEG;
	view.vieworg = idVec3(x, y, z);
	view.viewaxis = viewAngle.ToMat3();

	return world->GetVisibleVirtualTextureAreas( &view, numSurfaces );
}

extern "C" __declspec(dllexport) HDC TOOLAPI_Device_BeginRender( HWND hwnd, int width, int height )
{
	HDC hDC = ::GetDC(hwnd);

	if (!renderDevice->BindDeviceToWindow( hDC ) ) {
		return NULL;
	}

	qglMatrixMode( GL_PROJECTION );
	qglPushMatrix();
	qglLoadIdentity();
	qglMatrixMode( GL_MODELVIEW );
	qglPushMatrix();
	qglLoadIdentity();



	qglClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	qglViewport(0, 0, width, height);
	qglScissor(0, 0, width, height);
	qglClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );

	qglCullFace(GL_FRONT);
	qglDisable(GL_CULL_FACE);
	qglShadeModel(GL_SMOOTH);
	qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	qglEnable(GL_BLEND);
	qglDisable(GL_COLOR_MATERIAL);

	qglEnable(GL_DEPTH_TEST);
	qglDepthFunc(GL_LEQUAL);

	qglDisable(GL_LIGHTING);
	qglMatrixMode(GL_PROJECTION);

	SetProjectionMatrix(width, height);

	qglMatrixMode(GL_MODELVIEW);

	
	// render it
	renderSystem->BeginFrame( width , height );

	return hDC;
}

extern "C" __declspec(dllexport) void TOOLAPI_Device_EndRender( HWND hwnd, HDC hDC )
{
	//qglEnable(GL_CULL_FACE);
	qglMatrixMode( GL_PROJECTION );
	qglPopMatrix();

	qglMatrixMode( GL_MODELVIEW );
	qglPopMatrix();
	
	qglPushMatrix();
	renderSystem->EndFrame( NULL, NULL, false );
	qglPopMatrix();
	renderDevice->SwapBuffers(hDC);
	renderDevice->BindDeviceToWindow( NULL );
	::ReleaseDC( hwnd, hDC );
}

extern "C" __declspec(dllexport) idRenderWorld *TOOLAPI_Editor_LoadWorld( const char *name )
{
	static idRenderWorld *world = NULL;
	
	if(world != NULL)
	{
		renderSystem->FreeRenderWorld( world );
	}
	world = renderSystem->AllocRenderWorld();

	if(!world->InitFromMap( va("generated/maps%s", name), false )) {
		renderSystem->FreeRenderWorld( world );
		world = NULL;
		return NULL;
	}

	return world;
}

extern "C" __declspec(dllexport) idImage *TOOLAPI_Editor_GetDiffuseImageHandleForMaterial( const char *mtr )
{
	const idMaterial *mat = declManager->FindMaterial( mtr );

	return mat->GetEditorImage();
}

extern "C" __declspec(dllexport) byte *TOOLAPI_Editor_GetDiffuseImageForMaterial( const char *mtr, int &width, int &height )
{
	const idMaterial *mat = declManager->FindMaterial( mtr );

	idImage *editorImage = mat->GetEditorImage();
	if(editorImage->defaulted)
	{
		editorImage = renderSystem->FindImage("textures/editor/missingimage");
	}

	width = editorImage->uploadWidth;
	height = editorImage->uploadHeight;

	return editorImage->ReadDriverPixels( 0, 0 );
}

extern "C" __declspec(dllexport) int TOOLAPI_Editor_GetNumMaterials( void )
{
	return declManager->GetNumDecls( DECL_MATERIAL );
}

extern "C" __declspec(dllexport) const char *TOOLAPI_Editor_GetMaterialNameByIndex( int indx )
{
	const idMaterial *material = declManager->MaterialByIndex( indx, false );

	return material->GetName();
}

extern "C" __declspec(dllexport) const char *TOOLAPI_Editor_GetMapName( void )
{
	static idStr s;
	
	s = &currentmap[0];

	
	return s.StripFileExtension().c_str();
}


extern "C" __declspec(dllexport) idDict *TOOLAPI_Entity_GetTemplate( const char *type )
{
	idDeclEntityDef *def = (idDeclEntityDef *)declManager->FindType( DECL_ENTITYDEF, type );
	
	assert( def != NULL );

	return &def->dict;
}

extern "C" __declspec(dllexport) idDict *TOOLAPI_Entity_GetEntityDict( const char *name )
{
	entity_t *ent = FindEntity( "name", name );
	if(!ent) {
		return NULL;
	}

	return &ent->epairs;
}


/*
=================
idToolInterfaceLocal::GetValueFromManagedEnum
=================
*/
int idToolInterfaceLocal::GetValueFromManagedEnum(const char * enumTypeStrv, const char * enumValStrv)
{
	long value = 0;


	toolsManaged->GetValueFromManagedEnum( _com_util::ConvertStringToBSTR(enumTypeStrv), _com_util::ConvertStringToBSTR(enumValStrv), &value );
	return value;
}

void Brush_Update();
extern "C" __declspec(dllexport) void TOOLAPI_Brush_Update( void )
{
	Brush_Update();
}

// Export 

/*
==================
idToolInterfaceLocal::InitToolsManaged
==================
*/
void idToolInterfaceLocal::InitToolsManaged( void ) {
	CoCreateInstance( __uuidof(ToolsManagedPrivate), NULL, CLSCTX_INPROC_SERVER, __uuidof(IToolsManaged), (void **)&toolsManaged);
}

/*
==================
idToolInterfaceLocal::ShowDebugConsole
==================
*/
void idToolInterfaceLocal::ShowDebugConsole( void ) {
	toolsManaged->ShowDebugConsole();
}
/*
==================
idToolInterfaceLocal::RadiantPrint
==================
*/
void idToolInterfaceLocal::RadiantPrint( const char *text ) {
	if ( g_editorAlive ) {
		toolsManaged->PrintToConsole( _com_util::ConvertStringToBSTR(text) );
	}

	
}
