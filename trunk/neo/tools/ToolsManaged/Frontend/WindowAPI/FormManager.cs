using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Reflection;
using System.Runtime.InteropServices;

using ToolsManaged.Frontend;
using ToolsManaged.Private;

namespace ToolsManaged.Frontend.WindowAPI
{
    class FormManager
    {
        private List<Form> windows = new List<Form>();
        public List<IEditorWindow> editors = new List<IEditorWindow>();
        private IWin32Window _parent;
        private float leftSideBorder;
        private Form _fullscreenWindow;
        private Form _fullscreenModalWindow;

        public static FormManager handle;

        //
        // FormManager
        //
        public FormManager(IWin32Window parent)
        {
            _parent = parent;
            handle = this;
        }

        public void ForceRepaint()
        {
            foreach (Form f in windows)
            {
                f.Invalidate();
            }
        }

        //
        // FindFormFromType
        //
        public Form FindFormFromType(string type)
        {
            Form ret = null;
            Type t = Type.GetType("ToolsManaged.Frontend." + type);

            foreach (Form f in windows)
            {
                if (f.GetType() == t)
                    ret = f;
            }
            return ret;
        }

        //
        // SetFullscreenWindow
        //
        public void SetFullsreenModalWindow(Form form)
        {
            if (form == null && _fullscreenModalWindow != null)
            {
                foreach (Form window in windows)
                {
                    if (window.GetType().Name == "Viewport")
                    {
                        window.Show();

                    }

                    if (window.GetType().Name == "ToolBoxDock")
                    {
                     //   window.Show();

                    }

                    if (window.GetType().Name == "DebugConsole")
                    {
                        window.Show();

                    }
                }
                _fullscreenModalWindow.Visible = false;
            }
            _fullscreenModalWindow = form;
            UpdateDockedWindows();
        }

        //
        // SetFullscreenWindow
        //
        public void SetFullsreenWindow(Form form)
        {
            _fullscreenWindow = form;
            UpdateDockedWindows();
        }

        //
        // CreateWindow
        //
        public T CreateWindow<T>(bool isVisible,  DockStyle dockStyle = DockStyle.None) where T : Form, new()
        {
            T window = new T();


            window.Dock = dockStyle;


            if (isVisible)
            {
                window.Show(_parent);
                window.BringToFront();
            }

            windows.Add(window);

            return window;
        }

        //
        // CreateWindow
        //
        public Form CreateWindow(Type type, bool isVisible, DockStyle dockStyle = DockStyle.None)
        {
            Form window;
            Type[] types = new Type[1];
            types[0] = type;

           
            
            window = (Form)Activator.CreateInstance(type);


            window.Dock = dockStyle;


            if (isVisible)
            {
                window.Show(_parent);
                window.BringToFront();
            }

            windows.Add(window);

            return window;
        }

        //
        // UpdateDockedWindows
        //
        public void UpdateDockedWindows()
        {
            NativeAPI.Win32.RECT rect;
            NativeAPI.Win32.GetWindowRect( _parent.Handle, out rect );

            int height = (int)Math.Abs(rect.Top - rect.Bottom);
            leftSideBorder = 172;

            int viewportWidth = (int)Math.Abs(rect.Left - rect.Right) - (int)leftSideBorder;
            

            int viewportHeight = (int)Math.Abs(rect.Top - rect.Bottom) - 138;

            if (_fullscreenWindow == null && _fullscreenModalWindow == null)
            {
                viewportHeight /= 2;
                viewportWidth /= 2;
            }
            else if (_fullscreenModalWindow == null && ((Viewport)_fullscreenWindow).is3DView)
            {
                viewportHeight = 720;
                viewportWidth = 1280;
            }

            if (_fullscreenModalWindow != null)
            {
                // Ensure all the windows including the sidebar are hidden.
                foreach (Form window in windows)
                {
                    if (window != _fullscreenModalWindow && window.Visible)
                    {
                        if (window.GetType().Name != "DebugConsole")
                        {
                            window.Visible = false;

                        }
                        
                    }
                }

                if (!_fullscreenModalWindow.Visible)
                {
                    _fullscreenModalWindow.Visible = true;
                }

                _fullscreenModalWindow.DesktopLocation = new Point(rect.Left + 10, rect.Top + 78);
                _fullscreenModalWindow.Size = new Size(viewportWidth + (int)leftSideBorder - 10, viewportHeight);

                return;
            }

            Point[] viewportLocation = new Point[4];
            
            viewportLocation[0] = new Point(rect.Left + 10 + (int)leftSideBorder, rect.Top + 78);
            viewportLocation[1] = new Point(viewportLocation[0].X + viewportWidth, rect.Top + 78);
            viewportLocation[2] = new Point(viewportLocation[0].X, (rect.Top + 78) + viewportHeight);
            viewportLocation[3] = new Point(viewportLocation[0].X + viewportWidth, viewportLocation[2].Y);

            int viewportNum = 0;
            foreach (Form window in windows)
            {
                if(window.Dock == DockStyle.Left)
                {
                    window.DesktopLocation = new Point(rect.Left + 10, rect.Top + 78);
                    window.Size = new Size(window.Size.Width, height - 137);
                    if (window.Visible == false)
                    {
                        window.Show();
                    }
                }
                else if (window.GetType().Name == "Viewport")
                {
                    if (_fullscreenWindow != null && _fullscreenWindow != window)
                    {
                        window.Hide();
                        continue;
                    }

                    if (!window.Visible)
                    {
                        window.Show();
                        window.Invalidate();
                    }

                    window.DesktopLocation = viewportLocation[viewportNum++];
                    Size s = window.Size;
                    s.Width = viewportWidth;
                    s.Height = viewportHeight;
                    window.Size = s;
                    ((Viewport)window).window.OnSize();

                }
                else if (window.GetType().Name == "PaintTool")
                {
                    window.Visible = false;
                }
            }

            
        }
    }
}
