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

#include "precompiled.h"
#pragma hdrstop

#include "qe3.h"
#include "radiant.h"
#include "MainFrm.h"
#include "lightdlg.h"

#include <process.h>    // for _beginthreadex and _endthreadex
#include <ddeml.h>  // for MSGF_DDEMGR

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

idCVar radiant_entityMode( "radiant_entityMode", "0", CVAR_TOOL | CVAR_ARCHIVE, "" );

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp

BEGIN_MESSAGE_MAP(CRadiantApp, CWinApp)
	//{{AFX_MSG_MAP(CRadiantApp)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp construction

CRadiantApp::CRadiantApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRadiantApp object

CRadiantApp theApp;
HINSTANCE g_DoomInstance = NULL;
bool g_editorAlive = false;



/*
=================
RadiantInitTestWindow
=================
*/
void RadiantInitTestWindow( void ) {
	g_qeglobals.testRenderWorld = renderSystem->AllocRenderWorld();
	g_qeglobals.testRenderWorld->InitFromMap( "maps/editor/EditorBox" );
	
	// Load the editor box map.
	g_qeglobals.testRenderWorldMap = new idMapFile();
	if(!g_qeglobals.testRenderWorldMap->Parse( "maps/editor/EditorBox" )) {
		common->Warning( "Failed to load editorbox map file\n");
		return;
	}

	// Find all the lights
	for(int i = 0; i < g_qeglobals.testRenderWorldMap->GetNumEntities(); i++ ) {
		idMapEntity *ent = g_qeglobals.testRenderWorldMap->GetEntity( i );

		if(!strcmp(ent->epairs.GetString( "classname" ), "light")) {
			renderLight_t light;
			gameEdit->ParseSpawnArgsToRenderLight( &ent->epairs, &light );
			g_qeglobals.testRenderWorld->AddLightDef( &light );
		}
		else if(!strcmp(ent->epairs.GetString( "classname" ), "editor_testmodel" )) {

			ent->epairs.Set( "angle", "180" );
			ent->epairs.Set( "model", "models/mapobjects/testbox/testbox.md5staticmesh" );
			gameEdit->ParseSpawnArgsToRenderEntity( &ent->epairs, &g_qeglobals.testModel );
			g_qeglobals.testModelWorldHandle = g_qeglobals.testRenderWorld->AddEntityDef( &g_qeglobals.testModel );
		}
	}
}

/*
=================
RadiantInit

This is also called when you 'quit' in doom
=================
*/
void RadiantInit( void ) {

	// make sure the renderer is initialized
	if ( !renderSystem->IsOpenGLRunning() ) {
		common->Printf( "no OpenGL running\n" );
		return;
	}

	g_editorAlive = true;

	// allocate a renderWorld and a soundWorld
	if ( g_qeglobals.rw == NULL ) {
// jmarshall
	//	g_qeglobals.defaultEditorMaterial = declManager->FindMaterial( "_editordefault" );
		RadiantInitTestWindow();
// jmarshall end
		g_qeglobals.rw = renderSystem->AllocRenderWorld();
		g_qeglobals.rw->InitFromMap( NULL );
	}
	if ( g_qeglobals.sw == NULL ) {
		g_qeglobals.sw = soundSystem->AllocSoundWorld( g_qeglobals.rw );
	}

	if ( g_DoomInstance ) {
		if ( ::IsWindowVisible( win32.hWnd ) ) {
			::ShowWindow( win32.hWnd, SW_HIDE );
			g_pParentWnd->ShowWindow( SW_SHOW );
			g_pParentWnd->SetFocus();
		}
	} else {
		sys->GrabMouseCursor( false );

		g_DoomInstance = win32.hInstance;
		CWinApp* pApp = AfxGetApp();
		CWinThread *pThread = AfxGetThread();

		InitAfx();

		

		// App global initializations (rare)
		pApp->InitApplication();

		// Perform specific initializations
		pThread->InitInstance();

	

		qglFinish();
		//qwglMakeCurrent(0, 0);
		renderDevice->BindDeviceToWindow(win32.hDC);

		// hide the doom window by default
		::ShowWindow( win32.hWnd, SW_HIDE );

// jmarshall
		
		toolInterfaceLocal.ShowDebugConsole();
// jmarshall end
	}
}


extern void Map_VerifyCurrentMap(const char *map);

void RadiantSync( const char *mapName, const idVec3 &viewOrg, const idAngles &viewAngles ) {
	if ( g_DoomInstance == NULL ) {
		RadiantInit();
	}

	if ( g_DoomInstance ) {
		idStr osPath;
		osPath = fileSystem->RelativePathToOSPath( mapName );
		Map_VerifyCurrentMap( osPath );
		idAngles flip = viewAngles;
		flip.pitch = -flip.pitch;
		g_pParentWnd->GetCamera()->SetView( viewOrg, flip );
		g_pParentWnd->SetFocus();
		Sys_UpdateWindows( W_ALL );
		g_pParentWnd->RoutineProcessing();
	}
}

void RadiantRun( void ) {
	static bool exceptionErr = false;
	int show = ::IsWindowVisible(win32.hWnd);

	try {
		if (!exceptionErr && !show) {
			//qglPushAttrib(GL_ALL_ATTRIB_BITS);
			qglDepthMask(true);
			theApp.Run();
			//qglPopAttrib();
			//qwglMakeCurrent(0, 0);
			renderDevice->BindDeviceToWindow(win32.hDC);
		}
	}
	catch( idException &ex ) {
		::MessageBox(NULL, ex.error, "Exception error", MB_OK);
		RadiantShutdown();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp initialization

HINSTANCE g_hOpenGL32 = NULL;
HINSTANCE g_hOpenGL = NULL;
bool g_bBuildList = false;

BOOL CRadiantApp::InitInstance()
{
	

	// If there's a .INI file in the directory use it instead of registry

	char RadiantPath[_MAX_PATH];
	GetModuleFileName( NULL, RadiantPath, _MAX_PATH );

	// search for exe
	CFileFind Finder;
	Finder.FindFile( RadiantPath );
	Finder.FindNextFile();
	// extract root
	CString Root = Finder.GetRoot();
	// build root\*.ini
	CString IniPath = Root + "\\REGISTRY.INI";
	// search for ini file
	Finder.FindNextFile();

	char				dllPath[ MAX_OSPATH ];
	fileSystem->FindDLL( "tools/Tools", dllPath, false );
	if ( !dllPath[ 0 ] ) {
		return NULL;
	}
	m_pszAppName = dllPath;
	if (Finder.FindFile( IniPath ))
	{
		Finder.FindNextFile();
		// use the .ini file instead of the registry
		free((void*)m_pszProfileName);
		m_pszProfileName=_tcsdup(_T(Finder.GetFilePath()));
		// look for the registry key for void* buffers storage ( these can't go into .INI files )
		int i=0;
		CString key;
		HKEY hkResult;
		DWORD dwDisp;
		DWORD type;
		char iBuf[3];
		do
		{
			sprintf( iBuf, "%d", i );
			key = "Software\\Q3Radiant\\IniPrefs" + CString(iBuf);
			// does this key exists ?
			if ( RegOpenKeyEx( HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, &hkResult ) != ERROR_SUCCESS )
			{
				// this key doesn't exist, so it's the one we'll use
				strcpy( g_qeglobals.use_ini_registry, key.GetBuffer(0) );
				RegCreateKeyEx( HKEY_CURRENT_USER, key, 0, NULL, 
					REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisp );
				RegSetValueEx( hkResult, "RadiantName", 0, REG_SZ, reinterpret_cast<CONST BYTE *>(RadiantPath), strlen( RadiantPath )+1 );
				RegCloseKey( hkResult );
				break;
			}
			else
			{
				char RadiantAux[ _MAX_PATH ];
				unsigned long size = _MAX_PATH;
				// the key exists, is it the one we are looking for ?
				RegQueryValueEx( hkResult, "RadiantName", 0, &type, reinterpret_cast<BYTE *>(RadiantAux), &size );
				RegCloseKey( hkResult );
				if ( !strcmp( RadiantAux, RadiantPath ) )
				{
					// got it !
					strcpy( g_qeglobals.use_ini_registry, key.GetBuffer(0) );
					break;
				}
			}
			i++;
		} while (1);
		g_qeglobals.use_ini = true;
	}
	else
	{
		// Change the registry key under which our settings are stored.
		SetRegistryKey( EDITOR_REGISTRY_KEY );
		g_qeglobals.use_ini = false;
	}
	m_hInstance = win32.hInstance;
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

//	CMultiDocTemplate* pDocTemplate;
//	pDocTemplate = new CMultiDocTemplate(
//		IDR_RADIANTYPE,
//		RUNTIME_CLASS(CRadiantDoc),
//		RUNTIME_CLASS(CMainFrame), // custom MDI child frame
//		RUNTIME_CLASS(CRadiantView));
//	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	//afxCurrentWinApp = this;
	g_PrefsDlg.LoadPrefs();

	qglEnableClientState( GL_VERTEX_ARRAY );

	CString strTemp = m_lpCmdLine;
	strTemp.MakeLower();
	if (strTemp.Find("builddefs") >= 0) {
		g_bBuildList = true;
	}

	//afxCurrentInstanceHandle = win32.hInstance;
   // afxCurrentResourceHandle = win32.hInstance;

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MENU_QUAKE3)) {
		return FALSE;
	}

	if (pMainFrame->m_hAccelTable) {
		::DestroyAcceleratorTable(pMainFrame->m_hAccelTable);
	}
  
	pMainFrame->LoadAccelTable(MAKEINTRESOURCE(IDR_MINIACCEL));

	m_pMainWnd = pMainFrame;

	

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp commands

int CRadiantApp::ExitInstance() 
{
	g_pParentWnd = NULL;
	int ret = CWinApp::ExitInstance();

	// jmarshall
	cmdSystem->BufferCommandText(CMD_EXEC_INSERT, "quit\n");

	return ret;
}


BOOL CRadiantApp::OnIdle(LONG lCount) {
	if (g_pParentWnd) {
		g_pParentWnd->RoutineProcessing();
	}
	return FALSE;
	//return CWinApp::OnIdle(lCount);
}

void CRadiantApp::OnHelp()
{
	ShellExecute(m_pMainWnd->GetSafeHwnd(), "open", "http://www.idDevNet.com", NULL, NULL, SW_SHOW);
}

int CRadiantApp::Run( void ) 
{
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;


#if _MSC_VER >= 1300
	MSG *msg = AfxGetCurrentMessage();			// TODO Robert fix me!!
#else
	MSG *msg = &m_msgCur;
#endif

	// phase1: check to see if we can do idle work
	while (bIdle &&	!::PeekMessage(msg, NULL, NULL, NULL, PM_NOREMOVE)) {
		// call OnIdle while in bIdle state
		if (!OnIdle(lIdleCount++)) {
			bIdle = FALSE; // assume "no idle" state
		}
	}

	// phase2: pump messages while available
	do {
		// pump message, but quit on WM_QUIT
		if (!PumpMessage()) {
			return ExitInstance();
		}

		// reset "no idle" state after pumping "normal" message
		if (IsIdleMessage(msg)) {
			bIdle = TRUE;
			lIdleCount = 0;
		}

	} while (::PeekMessage(msg, NULL, NULL, NULL, PM_NOREMOVE));

	return 0;
}


/*
=============================================================

REGISTRY INFO

=============================================================
*/

bool SaveRegistryInfo(const char *pszName, void *pvBuf, long lSize)
{
	SetCvarBinary(pszName, pvBuf, lSize);
	common->WriteFlaggedCVarsToFile( "editor.cfg", CVAR_TOOL, "sett" );
	return true;
}

bool LoadRegistryInfo(const char *pszName, void *pvBuf, long *plSize)
{
	return GetCvarBinary(pszName, pvBuf, *plSize);
}

bool SaveWindowState(HWND hWnd, const char *pszName)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	if (hWnd != g_pParentWnd->GetSafeHwnd()) {
	    if (::GetParent(hWnd) != g_pParentWnd->GetSafeHwnd()) {
	      ::SetParent(hWnd, g_pParentWnd->GetSafeHwnd());
	    }
		MapWindowPoints(NULL, g_pParentWnd->GetSafeHwnd(), (POINT *)&rc, 2);
	}
	return SaveRegistryInfo(pszName, &rc, sizeof(rc));
}


bool LoadWindowState(HWND hWnd, const char *pszName)
{
	RECT rc;
	LONG lSize = sizeof(rc);

	if (LoadRegistryInfo(pszName, &rc, &lSize))
	{
		if (rc.left < 0)
			rc.left = 0;
		if (rc.top < 0)
			rc.top = 0;
		if (rc.right < rc.left + 16)
			rc.right = rc.left + 16;
		if (rc.bottom < rc.top + 16)
			rc.bottom = rc.top + 16;

		MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
		return true;
	}

	return false;
}

/*
===============================================================

  STATUS WINDOW

===============================================================
*/

void Sys_UpdateStatusBar( void )
{
	extern int   g_numbrushes, g_numentities;

	char numbrushbuffer[100] = "";

	sprintf( numbrushbuffer, "Brushes: %d Entities: %d", g_numbrushes, g_numentities );
	Sys_Status( numbrushbuffer, 2 );
}

void Sys_Status(const char *psz, int part )
{
	if ( part < 0 ) {
		common->Printf("%s", psz);
		part = 0;
	}
	g_pParentWnd->SetStatusText(part, psz);
}
