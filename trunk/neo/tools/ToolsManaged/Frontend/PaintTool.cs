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
        UserInterface _debugGui;
        NativeAPI.idManagedImage _currentBrushImage;
        
        RenderDevice _renderDevice;
        System.Drawing.Point lastMousePoint = new System.Drawing.Point();

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

            _debugGui = UserInterfaceManager.LoadGUI("guis/editors/vtpaintdebug.gui");
            
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
                float deltaX = p.X - lastMousePoint.X;
                viewOrigin.z += p.Y - lastMousePoint.Y;

                double radians = (Math.PI / 180) * (viewAxis.x + 90.0f);
                viewOrigin.x += deltaX * (float)Math.Cos(radians);
                viewOrigin.y += deltaX * (float)Math.Sin(radians);
            }

            else if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_RBUTTON) && KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LMENU))
            {
                float delta = p.Y - lastMousePoint.Y;
        
                if (delta != 0)
                {
                    double radians = (Math.PI / 180) * viewAxis.x;
                    viewOrigin.x += delta * (float)Math.Cos(radians);
                    viewOrigin.y += delta * (float)Math.Sin(radians);

                    radians = (Math.PI / 180) * viewAxis.y;
                    viewOrigin.z -= delta * (float)Math.Tan(radians);
                }

                
            }

            else if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LBUTTON) && KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LMENU))
            {
                viewAxis.x -= (p.X - lastMousePoint.X) * 0.5f;
                viewAxis.y += (p.Y - lastMousePoint.Y) * 0.5f;

                if (viewAxis.x > 360)
                {
                    viewAxis.x = -360 + (viewAxis.x - 360);
                }

                if (viewAxis.x < -360)
                {
                    viewAxis.x = 360 - (-viewAxis.x - 360);
                }

                if (viewAxis.y > 360)
                {
                    viewAxis.y = -360 + (viewAxis.y - 360);
                }

                if (viewAxis.y < -360)
                {
                    viewAxis.y = 360 - (-viewAxis.y - 360);
                }
            }
            

            lastMousePoint = p;
        }

        void DrawBrush(traceManaged_t trace)
        {
            float brushSize = brushTrackBar.Value;

            if (_currentBrushImage == null)
                return;


            NativeAPI.DrawPlane(brushSize, _currentBrushImage, trace.endposx, trace.endposy, trace.endposz, viewAxis.x + 90.0f, viewAxis.y, viewAxis.z);

        }

        void UpdateHighlightArea()
        {
            float x = 0, y = 0, z = 0;

            // If the mouse isn't within the view control don't do anything.
            if (!allowPainting)
            {
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "highlightedEntity", "");
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "vtPaintID", "");
                return;
            }

            _renderDevice.UnprojectMouseCoord(ref x, ref y, ref z);

          
            // Get the vector pointing from the view position to were the unprojection got put.
            Vector3 v = viewOrigin - new Vector3(x, y, z);
            Vector3 angle = new Vector3();

            v.Normalize180();
         
            angle.x = v.ToYaw();
            angle.y = v.ToPitch();


            traceManaged_t trace = new traceManaged_t();
            if (CollisionModelManager.TraceProjectedRay(ref trace, viewOrigin.x, viewOrigin.y, viewOrigin.z, x, y, z, 40000))
            {
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "highlightedEntity", "PaintEntity: " + trace.entNum);

                DrawBrush(trace);
            }
            else
            {
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "highlightedEntity", "No Entity");
            }
        }

        private int tics = 0;
        void PaintTool_Paint(object sender, PaintEventArgs e)
        {
            DateTime _startFrameTime, _endFrameTime;
            int numVisibleVtAreas;
            if (_rw == null)
                return;

            _startFrameTime = DateTime.Now;
            

            if (_megaProject != null && LayersBox.Items.Count != _megaProject.NumLayers)
            {
                LayersBox.Items.Clear();
                for (int i = 0; i < _megaProject.NumLayers; i++)
                {
                    LayersBox.Items.Add("Layer" + (LayersBox.Items.Count + 1));
                }
            }

            


            HandleInput();

            _renderDevice.BeginRender();

            

            // Get the current virtual texture areas that are in the current view.
            numVisibleVtAreas = _rw.FindVisibleVirtualTextureAreas(panel1.Size.Width, panel1.Size.Height, viewOrigin.x, viewOrigin.y, viewOrigin.z, viewAxis.x, viewAxis.y, viewAxis.z);

            

            // Render the virtualtexture areas
            for (int i = 0; i < numVisibleVtAreas; i++)
            {
                _rw.RenderVisibleArea(_defaultImage, i, 0,0,0,0,0,0);
            }

            UpdateHighlightArea();

            _debugGui.Redraw(_debugGui.GetNativeAddress(), (int)tics);

            _renderDevice.EndRender();

            _endFrameTime = DateTime.Now;

            float fps2 = (_endFrameTime.Ticks - _startFrameTime.Ticks) * 0.001f;
            
            _debugGui.SetStateString(_debugGui.GetNativeAddress(), "currentFPS", "" + (int)fps2 + "fps");
            tics++;
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
            _megaProject.CreateNewLayer(240);
        }


        private void exitPaintToolToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _megaProject.Save();
            _megaProject.Close();
            _megaProject = null;
            FormManager.handle.SetFullsreenModalWindow(null);
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            
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

            // Load the collision model f
            CollisionModelManager.LoadMap(mapname);

            if (mtrListBox.Items.Count != NativeAPI.GetNumMaterials())
            {
                mtrListBox.Items.Clear();

                for (int i = 0; i < NativeAPI.GetNumMaterials(); i++)
                {
                    string mtrName = NativeAPI.GetMaterialNameByIndex(i);

                    if (!mtrName.Contains("textures/megagen"))
                        continue;

                    mtrListBox.Items.Add(mtrName);
                }
            }

            if (!_megaProject.IsLoaded)
            {
                Visible = false;
            }

            _debugGui.Activate(_debugGui.GetNativeAddress(), true, 0);
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

            _currentBrushImage = NativeAPI.idManagedImage.GetDiffuseImageHandleForMaterial(mtrPath);
            StampImg.BackgroundImage = BitmapFromSource( NativeAPI.GetDiffuseImageForMaterial(mtrPath, ref width, ref height) );
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void saveMegaProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _megaProject.Save();
        }

    }
}
