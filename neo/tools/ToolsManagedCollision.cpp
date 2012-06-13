#include "precompiled.h"

#include "radiant/QERTYPES.H"
#include "radiant/editorEntity.h"
#include "radiant/EditorMap.h"

// jmarshall
#include "tools_managed.h"
#include "../renderer/Image.h"
// jmarshall end




extern "C" __declspec(dllexport) cmHandle_t TOOLAPI_CM_LoadMap( const char *mapName )
{
	// Disconnect from a game if a game is currently running.
	cmdSystem->BufferCommandText( CMD_EXEC_NOW, "disconnect" );

	// Free any current maps that are open.
	collisionModelManager->FreeMap();

	gameEdit->LoadMapCollision( mapName );
	return 1;
}
extern renderView_t view;
extern "C" __declspec(dllexport) bool TOOLAPI_CM_TraceProjectedRay( ToolsManaged::traceManaged_t &traceManaged, float startX, float startY, float startZ, float endPosX, float endPosY, float endPosZ, float dist )
{
	idVec3 startPos, endPos;
	trace_t trace;

	startPos.x = startX;
	startPos.y = startY;
	startPos.z = startZ;

	endPos.x = endPosX;
	endPos.y = endPosY;
	endPos.z = endPosZ;

	idVec3 diff = endPos - startPos;

	endPos = endPos + diff.ToAngles().Normalize180().ToForward() * dist;

	// If the trace didn't hit anything 
	if(gameEdit->Trace( trace, -1, startPos, endPos)) {
		return false;
	}

	traceManaged.endposx = trace.endpos.x;
	traceManaged.endposy = trace.endpos.y;
	traceManaged.endposz = trace.endpos.z;
	traceManaged.entNum = trace.c.id;
	traceManaged.fraction = trace.fraction;

	traceManaged.normalx = trace.c.normal.x;
	traceManaged.normaly = trace.c.normal.y;
	traceManaged.normalz = trace.c.normal.z;

	return true;
}

extern "C" __declspec(dllexport) int TOOLAPI_CM_TraceProjectedRayEx( float startX, float startY, float startZ, float endPosX, float endPosY, float endPosZ, float dist )
{
	idVec3 startPos, endPos;
	trace_t trace;

	startPos.x = startX;
	startPos.y = startY;
	startPos.z = startZ;

	endPos.x = endPosX;
	endPos.y = endPosY;
	endPos.z = endPosZ;

	// If the trace didn't hit anything 
	if(gameEdit->Trace( trace, -1, startPos, endPos)) {
		return -1;
	}

	return trace.c.id;
}