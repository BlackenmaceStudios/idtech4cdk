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

/*
 * bmMegaProject 
 * 
 * MegaProject is the project file that contains all the information for developing a virtual texture.
 * 
*/
namespace ToolsManaged.Private
{
    //
    // MegaProject
    //
    class MegaProject
    {
        private NativeAPI.File _megaFile;
        private List<MegaProjectLayer> _layers = new List<MegaProjectLayer>();

        private string _path;
        private const string MegaProjectFolder = "megaprojects";
        private const string MegaProjectExtension = ".megaproject";

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

        public MegaProject( string mapname )
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
