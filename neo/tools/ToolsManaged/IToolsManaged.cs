// ToolInterface.cs
//

using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using ToolsManaged.Private;
namespace ToolsManaged
{
    //
    // IToolsManaged
    //

    [Guid("DBE0E8C4-1C61-41f3-B6A4-4E2F353D3D05")]
    public interface IToolsManaged
    {
        void CreateEditorWindows(IntPtr parentHwnd);
        void PrintToConsole(string s);
        void ShowDebugConsole();
        void UpdateDockedWindows();
        void NewMapEvent();
        void AddEntity(string type, string name);
        void AddNewEntityType(string folder, string entityType);

        IEditorWindow CreateEditorWindow(string WindowClassName, IntPtr address);
    }
}
