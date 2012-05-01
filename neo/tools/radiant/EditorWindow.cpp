// EditorWindow.cpp
//

#include "precompiled.h"
#include "qe3.h"
#include "radiant.h"
#include "MainFrm.h"
#include "lightdlg.h"
int idManagedEditorWindow::OnCreate(LPCREATESTRUCT lpCreateStruct){ return 0; }
void idManagedEditorWindow::OnLButtonDown(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnMButtonDown(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnRButtonDown(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnLButtonUp(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnMButtonUp(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnRButtonUp(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnMouseMove(UINT nFlags, CPoint point){ }
void idManagedEditorWindow::OnPaint(){ }
void idManagedEditorWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){ }
void idManagedEditorWindow::OnSize(UINT nType, int cx, int cy){ }
void idManagedEditorWindow::OnDestroy(){ }
void idManagedEditorWindow::OnSelectMouserotate(){ }
void idManagedEditorWindow::OnTimer(UINT_PTR nIDEvent){ }
void idManagedEditorWindow::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){ }
void idManagedEditorWindow::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp){ }
void idManagedEditorWindow::OnKillFocus(CWnd* pNewWnd){ }
void idManagedEditorWindow::OnSetFocus(CWnd* pOldWnd){ }
void idManagedEditorWindow::OnClose(){ }
BOOL idManagedEditorWindow::OnEraseBkgnd(CDC* pDC){ return false; }
void idManagedEditorWindow::OnDropNewmodel(){ }
BOOL idManagedEditorWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt){ return false; }
BOOL idManagedEditorWindow::OnCmdMsg( UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo ){ return false; }

void idManagedEditorWindow::OnAddEntityEvent( const char *entityType, CPoint pt ) {

}

void idManagedEditorWindow::OpenEntityContextMenu( idDict *entDict ) {

	window->OpenEntityContextMenu( (__int64)entDict );
}

HWND idManagedEditorWindow::GetSafeHwnd( void ) {
	HWND hwnd = { 0 };
	window->GetSafeHandle((__int64 *)&hwnd);
	return (HWND)hwnd;
}

void idManagedEditorWindow::GetClientRect( CRect &rect ) {
	ToolsManaged::RECT r;
	window->GetScreenRect( &r );
	memcpy(&rect, &r, sizeof(ToolsManaged::RECT));
}

void idManagedEditorWindow::ShowWindow( int vis ) {

}

bool idManagedEditorWindow::IsWindowVisible( void ) {
	return true;
}

BOOL idManagedEditorWindow::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) {
	return FALSE;
}

BOOL idManagedEditorWindow::Create(LPCTSTR id, LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) {
	_bstr_t bstrt(va("%s", id)); 
	
	toolsManaged->CreateEditorWindow( bstrt, (__int64)this, &window );
	window->Init();
	return TRUE;
}

BOOL idManagedEditorWindow::RedrawWindow(LPCRECT lpRectUpdate, CRgn* prgnUpdate, UINT flags) {
	if(::IsWindowVisible( win32.hWnd ) ) {
		return FALSE;
	}
	window->Redraw();
	return TRUE;
}

void idManagedEditorWindow::Destroy( void ) {

}

void idManagedEditorWindow::SetCapture( void ) {
	::SetCapture( GetSafeHwnd() );
}

void idManagedEditorWindow::BringWindowToTop( void ) {
	::SetForegroundWindow( GetSafeHwnd() );
}

void idManagedEditorWindow::SetFocus( void ) {
//	::SetFocus( GetSafeHwnd() );
}

bool idManagedEditorWindow::OpenClipboard( void ) {
	return ::OpenClipboard( GetSafeHwnd() );
}

void idManagedEditorWindow::InvalidateRect( void *rect, bool repaint ) {
//	window->Redraw();
}

idManagedEditorWindow *idManagedEditorWindow::GetCapture( void ) {
	return NULL;
}

int idManagedEditorWindow::SetTimer( int v1, int v2, void *callback ) {
	return ::SetTimer(GetSafeHwnd(), v1, v2, (TIMERPROC)callback );
}

void idManagedEditorWindow::KillTimer( int handle ) {
	::KillTimer( GetSafeHwnd(), handle );
}

HDC idManagedEditorWindow::GetDC( void ) {
	return ::GetDC( GetSafeHwnd() );
}

idManagedEditorWindow *idManagedEditorWindow::GetTopWindow(void) {
	if(::GetForegroundWindow() != GetSafeHwnd()) {
		return NULL;
	}
	return this;
}

void idManagedEditorWindow::SetWindowText( const char *p ) {
	_bstr_t bstrt(p);

	window->SetWindowText( bstrt );
}