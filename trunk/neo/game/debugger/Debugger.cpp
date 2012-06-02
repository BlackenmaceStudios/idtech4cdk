#include "precompiled.h"
#pragma hdrstop

#include "../../tools/debugger/DebuggerNet.h"
#include "../../tools/debugger/DebuggerServer.h"

rvDebuggerServer *gDebuggerServer;
int gDebuggerServerThreadID = 0;
HANDLE gDebuggerServerThread = 0;
bool gDebuggerServerQuit = false;

/*
================
DebuggerServerThread

Thread proc for the debugger server
================
*/
DWORD CALLBACK DebuggerServerThread ( LPVOID param )
{
	assert ( gDebuggerServer );
	
	while ( !gDebuggerServerQuit )
	{
		gDebuggerServer->ProcessMessages ( );
		Sleep ( 1 );
	}

	return 0;
}

/*
================
DebuggerServerInit

Starts up the debugger server
================
*/
bool DebuggerServerInit ( void )
{
	// Allocate the new debugger server
	gDebuggerServer = new rvDebuggerServer;
	if ( !gDebuggerServer )
	{
		return false;
	}
	
	// Initialize the debugger server
	if ( !gDebuggerServer->Initialize ( ) )
	{
		delete gDebuggerServer;
		gDebuggerServer = NULL;
		return false;
	}
	
	// Start the debugger server thread
	gDebuggerServerThread = CreateThread ( NULL, 0, DebuggerServerThread, 0, 0, (LPDWORD)&gDebuggerServerThreadID );
	
	return true;
}

/*
================
DebuggerServerShutdown

Shuts down the debugger server
================
*/
void DebuggerServerShutdown ( void )
{
	if ( gDebuggerServerThread )
	{
		// Signal the debugger server to quit
		gDebuggerServerQuit = true;
		
		// Wait for the thread to finish
		WaitForSingleObject ( gDebuggerServerThread, INFINITE );
		
		// Shutdown the server now
		gDebuggerServer->Shutdown();

		delete gDebuggerServer;
		gDebuggerServer = NULL;
		
		// Cleanup the thread handle
		CloseHandle ( gDebuggerServerThread );
		gDebuggerServerThread = NULL;
	}
}

/*
================
DebuggerServerCheckBreakpoint

Check to see if there is a breakpoint associtated with this statement
================
*/
void DebuggerServerCheckBreakpoint ( idInterpreter* interpreter, idProgram* program, int instructionPointer )
{
	if ( !gDebuggerServer )
	{
		return;
	}
	
	gDebuggerServer->CheckBreakpoints ( interpreter, program, instructionPointer );
}

/*
================
DebuggerServerPrint

Sends a print message to the debugger client
================
*/
void DebuggerServerPrint ( const char* text )
{
	if ( !gDebuggerServer )
	{
		return;
	}
	
	gDebuggerServer->Print ( text );
}

