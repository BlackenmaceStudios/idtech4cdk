// ModelStudioEditor.cpp : implementation file
//

#include "precompiled.h"

#include "../../sys/win32/win_local.h"
#include "../../sys/win32/rc/common_resource.h"
#include "../comafx/DialogName.h"
#include "../radiant/QE3.H"
#include "ModelStudioEditor.h"

bmModelStudioEditor *modelStudioEditor = NULL;

// bmModelStudioEditor dialog

bmModelStudioEditor::bmModelStudioEditor(CWnd* pParent /*=NULL*/)
{

}

bmModelStudioEditor::~bmModelStudioEditor()
{
}

void bmModelStudioEditor::DoDataExchange(CDataExchange* pDX)
{
}


int bmModelStudioEditor::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	//CDialog::OnInitDialog();

	HDC hDC = GetDC();

	QEW_SetupPixelFormat(hDC, true);

	HFONT hfont = CreateFont(
				12,	// logical height of font 
				0,	// logical average character width 
				0,	// angle of escapement 
				0,	// base-line orientation angle 
				0,	// font weight 
				0,	// italic attribute flag 
				0,	// underline attribute flag 
				0,	// strikeout attribute flag 
				0,	// character set identifier 
				0,	// output precision 
				0,	// clipping precision 
				0,	// output quality 
				FIXED_PITCH | FF_MODERN,	// pitch and family 
				"Lucida Console" 	// pointer to typeface name string 
				);

	if (!hfont) {
		Error("couldn't create font");
	}

	HFONT hOldFont = (HFONT)SelectObject(hDC, hfont);

	wglMakeCurrent (hDC, win32.hGLRC);

	if ((g_qeglobals.d_font_list = qglGenLists(256)) == 0) {
		common->Warning( "couldn't create font dlists" );
	}

	// create the bitmap display lists we're making images of glyphs 0 thru 255
	if ( !qwglUseFontBitmaps(hDC, 0, 255, g_qeglobals.d_font_list) ) {
		common->Warning( "wglUseFontBitmaps failed (%d).  Trying again.", GetLastError() );

		// FIXME: This is really wacky, sometimes the first call fails, but calling it again makes it work
		//		This probably indicates there's something wrong somewhere else in the code, but I'm not sure what
		if ( !qwglUseFontBitmaps(hDC, 0, 255, g_qeglobals.d_font_list) ) {
			common->Warning( "wglUseFontBitmaps failed again (%d).  Trying outlines.", GetLastError() );

			if (!qwglUseFontOutlines(hDC, 0, 255, g_qeglobals.d_font_list, 0.0f, 0.1f, WGL_FONT_LINES, NULL)) {
				common->Warning( "wglUseFontOutlines also failed (%d), no coordinate text will be visible.", GetLastError() );
			}
		}
	}

	
	m_Camera.width = SCREEN_WIDTH;
	m_Camera.height = SCREEN_HEIGHT;

	SelectObject(hDC, hOldFont);
	::ReleaseDC(GetSafeHwnd(), GetDC());

	// indicate start of glyph display lists
	qglListBase(g_qeglobals.d_font_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*
=================
OnUpdateModelPreview
=================
*/
void bmModelStudioEditor::OnUpdateModelPreview( void ) {
	int frontEnd, backEnd = 0;
	renderView_t	refdef;

	qglViewport(0, 0, m_Camera.width, m_Camera.height);
	qglScissor(0, 0, m_Camera.width, m_Camera.height);
	qglClearColor(g_qeglobals.d_savedinfo.colors[COLOR_CAMERABACK][0], g_qeglobals.d_savedinfo.colors[COLOR_CAMERABACK][1], g_qeglobals.d_savedinfo.colors[COLOR_CAMERABACK][2], 0);


	qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	float scalew = SCREEN_WIDTH / m_Camera.width;
	float scaleh = SCREEN_HEIGHT / m_Camera.height;
	if(scalew <= 0 || scaleh <= 0)
	{
		scalew = 1;
		scaleh = 1;
	}
	else
	{
		scalew -= 0.005;
		scaleh -= 0.005;
	}


	// render it
	renderSystem->BeginFrame( SCREEN_WIDTH * scalew , SCREEN_HEIGHT * scaleh  );

	memset( &refdef, 0, sizeof( refdef ) );
	refdef.vieworg = idVec3( 0, 0, 0 );
	refdef.megaProject = &g_qeglobals.megaproject;

	m_Camera.angles = idAngles( 0, 0, 0 );

	// the editor uses opposite pitch convention
	refdef.viewaxis = idAngles( -m_Camera.angles.pitch, m_Camera.angles.yaw, m_Camera.angles.roll ).ToMat3();
	
	refdef.width = SCREEN_WIDTH * scalew;
	refdef.height = SCREEN_HEIGHT * scaleh;
	refdef.fov_x = 90;
	refdef.fov_y = 2 * atan((float)SCREEN_HEIGHT / SCREEN_WIDTH) * idMath::M_RAD2DEG;
	//refdef.forceUpdate = true;

	// only set in animation mode to give a consistent look 
	//refdef.time = eventLoop->Milliseconds();

	g_qeglobals.testRenderWorld->RenderScene( &refdef );

	game->DrawDefferedPass( SCREEN_WIDTH, SCREEN_HEIGHT );
	renderSystem->EndFrame( &frontEnd, &backEnd );

	//tr.viewDef->skipPostProcess = false;

	//
	// bind back to the default texture so that we don't have problems elsewhere
	// using/modifying texture maps between contexts
	//
	globalImages->BindNull();

	qglFinish();
	QE_CheckOpenGLForErrors();


}

/*
=================
bmModelStudioEditor::OnSize
=================
*/
void bmModelStudioEditor::OnSize(UINT nType, int cx, int cy) {
	
}

/*
=================
OnFileFbxImportStatic
=================
*/
void bmModelStudioEditor::OnPaint( void ) {
	HDC			hdc;
	bool		bPaint = true;

	hdc = ::GetDC( GetSafeHwnd() );
	if (!renderDevice->BindDeviceToWindow( hdc )) {
		common->Printf("ERROR: wglMakeCurrent failed..\n ");
		common->Printf("Please restart " EDITOR_WINDOWTEXT " if the model studio previewer is not working\n");
	}
	else {
		QE_CheckOpenGLForErrors();
		
		OnUpdateModelPreview();

		QE_CheckOpenGLForErrors();
		renderDevice->SwapBuffers(hdc);
	}

	// jmarshall
	renderDevice->BindDeviceToWindow( NULL );
	// jmarshall end
}
/*
=================
OnFileFbxImportStatic
=================
*/
void bmModelStudioEditor::OnNamedEvent( const char *eventName ) {
	if(!strcmp( eventName, "ImportStatic" )) {
		OnFileFbxImportStatic();
	}
	else {
		common->FatalError("ModelStudio: Unknown event %s\n", eventName );
	}
}

/*
=================
OnFileFbxImportStatic
=================
*/
void bmModelStudioEditor::OnFileFbxImportStatic( void ) {

	idStr fileName = Sys_OpenFileDialog( "Import Static Model", "Autodesk FBX Model(*.fbx)|*.fbx*|Wavefront OBJ Model(*.obj)|*.obj*||", "obj");
	idStr modelPath;

	// Use the right importer for the model.
	if(fileName.CheckExtension( "fbx" ))
	{
		if(!FbxExportToStaticMesh( fileName.c_str(), modelPath ))
			return;
	}
	else if(fileName.CheckExtension( "obj" ))
	{
		if(!ObjExportToStaticMesh( fileName.c_str(), modelPath ))
			return;
	}
	else
	{
		common->Warning("This model type isn't supported for static mesh import.\n");
	}

	g_qeglobals.testModel.hModel = renderModelManager->FindModel( modelPath );

	g_qeglobals.testRenderWorld->UpdateEntityDef( g_qeglobals.testModelWorldHandle, &g_qeglobals.testModel );

	modelPath.Replace( "\\", "/" );
	collisionModelManager->GenerateCollisionMapForModel( modelPath.c_str() );
}

/*
=================
ModelStudioEditorInit
=================
*/
void ModelStudioEditorInit( const idDict *spawnArgs ) {

	if ( renderSystem->IsFullScreen() ) {
		common->Printf( "Cannot run the ModelStudio in fullscreen mode.\n"
					"Set r_fullscreen to 0 and vid_restart.\n" );
		return;
	}

	if ( modelStudioEditor == NULL ) {
		CRect rect;
		modelStudioEditor = new bmModelStudioEditor();
		modelStudioEditor->Create("ModelStudio", "", QE3_CHILDSTYLE, rect, g_pParentWnd, 1234);

	}

	

	modelStudioEditor->ShowWindow( SW_SHOW );
	modelStudioEditor->SetFocus();

	idKeyInput::ClearStates();
}

/*
=================
ModelStudioEditorRun
=================
*/
void ModelStudioEditorRun( void ) {
#if _MSC_VER >= 1300
	MSG *msg = AfxGetCurrentMessage();			// TODO Robert fix me!!
#else
	MSG *msg = &m_msgCur;
#endif

	while( ::PeekMessage(msg, NULL, NULL, NULL, PM_NOREMOVE) ) {
		// pump message
		if ( !AfxGetApp()->PumpMessage() ) {
		}
	}
}

/*
=================
ModelStudioEditorShutdown
=================
*/
void ModelStudioEditorShutdown( void ) {
	modelStudioEditor->ShowWindow( SW_HIDE );
}
// bmModelStudioEditor message handlers
