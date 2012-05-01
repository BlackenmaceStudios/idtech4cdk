// EditorWindow.h
//

//
// idManagedEditorWindow
//
class idManagedEditorWindow {
public:
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnPaint();
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnDestroy();
	virtual void OnSelectMouserotate();
	virtual void OnTimer(UINT_PTR nIDEvent);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	virtual void OnKillFocus(CWnd* pNewWnd);
	virtual void OnSetFocus(CWnd* pOldWnd);
	virtual void OnClose();
	virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnDropNewmodel();
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnCmdMsg( UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo );
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	virtual BOOL Create(LPCTSTR id, LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	virtual BOOL RedrawWindow(LPCRECT lpRectUpdate = NULL, CRgn* prgnUpdate = NULL, UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	virtual void Destroy( void );
	virtual void OnAddEntityEvent( const char *entityType, CPoint pt );
	virtual void OnNamedEvent( const char *eventName );
public:
	void	OpenEntityContextMenu( idDict *entDict );
	void	SetCapture( void );
	void	BringWindowToTop( void );
	void	SetFocus( void );
	bool	OpenClipboard( void );
	void	InvalidateRect( void *rect, bool repaint );
	HWND	GetSafeHwnd( void );
	void	GetClientRect( CRect &rect );
	void	ShowWindow( int vis );
	bool	IsWindowVisible( void );

	int		SetTimer( int v1, int v2, void *callback );
	void    KillTimer( int handle );
	HDC		GetDC( void );
	void	SetWindowText( const char *p );

	idManagedEditorWindow *GetCapture( void );
	idManagedEditorWindow *GetTopWindow(void);
private:

	struct ToolsManaged::IEditorWindow	*window;
};