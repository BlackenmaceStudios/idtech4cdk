// win_errordump.cpp
//

#include "precompiled.h"

#include "../sys_local.h"
#include <DbgHelp.h>

void GLimp_SafeShutdown( void );
void Sys_CrashDumpTerminateFunc( void ) {}



/*
===================
idSys::HandleCrashEvent
===================
*/

void idSysLocal::HandleCrashEvent( void ) {
	static bool inCrashEvent = false;
	CONTEXT currentContext;	

	memset(&currentContext, 0, sizeof(CONTEXT));

	// Incase we get dual crashes, don't recurse
	if(inCrashEvent)
	{
		
		GLimp_SafeShutdown();
	}
	inCrashEvent = true;

	//	Create the first stack frame.	
	STACKFRAME64 currentStackFrame;	
#if ID_USE_INLINEASM
	__asm	pop eax	__asm	mov currentContext.Eip, eax	
	__asm	mov currentContext.Ebp, ebp	
	__asm	mov currentContext.Esp, esp		
	currentContext.ContextFlags = CONTEXT_FULL;	
	
	
	
	memset( &currentStackFrame, 0, sizeof( STACKFRAME64 ) );	
	
	currentStackFrame.AddrPC.Offset         = currentContext.Eip;	
	currentStackFrame.AddrPC.Mode           = AddrModeFlat;	
	currentStackFrame.AddrStack.Offset      = currentContext.Esp;	
	currentStackFrame.AddrStack.Mode        = AddrModeFlat;	
	currentStackFrame.AddrFrame.Offset      = currentContext.Ebp;	
	currentStackFrame.AddrFrame.Mode        = AddrModeFlat;
#else
	DWORD machine = IMAGE_FILE_MACHINE_AMD64; 
  RtlCaptureContext (&currentContext); 
  currentStackFrame.AddrPC.Offset = currentContext.Rip; 
  currentStackFrame.AddrFrame.Offset = currentContext.Rsp; 
  currentStackFrame.AddrStack.Offset = currentContext.Rsp; 
  currentStackFrame.AddrPC.Mode = AddrModeFlat; 
  currentStackFrame.AddrFrame.Mode = AddrModeFlat; 
  currentStackFrame.AddrStack.Mode = AddrModeFlat; 

#endif
	bool hasSymbols = SymInitialize(GetCurrentProcess(),NULL,TRUE);
	if(hasSymbols) {
		common->Printf("Crash dump will contain symbols...\n");
	}
	else {
		common->Warning("Symbols aren't present for this build...\n");
	}

	//	Do the walking, making a back up of each frame as we go.	

	common->Printf("Stack Trace:\n");

	FILE *f = fopen("debugLastCrash.log", "w");
	fprintf(f, "Stack Trace: \n");
	while ( true )	{		
		if(!StackWalk64(	IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &currentStackFrame, &currentContext, NULL, SymFunctionTableAccess64,	SymGetModuleBase64,	NULL )) {
			break;
		}
		
		// Get the function name for the address.
		 #define MAX_STRING_LEN	(512)
		 unsigned char byBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_STRING_LEN] = { 0 };

		 IMAGEHLP_SYMBOL64 * pSymbol = (IMAGEHLP_SYMBOL64*)byBuffer;
		 pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		 pSymbol->MaxNameLength = MAX_STRING_LEN;

		 
		 if (hasSymbols && SymGetSymFromAddr64(GetCurrentProcess(), currentStackFrame.AddrPC.Offset, NULL, pSymbol))
		 {
			 if(!strcmp(pSymbol->Name, "WinMain")) {
				 break;
			 }
			common->Printf("...Function: %s Address: %p\n", pSymbol->Name,currentStackFrame.AddrPC.Offset);
			fprintf(f, "...Function: %s Address: %p\n", pSymbol->Name,currentStackFrame.AddrPC.Offset);
		 }
		 else {
			 common->Printf("...Function: <invalid> Address: %p\n", currentStackFrame.AddrPC.Offset);
			fprintf(f, "...Function: <invalid> Address: %p\n", currentStackFrame.AddrPC.Offset);
		 }
	}

	fclose( f );
	if(IsDebuggerPresent()) {
		DebugBreak();
		DebugSetProcessKillOnExit( TRUE );
		
	}

	Sys_Error( "A crash has a occurd please review the console for any information related to this crash");

	// Set the terminate function and terminate the app.

	terminate_function dumpptr = Sys_CrashDumpTerminateFunc;

	set_terminate(dumpptr);
	terminate();
}