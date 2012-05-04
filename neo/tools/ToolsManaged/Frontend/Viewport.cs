using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using ToolsManaged.Private.idLib;
using ToolsManaged.Frontend.WindowAPI;
namespace ToolsManaged.Frontend
{
    public partial class Viewport : Form
    {
        private string ViewportTitle;
        public IEditorWindow window;
        public Viewport()
        {
            InitializeComponent();
        }

        public bool is3DView;
        private ToolsManaged.Private.NativeAPI.idManagedEditorWindowNative.Point _lastPoint;
        private ContextMenu _entityContextMenu;
        private idLibNativeAPI.idDictNative _currentSelectedEntityDict;
        private const int CP_NOCLOSE_BUTTON = 0x200;
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams myCp = base.CreateParams;
                myCp.ClassStyle = myCp.ClassStyle | CP_NOCLOSE_BUTTON;
                return myCp;
            }
        }

        private void viewToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void AddItemToContextMenu(ContextMenu menu, string name, EventHandler clickHandler)
        {
            MenuItem mnuItem = new MenuItem();

            mnuItem.Text = name;
            mnuItem.Click += clickHandler;
            menu.MenuItems.Add(mnuItem);
        }

        private void CreateEntityContextMenu()
        {
            _entityContextMenu = new System.Windows.Forms.ContextMenu();
            AddItemToContextMenu(_entityContextMenu, "Translate", null);
            AddItemToContextMenu(_entityContextMenu, "Rotate", null);
            AddItemToContextMenu(_entityContextMenu, "Scale", null);
            AddItemToContextMenu(_entityContextMenu, "Properties For...", new EventHandler(EntityContextMenu_OnClickProperties));
        }

        private void EntityContextMenu_OnClickProperties(object sender, EventArgs e)
        {
            ToolsManagedPrivate.handle.ShowEntityPropterties(_currentSelectedEntityDict);
        }

        private void Viewport_Load(object sender, EventArgs e)
        {
            panel1.MouseMove += new MouseEventHandler(panel1_MouseMove);
            panel1.MouseClick += new MouseEventHandler(panel1_MouseClick);
            panel1.MouseUp += new MouseEventHandler(panel1_MouseUp);
            panel1.MouseDown += new MouseEventHandler(panel1_MouseDown);
            panel1.MouseWheel += new MouseEventHandler(panel1_MouseWheel);

            panel1.AllowDrop = true;
            panel1.DragDrop += new DragEventHandler(panel1_DragDrop);
            panel1.DragEnter += new DragEventHandler(panel1_DragEnter);
            panel1.DragLeave += new EventHandler(panel1_DragLeave);

            this.AllowDrop = true;

            CreateEntityContextMenu();
     
            Invalidate();
        }

        void panel1_MouseWheel(object sender, MouseEventArgs e)
        {
            ToolsManaged.Private.NativeAPI.idManagedEditorWindowNative.Point point = new Private.NativeAPI.idManagedEditorWindowNative.Point();
            Point screenCoords = Cursor.Position;
            Point controlRelatedCoords = this.panel1.PointToClient(screenCoords);


            point.x = controlRelatedCoords.X;
            point.y = controlRelatedCoords.Y;

            ((EditorWindow)window).OnMouseScroll((short)e.Delta, point);
        }

        void panel1_DragLeave(object sender, EventArgs e)
        {
            SetWindowTitle(null);
        }

        void panel1_DragEnter(object sender, DragEventArgs e)
        {
            string[] EntityType = (string[])e.Data.GetData(e.Data.GetFormats()[0]);
            SetWindowTitle("Place " + EntityType[0]);

            e.Effect = DragDropEffects.Move;
        }

        void panel1_DragDrop(object sender, DragEventArgs e)
        {
            string[] EntityType = (string[])e.Data.GetData(e.Data.GetFormats()[0]);
            SetWindowTitle(null);

            ToolsManaged.Private.NativeAPI.idManagedEditorWindowNative.Point point = new Private.NativeAPI.idManagedEditorWindowNative.Point();

            Point screenCoords = Cursor.Position;
            Point controlRelatedCoords = this.panel1.PointToClient(screenCoords);


            point.x = controlRelatedCoords.X;
            point.y = controlRelatedCoords.Y;

            ((EditorWindow)window).OnAddEntityEvent(EntityType[0], point);
           
        }

        static int ignoreDown = 0;
        void panel1_MouseDown(object sender, MouseEventArgs e)
        {
            int downTime = System.Environment.TickCount;
            if (downTime - ignoreDown < 2)
            {
               
                return;
            }

            ignoreDown = downTime;
            
            ToolsManaged.Private.NativeAPI.idManagedEditorWindowNative.Point point = new Private.NativeAPI.idManagedEditorWindowNative.Point();

            point.x = e.Location.X;
            point.y = e.Location.Y;

            _lastPoint = point;
            

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                ((EditorWindow)window).OnMouseDownLeft(0, point);
            }
            else if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                ((EditorWindow)window).OnMouseDownRight(0, point);
            }
        }

        void panel1_MouseUp(object sender, MouseEventArgs e)
        {
            ToolsManaged.Private.NativeAPI.idManagedEditorWindowNative.Point point = new Private.NativeAPI.idManagedEditorWindowNative.Point();

            point.x = e.Location.X;
            point.y = e.Location.Y;

            

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
              ((EditorWindow)window).OnMouseUpLeft(0, point);
            }
            else if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                ((EditorWindow)window).OnMouseUpRight(0, point);
            }
        }

        void panel1_MouseClick(object sender, MouseEventArgs e)
        {
           
        }

        void panel1_MouseMove(object sender, MouseEventArgs e)
        {
            ToolsManaged.Private.NativeAPI.idManagedEditorWindowNative.Point point = new Private.NativeAPI.idManagedEditorWindowNative.Point();

            point.x = e.Location.X;
            point.y = e.Location.Y;

            ((EditorWindow)window).OnMouseMove(e.Button, point);
           
            base.OnMouseMove(e);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            window.Redraw();
            base.OnPaint(e);

            AllowDrop = true;
        }

 

        protected override void OnKeyDown(KeyEventArgs e)
        {
            ((EditorWindow)window).OnKeyDown(e.KeyCode);
            base.OnKeyDown(e);
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            ((EditorWindow)window).OnKeyUp(e.KeyCode);
            base.OnKeyDown(e);
        }

        protected override void OnResize(EventArgs e)
        {
           
            base.OnResize(e);
        }

    

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            
        }

        
        public void OpenEntityContextMenu(IntPtr dict)
        {
            if (dict == IntPtr.Zero)
            {
                ToolsManagedPrivate.handle.HideEntityProperties();
                this.ContextMenu = null;
                return;
            }

            _currentSelectedEntityDict = null;
            _currentSelectedEntityDict = new idLibNativeAPI.idDictNative(dict);

            _entityContextMenu.MenuItems[_entityContextMenu.MenuItems.Count - 1].Text = "Properties for " + _currentSelectedEntityDict.GetString("name");
            this.ContextMenu = _entityContextMenu;
            this.ContextMenu.Show(panel1, new Point(_lastPoint.x, _lastPoint.y));
        }

        public void SetWindowTitle(string txt)
        {
            if (!is3DView)
            {
                if (txt == "Persp")
                    is3DView = true;
            }
            if (ViewportTitle == null)
            {
                ViewportTitle = txt;
            }

            if (txt == null)
            {
                windowTitleItem.Text = ViewportTitle;
                return;
            }

            windowTitleItem.Text = txt;
        }

        public IntPtr GetPanelHandle()
        {
            return panel1.Handle;
        }

        private void maximizeViewportToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (maximizeViewportToolStripMenuItem.Text == "&Maximize Viewport")
            {
                maximizeViewportToolStripMenuItem.Text = "&Minimize Viewport";
                FormManager.handle.SetFullsreenWindow(this);
                window.Redraw();
            }
            else
            {
                FormManager.handle.SetFullsreenWindow(null);
                maximizeViewportToolStripMenuItem.Text = "&Maximize Viewport";
                window.Redraw();
            }
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }
    }
}
