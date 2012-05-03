using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Runtime.InteropServices;

using ToolsManaged.Private.Editor;

namespace ToolsManaged.Private.idLib
{
    [EditorShowProperties]
    public class Vector3
    {
        public float x{ get; set; }
        public float y{ get; set; }
        public float z{ get; set; }


        public Vector3(bool nullParam = false)
        {
            x = 0;
            y = 0;
            z = 0;
        }

        public Vector3(string val)
        {
            string[] vec = val.Split(' ');
            x = float.Parse(vec[0]);
            y = float.Parse(vec[1]);
            z = float.Parse(vec[2]);
        }

        public override string ToString()
        {
            return x + " " + y + " " + z;
        }
    }

   
}
