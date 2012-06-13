using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private
{
    // trace result
   public struct traceManaged_t {
	   public float					fraction;		// fraction of movement completed, 1.0 = didn't hit anything
       public float endposx;
       public float endposy;
       public float endposz;
       public int entNum;

       public float normalx;
       public float normaly;
       public float normalz;
   };

    public class CollisionModel
    {
        private int _cmHandle;

        public CollisionModel(int handle)
        {
            _cmHandle = handle;
        }
    }

    public class CollisionModelManager
    {
        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_CM_LoadMap")]
        private static extern int TOOLAPI_CM_LoadMap(string mapName);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_CM_TraceProjectedRay")]
        private static extern bool TOOLAPI_CM_TraceProjectedRay(ref traceManaged_t trace, float startX, float startY, float startZ, float yaw, float pitch, float roll, float dist);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_CM_TraceProjectedRayEx")]
        private static extern int TOOLAPI_CM_TraceProjectedRayEx(float startX, float startY, float startZ, float endPosX, float endPosY, float endPosZ, float dist);

        public static int TraceProjectedRayEx(float startX, float startY, float startZ, float endPosX, float endPosY, float endPosZ, float dist)
        {
            int entityId = TOOLAPI_CM_TraceProjectedRayEx(startX, startY, startZ, endPosX, endPosY, endPosZ, dist);

            return entityId;
        }


        public static bool TraceProjectedRay(ref traceManaged_t trace, float startX, float startY, float startZ, float endPosX, float endPosY, float endPosZ, float dist)
        {
            return TOOLAPI_CM_TraceProjectedRay(ref trace, startX, startY, startZ, endPosX, endPosY, endPosZ, dist);
        }

        public static CollisionModel LoadMap(string mapName)
        {
            int handle = TOOLAPI_CM_LoadMap("maps" + mapName + ".entities");

            return new CollisionModel(handle);
        }
    }
}
