using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;

using ToolsManaged.Private.idLib;

namespace ToolsManaged.Private
{
    //
    // NativeAPI
    //
    public static class NativeAPI
    {
        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Brush_Update")]
        public static extern void UpdateBrush();

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Entity_GetTemplate")]
        private static extern IntPtr TOOLAPI_Entity_GetTemplate(string type);

        public static idLibNativeAPI.idDictNative Entity_GetTemplate(string type)
        {
            idLibNativeAPI.idDictNative dict;

            dict = new idLibNativeAPI.idDictNative(TOOLAPI_Entity_GetTemplate(type));

            return dict;
        }
        
        
        
        public static class FileSystem
        {
            public static class Internal
            {
                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_OSPathToRelativePath")]
                public static extern IntPtr OSPathToRelativePath(string str);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_fileSystem_RelativePathToOSPath")]
                public static extern IntPtr RelativePathToOSPath(string str);

                
            }
            
            public static string OSPathToRelativePath(string str)
            {
                return Marshal.PtrToStringAnsi(Internal.OSPathToRelativePath(str));
            }

            public static string RelativePathToOSPath(string str)
            {
                return Marshal.PtrToStringAnsi(Internal.RelativePathToOSPath(str));
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

        }
    }
}
