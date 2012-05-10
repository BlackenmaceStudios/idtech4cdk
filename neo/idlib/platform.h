// platform.h
//

// jmarshall



// ------------------------------------------
#ifdef _WIN64
#define ID_X64
#define ID_USE_INLINEASM		0

#define ID_PLATFORM				"64-bit"
#define	CPUSTRING				"x64"
#undef _USE_32BIT_TIME_T 
#define ID_EDITOR_BOOL			INT_PTR
#define ID_SYS_HANDLE			__int64
#else

#define ID_PLATFORM				"32-bit"
#define	CPUSTRING				"x86"
#define ID_USE_INLINEASM		1
#define ID_EDITOR_BOOL			BOOL
#define ID_SYS_HANDLE			long
#endif
// -------------------------------------------

#define DIRECTINPUT_VERSION  0x0800			// was 0x0700 with the old mssdk
#define DIRECTSOUND_VERSION  0x0800

#define WINVER				0x501


// jmarshall end