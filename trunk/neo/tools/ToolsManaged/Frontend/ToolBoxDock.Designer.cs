namespace ToolsManaged.Frontend
{
    partial class ToolBoxDock
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("WorldSpawn");
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ToolBoxDock));
            this.infoTabControl = new System.Windows.Forms.TabControl();
            this.EntitiesPage = new System.Windows.Forms.TabPage();
            this.entityTreeView = new System.Windows.Forms.TreeView();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.newEntityTypeList = new System.Windows.Forms.ListView();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.infoTabControl.SuspendLayout();
            this.EntitiesPage.SuspendLayout();
            this.tabPage3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // infoTabControl
            // 
            this.infoTabControl.Controls.Add(this.EntitiesPage);
            this.infoTabControl.Controls.Add(this.tabPage3);
            this.infoTabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.infoTabControl.Location = new System.Drawing.Point(0, 0);
            this.infoTabControl.Multiline = true;
            this.infoTabControl.Name = "infoTabControl";
            this.infoTabControl.SelectedIndex = 0;
            this.infoTabControl.Size = new System.Drawing.Size(168, 326);
            this.infoTabControl.TabIndex = 0;
            // 
            // EntitiesPage
            // 
            this.EntitiesPage.Controls.Add(this.entityTreeView);
            this.EntitiesPage.Location = new System.Drawing.Point(4, 22);
            this.EntitiesPage.Name = "EntitiesPage";
            this.EntitiesPage.Size = new System.Drawing.Size(160, 300);
            this.EntitiesPage.TabIndex = 0;
            this.EntitiesPage.Text = "WorldEntities";
            this.EntitiesPage.UseVisualStyleBackColor = true;
            // 
            // entityTreeView
            // 
            this.entityTreeView.BackColor = System.Drawing.Color.Gray;
            this.entityTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.entityTreeView.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.entityTreeView.ForeColor = System.Drawing.SystemColors.Window;
            this.entityTreeView.Location = new System.Drawing.Point(0, 0);
            this.entityTreeView.Name = "entityTreeView";
            treeNode1.Name = "Node0";
            treeNode1.Text = "WorldSpawn";
            this.entityTreeView.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode1});
            this.entityTreeView.Size = new System.Drawing.Size(160, 300);
            this.entityTreeView.TabIndex = 0;
            this.entityTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.entityTreeView_AfterSelect);
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.newEntityTypeList);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(160, 300);
            this.tabPage3.TabIndex = 1;
            this.tabPage3.Text = "Add New Entity";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // newEntityTypeList
            // 
            this.newEntityTypeList.Activation = System.Windows.Forms.ItemActivation.OneClick;
            this.newEntityTypeList.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.newEntityTypeList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.newEntityTypeList.Location = new System.Drawing.Point(0, 0);
            this.newEntityTypeList.MultiSelect = false;
            this.newEntityTypeList.Name = "newEntityTypeList";
            this.newEntityTypeList.Size = new System.Drawing.Size(160, 300);
            this.newEntityTypeList.TabIndex = 0;
            this.newEntityTypeList.UseCompatibleStateImageBehavior = false;
            this.newEntityTypeList.View = System.Windows.Forms.View.List;
            this.newEntityTypeList.SelectedIndexChanged += new System.EventHandler(this.newEntityTypeList_SelectedIndexChanged);
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            this.splitContainer1.Panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.splitContainer1_Panel1_Paint);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.infoTabControl);
            this.splitContainer1.Size = new System.Drawing.Size(172, 677);
            this.splitContainer1.SplitterDistance = 343;
            this.splitContainer1.TabIndex = 1;
            // 
            // splitContainer2
            // 
            this.splitContainer2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.button3);
            this.splitContainer2.Panel2.Controls.Add(this.button2);
            this.splitContainer2.Panel2.Controls.Add(this.button1);
            this.splitContainer2.Size = new System.Drawing.Size(172, 343);
            this.splitContainer2.SplitterDistance = 283;
            this.splitContainer2.TabIndex = 0;
            // 
            // button3
            // 
            this.button3.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.button3.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("button3.BackgroundImage")));
            this.button3.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.button3.Location = new System.Drawing.Point(110, 3);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(48, 45);
            this.button3.TabIndex = 2;
            this.button3.UseVisualStyleBackColor = false;
            // 
            // button2
            // 
            this.button2.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.button2.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("button2.BackgroundImage")));
            this.button2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.button2.Location = new System.Drawing.Point(58, 3);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(48, 45);
            this.button2.TabIndex = 1;
            this.button2.UseVisualStyleBackColor = false;
            // 
            // button1
            // 
            this.button1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.button1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("button1.BackgroundImage")));
            this.button1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.button1.Location = new System.Drawing.Point(4, 3);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(48, 45);
            this.button1.TabIndex = 0;
            this.button1.UseVisualStyleBackColor = false;
            // 
            // groupBox1
            // 
            this.groupBox1.Location = new System.Drawing.Point(6, 7);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(157, 266);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Map Statistics";
            // 
            // ToolBoxDock
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoValidate = System.Windows.Forms.AutoValidate.EnableAllowFocusChange;
            this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.ClientSize = new System.Drawing.Size(172, 677);
            this.Controls.Add(this.splitContainer1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "ToolBoxDock";
            this.ShowInTaskbar = false;
            this.Text = "ToolBoxDock";
            this.Load += new System.EventHandler(this.ToolBoxDock_Load);
            this.infoTabControl.ResumeLayout(false);
            this.EntitiesPage.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl infoTabControl;
        private System.Windows.Forms.TabPage EntitiesPage;
        private System.Windows.Forms.TreeView entityTreeView;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ListView newEntityTypeList;
        private System.Windows.Forms.GroupBox groupBox1;
    }
}