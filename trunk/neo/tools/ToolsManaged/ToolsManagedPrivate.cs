using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend;
using ToolsManaged.Frontend.WindowAPI;
using System.Runtime.InteropServices;

namespace ToolsManaged
{
    //
    // ToolsManagedPrivate
    //

    [Guid("C6659361-1625-4746-931C-36014B146679")]
    public class ToolsManagedPrivate : IToolsManaged
    {
        FormManager _formManager;
        DebugConsole _debugConsoleFrm;
        ToolBoxDock _toolBoxFrm;
        EntityProperties _entityPropertiesFrm;

        bool consoleHack = false;

        public static ToolsManagedPrivate handle;

        public ToolsManagedPrivate()
        {
            handle = this;
        }

        //
        // ShowDebugConsole
        //
        public void ShowDebugConsole()
        {
            if(_debugConsoleFrm == null)
                _debugConsoleFrm = _formManager.CreateWindow<DebugConsole>(true);

            
            _debugConsoleFrm.Show();
            _debugConsoleFrm.BringToFront();
            _debugConsoleFrm.Refresh();
            
            
        }

        //
        // PrintToConsole
        //
        public void PrintToConsole(string s)
        {
            if (_debugConsoleFrm == null)
                return;

            _debugConsoleFrm.Printf(s);
        }


        public void NewMapEvent()
        {
            _toolBoxFrm.ClearEntityList();
            AddEntity("Worldspawn", "");
        }

        public void AddEntity(string type, string name)
        {
            _toolBoxFrm.AddEntity(type, name);
        }
        

        //
        // CreateEditorWindows
        //
        public void CreateEditorWindows(IntPtr parentHwnd)
        {
            NativeWindow parentFrm = new NativeWindow();
            parentFrm.AssignHandle(parentHwnd);

            _formManager = new FormManager(parentFrm);

            // Create all the forms.
            _toolBoxFrm = _formManager.CreateWindow<ToolBoxDock>( true, DockStyle.Left);
            
        }

        //
        // ShowEntityPropterties
        //
        public void ShowEntityPropterties(idLibNativeAPI.idDictNative dict)
        {
            if (_entityPropertiesFrm != null)
            {
               
                _entityPropertiesFrm.Dispose();
                _entityPropertiesFrm = null;
            }
            _entityPropertiesFrm = new EntityProperties();
            _entityPropertiesFrm.ShowWindow(dict);
            _entityPropertiesFrm.BringToFront();
        }

        //
        // HideEntityProperties
        //
        public void HideEntityProperties()
        {
            if (_entityPropertiesFrm != null)
            {
                _entityPropertiesFrm.Hide();
                _entityPropertiesFrm.Dispose();
                _entityPropertiesFrm = null;
            }
        }

        //
        // UpdateDockedWindows
        //
        public void UpdateDockedWindows()
        {
            if (_formManager == null)
                return;


            _formManager.UpdateDockedWindows();
        }

        public void AddNewEntityType(string folder, string entityType)
        {
            _toolBoxFrm.AddNewEntityType(folder, entityType);
        }

        //
        // CreateEditorWindow
        //
        public IEditorWindow CreateEditorWindow(string WindowClassName, IntPtr address)
        {
            Type windowType = Assembly.GetExecutingAssembly().GetType("ToolsManaged.Frontend." + WindowClassName);
            IEditorWindow window;

            Form form = _formManager.CreateWindow(windowType, true);

            window = new EditorWindow(form, address);
           
            _formManager.editors.Add(window);

            FieldInfo fi = windowType.GetField("window");
            if (fi != null)
            {
                
                fi.SetValue(form, window);
            }

            return window;
        }
    }
}
