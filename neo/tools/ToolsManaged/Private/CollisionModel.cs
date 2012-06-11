using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private
{
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

        public static CollisionModel LoadMap(string mapName)
        {
            int handle = TOOLAPI_CM_LoadMap(mapName);

            return new CollisionModel(handle);
        }
    }
}
