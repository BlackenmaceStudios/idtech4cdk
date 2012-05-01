#ifdef ID_X64
#undef  GetWindowLong
#undef  SetWindowLong

#define GWL_WNDPROC GWLP_WNDPROC

#define GetWindowLong	GetWindowLongPtr
#define SetWindowLong	SetWindowLongPtr
#define GWL_USERDATA GWLP_USERDATA
#define GCL_HICON GCLP_HICON
#define DWL_MSGRESULT DWLP_MSGRESULT
#define DWL_DLGPROC DWLP_DLGPROC
#endif