using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;

using ToolsManaged.Private;
using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend.WindowAPI;
using ToolsManaged.Private.Editor.Input;
namespace ToolsManaged.Frontend
{
    public unsafe partial class MoCapStudio : Form
    {
        RenderDevice _renderDevice;
        NativeAPI.RenderWorld _rw;
        IntPtr _testModel = IntPtr.Zero;
        KinectPlayer _trackedPlayer;

        byte* depthCameraData;
        public MoCapStudio()
        {
            InitializeComponent();
        }
        System.Drawing.Point lastMousePoint = new System.Drawing.Point();

        Vector3 viewOrigin = new Vector3(), viewAxis = new Vector3();


        void MoCapStudio_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            Hide();
        }

        private unsafe void MoCapStudio_Load(object sender, EventArgs e)
        {
            FormClosing += new FormClosingEventHandler(MoCapStudio_FormClosing);
            VisibleChanged += new EventHandler(MoCapStudio_VisibleChanged);
            Paint += new PaintEventHandler(MoCapStudio_Paint);
            kinectControlGroupBox.MouseLeave += new EventHandler(kinectControlGroupBox_MouseLeave);
            _renderDevice = new RenderDevice(panel1);

            // Force a refresh.
            panel1.Focus();
            Invalidate();

            _rw = NativeAPI.RenderWorld.AllocWorld();

            depthCameraData = (byte *)Marshal.AllocHGlobal(640 * 480 * 4);

            for (int i = 0; i < (int)Kinect.NUI_SKELETON_POSITION_INDEX.NUI_SKELETON_POSITION_COUNT; i++)
            {
                kinectJointRemapList.Items.Add(Kinect.GetSkeletonPositionIndexName(i));
            }

            modelJointList.SelectedIndex = -1;
        }

        void HandleInput()
        {
            System.Drawing.Point p = panel1.PointToScreen(MousePosition);


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

        void kinectControlGroupBox_MouseLeave(object sender, EventArgs e)
        {
           
        }

        void MoCapStudio_VisibleChanged(object sender, EventArgs e)
        {
            if (!Visible)
                return;

            elevationBar.Value = Kinect.GetElevationAngle();
            elevationValueTxt.Text = "Elevation: " + elevationBar.Value;

            // If we just got loaded find all the skeletal models.
            string[] skelmodels = NativeAPI.ModelManager.ListSkeletalMeshes();
            SkeletalModelListBox.Items.Clear();
            foreach (string path in skelmodels)
            {
                SkeletalModelListBox.Items.Add(path);
            }
        }

        unsafe void UpdateColorCameraData()
        {
            byte* data;
            int size = 0, pitch = 0;
            

            data = (byte*)Kinect.GetColorCameraData(ref size, ref pitch);

            if (data == null)
                return;

            colorCameraData.Image = NativeAPI.BitmapFromSource(NativeAPI.ImageBufferToBitmapSource(data, 640, 480));
        }

        unsafe void UpdateDepthCameraData()
        {
            byte* data;
            int size = 0, pitch = 0;


            data = (byte*)Kinect.GetDepthCameraData(ref size, ref pitch);

            if (data == null)
                return;

            byte * rgbrun = depthCameraData;
            ushort* pBufferRun = (ushort*)data;

            for (int i = 0; i < (640 * 480) * 4; i+=4)
            {

                byte pixel = (byte)*pBufferRun;


                // discard the portion of the depth that contains only the player index
                ushort depth =  Kinect.DepthPixelToDepth(pixel);

                // to convert to a byte we're looking at only the lower 8 bits
                // by discarding the most significant rather than least significant data
                // we're preserving detail, although the intensity will "wrap"
                byte intensity = (byte)((depth % 256) * 2);

                rgbrun[i + 0] = intensity;
                rgbrun[i + 1] = intensity;
                rgbrun[i + 2] = intensity;
                rgbrun[i + 3] = 255;



                // Increment our index into the Kinect's depth buffer
                ++pBufferRun;
            }
            depthPicBox.Image = NativeAPI.BitmapFromSource(NativeAPI.ImageBufferToBitmapSource(depthCameraData, 640, 480));
        }

        void MoCapStudio_Paint(object sender, PaintEventArgs e)
        {
            // Update the color camera
            UpdateColorCameraData();

            // Update the depth camera.
            UpdateDepthCameraData();

            // Process the next frame.
            Kinect.NextFrame();

            if (_trackedPlayer == null || !_trackedPlayer.IsTracking)
            {
                _trackedPlayer = null;

                for (int i = 0; i < 6; i++)
                {
                    if (Kinect.GetIndex(i).IsTracking)
                    {
                        trackTxt.Text = "Tracking";
                        trackTxt.BackColor = Color.Green;
                        _trackedPlayer = Kinect.GetIndex(i);
                        break;
                    }
                    else
                    {
                        trackTxt.Text = "Not Tracking";
                        trackTxt.BackColor = Color.Red;
                    }
                }
            }

            // Update the joint orientations
            
            if (_trackedPlayer != null && kinectJointRemapList.Enabled)
            {
                Quat q = new Quat();

                // Reset the bone transforms to idle.
                NativeAPI.SetTestModelIdle(null);

                for (int i = 0; i < (int)Kinect.NUI_SKELETON_POSITION_INDEX.NUI_SKELETON_POSITION_COUNT; i++)
                {
                    
                    string val = MotionCapRemap.GetModelJointMapping((string)Kinect.GetSkeletonPositionIndexName(i));

                    if (val == null || val == "-")
                        continue;


                    string[] valpool = val.Split('-');

                    if (valpool == null)
                        continue;

                    _trackedPlayer.GetJointTransform(i, ref q);
                    for(int d = 0; d < valpool.Length; d++)
                    {
                        for (int c = 0; c < modelJointList.Items.Count; c++)
                        {
                            if (NativeAPI.GetJointNameForTestModel(c) == valpool[d])
                            {
                                NativeAPI.SetTestModelJointRotation(c, q);
                                break;
                            }
                        }
                    }
                }

                // Rotate all the children.
                NativeAPI.SetTestModelJointRotation(-1, q);
            }
           

            _renderDevice.BeginRender();

            NativeAPI.RenderWorld.DrawEditorWorld(viewOrigin.x, viewOrigin.y, viewOrigin.z, 0, 0, 0);

            _renderDevice.EndRender();
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void elevationBar_Scroll(object sender, ScrollEventArgs e)
        {
            elevationValueTxt.Text = "Elevation: " + elevationBar.Value;

        }

        private void SetTitleAngleBtn_Click(object sender, EventArgs e)
        {
            Kinect.SetElevationAngle(elevationBar.Value);
        }

        private void kinectControlGroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void groupBox3_Enter(object sender, EventArgs e)
        {

        }

        private void SkeletalModelListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (_testModel != IntPtr.Zero)
            {
                _rw.FreeEntity(_testModel);
            }

            _testModel = _rw.AddModelAtPosition((string)SkeletalModelListBox.Items[SkeletalModelListBox.SelectedIndex], 0, 0, 0);

            if (NativeAPI.GetNumJointsForTestModel() <= 0)
            {
                kinectJointRemapList.Enabled = false;
            }
            else
            {
                kinectJointRemapList.Enabled = true;

                modelJointList.Items.Clear();

                for (int i = 0; i < NativeAPI.GetNumJointsForTestModel(); i++)
                {
                    modelJointList.Items.Add(NativeAPI.GetJointNameForTestModel(i));
                }
                MotionCapRemap.LoadRemapForModel((string)SkeletalModelListBox.Items[SkeletalModelListBox.SelectedIndex]);
                modelJointList.SelectedIndex = -1;
            }

            
        }

        private void kinectJointRemapList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (kinectJointRemapList.SelectedIndex != -1)
            {
                modelJointList.Enabled = true;
                string val = MotionCapRemap.GetModelJointMapping((string)Kinect.GetSkeletonPositionIndexName(kinectJointRemapList.SelectedIndex));
                modelJointList.SelectedItems.Clear();
                if (val == null)
                {
                    modelJointList.SelectedIndex = -1;
                    return;
                }

                string[] selectedPool = val.Split('-');
                
                
                foreach (string selection in selectedPool)
                {
                    modelJointList.SelectedItems.Add(selection);
                }
            }
            else
            {
                modelJointList.Enabled = false;
            }
        }

        private void groupBox4_Enter(object sender, EventArgs e)
        {

        }


        private void SaveRemapBtn_Click(object sender, EventArgs e)
        {
            MotionCapRemap.Save();
        }

        private void modelJointList_SelectedIndexChanged_1(object sender, EventArgs e)
        {
            string key = (string)kinectJointRemapList.SelectedItem;
            string val = "";

            foreach (string s in modelJointList.SelectedItems)
            {
                val += s + "-";
            }

            val = val.TrimEnd('-');

            MotionCapRemap.MapJoint(key, val);
        }

        private void boneSearchTxt_TextChanged(object sender, EventArgs e)
        {
            string val = MotionCapRemap.GetModelJointMapping((string)Kinect.GetSkeletonPositionIndexName(kinectJointRemapList.SelectedIndex));
            modelJointList.SelectedItems.Clear();
            if (val == null)
            {
                modelJointList.SelectedIndex = -1;
                return;
            }

            string[] selectedPool = val.Split('-');


            foreach (string selection in selectedPool)
            {
                if (boneSearchTxt.Text.Length > 0 && selection.Contains(boneSearchTxt.Text) == false)
                {

                }
                else
                {
                    modelJointList.SelectedItems.Add(selection);
                }
            }
        }

    }
}
