using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ToolsManaged.Private
{
    public interface IMegaProject 
    {
        void LoadMegaProject(string mapname);
        bool RenderChart(int chartId, IntPtr _chart);
      

        
    }
}
