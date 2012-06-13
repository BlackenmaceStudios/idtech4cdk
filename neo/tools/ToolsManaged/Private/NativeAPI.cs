using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;

using System.Windows.Media.Imaging;
using ToolsManaged.Private.idLib;


namespace ToolsManaged.Private
{
    //
    // NativeAPI
    //
    public unsafe static class NativeAPI
    {
        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Brush_Update")]
        public static extern void UpdateBrush();

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Entity_GetTemplate")]
        private static extern IntPtr TOOLAPI_Entity_GetTemplate(string type);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Entity_GetEntityDict")]
        private static extern IntPtr TOOLAPI_Entity_GetEntityDict(string name);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_GetMapName")]
        private static extern IntPtr TOOLAPI_Editor_GetMapName();

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_GetMaterialNameByIndex")]
        private static extern IntPtr TOOLAPI_Editor_GetMaterialNameByIndex(int indx);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_GetNumMaterials")]
        private static extern int TOOLAPI_Editor_GetNumMaterials();

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_GetDiffuseImageForMaterial")]
        private static extern IntPtr TOOLAPI_Editor_GetDiffuseImageForMaterial(string mtr, ref int width, ref int height);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_LoadWorld")]
        private static extern IntPtr TOOLAPI_Editor_LoadWorld(string path);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_GetViewPosition")]
        private static extern float *TOOLAPI_Editor_GetViewPosition();

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_DrawRenderSurf")]
        private static extern void TOOLAPI_Editor_DrawRenderSurf(IntPtr surf, IntPtr image, float x, float y, float z, float yaw, float pitch, float roll, bool cameraView);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_RendererSystem_DrawPlane")]
        private static extern void TOOLAPI_RendererSystem_DrawPlane(float size, IntPtr image, float x, float y, float z, float yaw, float pitch, float roll);

        public static void DrawPlane(float size, idManagedImage image, float x, float y, float z, float yaw, float pitch, float roll)
        {
            if (image != null)
            {
                TOOLAPI_RendererSystem_DrawPlane(size, image.Handle, x, y, z, yaw, pitch, roll);
            }
            else
            {
                TOOLAPI_RendererSystem_DrawPlane(size, IntPtr.Zero, x, y, z, yaw, pitch, roll);
            }
        }

        public static void GetEditorViewPosition(ref Vector3 v)
        {
            float* xyz = TOOLAPI_Editor_GetViewPosition();

            v.x = xyz[0];
            v.y = xyz[1];
            v.z = xyz[2];
        }

        public static RenderWorld LoadWorld(string path)
        {
            IntPtr ptr;

            ptr = TOOLAPI_Editor_LoadWorld(path);

            if (ptr == IntPtr.Zero)
            {
                return null;
            }

            return new RenderWorld(ptr);
        }

        public unsafe static System.Windows.Media.Imaging.BitmapSource GetDiffuseImageForMaterial(string mtr, ref int width, ref int height)
        {
            byte *imgPtr;
            byte[] imgData;

            imgPtr = (byte *)TOOLAPI_Editor_GetDiffuseImageForMaterial(mtr, ref width, ref height);

            imgData = new byte[width * height * 3];

            for (int i = 0, d = 0; i < width * height * 4; i += 4, d += 3)
            {
                imgData[d + 0] = imgPtr[i + 0];
                imgData[d + 1] = imgPtr[i + 1];
                imgData[d + 2] = imgPtr[i + 2];
            }

            List<System.Windows.Media.Color> colors = new List<System.Windows.Media.Color>();
            colors.Add(System.Windows.Media.Colors.Red);
            colors.Add(System.Windows.Media.Colors.Blue);
            colors.Add(System.Windows.Media.Colors.Green);
 
            BitmapPalette myPalette = new BitmapPalette(colors);



            System.Windows.Media.Imaging.BitmapSource bitmap = WriteableBitmap.Create(width, height, 96, 96, System.Windows.Media.PixelFormats.Rgb24, myPalette, imgData, width * 3);
            

            return bitmap;
        }

        public static int GetNumMaterials()
        {
            return TOOLAPI_Editor_GetNumMaterials();
        }

        public static string GetMaterialNameByIndex( int indx )
        {
            return Marshal.PtrToStringAnsi(TOOLAPI_Editor_GetMaterialNameByIndex(indx));
        }

        public static string GetCurrentMapName()
        {
            return Marshal.PtrToStringAnsi(TOOLAPI_Editor_GetMapName());
        }

        public static idLibNativeAPI.idDictNative Entity_GetEntityDict(string name)
        {
            idLibNativeAPI.idDictNative dict;

            dict = new idLibNativeAPI.idDictNative(TOOLAPI_Entity_GetEntityDict(name));

            return dict;
        }

        public static idLibNativeAPI.idDictNative Entity_GetTemplate(string type)
        {
            idLibNativeAPI.idDictNative dict;

            dict = new idLibNativeAPI.idDictNative(TOOLAPI_Entity_GetTemplate(type));

            return dict;
        }

        public static class DeclManager
        {
            public enum idDeclType_t
            {
                DECL_TABLE = 0,
                DECL_MATERIAL,
                DECL_SKIN,
                DECL_SOUND,
                DECL_ENTITYDEF,
                DECL_MODELDEF,
                DECL_FX,
                DECL_PARTICLE,
                DECL_AF,
                DECL_MODELEXPORT,
                DECL_MAPDEF,
                DECL_OBJECTIVE
            }

            private static class Internal
            {
                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_declManager_FindType")]
                public static extern IntPtr FindType(int type, string name, bool makeDefault);
            }

            public static idLibNativeAPI.idDictNative FindType(idDeclType_t type, string name, bool makeDefault)
            {
                IntPtr ptr;

                ptr = Internal.FindType((int)type, name, makeDefault);

                return new idLibNativeAPI.idDictNative(ptr);
            }
        }

       

        public class idManagedImage
        {
            IntPtr _internalPtr;

            [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_FindImage")]
            private static extern IntPtr TOOLAPI_Editor_FindImage(string path);

            [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Editor_GetDiffuseImageHandleForMaterial")]
            private static extern IntPtr TOOLAPI_Editor_GetDiffuseImageHandleForMaterial( string mtr );

            public idManagedImage(IntPtr ptr)
            {
                _internalPtr = ptr;
            }

            public IntPtr Handle
            {
                get
                {
                    return _internalPtr;
                }
            }

            //
            // FindImage
            //
            public static idManagedImage FindImage(string path)
            {
                IntPtr ptr;

                ptr = TOOLAPI_Editor_FindImage(path);

                if (ptr == IntPtr.Zero)
                    return null;

                return new idManagedImage(ptr);
            }

            //
            // FindImage
            //
            public static idManagedImage GetDiffuseImageHandleForMaterial(string path)
            {
                IntPtr ptr;

                ptr = TOOLAPI_Editor_GetDiffuseImageHandleForMaterial(path);

                if (ptr == IntPtr.Zero)
                    return null;

                return new idManagedImage(ptr);
            }
        }

        //
        // RenderWorld
        //
        public unsafe class RenderWorld
        {
            IntPtr internalPtr;
            int* vtVisibleAreasPool;
            int numVisibleVirtualTextureAreas;

            //
            // RenderWorld
            //
            public RenderWorld(IntPtr ptr)
            {
                internalPtr = ptr;
                numVisibleVirtualTextureAreas = 0;
            }

            public IntPtr NativeHandle
            {
                get
                {
                    return internalPtr;
                }
            }

            [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_RenderWorld_GetVisibleVirtualTextureArea")]
            private static extern int *TOOLAPI_RenderWorld_GetVisibleVirtualTextureArea( IntPtr world, ref int numSurfaces, float width, float height, float x, float y, float z, float yaw, float pitch, float roll );

            [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_RenderWorld_GetVisibleVirtualTextureAreaSurface")]
            private static extern IntPtr TOOLAPI_RenderWorld_GetVisibleVirtualTextureAreaSurface(IntPtr world, int surfId);

            public int FindVisibleVirtualTextureAreas(float width, float height, float x, float y, float z, float yaw, float pitch, float roll)
            {
                vtVisibleAreasPool = TOOLAPI_RenderWorld_GetVisibleVirtualTextureArea(internalPtr, ref numVisibleVirtualTextureAreas, width, height, x, y, z, yaw, pitch, roll);

                return numVisibleVirtualTextureAreas;
            }

            public void RenderVisibleArea(idManagedImage image, int areaId, float x, float y, float z, float yaw, float pitch, float roll)
            {
                IntPtr surf = TOOLAPI_RenderWorld_GetVisibleVirtualTextureAreaSurface(internalPtr, vtVisibleAreasPool[areaId]);
                TOOLAPI_Editor_DrawRenderSurf(surf, image.Handle, x, y, z, yaw, pitch, roll, false); 
            }
        }

        //
        // File
        //
        public class File : NativeClass
        {
            private delegate void Deconstructor_t();
            						// Get the name of the file.
            private delegate string GetName_t(IntPtr obj);
							        // Get the full file path.
            private delegate string GetFullPath_t(IntPtr obj);
							        // Read data from the file to the buffer.
	        private delegate int		Read_t( IntPtr obj,IntPtr buffer, int len );
							        // Write data from the buffer to the file.
            private delegate int Write_t(IntPtr obj, IntPtr buffer, int len);
							        // Returns the length of the file.
	        private delegate int		Length_t( IntPtr obj );
	        private delegate Int64	Length64_t( IntPtr obj );
							        // Return a time value for reload operations.
	        private delegate IntPtr	Timestamp_t( IntPtr obj );
							        // Returns offset in file.
	        private delegate int		Tell_t(  IntPtr obj);
							        // Forces flush on files being writting to.
	        private delegate void	ForceFlush_t( IntPtr obj );
							        // Causes any buffered data to be written to the file.
	        private delegate void	Flush_t( IntPtr obj );
							        // Seek on a file.
            private delegate int Seek_t(IntPtr obj, long offset, FileSystem.fsOrigin origin);
            private delegate Int64 Seek64_t(IntPtr obj, Int64 offset, FileSystem.fsOrigin origin);
							        // Go back to the beginning of the file.
	        private delegate void	Rewind_t(  IntPtr obj );
							        // Like fprintf.
            private delegate int Printf_t(IntPtr obj, string format, IntPtr Args);
							        // Like fprintf but with argument pointer
            private delegate int VPrintf_t(IntPtr obj, string format, IntPtr va_args);
							        // Write a string with high precision floating point numbers to the file.
            private delegate int WriteFloatString_t(IntPtr obj, string format, IntPtr Args);
	
	        // Endian portable alternatives to Read(...)
            private delegate int ReadInt_t(IntPtr obj, ref int value);
            private delegate int ReadUnsignedInt_t(IntPtr obj, ref uint value);
            private delegate int ReadShort_t(IntPtr obj, ref short value);
            private delegate int ReadUnsignedShort_t(IntPtr obj, ref ushort value);
            private delegate int ReadChar_t(IntPtr obj, ref char value);
            private delegate int ReadUnsignedChar_t(IntPtr obj, ref char value);
            private delegate int ReadFloat_t(IntPtr obj, ref float value);
            private delegate int ReadBool_t(IntPtr obj, ref bool value);
            private delegate int ReadString_t(IntPtr obj, ref string str);
            private delegate int ReadVec2_t(IntPtr obj, ref IntPtr vec);
            private delegate int ReadVec3_t(IntPtr obj, ref IntPtr vec);
            private delegate int ReadVec4_t(IntPtr obj, ref IntPtr vec);
            private delegate int ReadVec6_t(IntPtr obj, ref IntPtr vec);
            private delegate int ReadMat3_t(IntPtr obj, ref IntPtr mat);
	
	        // Endian portable alternatives to Write(...)
            private delegate int WriteInt_t(IntPtr obj, int value);
            private delegate int WriteUnsignedInt_t(IntPtr obj, uint value);
            private delegate int WriteShort_t(IntPtr obj, short value);
            private delegate int WriteUnsignedShort_t(IntPtr obj, ushort value);
            private delegate int WriteChar_t(IntPtr obj, char value);
            private delegate int WriteUnsignedChar_t(IntPtr obj, char value);
            private delegate int WriteFloat_t(IntPtr obj, float value);
            private delegate int WriteBool_t(IntPtr obj, bool value);
            private delegate int WriteString_t(IntPtr obj, string str);
            private delegate int WriteVec2_t(IntPtr obj, IntPtr vec);
            private delegate int WriteVec3_t(IntPtr obj, IntPtr vec);
            private delegate int WriteVec4_t(IntPtr obj, IntPtr vec);
            private delegate int WriteVec6_t(IntPtr obj, IntPtr vec);
            private delegate int WriteMat3_t(IntPtr obj, IntPtr mat);

            private Deconstructor_t Deconstructor_Internal;

            // Get the name of the file.
            private GetName_t GetName_Internal;
            // Get the full file path.
            private GetFullPath_t GetFullPath_Internal;
            // Read data from the file to the buffer.
            private Read_t Read_Internal;
            // Write data from the buffer to the file.
            private Write_t Write_Internal;
            // Returns the length of the file.
            private Length_t Length_Internal;
            private Length64_t Length64_Internal;
            // Return a time value for reload operations.
            private Timestamp_t Timestamp_Internal;
            // Returns offset in file.
            private Tell_t Tell_Internal;
            // Forces flush on files being writting to.
            private ForceFlush_t ForceFlush_Internal;
            // Causes any buffered data to be written to the file.
            private Flush_t Flush_Internal;
            // Seek on a file.
            private Seek_t Seek_Internal;
            private Seek64_t Seek64_Internal;
            // Go back to the beginning of the file.
            private Rewind_t Rewind_Internal;
            // Like fprintf.
            private Printf_t Printf_Internal;
            // Like fprintf but with argument pointer
            private VPrintf_t VPrintf_t_Internal;
            // Write a string with high precision floating point numbers to the file.
            private WriteFloatString_t WriteFloatString_Internal;

            // Endian portable alternatives to Read(...)
            private ReadInt_t ReadInt_Internal;
            private ReadUnsignedInt_t ReadUnsignedInt_Internal;
            private ReadShort_t ReadShort_Internal;
            private ReadUnsignedShort_t ReadUnsignedShort_Internal;
            private ReadChar_t ReadChar_Internal;
            private ReadUnsignedChar_t ReadUnsignedChar_Internal;
            private ReadFloat_t ReadFloat_Internal;
            private ReadBool_t ReadBool_Internal;
            private ReadString_t ReadString_Internal;
            private ReadVec2_t ReadVec2_Internal;
            private ReadVec3_t  ReadVec3_Internal;
            private ReadVec4_t ReadVec4_Internal;
            private ReadVec6_t  ReadVec6_Internal;
            private ReadMat3_t ReadMat3_Internal;

            // Endian portable alternatives to Write(...)
            private WriteInt_t WriteInt_Internal;
            private WriteUnsignedInt_t WriteUnsignedInt_Internal;
            private WriteShort_t WriteShort_Internal;
            private WriteUnsignedShort_t WriteUnsignedShort_Internal;
            private WriteChar_t WriteChar_Internal;
            private WriteUnsignedChar_t WriteUnsignedChar_Internal;
            private WriteFloat_t WriteFloat_Internal;
            private WriteBool_t WriteBool_Internal;
            private WriteString_t WriteString_Internal;
            private WriteVec2_t WriteVec2_Internal;
            private WriteVec3_t WriteVec3_Internal;
            private WriteVec4_t WriteVec4_Internal;
            private WriteVec6_t WriteVec6_Internal;
            private WriteMat3_t WriteMat3_Internal;
            

            public void Free()
            {
                Deconstructor_Internal();
            }

            // Get the name of the file.
            public string GetName()
            {
                return GetName_Internal(GetNativeAddress());
            }

            // Get the full file path.
            public string GetFullPath()
            {
                return GetFullPath_Internal(GetNativeAddress());
            }
            // Read data from the file to the buffer.
            public int Read(ref object buffer, int len)
            {
                IntPtr tempBufferBlockPtr;

                tempBufferBlockPtr = Marshal.AllocHGlobal( len );
                int readLen = Read_Internal( GetNativeAddress(), tempBufferBlockPtr, len );
                Marshal.PtrToStructure(tempBufferBlockPtr, buffer);
                Marshal.FreeHGlobal( tempBufferBlockPtr );

                return readLen;
            }

            // Read data from the file to the buffer.
            public unsafe int ReadUnsafe(void *buffer, int len)
            {
                int readLen = Read_Internal(GetNativeAddress(), (IntPtr)buffer, len);
                return readLen;
            }

            // Write data from the buffer to the file.
            public int Write(object buffer, int len)
            {
                IntPtr tempBufferBlockPtr;

                tempBufferBlockPtr = Marshal.AllocHGlobal(len);
                Marshal.StructureToPtr(buffer, tempBufferBlockPtr, false);

                int writeLen = Write_Internal(GetNativeAddress(), tempBufferBlockPtr, len);
                Marshal.FreeHGlobal(tempBufferBlockPtr);

                return writeLen;
            }

            // Write data from the buffer to the file.
            public unsafe int WriteUnsafe(void *buffer, int len)
            {
                int writeLen = Write_Internal(GetNativeAddress(), (IntPtr)buffer, len);

                return writeLen;
            }

            // Returns the length of the file.
            public int Length()
            {
                return Length_Internal(GetNativeAddress());
            }
            public Int64 Length64()
            {
                return Length64_Internal(GetNativeAddress());
            }

            // Return a time value for reload operations.
            public IntPtr Timestamp()
            {
                return IntPtr.Zero;
            }
            // Returns offset in file.
            public int Tell()
            {
                return Tell_Internal(GetNativeAddress());
            }
            // Forces flush on files being writting to.
            public void ForceFlush()
            {
                ForceFlush_Internal(GetNativeAddress());
            }
            // Causes any buffered data to be written to the file.
            public void Flush()
            {
                Flush_Internal(GetNativeAddress());
            }
            // Seek on a file.
            public int Seek(long offset, FileSystem.fsOrigin origin)
            {
                return Seek_Internal(GetNativeAddress(), offset, origin);
            }
            public Int64 Seek64(Int64 offset, FileSystem.fsOrigin origin)
            {
                return Seek64_Internal(GetNativeAddress(), offset, origin);
            }
            // Go back to the beginning of the file.
            public void Rewind()
            {
                Rewind_Internal(GetNativeAddress());
            }
            // Like fprintf.
            public int Printf(string format, IntPtr Args) { return 0; }
            // Like fprintf but with argument pointer
            public int VPrintf(string format, IntPtr va_args) { return 0; }
            // Write a string with high precision floating point numbers to the file.
            public int WriteFloatString(string format, IntPtr Args) { return 0; }

            // Endian portable alternatives to Read(...)
            public int ReadInt(ref int value)
            {
                return ReadInt_Internal(GetNativeAddress(), ref value);
            }
            public int ReadUnsignedInt(ref uint value)
            {
                return ReadUnsignedInt_Internal(GetNativeAddress(), ref value);
            }
            public int ReadShort(ref short value)
            {
                return ReadShort_Internal(GetNativeAddress(), ref value);
            }
            public int ReadUnsignedShort(ref ushort value)
            {
                return ReadUnsignedShort_Internal(GetNativeAddress(), ref value);
            }
            public int ReadChar(ref char value)
            {
                return ReadChar_Internal(GetNativeAddress(), ref value);
            }
            public int ReadUnsignedChar(ref char value)
            {
                return ReadChar_Internal(GetNativeAddress(), ref value);
            }
            public int ReadFloat(ref float value)
            {
                return ReadFloat_Internal(GetNativeAddress(), ref value);
            }
            public int ReadBool(ref bool value)
            {
                return ReadBool_Internal(GetNativeAddress(), ref value);
            }
            public int ReadString(ref String str)
            {
                return ReadString_Internal(GetNativeAddress(), ref str);
            }
            public int ReadVec2(ref IntPtr vec)
            {
                return ReadVec2_Internal(GetNativeAddress(), ref vec);
            }
            public int ReadVec3(ref IntPtr vec)
            {
                return ReadVec3_Internal(GetNativeAddress(), ref vec);
            }
            public int ReadVec4(ref IntPtr vec)
            {
                return ReadVec4_Internal(GetNativeAddress(), ref vec);
            }
            public int ReadVec6(ref IntPtr vec)
            {
                return ReadVec6_Internal(GetNativeAddress(), ref vec);
            }
            public int ReadMat3(ref IntPtr mat)
            {
                return ReadMat3_Internal(GetNativeAddress(), ref mat);
            }

            // Endian portable alternatives to Write(...)
            public int WriteInt(int value)
            {
                return WriteInt_Internal(GetNativeAddress(), value);
            }
            public int WriteUnsignedInt(uint value)
            {
                return WriteUnsignedInt_Internal(GetNativeAddress(), value);
            }
            public int WriteShort(short value)
            {
                return WriteShort_Internal(GetNativeAddress(), value);
            }
            public int WriteUnsignedShort(ushort value)
            {
                return WriteUnsignedShort_Internal(GetNativeAddress(), value);
            }
            public int WriteChar(char value)
            {
                return WriteChar_Internal(GetNativeAddress(), value);
            }
            public int WriteUnsignedChar(char value)
            {
                return WriteUnsignedChar_Internal(GetNativeAddress(), value);
            }
            public int WriteFloat(float value)
            {
                return WriteFloat_Internal(GetNativeAddress(), value);
            }
            public int WriteBool(bool value)
            {
                return WriteBool_Internal(GetNativeAddress(), value);
            }
            public int WriteString(string str)
            {
                return WriteString_Internal(GetNativeAddress(), str);
            }
            public int WriteVec2(IntPtr vec)
            {
                return WriteVec2_Internal(GetNativeAddress(), vec);
            }
            public int WriteVec3(IntPtr vec)
            {
                return WriteVec3_Internal(GetNativeAddress(), vec);
            }
            public int WriteVec4(IntPtr vec)
            {
                return WriteVec4_Internal(GetNativeAddress(), vec);
            }
            public int WriteVec6(IntPtr vec)
            {
                return WriteVec6_Internal(GetNativeAddress(), vec);
            }
            public int WriteMat3(IntPtr mat)
            {
                return WriteMat3_Internal(GetNativeAddress(), mat);
            }
        }
        
        public static class FileSystem
        {

            // mode parm for Seek
            public enum fsOrigin {
	            FS_SEEK_CUR,
	            FS_SEEK_END,
	            FS_SEEK_SET
            };
            public static class Internal
            {
                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_OSPathToRelativePath")]
                public static extern IntPtr OSPathToRelativePath(string str);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_RelativePathToOSPath")]
                public static extern IntPtr RelativePathToOSPath(string str);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_OpenFileRead")]
                public static extern IntPtr OpenFileRead(string str, bool allowCopyFiles, string gamedir);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_OpenFileWrite")]
                public static extern IntPtr OpenFileWrite(string str, string gamedir);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_CloseFile")]
                public static extern void CloseFile( IntPtr nativePtr );
            }

            public static File OpenFileRead(string str, bool allowCopyFiles, string gamedir)
            {
                IntPtr filePtr = Internal.OpenFileRead(str, allowCopyFiles, gamedir);

                if (filePtr == IntPtr.Zero)
                    return null;

                File f = new File();

                f.AttachToMemory(typeof(File), filePtr, true);

                return f;
            }

           
            public static File OpenFileWrite(string str, string gamedir)
            {
                IntPtr filePtr = Internal.OpenFileWrite(str, gamedir);

                if (filePtr == IntPtr.Zero)
                    return null;

                File f = new File();

                f.AttachToMemory(typeof(File), filePtr, true);

                return f;
            }
            
            public static string OSPathToRelativePath(string str)
            {
                return Marshal.PtrToStringAnsi(Internal.OSPathToRelativePath(str));
            }

            public static string RelativePathToOSPath(string str)
            {
                return Marshal.PtrToStringAnsi(Internal.RelativePathToOSPath(str));
            }


            public static void CloseFile(ref File f)
            {
                if (f == null)
                {
                    throw new Exception("CloseFile with null file handle");
                }

                Internal.CloseFile(f.GetNativeAddress());
                f = null;
            }
        }


        public static class CmdSystem
        {
            [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_cmdSystem_BufferCommandText")]
            public static extern void BufferCommandText(string str);
        }

        public static class Win32
        {
            [StructLayout(LayoutKind.Sequential)]
            public struct RECT
            {
                public int Left;        // x position of upper-left corner
                public int Top;         // y position of upper-left corner
                public int Right;       // x position of lower-right corner
                public int Bottom;      // y position of lower-right corner
            }

            [DllImport("user32.dll")]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool GetWindowRect(IntPtr hwnd, out RECT lpRect);
        }

        public class idManagedEditorWindowNative : NativeClass
        {
             [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
            public struct CREATESTRUCT
            {
                public IntPtr lpCreateParams;
                public IntPtr hInstance;
                public IntPtr hMenu;
                public IntPtr hwndParent;
                public int cy;
                public int cx;
                public int y;
                public int x;
                public int style;
                public string lpszName;
                public string lpszClass;
                public int dwExStyle;
            }

            [StructLayout(LayoutKind.Sequential)] 
            public struct RECT 
            { 
                public int Left; 
                public int Top; 
                public int Right; 
                public int Bottom; 
            }

            [StructLayout(LayoutKind.Sequential)] 
            public struct Point
            {
                public int x;
                public int y;
            }



            public idManagedEditorWindowNative()
            {

            }

            public delegate int OnCreate_t( IntPtr obj, IntPtr lpCreateStruct);
            public delegate void OnLButtonDown_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnMButtonDown_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnRButtonDown_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnLButtonUp_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnMButtonUp_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnRButtonUp_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnMouseMove_t(IntPtr obj, UInt32 nFlags, Point point);
            public delegate void OnPaint_t(IntPtr obj,IntPtr obj2);
            public delegate void OnKeyDown_t(IntPtr obj, UInt32 nChar, UInt32 nRepCnt, UInt32 nFlags);
            public delegate void OnSize_t(IntPtr obj, UInt32 nType, int cx, int cy);
            public delegate void OnDestroy_t(IntPtr obj);
            public delegate void OnSelectMouserotate_t(IntPtr obj);
            public delegate void OnTimer_t(IntPtr obj, IntPtr nIDEvent);
            public delegate void OnKeyUp_t(IntPtr obj, UInt32 nChar, UInt32 nRepCnt, UInt32 nFlags);
            public delegate void OnNcCalcSize_t(IntPtr obj, bool bCalcValidRects, IntPtr lpncsp);
            public delegate void OnKillFocus_t(IntPtr obj, IntPtr pNewWnd);
            public delegate void OnSetFocus_t(IntPtr obj, IntPtr pOldWnd);
            public delegate void OnClose_t(IntPtr obj);
            public delegate bool OnEraseBkgnd_t(IntPtr obj, IntPtr pDC);
            public delegate void OnDropNewmode_t(IntPtr obj);
            public delegate bool OnMouseWheel_t(IntPtr obj, UInt32 nFlags, short zDelta, Point pt);
            public delegate bool OnCmdMsg_t(IntPtr obj, UInt32 nID, int nCode, IntPtr pExtra, IntPtr pHandlerInfo);
            public delegate bool Create_t(IntPtr obj, string lpszText, int dwStyle, ref RECT rect, IntPtr pParentWnd, UInt32 nID);
            public delegate bool CreateEx_t(IntPtr obj, string id, string lpszText, int dwStyle, ref RECT rect, IntPtr pParentWnd, UInt32 nID);
            public delegate bool RedrawWindow_t(IntPtr obj, ref RECT lpRectUpdate, IntPtr prgnUpdate, UInt32 flags);
            public delegate void Destroy_t(IntPtr obj);
            public delegate void OnAddEntityEvent_t(IntPtr obj, string entityType, Point pt);
            public delegate void OnNamedEvent_t(IntPtr obj, string eventName);

            public OnCreate_t OnCreate; // (IntPtr lpCreateStruct);
            public OnLButtonDown_t OnLButtonDown; // (UInt32 nFlags, Point point);
            public OnMButtonDown_t OnMButtonDown; // (UInt32 nFlags, Point point);
            public OnRButtonDown_t OnRButtonDown; // (UInt32 nFlags, Point point);
            public OnLButtonUp_t OnLButtonUp; // (UInt32 nFlags, Point point);
            public OnMButtonUp_t OnMButtonUp;// (UInt32 nFlags, Point point);
            public OnRButtonUp_t OnRButtonUp; // (UInt32 nFlags, Point point);
            public OnMouseMove_t OnMouseMove;// (UInt32 nFlags, Point point);
            public OnPaint_t OnPaint;// ();
            public OnKeyDown_t OnKeyDown; // (UInt32 nChar, UInt32 nRepCnt, UInt32 nFlags);
            public OnSize_t OnSize; // (UInt32 nType, int cx, int cy);
            public OnDestroy_t OnDestroy; // ();
            public OnSelectMouserotate_t OnSelectMouserotate; // ();
            public OnTimer_t OnTimer; // (IntPtr nIDEvent);
            public OnKeyUp_t OnKeyUp; // (UInt32 nChar, UInt32 nRepCnt, UInt32 nFlags);
            public OnNcCalcSize_t OnNcCalcSize;// (bool bCalcValidRects, IntPtr lpncsp);
            public OnKillFocus_t OnKillFocus;// (IntPtr pNewWnd);
            public OnSetFocus_t OnSetFocus; // (IntPtr pOldWnd);
            public OnClose_t OnClose;// ();
            public OnEraseBkgnd_t OnEraseBkgnd; // (IntPtr pDC);
            public OnDropNewmode_t OnDropNewmode;// ();
            public OnMouseWheel_t OnMouseWheelPtr; // (UInt32 nFlags, short zDelta, Point pt);
            public OnCmdMsg_t OnCmdMsg; // (UInt32 nID, int nCode, IntPtr pExtra, IntPtr pHandlerInfo);
            public Create_t Create;// (string lpszText, int dwStyle, ref RECT rect, IntPtr pParentWnd, UInt32 nID);
            public CreateEx_t CreateEx;// (string id, string lpszText, int dwStyle, ref RECT rect, IntPtr pParentWnd, UInt32 nID);
             public RedrawWindow_t RedrawWindow; // (ref RECT lpRectUpdate, IntPtr prgnUpdate, UInt32 flags);
             public Destroy_t Destroy; // ();
             public OnAddEntityEvent_t OnAddEntityEvent;
             public OnNamedEvent_t OnNamedEvent;
        }
    }
}
