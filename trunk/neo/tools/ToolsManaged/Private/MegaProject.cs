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

        private const string MegaProjectFolder = "megaprojects";
        private const string MegaProjectExtension = ".megaproject";

        private struct MegaProjectHeader
        {
            public int iden;
            public int version;
            public int numCharts;
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
                    numCharts = 0;
                    numLayers = 0;

                    f.WriteInt(iden);
                    f.WriteInt(version);
                    f.WriteInt(numCharts);
                    f.WriteInt(numLayers);
                    f.ForceFlush();
                    return;
                }

                iden = 0;
                version = 0;
                numCharts = 0;
                numLayers = 0;

                f.ReadInt(ref iden);
                f.ReadInt(ref version);
                f.ReadInt(ref numCharts);
                f.ReadInt(ref numLayers);
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

            // Check to see if the megaproject already exists
            _megaFile = NativeAPI.FileSystem.OpenFileRead(MegaProjectFolder + "/" + mapname + MegaProjectExtension, false, "fs_basepath");
            if (_megaFile == null)
            {
                _megaFile = NativeAPI.FileSystem.OpenFileWrite(MegaProjectFolder + "/" + mapname + MegaProjectExtension, "fs_basepath");
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
        }

        public bool IsLoaded
        {
            get
            {
                return _megaFile != null;
            }
        }
    }
}
