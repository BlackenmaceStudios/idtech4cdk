// ModelStudioEditor.h
//
#pragma once

bool ObjExportToStaticMesh( const char *meshpath, idStr &outPath );
bool FbxExportToStaticMesh( const char *meshpath, idStr &outPath );

/////////////////////////////////////////////////////////////////////////////
// CCDialogPDAEditor dialog

class bmModelStudioEditor : public idManagedEditorWindow {
public:
							bmModelStudioEditor(CWnd* pParent = NULL);   // standard constructor
							~bmModelStudioEditor();
	//{{AFX_VIRTUAL(CDialogPDAEditor)
	virtual int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void			DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void			OnNamedEvent( const char *eventName );
private:
	void					OnUpdateModelPreview( void );


	camera_t m_Camera;
private:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnFileFbxImportStatic( void );
};