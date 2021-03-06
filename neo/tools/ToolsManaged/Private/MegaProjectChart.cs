﻿using System;
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
    public struct MegaProjectChartPixel
    {
        public byte tileTransX;
        public byte tileTransY;
        public byte tileScale;
        public byte paintLevel;
    }

    //
    // MegaProjectChart
    //
    public unsafe class MegaProjectChart
    {
        int _chartDimen;
        MegaProjectChartPixel* _pixels;
        public string materialName;
        public NativeAPI.idManagedImage _stencilImage;

        public MegaProjectChartPixel* pixels
        {
            get
            {
                return _pixels;
            }
        }

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

        public void Blit(IntPtr brushData, int brushWidth, int brushHeight, float u, float v, byte transX, byte transY, byte scale, bool remove)
        {
            u = u * _chartDimen;
            v = v * _chartDimen;

   
            u -= (float)brushWidth / 2.0f;
            v -= (float)brushHeight / 2.0f;
            if (u > _chartDimen)
                u = _chartDimen;

            if (v > _chartDimen)
                v = _chartDimen;

            // The blit functions blit starting from (0,0) we have to offset .5
            if (remove)
            {
                NativeAPI.idManagedImage.RemoveImageToImageBufferRegion((System.IntPtr)_pixels, brushData, (int)u, (int)v, brushWidth, brushHeight, _chartDimen, false, true);
            }
            else
            {
                NativeAPI.idManagedImage.CopyImageToImageBufferRegion((System.IntPtr)_pixels, brushData, (int)u, (int)v, brushWidth, brushHeight, _chartDimen, false, true);
            }
        }
    }
}
