using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ToolsManaged.Private
{
    class PaintBrush
    {
        NativeAPI.idManagedImage _currentStencilImage;
        NativeAPI.idManagedImage _currentBrushImage;
        float _currentBrushSize = 0;
        IntPtr brushTexData;

        private void CreateBrushImageData(NativeAPI.idManagedImage brushImage, MegaProjectChart chart, int brushSize)
        {
            IntPtr rawBrushImageData = brushImage.ReadDriverPixels(false);

            brushTexData = NativeAPI.idManagedImage.ResampleTextureBuffer(rawBrushImageData, brushImage.Width, brushImage.Height, 32, 32);
        }

        /*
         * ==============================
         * Paint
         * 
         * BrushImage <-- Alpha mask to paint with
         * StencilImage <-- Image to paint with.
         * chart <-- what chart to paint to.
         * u <-- u coord
         * v <-- v coord
         * brushSize <-- size of brush.
         * ===============================
        */
        public void Paint(NativeAPI.idManagedImage brushImage, string stencilName, NativeAPI.idManagedImage stencilImage, MegaProjectChart chart, float u, float v, int brushSize)
        {
            if (chart.HasMaterial)
            {
                if (chart.materialName != stencilName)
                {
                    MessageBox.Show("This chart already has a texture on it, if you would like to paint on this area add a new layer");
                    return;
                }
            }

            if (brushTexData == null || _currentBrushImage != brushImage || _currentBrushSize != brushSize)
            {
                CreateBrushImageData(brushImage, chart, brushSize);
            }

            chart.materialName = stencilName;

            chart.Blit(brushTexData, 32, 32, u, v, 0, 0, 0);

            // Save the current data presets.
            _currentBrushImage = brushImage;
            _currentBrushSize = brushSize;
        }
    }
}
