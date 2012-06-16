using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using ToolsManaged.Frontend;

namespace ToolsManaged.Private
{
    public class MegaProjectLayer 
    {
        private List<MegaProjectChart> _charts = new List<MegaProjectChart>();

        public MegaProjectChart this[int index]
        {
            get
            {
                return _charts[index];
            }
        }

        //
        // MegaProjectLayer
        //
        public MegaProjectLayer(int numCharts, int chartDimen)
        {
            ProgressDialog progress = new ProgressDialog();

            progress.Show();
            
            for (int i = 0; i < numCharts; i++)
            {
                progress.SetUpdateMsg("Building chart " + i);
                _charts.Add(new MegaProjectChart(chartDimen));

                float percent = ((float)i / (float)numCharts) * 100f;
                progress.SetProgress((int)percent);
            }

            progress.Hide();
            progress.Dispose();
        }

        //
        // MegaProjectLayer(Read from HD)
        //
        public MegaProjectLayer(NativeAPI.File _megaFile)
        {
            ProgressDialog progress = new ProgressDialog();

            int numCharts = 0;

            _megaFile.ReadInt(ref numCharts);

            progress.Show();

            for (int i = 0; i < numCharts; i++)
            {
                progress.SetUpdateMsg("Loading chart " + i);
                _charts.Add(new MegaProjectChart(_megaFile));

                float percent = ((float)i / (float)numCharts) * 100f;
                progress.SetProgress((int)percent);
            }

            progress.Hide();
            progress.Dispose();
        }

        public void Write(NativeAPI.File _megaFile)
        {
            _megaFile.WriteInt(_charts.Count);
            for (int i = 0; i < _charts.Count; i++)
            {
                _charts[i].Write(_megaFile);
            }
        }

        //
        // RenderChart
        //
        public unsafe bool RenderLayer(int chartId, IntPtr LayerData)
        {
            if (!_charts[chartId].HasMaterial)
            {
                return false;
            }

            NativeAPI.idManagedImage _stencilImage = _charts[chartId]._stencilImage;
            IntPtr _stencilData = _stencilImage.ReadDriverPixels( false );

            // Resize the chart data to 4096x4096
            byte* resizedMaskPtr = (byte *)NativeAPI.idManagedImage.ResampleTextureBuffer((IntPtr)_charts[chartId].pixels, 512, 512, 4096, 4096);

            float numStridesWidth = 4096.0f / _stencilImage.Width;
            float numStridesHeight = 4096.0f / _stencilImage.Height;

            for(int w = 0; w < numStridesWidth; w++)
            {
                for(int h = 0; h < numStridesHeight; h++)
                {
                    NativeAPI.idManagedImage.CopyImageToImageBufferRegionWithAlpha(LayerData, _stencilData, (IntPtr)resizedMaskPtr, (int)(w * _stencilImage.Width), (int)(h * _stencilImage.Height), _stencilImage.Width, _stencilImage.Height, 4096);
                }
            }

            return true;
        }
    }
}
