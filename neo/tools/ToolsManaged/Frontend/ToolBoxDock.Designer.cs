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
            this.infoTabControl = new System.Windows.Forms.TabControl();
            this.EntitiesPage = new System.Windows.Forms.TabPage();
            this.entityTreeView = new System.Windows.Forms.TreeView();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.newEntityTypeList = new System.Windows.Forms.ListView();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.OpenPaintToolsBtn = new System.Windows.Forms.Button();
            this.OpenWorldToolsBtn = new System.Windows.Forms.Button();
            this.infoTabControl.SuspendLayout();
            this.EntitiesPage.SuspendLayout();
            this.tabPage3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
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
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.OpenPaintToolsBtn);
            this.splitContainer2.Panel2.Controls.Add(this.OpenWorldToolsBtn);
            this.splitContainer2.Size = new System.Drawing.Size(172, 343);
            this.splitContainer2.SplitterDistance = 200;
            this.splitContainer2.TabIndex = 0;
            // 
            // OpenPaintToolsBtn
            // 
            this.OpenPaintToolsBtn.BackgroundImage = global::ToolsManaged.Properties.Resources.paintbrush1;
            this.OpenPaintToolsBtn.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.OpenPaintToolsBtn.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.OpenPaintToolsBtn.Location = new System.Drawing.Point(93, 3);
            this.OpenPaintToolsBtn.Name = "OpenPaintToolsBtn";
            this.OpenPaintToolsBtn.Size = new System.Drawing.Size(65, 58);
            this.OpenPaintToolsBtn.TabIndex = 2;
            this.OpenPaintToolsBtn.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.OpenPaintToolsBtn.UseVisualStyleBackColor = true;
            this.OpenPaintToolsBtn.Click += new System.EventHandler(this.OpenPaintToolsBtn_Click);
            // 
            // OpenWorldToolsBtn
            // 
            this.OpenWorldToolsBtn.BackgroundImage = global::ToolsManaged.Properties.Resources.builder;
            this.OpenWorldToolsBtn.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.OpenWorldToolsBtn.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.OpenWorldToolsBtn.Location = new System.Drawing.Point(4, 3);
            this.OpenWorldToolsBtn.Name = "OpenWorldToolsBtn";
            this.OpenWorldToolsBtn.Size = new System.Drawing.Size(64, 58);
            this.OpenWorldToolsBtn.TabIndex = 1;
            this.OpenWorldToolsBtn.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.OpenWorldToolsBtn.UseVisualStyleBackColor = true;
            this.OpenWorldToolsBtn.Click += new System.EventHandler(this.button2_Click_1);
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
        private System.Windows.Forms.ListView newEntityTypeList;
        private System.Windows.Forms.Button OpenWorldToolsBtn;
        private System.Windows.Forms.Button OpenPaintToolsBtn;
    }
}