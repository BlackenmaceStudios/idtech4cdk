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
using System.Runtime.InteropServices;

using ToolsManaged.Private;
using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend.WindowAPI;
using ToolsManaged.Private.Editor.Input;
namespace ToolsManaged.Frontend
{
    public partial class PaintTool : Form
    {

        MegaProject _megaProject;
        MegaProjectLayer _currentPaintLayer;
        NativeAPI.RenderWorld _rw;
        NativeAPI.idManagedImage _defaultImage;
        UserInterface _debugGui;
        NativeAPI.idManagedImage _currentStencilImage, _currentBrushImage;
        RenderProgram _brushPaintProgram, _livePaintProgram;
        PaintBrush _brush;
        NativeAPI.idManagedImage _chartImage;
        string _currentMaterialName;
        
        RenderDevice _renderDevice;
        System.Drawing.Point lastMousePoint = new System.Drawing.Point();

        Vector3 viewOrigin = new Vector3(), viewAxis = new Vector3();
        bool allowPainting = false;

        float _paintU = 0;
        float _paintV = 0;
        int _paintChart = -1;


        public PaintTool()
        {
            InitializeComponent();

            this.Resize += new EventHandler(PaintTool_Resize);
            this.Load += new EventHandler(PaintTool_Load);
            this.Paint += new PaintEventHandler(PaintTool_Paint);
            panel1.MouseEnter += new EventHandler(panel1_MouseEnter);
            panel1.MouseLeave += new EventHandler(panel1_MouseLeave);

            _debugGui = UserInterfaceManager.LoadGUI("guis/editors/vtpaintdebug.gui");
            _brushPaintProgram = RenderProgram.LoadRenderProgram("editors/vt_paint_editorbrush.crp", 1);
            _livePaintProgram = RenderProgram.LoadRenderProgram("editors/vt_paint_world.crp", 1);
            _chartImage = NativeAPI.idManagedImage.FindImage("_defaultVTEditChart", true);
            _brush = new PaintBrush();
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
            else if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LBUTTON) && _paintChart != -1)
            {
                if(_currentPaintLayer == null)
                {
                    MessageBox.Show("You must select a layer to paint on before you can begin painting.");
                }
                else
                {
                    _brush.Paint(_currentBrushImage, (string)mtrListBox.Items[mtrListBox.SelectedIndex], _currentStencilImage, _currentPaintLayer[_paintChart], _paintU, _paintV, BrushSize, false);
                }
            }
            else if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_RBUTTON) && _paintChart != -1)
            {
                if (_currentPaintLayer == null)
                {
                    MessageBox.Show("You must select a layer to paint on before you can begin erasing.");
                }
                else
                {
                    _brush.Paint(_currentBrushImage, (string)mtrListBox.Items[mtrListBox.SelectedIndex], _currentStencilImage, _currentPaintLayer[_paintChart], _paintU, _paintV, BrushSize, true);
                }
            }

            

            lastMousePoint = p;
        }

        void DrawBrush(traceManaged_t trace)
        {
            float brushSize = brushTrackBar.Value;

            if (_currentStencilImage == null)
                return;
            _brushPaintProgram.SetCurrentPass(_brushPaintProgram.GetNativeAddress(), 0);

            

            _brushPaintProgram.Bind(_brushPaintProgram.GetNativeAddress());

                // Bind the stencil image.
                _renderDevice.BindImageToTextureUnit(_currentStencilImage, 0);
                _brushPaintProgram.BindTextureVar(_brushPaintProgram.GetNativeAddress(), (uint)RenderProgram.renderProgramParameter.PP_TEX_DIFFUSE);

                // Bind the brush image.
                _renderDevice.BindImageToTextureUnit(_currentBrushImage, 1);
                _brushPaintProgram.BindTextureVar(_brushPaintProgram.GetNativeAddress(), (uint)RenderProgram.renderProgramParameter.PP_COLOR_MODULATE);

            
                NativeAPI.DrawPlane( brushSize, null, trace.endposx, trace.endposy, trace.endposz, viewAxis.x + 90.0f, 0, viewAxis.y);
            _brushPaintProgram.UnBind(_brushPaintProgram.GetNativeAddress() );

            // UnBind everything.
             _renderDevice.UnBindTextureUnit( 1 );
            _renderDevice.UnBindTextureUnit( 0 );
        }

        void DrawArea(MegaProjectChart chart, int areaId, bool firstPass)
        {
            float brushSize = brushTrackBar.Value;

            chart.UpdateToImage(_chartImage);

            _livePaintProgram.SetCurrentPass(_livePaintProgram.GetNativeAddress(), 0);
            _livePaintProgram.Bind(_livePaintProgram.GetNativeAddress());

            if (firstPass)
            {
                _livePaintProgram.SetVar1fi(_livePaintProgram.GetNativeAddress(), (uint)RenderProgram.renderProgramParameter.PP_COLOR_ADD, 1);
            }
            else
            {
                _livePaintProgram.SetVar1fi(_livePaintProgram.GetNativeAddress(), (uint)RenderProgram.renderProgramParameter.PP_COLOR_ADD, 2);
            }

            // Bind the stencil image.
            _renderDevice.BindImageToTextureUnit(chart._stencilImage, 0);
            _livePaintProgram.BindTextureVar(_livePaintProgram.GetNativeAddress(), (uint)RenderProgram.renderProgramParameter.PP_TEX_DIFFUSE);

            // Bind the brush image.
            _renderDevice.BindImageToTextureUnit(_chartImage, 1);
            _livePaintProgram.BindTextureVar(_livePaintProgram.GetNativeAddress(), (uint)RenderProgram.renderProgramParameter.PP_COLOR_MODULATE);


            _rw.RenderVisibleArea(null, areaId, 0, 0, 0, 0, 0, 0);
            _livePaintProgram.UnBind(_livePaintProgram.GetNativeAddress());

            // UnBind everything.
            _renderDevice.UnBindTextureUnit(1);
            _renderDevice.UnBindTextureUnit(0);
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
                _paintChart = trace.entNum;
                _rw.VTTrace(ref _paintU, ref _paintV, trace.entNum, viewOrigin.x, viewOrigin.y, viewOrigin.z, x, y, z, 40000);
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "highlightedEntity", "VT Paint Chart: " + trace.entNum);
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "vtPaintID", "U: " + _paintU + " V: " + _paintV);

                if (_currentPaintLayer != null && _paintChart != -1)
                {
                    int width = 0, height = 0;
                    if (_currentMaterialName != _currentPaintLayer[_paintChart].materialName)
                    {
                        chartLayerImg.BackgroundImage = BitmapFromSource(NativeAPI.GetDiffuseImageForMaterial(_currentPaintLayer[_paintChart].materialName, ref width, ref height));
                    }
                    _currentMaterialName = _currentPaintLayer[_paintChart].materialName;
                }

                DrawBrush(trace);
            }
            else
            {
                _paintChart = -1;
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "highlightedEntity", "No Entity");
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "vtPaintID", "No Paint Area");
            }
        }

        private unsafe void RenderVTAreas()
        {
            int numVisibleVtAreas;

            // Get the current virtual texture areas that are in the current view.
            numVisibleVtAreas = _rw.FindVisibleVirtualTextureAreas(panel1.Size.Width, panel1.Size.Height, viewOrigin.x, viewOrigin.y, viewOrigin.z, viewAxis.x, viewAxis.y, viewAxis.z);
            bool _forceOneInView = false;
            for (int LayerId = 0; LayerId < _megaProject.NumLayers; LayerId++)
            {
                MegaProjectLayer _layer = _megaProject.GetLayerByIndex(LayerId);

                // Render the virtualtexture areas
               
              //  for (int i = 0; i < numVisibleVtAreas; i++)
               // {
                for (int ind = 0; ind < numVisibleVtAreas; ind++)
                { 
                    int i = _rw.visibleVTAreas[ind];
                    if (!_layer[i].HasMaterial)
                    {
                        if (LayerId == 0)
                        {
                            _rw.RenderVisibleArea(_defaultImage, i, 0, 0, 0, 0, 0, 0);
                        }
                    }
                    else
                    {
                        if (_forceOneInView)
                        {
                            continue;
                        }
                        DrawArea(_layer[i], i, LayerId == 0);
                        //_forceOneInView = true;
                       
                    }
                }
                
            }
        }

        private int tics = 0;
        void PaintTool_Paint(object sender, PaintEventArgs e)
        {
            DateTime _startFrameTime, _endFrameTime;
            
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

            if (_megaProject.NumLayers <= 0)
            {
                _renderDevice.BeginRender();

                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "highlightedEntity", "No Layers");
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "vtPaintID", "No Layers");
                _debugGui.SetStateString(_debugGui.GetNativeAddress(), "currentFPS", "No Layers");

                // Redraw the debug gui.
                _debugGui.Redraw(_debugGui.GetNativeAddress(), (int)tics);

                _renderDevice.EndRender();
                tics++;
                return;
            }

            HandleInput();

            _renderDevice.BeginRender();

            // Render VT Areas.
            RenderVTAreas();
            
            // Update what the cursor has selected.
            UpdateHighlightArea();

            // Redraw the debug gui.
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

        private void UpdateStencilList()
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

        private void UpdateBrushList()
        {
            int defaultBrushListItem = 0;
            brushList.Items.Clear();
            for (int i = 0; i < NativeAPI.GetNumMaterials(); i++)
            {
                string mtrName = NativeAPI.GetMaterialNameByIndex(i);

                if (!mtrName.Contains("textures/megabrushes"))
                    continue;

                if (mtrName.Contains("defaultbrush"))
                {
                    defaultBrushListItem = brushList.Items.Count;
                }

                brushList.Items.Add(mtrName);
            }

            brushList.SetSelected(defaultBrushListItem, true);
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

            brushSizeTxt.Text = "" + brushTrackBar.Value;

            NativeAPI.GetEditorViewPosition(ref viewOrigin );

            ProgressDialog progress = new ProgressDialog();

            progress.Show();

            progress.SetUpdateMsg("Loading Collision");
         
            progress.SetProgress(99);

            // Load the collision model f
            CollisionModelManager.LoadMap(_rw);

            progress.Hide();
            progress.Dispose();

            if (mtrListBox.Items.Count != NativeAPI.GetNumMaterials())
            {
                UpdateStencilList();

                UpdateBrushList();
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

            _currentStencilImage = NativeAPI.idManagedImage.GetDiffuseImageHandleForMaterial(mtrPath);
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

        private void brushList_SelectedIndexChanged(object sender, EventArgs e)
        {
            string mtrPath;
            int width = 0, height = 0;

            mtrPath = (string)brushList.Items[brushList.SelectedIndex];

            _currentBrushImage = NativeAPI.idManagedImage.GetDiffuseImageHandleForMaterial(mtrPath);
            brushImage.BackgroundImage = BitmapFromSource(NativeAPI.GetDiffuseImageForMaterial(mtrPath, ref width, ref height));
        }

        private int BrushSize
        {
            get
            {
                return brushTrackBar.Value;
            }
        }

        private void brushTrackBar_Scroll(object sender, EventArgs e)
        {
            if(Int32.Parse(brushSizeTxt.Text) != brushTrackBar.Value)
            {
                brushSizeTxt.Text = "" + brushTrackBar.Value;
            }
        }

        private void brushSizeTxt_TextChanged(object sender, EventArgs e)
        {
            if (Int32.Parse(brushSizeTxt.Text) != brushTrackBar.Value)
            {
                if (Int32.Parse(brushSizeTxt.Text) <= 0)
                {
                    brushSizeTxt.Text = "1";
                }
                if (Int32.Parse(brushSizeTxt.Text) >= brushTrackBar.Maximum)
                {
                    brushSizeTxt.Text = "" + brushTrackBar.Maximum;
                }
                brushTrackBar.Value = Int32.Parse(brushSizeTxt.Text);
            }
        }

        private void LayersBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _currentPaintLayer = _megaProject.GetLayerByIndex(LayersBox.SelectedIndex);
        }

        private void renderChartToolStripMenuItem_Click(object sender, EventArgs e)
        {
            IntPtr _temp = Marshal.AllocHGlobal( 4096 * 4096 * 4 );
            _megaProject.RenderChart(163, _temp);

            NativeAPI.WriteTGA("vt/debug/163.tga", _temp, 4096, 4096, false);

            Marshal.FreeHGlobal(_temp);
        }

        private void updateStencilsToolStripMenuItem_Click(object sender, EventArgs e)
        {

            ProgressDialog progress = new ProgressDialog();

            progress.Show();
            progress.SetUpdateMsg("Updating Stencils...");
            progress.SetProgress(99);

            NativeAPI.ExecuteCmd("reloaddecls");
            UpdateStencilList();

            progress.Hide();
            progress.Dispose();
        }

        private void updateBrushesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ProgressDialog progress = new ProgressDialog();

            progress.Show();
            progress.SetUpdateMsg("Updating Brushes...");
            progress.SetProgress(99);

            NativeAPI.ExecuteCmd("reloaddecls");
            UpdateBrushList();

            progress.Hide();
            progress.Dispose();
        }

        private void groupBox4_Enter(object sender, EventArgs e)
        {

        }

    }
}
