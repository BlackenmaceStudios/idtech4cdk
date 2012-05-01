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
            

            int viewportHeight = (int)Math.Abs(rect.Left - rect.Right) - 137;

            if (_fullscreenWindow == null)
            {
                viewportHeight /= 4;
                viewportWidth /= 2;
            }
            else if(((Viewport)_fullscreenWindow).is3DView)
            {
                viewportHeight = 720;
                viewportWidth = 1280;
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
                    window.Size = new Size(viewportWidth, viewportHeight);
                    ((Viewport)window).window.OnSize();

                }
            }

            
        }
    }
}
