using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ToolsManaged.Frontend
{
    public partial class ToolBoxDock : Form
    {
        public ToolBoxDock()
        {
            InitializeComponent();
        }

        private void ToolBoxDock_Load(object s, EventArgs evemt)
        {

            ListView view = (ListView)infoTabControl.TabPages[1].Controls[0];

            view.ItemDrag += delegate(object sender, ItemDragEventArgs e)
            {
                List<string> itemIDs = new List<string>((sender as ListView).SelectedItems.Count);
                foreach (ListViewItem item in (sender as ListView).SelectedItems)
                {
                    itemIDs.Add(item.Text);
                }
                (sender as ListView).DoDragDrop(itemIDs.ToArray(), DragDropEffects.Move);
            };

            entityTreeView.MouseClick += new MouseEventHandler(entityTreeView_MouseClick);
            entityTreeView.MouseDoubleClick += new MouseEventHandler(entityTreeView_MouseDoubleClick);
            AddNewEntityType("", "light");
        }

        void entityTreeView_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            string[] s = entityTreeView.SelectedNode.Text.Split('(');

            if (s[0] == "Worldspawn")
                return;

            Private.idLib.idLibNativeAPI.idDictNative dict = Private.NativeAPI.Entity_GetEntityDict(s[0]);

            ToolsManagedPrivate.handle.ShowEntityPropterties(dict);
        }

        void entityTreeView_MouseClick(object sender, MouseEventArgs e)
        {
            
        }

        void view_ItemDrag(object sender, ItemDragEventArgs e)
        {
            throw new NotImplementedException();
        }

       
       

        public void ClearEntityList()
        {
            TreeView view = (TreeView)infoTabControl.TabPages[0].Controls[0];

            view.Nodes.Clear();

        }

        public void AddEntity(string entityType, string entityName)
        {
            TreeView view = (TreeView)infoTabControl.TabPages[0].Controls[0];

            if (view.Nodes.Count <= 0)
            {
                view.Nodes.Add(entityType);
                return;
            }

            if (entityType.Contains("worldspawn"))
                return;

            view.Nodes[0].Nodes.Add(entityName + "( " + entityType  + " ) ");
            view.Nodes[0].Expand();
        }

        public void AddNewEntityType(string folder, string entityType)
        {
            ListView view = (ListView)infoTabControl.TabPages[1].Controls[0];
        
            ListViewItem newNode;

            for (int i = 0; i < view.Items.Count; i++)
            {
                if (view.Items[i].Text == folder)
                {
                   // view.Items[i].SubItems.Add(entityType);
                    
                   // return;
                }
            }

            ListViewItem groupNode;

            if (folder.Length <= 0)
            {
                groupNode = view.Items.Add(entityType);
            }
            else
            {
                groupNode = view.Items.Add(folder + "_" + entityType);
            }

            Font font = new System.Drawing.Font(newEntityTypeList.Font, FontStyle.Bold);
            newEntityTypeList.ForeColor = Color.White;
            groupNode.Font = font;

           // view.Items[view.Items.Count - 1].SubItems.Add(entityType);
        }

        private void panel2_Paint(object sender, PaintEventArgs e)
        {

        }

        private void splitContainer1_Panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void newEntityTypeList_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }

        private void newEntityTypeList_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void entityTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }
    }
}
