using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using ToolsManaged.Private;

namespace ToolsManaged.Private
{
    //
    // MegaProjectChartPixel
    //
    [StructLayout(LayoutKind.Sequential)]
    struct MegaProjectChartPixel
    {
        public byte tileNum;
        public byte tileTransX;
        public byte tileTransY;
        public byte paintLevel;
    }

    //
    // MegaProjectChart
    //
    unsafe struct MegaProjectChart
    {
        int _chartDimen;
        MegaProjectChartPixel* _pixels;

        public MegaProjectChart(int chartDimen)
        {
            _pixels = (MegaProjectChartPixel*)Marshal.AllocHGlobal(chartDimen * chartDimen * 4);
            _chartDimen = chartDimen;
        }

        public MegaProjectChart(NativeAPI.File f)
        {
            _chartDimen = 512;
            _pixels = (MegaProjectChartPixel*)Marshal.AllocHGlobal(_chartDimen * _chartDimen * 4);

            f.ReadUnsafe(_pixels, _chartDimen * _chartDimen * 4);
        }

        public void Write(NativeAPI.File f)
        {
            f.WriteUnsafe(_pixels, _chartDimen * _chartDimen * 4);
        }
    }
}
