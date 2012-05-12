#ifdef ID_X64
#undef  GetWindowLong
#undef  SetWindowLong
#undef SetClassLong
#undef LONG 
#define GWL_WNDPROC GWLP_WNDPROC

#define SetClassLong	SetClassLongPtr
#define GetWindowLong	GetWindowLongPtr
#define SetWindowLong	SetWindowLongPtr
#define GWL_USERDATA GWLP_USERDATA
#define GCL_HICON GCLP_HICON
#define DWL_MSGRESULT DWLP_MSGRESULT
#define DWL_DLGPROC DWLP_DLGPROC
#endif