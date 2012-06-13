using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ToolsManaged.Private
{
    class RenderDevice
    {
        private Control _renderControl;
        private IntPtr _currentDC;
        public RenderDevice(Control control)
        {
            _renderControl = control;
        }

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Device_BeginRender")]
        private static extern IntPtr TOOLAPI_Device_BeginRender( IntPtr hwnd, int width, int height );

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_Device_EndRender")]
        private static extern void TOOLAPI_Device_EndRender( IntPtr hwnd, IntPtr hDC );

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_RendererSystem_UnProject")]
        private static extern void TOOLAPI_RendererSystem_UnProject(IntPtr wndHandle, ref float x, ref float y, ref float z);

        public void BeginRender()
        {
            _currentDC = TOOLAPI_Device_BeginRender(_renderControl.Handle, _renderControl.Size.Width, _renderControl.Size.Height);
        }

        public void EndRender()
        {
            TOOLAPI_Device_EndRender(_renderControl.Handle, _currentDC);
            _currentDC = IntPtr.Zero;
        }

        public void UnprojectMouseCoord(ref float x, ref float y, ref float z)
        {
            if (_currentDC == IntPtr.Zero)
                throw new Exception("UnprojectMouseCoord must be called between begin/end render!");
            TOOLAPI_RendererSystem_UnProject(_renderControl.Handle, ref x, ref y, ref z);
        }
    }
}
