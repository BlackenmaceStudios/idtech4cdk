using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Windows.Media.Imaging;

using ToolsManaged.Private;
using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend.WindowAPI;

namespace ToolsManaged.Frontend
{
    public partial class PaintTool : Form
    {

        MegaProject _megaProject;
        bool _ignoreFirstShow = true;
        

        public PaintTool()
        {
            InitializeComponent();

            this.Resize += new EventHandler(PaintTool_Resize);
            this.Paint += new PaintEventHandler(PaintTool_Paint);
        }

        void PaintTool_Paint(object sender, PaintEventArgs e)
        {
            

        }

        void PaintTool_Resize(object sender, EventArgs e)
        {
            tableLayoutPanel1.Size = this.Size;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        

        private void exitPaintToolToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FormManager.handle.SetFullsreenModalWindow(null);
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            base.OnVisibleChanged(e);

            if (_ignoreFirstShow)
            {
                _ignoreFirstShow = false;
                return;
            }

            if (this.Visible == false)
                return;

            _megaProject = null;
            _megaProject = new MegaProject(NativeAPI.GetCurrentMapName());

            if (mtrListBox.Items.Count != NativeAPI.GetNumMaterials())
            {
                mtrListBox.Items.Clear();

                for (int i = 0; i < NativeAPI.GetNumMaterials(); i++)
                {
                    mtrListBox.Items.Add(NativeAPI.GetMaterialNameByIndex(i));
                }
            }

            if (!_megaProject.IsLoaded)
            {
                Visible = false;
            }
        }

        private System.Drawing.Bitmap BitmapFromSource(BitmapSource bitmapsource)
        {
            System.Drawing.Bitmap bitmap;
            using (MemoryStream outStream = new MemoryStream())
            {
                BitmapEncoder enc = new BmpBitmapEncoder();
                enc.Frames.Add(BitmapFrame.Create(bitmapsource));
                enc.Save(outStream);
                bitmap = new System.Drawing.Bitmap(outStream);
            }
            return bitmap;
        } 


        private void mtrListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            string mtrPath;
            Bitmap img;
            int width=0, height=0;

            mtrPath = (string)mtrListBox.Items[mtrListBox.SelectedIndex];
            StampImg.Image = BitmapFromSource( NativeAPI.GetDiffuseImageForMaterial(mtrPath, ref width, ref height) );
        }

    }
}
