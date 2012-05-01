// ManagedDialog.h
//

//
// bmManagedDialog
//
__gc class bmManagedDialog : public bmEditorNative {
public:
	bmManagedDialog::bmManagedDialog( const wchar_t *name );

	virtual void OnInit( bmEditorInterface *editor ){ }
	virtual void Paint(){ }
	virtual System::IntPtr GetParentHwnd( void ) { return (System::IntPtr)AfxGetMainWnd()->GetSafeHwnd(); };
protected:
	bmEditorInterface *_class;
};