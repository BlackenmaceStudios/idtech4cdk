using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using ToolsManaged.Private;
namespace ToolsManaged
{
    [Guid("FBE0E8B4-1C61-41f3-B6A4-2E2F353D3D05")]
    public unsafe interface IEditorWindow
    {
        void OnSize();
        void Redraw();
        void Init();
        IntPtr GetSafeHandle();
        NativeAPI.Win32.RECT GetScreenRect();
        void SetWindowText(string name);
        void OpenEntityContextMenu(IntPtr entdict);
        void ShowWindow();
    }
}
