// ModelStudioEditor.cpp : implementation file
//

#include "precompiled.h"

#include "../../sys/win32/win_local.h"
#include "../../sys/win32/rc/common_resource.h"
#include "../../sys/win32/rc/ModelManager_resource.h"
#include "../comafx/DialogName.h"
#include "../radiant/QE3.H"
#include "ModelStudioEditor.h"

bmModelStudioEditor *modelStudioEditor = NULL;

// bmModelStudioEditor dialog

bmModelStudioEditor::bmModelStudioEditor(CWnd* pParent /*=NULL*/)
	: CDialog(bmModelStudioEditor::IDD, pParent)
{

}

bmModelStudioEditor::~bmModelStudioEditor()
{
}

void bmModelStudioEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_MODELMANAGER_PREVIEW, previewWindow );
}

BEGIN_MESSAGE_MAP(bmModelStudioEditor, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED( ID_IMPORT_SKELETALMESH, OnFileFbxImportStatic )
END_MESSAGE_MAP()

BOOL bmModelStudioEditor::OnInitDialog() {
	CDialog::OnInitDialog();

	CDC *pDC = previewWindow.GetDC();
	HDC hDC = pDC->GetSafeHdc();

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

	CRect	rect;
	GetDlgItem(IDC_MODELMANAGER_PREVIEW)->GetClientRect(rect);
	m_Camera.width = rect.right;
	m_Camera.height = rect.bottom;

	SelectObject(hDC, hOldFont);
	ReleaseDC(pDC);

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
/*
	//qglClearColor( 0.1f, 0.1f, 0.1f, 0.0f );
	//qglScissor( 0, 0, m_Camera.width, m_Camera.height );
	//qglClear( GL_COLOR_BUFFER_BIT );

	viewDef_t *viewDef = renderSystem->GetNewFrameViewdef();
	viewDef->renderView.x = 0;
	viewDef->renderView.y = 0;
	viewDef->renderView.width = m_Camera.width * 1.8;
	viewDef->renderView.height = m_Camera.height  * 1.55;
	viewDef->scissor.x1 = 0;
	viewDef->scissor.y1 = 0;
	viewDef->scissor.x2 = m_Camera.width  * 1.8;
	viewDef->scissor.y2 = m_Camera.height  * 1.55;
	viewDef->isEditor = true;
	//tr.viewDef->skipPostProcess = true;
	// render it
	renderSystem->BeginFrame( m_Camera.width * 1.2, m_Camera.height );

	memset( &refdef, 0, sizeof( refdef ) );
	refdef.vieworg = idVec3( 0, 0, 0 );

	m_Camera.angles = idAngles( 0, 0, 0 );

	// the editor uses opposite pitch convention
	refdef.viewaxis = idAngles( -m_Camera.angles.pitch, m_Camera.angles.yaw, m_Camera.angles.roll ).ToMat3();
	
	refdef.width = SCREEN_WIDTH;
	refdef.height = SCREEN_HEIGHT;
	refdef.fov_x = 90;
	refdef.fov_y = 2 * atan((float)m_Camera.height / m_Camera.width) * idMath::M_RAD2DEG;
	refdef.forceUpdate = true;

	// only set in animation mode to give a consistent look 
	//refdef.time = eventLoop->Milliseconds();

	g_qeglobals.testRenderWorld->RenderScene( &refdef );

	game->DrawDefferedPass( SCREEN_WIDTH, SCREEN_HEIGHT );
	renderSystem->EndFrame( &frontEnd, &backEnd );
	viewDef->isEditor = false;
	//tr.viewDef->skipPostProcess = false;

	//
	// bind back to the default texture so that we don't have problems elsewhere
	// using/modifying texture maps between contexts
	//
	globalImages->BindNull();

	qglFinish();
	QE_CheckOpenGLForErrors();
*/
}

/*
=================
bmModelStudioEditor::OnSize
=================
*/
void bmModelStudioEditor::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);

	//CRect	rect;
	//GetClientRect(rect);
	//m_Camera.width = rect.right;
	//m_Camera.height = rect.bottom;
	//GetDlgItem(IDC_MODELMANAGER_PREVIEW)->InvalidateRect(NULL, false);
}

/*
=================
OnFileFbxImportStatic
=================
*/
void bmModelStudioEditor::OnPaint( void ) {
	CPaintDC	dc(GetDlgItem(IDC_MODELMANAGER_PREVIEW));	// device context for painting
	bool		bPaint = true;

	if (!renderDevice->BindDeviceToWindow( dc.m_hDC )) {
		common->Printf("ERROR: wglMakeCurrent failed..\n ");
		common->Printf("Please restart " EDITOR_WINDOWTEXT " if the model studio previewer is not working\n");
	}
	else {
		QE_CheckOpenGLForErrors();
		
		OnUpdateModelPreview();

		QE_CheckOpenGLForErrors();
		renderDevice->SwapBuffers(dc.m_hDC);
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
void bmModelStudioEditor::OnFileFbxImportStatic( void ) {

	const char *fileName = Sys_OpenFileDialog( "Import Static FBX Model", "Autodesk FBX Model(*.fbx)||", "fbx");
	idStr modelPath;

	FbxExportToStaticMesh( fileName, modelPath );
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
		InitAfx();
		modelStudioEditor = new bmModelStudioEditor();
	}

	if ( modelStudioEditor->GetSafeHwnd() == NULL ) {
		modelStudioEditor->Create(IDD_MODELMODELDLG);
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
