// MegaProject.cs (c) 2012 BlackenMace Studios
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

using ToolsManaged.Private;
using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend.WindowAPI;
using System.Runtime.InteropServices;

/*
 * bmMegaProject 
 * 
 * MegaProject is the project file that contains all the information for developing a virtual texture.
 * 
*/
namespace ToolsManaged
{
    //
    // MegaProject
    //
    public class MegaProject : IMegaProject
    {
        private NativeAPI.File _megaFile;
        private List<MegaProjectLayer> _layers = new List<MegaProjectLayer>();

        private string _path;
        private const string MegaProjectFolder = "megaprojects";
        private const string MegaProjectExtension = ".megaproject";

        public MegaProjectLayer GetLayerByIndex(int i)
        {
            return _layers[i];
        }

        private struct MegaProjectHeader
        {
            public int iden;
            public int version;
            public int numLayers;

            private static int MegaProjectHeaderIden
            {
                get
                {
                    return (('A' << 24) + ('G' << 16) + ('M' << 8) + 'J');
                }
            }

            private static int MegaProjectHeaderVersion
            {
                get
                {
                    return 1;
                }
            }

            public MegaProjectHeader( NativeAPI.File f, bool write )
            {
                if (write)
                {
                    iden = MegaProjectHeaderIden;
                    version = MegaProjectHeaderVersion;
                    numLayers = 0;
                    WriteHeader(f);
                    return;
                }

                iden = 0;
                version = 0;
                numLayers = 0;

                f.ReadInt(ref iden);
                f.ReadInt(ref version);
                f.ReadInt(ref numLayers);
            }

            public void WriteHeader(NativeAPI.File f)
            {

                f.WriteInt(iden);
                f.WriteInt(version);

                f.WriteInt(numLayers);
                f.ForceFlush();
            }
        }

        MegaProjectHeader _projectheader;

        public MegaProject()
        {

        }

        public MegaProject(string mapname)
        {
            LoadMegaProject(mapname);
        }

        public void LoadMegaProject( string mapname )
        {
            if (mapname.Contains("unnamed"))
            {
                MessageBox.Show("You must save your map first before opening this editor");
                return;
            }

            mapname = Regex.Split(mapname, "maps")[1];

            _path = MegaProjectFolder + mapname + MegaProjectExtension;

            // Check to see if the megaproject already exists
            _megaFile = NativeAPI.FileSystem.OpenFileRead(_path, false, null);
            if (_megaFile == null)
            {
                _megaFile = NativeAPI.FileSystem.OpenFileWrite(_path, "fs_basepath");
                if (_megaFile == null)
                {
                    throw new Exception("Failed to open mega project for reading or writing(file in use?)");
                }

                CreateNewProject();
                return;
            }

            // Load the project file.
            LoadProject();
        }

        //
        // CreateNewLayer
        //
        public void CreateNewLayer(int numCharts)
        {
            MegaProjectLayer layer = new MegaProjectLayer(numCharts, 512);

            _layers.Add(layer);
        }

        //
        // CreateNewProject
        //
        private void CreateNewProject()
        {
            _projectheader = new MegaProjectHeader(_megaFile, true);
        }

        //
        // LoadProject
        //
        private void LoadProject()
        {
            _projectheader = new MegaProjectHeader(_megaFile, false);
            for (int i = 0; i < _projectheader.numLayers; i++)
            {
                MegaProjectLayer layer = new MegaProjectLayer(_megaFile);

                _layers.Add(layer);
            }
            NativeAPI.FileSystem.CloseFile(ref _megaFile);

            
        }

        static IntPtr _chartTemp = IntPtr.Zero;
        public bool RenderChart(int chartId, IntPtr _chart)
        {
            int layerNum = 0;

            if (_chartTemp == IntPtr.Zero)
            {
                _chartTemp = Marshal.AllocHGlobal(4096 * 4096 * 4);
            }

            foreach (MegaProjectLayer layer in _layers)
            {
                // Render the layer.
                if (!layer.RenderLayer(chartId, _chartTemp))
                    continue;

                if (layerNum == 0)
                {
                    NativeAPI.idManagedImage.CopyImageToImageBufferRegion(_chart, _chartTemp, 0, 0, 4096, 4096, 4096, true);
                    layerNum++;
                    continue;
                }

                NativeAPI.idManagedImage.CopyImageToImageBufferRegion(_chart, _chartTemp, 0, 0, 4096, 4096, 4096 );
                layerNum++;
            }

            if (layerNum == 0)
            {
                return false;
            }

            return true;
        }

        public int NumLayers
        {
            get
            {
                return _layers.Count;
            }
        }

        //
        // Save
        //
        public void Save()
        {
            _megaFile = NativeAPI.FileSystem.OpenFileWrite(_path, "fs_basepath");
            if (_megaFile == null)
            {
                throw new Exception("Failed to open mega project for writing(file in use?)");
            }

            _megaFile.Rewind();
            _projectheader.numLayers = _layers.Count;
            _projectheader.WriteHeader(_megaFile);

            foreach (MegaProjectLayer layer in _layers)
            {
                layer.Write(_megaFile);
            }

            _megaFile.ForceFlush();
            NativeAPI.FileSystem.CloseFile(ref _megaFile);
        }

        public void Close()
        {
            
        }

        public bool IsLoaded
        {
            get
            {
                return true;
            }
        }
    }
}
