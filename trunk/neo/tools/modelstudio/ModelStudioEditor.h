// ModelStudioEditor.h
//
#pragma once

void FbxExportToStaticMesh( const char *meshpath, idStr &outPath );

/////////////////////////////////////////////////////////////////////////////
// CCDialogPDAEditor dialog

class bmModelStudioEditor : public CDialog {
public:
							bmModelStudioEditor(CWnd* pParent = NULL);   // standard constructor
							~bmModelStudioEditor();
	//{{AFX_VIRTUAL(CDialogPDAEditor)
	virtual BOOL			OnInitDialog();
	virtual void			DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
private:
	void					OnUpdateModelPreview( void );

	DECLARE_MESSAGE_MAP()
	camera_t m_Camera;
	CWnd previewWindow;
private:
	//{{AFX_DATA(CDialogPDAEditor)
	enum					{ IDD = IDD_MODELMODELDLG };

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnFileFbxImportStatic( void );
};