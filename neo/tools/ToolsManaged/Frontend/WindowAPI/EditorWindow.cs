using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;


using ToolsManaged.Private;
using ToolsManaged.Private.Editor.Input;
namespace ToolsManaged.Frontend.WindowAPI
{
    //
    // EditorWindow
    //

    [Guid("FAA0EAB1-9A61-41f3-B6A4-292F953D3D05")]
    public unsafe class EditorWindow : IEditorWindow
    {
        private NativeAPI.idManagedEditorWindowNative _nativeWindow;
        private Form _window;


   

        //
        // EditorWindow Constructor.
        //
        public EditorWindow(Form form, IntPtr pAddress)
        {
            _nativeWindow = new NativeAPI.idManagedEditorWindowNative();
            _nativeWindow.AttachToMemory(typeof(NativeAPI.idManagedEditorWindowNative), pAddress);
            _window = form;
           
            
        }

        public void OnNamedEvent(string eventName)
        {
            _nativeWindow.OnNamedEvent(_nativeWindow.GetNativeAddress(),eventName);
        }

        public NativeAPI.Win32.RECT GetScreenRect()
        {
            NativeAPI.Win32.RECT rect;

            if (_window.GetType().Name == "Viewport")
            {
                Viewport viewport = (Viewport)_window;
                NativeAPI.Win32.GetWindowRect(viewport.GetPanelHandle(), out rect);
            }
            else if (_window.GetType().Name == "ModelStudio")
            {
                ModelStudio viewport = (ModelStudio)_window;
                NativeAPI.Win32.GetWindowRect(viewport.GetPanelHandle(), out rect);
            }
            else
            {
                NativeAPI.Win32.GetWindowRect(_window.Handle, out rect);
            }
            

            return rect;
        }

        public void OnSize()
        {
            _nativeWindow.OnSize(_nativeWindow.GetNativeAddress(), 0, 0, 0);
        }

        public void Redraw()
        {
            _nativeWindow.OnPaint(_nativeWindow.GetNativeAddress(), IntPtr.Zero);
        }

        public void OnKeyDown(Keys key)
        {
            
            _nativeWindow.OnKeyDown(_nativeWindow.GetNativeAddress(), (uint)key, 0, 0);
        }

        public void OnKeyUp(Keys key)
        {
           
            _nativeWindow.OnKeyUp(_nativeWindow.GetNativeAddress(), (uint)key, 0, 0);
        }

        public void OnAddEntityEvent(string entityType, NativeAPI.idManagedEditorWindowNative.Point pt)
        {
            _nativeWindow.OnAddEntityEvent(_nativeWindow.GetNativeAddress(), entityType, pt);
        }

        public void OpenBrushContextMenu()
        {
            Viewport viewport = (Viewport)_window;



            viewport.OpenBrushContextMenu();
        }

        public void OpenEntityContextMenu(IntPtr dict)
        {
            Viewport viewport = (Viewport)_window;



            viewport.OpenEntityContextMenu(dict);
        }

        public void OnMouseMove(MouseButtons button, NativeAPI.idManagedEditorWindowNative.Point point)
        {
            uint val = 0;

            if (button == MouseButtons.Left)
            {
                val = 0x0001;
            }
            _nativeWindow.OnMouseMove(_nativeWindow.GetNativeAddress(),val, point);
        }

        public void OnMouseScroll(short delta, NativeAPI.idManagedEditorWindowNative.Point point)
        {
            _nativeWindow.OnMouseWheelPtr(_nativeWindow.GetNativeAddress(), 0, delta, point);
        }
        private KeyHandler.VirtualKeyStates _lastKey;
        public void OnMouseDownLeft(Keys key, NativeAPI.idManagedEditorWindowNative.Point point)
        {
            uint keyval = 0x0001;
            if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_SHIFT))
            {
                keyval |= 0x0004;
                _lastKey = KeyHandler.VirtualKeyStates.VK_SHIFT;
            }
            if (KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_LMENU))
            {
                keyval |= 0xA4;
            }
            
            _nativeWindow.OnLButtonDown(_nativeWindow.GetNativeAddress(), keyval, point);
        }

        public void OnMouseUpLeft(Keys key, NativeAPI.idManagedEditorWindowNative.Point point)
        {
            uint keyval = 0x0001;
            if (!KeyHandler.IsKeyDown(KeyHandler.VirtualKeyStates.VK_SHIFT) && _lastKey == KeyHandler.VirtualKeyStates.VK_SHIFT)
            {
                keyval |= 0x0004;
                _lastKey = KeyHandler.VirtualKeyStates.VK_NONAME;
            }
            _nativeWindow.OnLButtonUp(_nativeWindow.GetNativeAddress(), keyval, point);


        }

        public void OnMouseDownRight(Keys key, NativeAPI.idManagedEditorWindowNative.Point point)
        {
            _nativeWindow.OnRButtonDown(_nativeWindow.GetNativeAddress(), 0x0002, point);
        }

        public void OnMouseUpRight(Keys key, NativeAPI.idManagedEditorWindowNative.Point point)
        {
            _nativeWindow.OnRButtonUp(_nativeWindow.GetNativeAddress(), 0x0002, point);
        }

        public void Init()
        {
            NativeAPI.Win32.RECT r = GetScreenRect();

            _nativeWindow.OnCreate(_nativeWindow.GetNativeAddress(), IntPtr.Zero);
            _nativeWindow.OnSize(_nativeWindow.GetNativeAddress(),0, 0, 0);
        }

        public void ShowWindow()
        {
            _window.Show();
        }

        public IntPtr GetSafeHandle()
        {
            return _window.Controls["panel1"].Handle;
        }

        public void SetWindowText(string name)
        {
            if (_window.GetType().Name == "Viewport")
            {
                Viewport viewport = (Viewport)_window;

                viewport.SetWindowTitle(name);
            }
        }
    }
}
