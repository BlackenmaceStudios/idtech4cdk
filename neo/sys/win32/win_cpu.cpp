/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "win_local.h"


/*
==============================================================

	Clock ticks

==============================================================
*/


/*
================
Sys_GetClockTicks
================
*/
double Sys_GetClockTicks( void ) {
#if !ID_USE_INLINEASM

	LARGE_INTEGER li;

	QueryPerformanceCounter( &li );
	return (double ) li.LowPart + (double) 0xFFFFFFFF * li.HighPart;

#else

	unsigned long lo, hi;

	__asm {
		push ebx
		xor eax, eax
		cpuid
		rdtsc
		mov lo, eax
		mov hi, edx
		pop ebx
	}
	return (double ) lo + (double) 0xFFFFFFFF * hi;

#endif
}

/*
================
Sys_ClockTicksPerSecond
================
*/
double Sys_ClockTicksPerSecond( void ) {
	static double ticks = 0;
#if !ID_USE_INLINEASM

	if ( !ticks ) {
		LARGE_INTEGER li;
		QueryPerformanceFrequency( &li );
		ticks = li.QuadPart;
	}

#else

	if ( !ticks ) {
		HKEY hKey;
		LPBYTE ProcSpeed;
		DWORD buflen, ret;

		if ( !RegOpenKeyEx( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey ) ) {
			ProcSpeed = 0;
			buflen = sizeof( ProcSpeed );
			ret = RegQueryValueEx( hKey, "~MHz", NULL, NULL, (LPBYTE) &ProcSpeed, &buflen );
			// If we don't succeed, try some other spellings.
			if ( ret != ERROR_SUCCESS ) {
				ret = RegQueryValueEx( hKey, "~Mhz", NULL, NULL, (LPBYTE) &ProcSpeed, &buflen );
			}
			if ( ret != ERROR_SUCCESS ) {
				ret = RegQueryValueEx( hKey, "~mhz", NULL, NULL, (LPBYTE) &ProcSpeed, &buflen );
			}
			RegCloseKey( hKey );
			if ( ret == ERROR_SUCCESS ) {
				ticks = (double) ((unsigned long)ProcSpeed) * 1000000;
			}
		}
	}

#endif
	return ticks;
}


/*
==============================================================

	CPU

==============================================================
*/

/*
================
HasCPUID
================
*/
static bool HasCPUID( void ) {
#if ID_USE_INLINEASM
	__asm 
	{
		pushfd						// save eflags
		pop		eax
		test	eax, 0x00200000		// check ID bit
		jz		set21				// bit 21 is not set, so jump to set_21
		and		eax, 0xffdfffff		// clear bit 21
		push	eax					// save new value in register
		popfd						// store new value in flags
		pushfd
		pop		eax
		test	eax, 0x00200000		// check ID bit
		jz		good
		jmp		err					// cpuid not supported
set21:
		or		eax, 0x00200000		// set ID bit
		push	eax					// store new value
		popfd						// store new value in EFLAGS
		pushfd
		pop		eax
		test	eax, 0x00200000		// if bit 21 is on
		jnz		good
		jmp		err
	}

err:
	return false;
good:
	return true;
#else
	return false;
#endif
	
}

#define _REG_EAX		0
#define _REG_EBX		1
#define _REG_ECX		2
#define _REG_EDX		3

/*
================
CPUID
================
*/
static void CPUID( int func, unsigned regs[4] ) {
	unsigned regEAX, regEBX, regECX, regEDX;
#if ID_USE_INLINEASM
	__asm pusha
	__asm mov eax, func
	__asm __emit 00fh
	__asm __emit 0a2h
	__asm mov regEAX, eax
	__asm mov regEBX, ebx
	__asm mov regECX, ecx
	__asm mov regEDX, edx
	__asm popa

	regs[_REG_EAX] = regEAX;
	regs[_REG_EBX] = regEBX;
	regs[_REG_ECX] = regECX;
	regs[_REG_EDX] = regEDX;
#endif
}


/*
================
IsAMD
================
*/
static bool IsAMD( void ) {
	
	return false;
}

/*
================
HasCMOV
================
*/
static bool HasCMOV( void ) {
	
	return false;
}

/*
================
Has3DNow
================
*/
static bool Has3DNow( void ) {
	

	return false;
}

/*
================
HasMMX
================
*/
static bool HasMMX( void ) {
	
	return false;
}

/*
================
HasSSE
================
*/
static bool HasSSE( void ) {
	
	return false;
}

/*
================
HasSSE2
================
*/
static bool HasSSE2( void ) {
	
	return false;
}

/*
================
HasSSE3
================
*/
static bool HasSSE3( void ) {
	
	return false;
}

/*
================
LogicalProcPerPhysicalProc
================
*/
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
                                          // processors per physical processor when execute cpuid with 
                                          // eax set to 1
static unsigned char LogicalProcPerPhysicalProc( void ) {
	return 0;
}

/*
================
GetAPIC_ID
================
*/
#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
                                          // initial APIC ID for the processor this code is running on.
                                          // Default value = 0xff if HT is not supported
static unsigned char GetAPIC_ID( void ) {
return 0;
}

/*
================
CPUCount

	logicalNum is the number of logical CPU per physical CPU
    physicalNum is the total number of physical processor
	returns one of the HT_* flags
================
*/
#define HT_NOT_CAPABLE				0
#define HT_ENABLED					1
#define HT_DISABLED					2
#define HT_SUPPORTED_NOT_ENABLED	3
#define HT_CANNOT_DETECT			4

int CPUCount( int &logicalNum, int &physicalNum ) {
	return 1;
}

/*
================
HasHTT
================
*/
static bool HasHTT( void ) {
	
	return true;
}

/*
================
HasHTT
================
*/
static bool HasDAZ( void ) {
	return false;
}

/*
================
Sys_GetCPUId
================
*/
cpuid_t Sys_GetCPUId( void ) {
	return (cpuid_t)0;
}


/*
===============================================================================

	FPU

===============================================================================
*/

typedef struct bitFlag_s {
	char *		name;
	int			bit;
} bitFlag_t;

static byte fpuState[128], *statePtr = fpuState;
static char fpuString[2048];
static bitFlag_t controlWordFlags[] = {
	{ "Invalid operation", 0 },
	{ "Denormalized operand", 1 },
	{ "Divide-by-zero", 2 },
	{ "Numeric overflow", 3 },
	{ "Numeric underflow", 4 },
	{ "Inexact result (precision)", 5 },
	{ "Infinity control", 12 },
	{ "", 0 }
};
static char *precisionControlField[] = {
	"Single Precision (24-bits)",
	"Reserved",
	"Double Precision (53-bits)",
	"Double Extended Precision (64-bits)"
};
static char *roundingControlField[] = {
	"Round to nearest",
	"Round down",
	"Round up",
	"Round toward zero"
};
static bitFlag_t statusWordFlags[] = {
	{ "Invalid operation", 0 },
	{ "Denormalized operand", 1 },
	{ "Divide-by-zero", 2 },
	{ "Numeric overflow", 3 },
	{ "Numeric underflow", 4 },
	{ "Inexact result (precision)", 5 },
	{ "Stack fault", 6 },
	{ "Error summary status", 7 },
	{ "FPU busy", 15 },
	{ "", 0 }
};

/*
===============
Sys_FPU_PrintStateFlags
===============
*/
int Sys_FPU_PrintStateFlags( char *ptr, int ctrl, int stat, int tags, int inof, int inse, int opof, int opse ) {
	
	return 0;
}

/*
===============
Sys_FPU_StackIsEmpty
===============
*/
bool Sys_FPU_StackIsEmpty( void ) {
	
	return true;
}

/*
===============
Sys_FPU_ClearStack
===============
*/
void Sys_FPU_ClearStack( void ) {
	
}

/*
===============
Sys_FPU_GetState

  gets the FPU state without changing the state
===============
*/
const char *Sys_FPU_GetState( void ) {
	return "64bit CPU";
}

/*
===============
Sys_FPU_EnableExceptions
===============
*/
void Sys_FPU_EnableExceptions( int exceptions ) {
	
}

/*
===============
Sys_FPU_SetPrecision
===============
*/
void Sys_FPU_SetPrecision( int precision ) {
	
}

/*
================
Sys_FPU_SetRounding
================
*/
void Sys_FPU_SetRounding( int rounding ) {
	
}

/*
================
Sys_FPU_SetDAZ
================
*/
void Sys_FPU_SetDAZ( bool enable ) {
	
}

/*
================
Sys_FPU_SetFTZ
================
*/
void Sys_FPU_SetFTZ( bool enable ) {
	
}
