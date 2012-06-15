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
        public byte tileTransX;
        public byte tileTransY;
        public byte tileScale;
        public byte paintLevel;
    }

    //
    // MegaProjectChart
    //
    unsafe class MegaProjectChart
    {
        int _chartDimen;
        MegaProjectChartPixel* _pixels;
        public string materialName;
        public NativeAPI.idManagedImage _stencilImage;

        public MegaProjectChart(int chartDimen)
        {
            _pixels = (MegaProjectChartPixel*)Marshal.AllocHGlobal(chartDimen * chartDimen * 4);
            _chartDimen = chartDimen;
            materialName = "";
        }

        public MegaProjectChart(NativeAPI.File f)
        {
            _chartDimen = 512;
            _pixels = (MegaProjectChartPixel*)Marshal.AllocHGlobal(_chartDimen * _chartDimen * 4);
            materialName = "";
            f.ReadString(ref materialName);
            f.ReadUnsafe(_pixels, _chartDimen * _chartDimen * 4);

            if (materialName.Length > 0)
            {
                _stencilImage = NativeAPI.idManagedImage.GetDiffuseImageHandleForMaterial(materialName);
            }
        }

        public bool HasMaterial
        {
            get
            {
                return materialName.Length > 0;
            }
        }

        public void Write(NativeAPI.File f)
        {
            f.WriteString(materialName);
            f.WriteUnsafe(_pixels, _chartDimen * _chartDimen * 4);
        }

        public void UpdateToImage(NativeAPI.idManagedImage image)
        {
            image.CopyUncompressedBufferIntoRegion((System.IntPtr)_pixels, 0, 0, 0, _chartDimen, _chartDimen);
        }

        public void Blit(IntPtr brushData, int brushWidth, int brushHeight, float u, float v, byte transX, byte transY, byte scale)
        {
            u = u * _chartDimen;
            v = v * _chartDimen;

            if (u < 0)
                u = 0;

            if (u > _chartDimen)
                u = _chartDimen;

            if (v < 0)
                v = 0;

            if (v > _chartDimen)
                v = _chartDimen;

            NativeAPI.idManagedImage.CopyImageToImageBufferRegion((System.IntPtr)_pixels, brushData, (int)u, (int)v, brushWidth, brushHeight, _chartDimen);
        }
    }
}
