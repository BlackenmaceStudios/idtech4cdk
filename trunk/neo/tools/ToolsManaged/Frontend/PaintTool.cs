using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Windows.Media.Imaging;
using System.Text.RegularExpressions;
using System.Windows.Input;

using ToolsManaged.Private;
using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend.WindowAPI;
using ToolsManaged.Private.Editor.Input;
namespace ToolsManaged.Frontend
{
    public partial class PaintTool : Form
    {

        MegaProject _megaProject;
        NativeAPI.RenderWorld _rw;
        NativeAPI.idManagedImage _defaultImage;
        
        RenderDevice _renderDevice;
        System.Drawing.Point lastMousePoint = new System.Drawing.Point();

        bool _ignoreFirstShow = true;
        Vector3 viewOrigin = new Vector3(), viewAxis = new Vector3();
        bool allowPainting = false;


        public PaintTool()
        {
            InitializeComponent();

            this.Resize += new EventHandler(PaintTool_Resize);
            this.Load += new EventHandler(PaintTool_Load);
            this.Paint += new PaintEventHandler(PaintTool_Paint);
            panel1.MouseEnter += new EventHandler(panel1_MouseEnter);
            panel1.MouseLeave += new EventHandler(panel1_MouseLeave);
         
        }



        void panel1_MouseLeave(object sender, EventArgs e)
        {
            allowPainting = false;
        }

        void panel1_MouseEnter(object sender, EventArgs e)
        {
            allowPainting = true;
        }

        void HandleInput()
        {
            System.Drawing.Point p = panel1.PointToScreen(MousePosition);

            if (!allowPainting)
                return;

            Mouse.Synchronize();

            if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_MBUTTON) && KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LMENU))
            {
                viewOrigin.z += p.Y - lastMousePoint.Y;
            }

            else if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_RBUTTON) && KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LMENU))
            {
                float delta = p.Y - lastMousePoint.Y;
          

                viewOrigin.x -= delta  *  (float)Math.Sin(viewAxis.x);
                viewOrigin.y -= delta * (float)Math.Cos(viewAxis.x);

            }

            else if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LBUTTON) && KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LMENU))
            {
                viewAxis.x -= (p.X - lastMousePoint.X);

            }
            

            lastMousePoint = p;
        }

        void PaintTool_Paint(object sender, PaintEventArgs e)
        {
            int numVisibleVtAreas;
            if (_rw == null)
                return;

            HandleInput();

            _renderDevice.BeginRender();

            // Get the current virtual texture areas that are in the current view.
            numVisibleVtAreas = _rw.FindVisibleVirtualTextureAreas(panel1.Size.Width, panel1.Size.Height, viewOrigin.x, viewOrigin.y, viewOrigin.z, viewAxis.x, viewAxis.y, viewAxis.z); 

            // Render the virtualtexture areas
            for (int i = 0; i < numVisibleVtAreas; i++)
            {
                _rw.RenderVisibleArea(_defaultImage, i, 0,0,0,0,0,0);
            }

            _renderDevice.EndRender();
        }

        void PaintTool_Load(object sender, EventArgs e)
        {
            _renderDevice = new RenderDevice(panel1);
            _defaultImage = NativeAPI.idManagedImage.FindImage("textures/editor/checker");
        }

        void PaintTool_Resize(object sender, EventArgs e)
        {
            tableLayoutPanel1.Size = this.Size;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            AddNewLayer();   
        }

        private void AddNewLayer()
        {
            LayersBox.Items.Add("Layer" + (LayersBox.Items.Count + 1));
        }


        private void exitPaintToolToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FormManager.handle.SetFullsreenModalWindow(null);
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            base.OnVisibleChanged(e);

            // Windows automatically get shown -- this is just a quick fix so I dont have to fix the layout system that needs a overhaul.
            if (_ignoreFirstShow)
            {
                _ignoreFirstShow = false;
                return;
            }

            if (this.Visible == false)
                return;

            // Load in the renderworld.
            _rw = null;
            

            // Load in or create our megaproject.
            _megaProject = null;
            _megaProject = new MegaProject(NativeAPI.GetCurrentMapName());

            if (!_megaProject.IsLoaded)
            {
                Visible = false;
                return;
            }

            string mapname = Regex.Split(NativeAPI.GetCurrentMapName(), "maps")[1];
            _rw = NativeAPI.LoadWorld(mapname + ".world");
            if (_rw == null)
            {
                _megaProject = null;
                MessageBox.Show("You must first compile your world before opening the paint tool!");
                return;
            }

            NativeAPI.GetEditorViewPosition(ref viewOrigin );
          

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
            System.Drawing.Bitmap img;
            int width=0, height=0;

            mtrPath = (string)mtrListBox.Items[mtrListBox.SelectedIndex];
            StampImg.BackgroundImage = BitmapFromSource( NativeAPI.GetDiffuseImageForMaterial(mtrPath, ref width, ref height) );
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

    }
}
