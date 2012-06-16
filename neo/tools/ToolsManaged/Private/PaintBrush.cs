using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ToolsManaged.Private.Editor.Input;
namespace ToolsManaged.Private
{
    class PaintBrush
    {
        NativeAPI.idManagedImage _currentStencilImage;
        NativeAPI.idManagedImage _currentBrushImage;
        float _currentBrushSize = 0;
        IntPtr brushTexData = IntPtr.Zero;

        private void CreateBrushImageData(NativeAPI.idManagedImage brushImage, MegaProjectChart chart, int brushSize, int scaledSize)
        {
            IntPtr rawBrushImageData = brushImage.ReadDriverPixels(false);

            if (brushTexData != IntPtr.Zero)
                NativeAPI.idManagedImage.FreeTextureBuffer(brushTexData);

            brushTexData = NativeAPI.idManagedImage.ResampleTextureBuffer(rawBrushImageData, brushImage.Width, brushImage.Height, scaledSize, scaledSize);
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
        public void Paint(NativeAPI.idManagedImage brushImage, string stencilName, NativeAPI.idManagedImage stencilImage, MegaProjectChart chart, float u, float v, int brushSize, bool remove)
        {
            if (chart.HasMaterial)
            {
                if (chart.materialName != stencilName && remove == false)
                {
                    if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LSHIFT))
                    {
                        DialogResult r = MessageBox.Show("Are you sure you want to replace the current texture on this chart?", "Replace Texture", MessageBoxButtons.OKCancel);

                        if (r == DialogResult.Cancel)
                            return;
                    }
                    else
                    {
                        MessageBox.Show("This chart already has a texture on it, if you would like to paint on this area add a new layer");
                        return;
                    }
                }
            }

            float scaledSize = brushSize / 3000.0f;
            scaledSize = (scaledSize * 512);

            if (brushTexData == null || _currentBrushImage != brushImage || _currentBrushSize != brushSize)
            {
                CreateBrushImageData(brushImage, chart, brushSize, (int)scaledSize);
            }

            if (remove == false)
            {
                chart.materialName = stencilName;
                chart._stencilImage = stencilImage;
            }

            if (u > 1)
                u = 1;

            if (v > 1)
                v = 1;

            if (u < 0)
                u = 0;
            if (v < 0)
                v = 0;
            chart.Blit(brushTexData, (int)scaledSize, (int)scaledSize, u, v, 0, 0, 0, remove);

            // Save the current data presets.
            _currentBrushImage = brushImage;
            _currentBrushSize = brushSize;
        }
    }
}
