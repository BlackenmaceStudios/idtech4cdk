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

/*
===============================================================================

	Definitions for information that is related to a licensee's game name and location.

===============================================================================
*/

// jmarshall
// Developer builds have the cd-key check disabled(basically a nag message).
#ifdef _DEBUG
#ifdef JV_DEVELOPER_BUILD
#define ENGINE_BUILDTYPE					"DEBUG-Licensee"
#else
#define ENGINE_BUILDTYPE					"DEBUG"
#endif
#else
#define ENGINE_BUILDTYPE					"RELEASE"
#endif
// jmarshall end

#define GAME_NAME						"Deadly Law"

#ifdef JV_DEVELOPER_BUILD
#define GAME_WINDOW_TITLE				"Deadly Law ( " ENGINE_BUILDTYPE " )" " Build( " ID_PLATFORM " )" __DATE__ " " __TIME__		// appears on window titles and errors
#else
#define GAME_WINDOW_TITLE				"Deadly Law"		// appears on window titles and errors
#endif

#define ENGINE_VERSION					"Deadly Law( " ID_PLATFORM " ) 0.2312" 	// printed in console

// paths
#define	CD_BASEDIR						"DeadlyLaw"
#ifdef ID_DEMO_BUILD
	#define BASE_GAMEDIR					"demo"
#else
	#define	BASE_GAMEDIR					"base"
#endif

// filenames
#define	CD_EXE							"deadlylaw.exe"
#define CONFIG_FILE						"DeadlyLawConfig.cfg"

// base folder where the source code lives
#define SOURCE_CODE_BASE_FOLDER			"neo"


// default idnet host address
#ifndef IDNET_HOST
	#define IDNET_HOST					"some.host.addthis.here.com"
#endif

// default idnet master port
#ifndef IDNET_MASTER_PORT
	#define IDNET_MASTER_PORT			"27650"
#endif

// default network server port
#ifndef PORT_SERVER
	#define	PORT_SERVER					27666
#endif

// broadcast scan this many ports after PORT_SERVER so a single machine can run multiple servers
#define	NUM_SERVER_PORTS				4

// see ASYNC_PROTOCOL_VERSION
// use a different major for each game
#define ASYNC_PROTOCOL_MAJOR			2

// Savegame Version
// Update when you can no longer maintain compatibility with previous savegames
// NOTE: a seperate core savegame version and game savegame version could be useful
#define SAVEGAME_VERSION				3
#define RENDERDEMO_VERSION				1

// editor info
#define EDITOR_DEFAULT_PROJECT			"deadlylaw.qe4"
#define EDITOR_REGISTRY_KEY				"StudioRadiant"

#define EDITOR_WINDOWTEXT				"World Studio(" ENGINE_BUILDTYPE "-" ID_PLATFORM") for " GAME_NAME " Build " __DATE__ " " __TIME__ 

// win32 info
#define WIN32_CONSOLE_CLASS				"DeadlyLaw WinConsole"
#define	WIN32_WINDOW_CLASS_NAME			"DeadlyLaw"
#define	WIN32_FAKE_WINDOW_CLASS_NAME	"DEADLYLAW_WGL_FAKE"

// Linux info
#ifdef ID_DEMO_BUILD
	#define LINUX_DEFAULT_PATH			"/usr/local/games/deadlylaw-demo"
#else
	#define LINUX_DEFAULT_PATH			"/usr/local/games/deadlylaw"
#endif

// CD Key file info
// goes into BASE_GAMEDIR whatever the fs_game is set to
// two distinct files for easier win32 installer job
#define CDKEY_FILE						"deadlykey"
#define CDKEY_TEXT						"\n// Do not give this file to ANYONE.\n" \
										"// BadassMedia will NEVER ask you to send this file to them.\n"

#define CONFIG_SPEC						"config.spec"
