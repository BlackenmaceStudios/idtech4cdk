namespace ToolsManaged.Frontend
{
    partial class Viewport
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
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.windowTitleItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
            this.texturedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.rendererToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showPreviewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.useLightingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.maximizeViewportToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.menuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip
            // 
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.windowTitleItem,
            this.viewToolStripMenuItem,
            this.rendererToolStripMenuItem,
            this.maximizeViewportToolStripMenuItem});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Size = new System.Drawing.Size(584, 24);
            this.menuStrip.TabIndex = 0;
            this.menuStrip.Text = "Window Name";
            this.menuStrip.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.menuStrip1_ItemClicked);
            // 
            // windowTitleItem
            // 
            this.windowTitleItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.windowTitleItem.Enabled = false;
            this.windowTitleItem.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.windowTitleItem.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.windowTitleItem.Name = "windowTitleItem";
            this.windowTitleItem.Size = new System.Drawing.Size(92, 20);
            this.windowTitleItem.Text = "Window Text";
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem2,
            this.texturedToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(62, 20);
            this.viewToolStripMenuItem.Text = "&Shading";
            this.viewToolStripMenuItem.Click += new System.EventHandler(this.viewToolStripMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(129, 22);
            this.toolStripMenuItem2.Text = "&Wireframe";
            // 
            // texturedToolStripMenuItem
            // 
            this.texturedToolStripMenuItem.Name = "texturedToolStripMenuItem";
            this.texturedToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.texturedToolStripMenuItem.Text = "&Textured";
            // 
            // rendererToolStripMenuItem
            // 
            this.rendererToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showPreviewToolStripMenuItem,
            this.useLightingToolStripMenuItem});
            this.rendererToolStripMenuItem.Name = "rendererToolStripMenuItem";
            this.rendererToolStripMenuItem.Size = new System.Drawing.Size(63, 20);
            this.rendererToolStripMenuItem.Text = "&Lighting";
            // 
            // showPreviewToolStripMenuItem
            // 
            this.showPreviewToolStripMenuItem.Name = "showPreviewToolStripMenuItem";
            this.showPreviewToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.showPreviewToolStripMenuItem.Text = "&Use no lighting";
            // 
            // useLightingToolStripMenuItem
            // 
            this.useLightingToolStripMenuItem.Name = "useLightingToolStripMenuItem";
            this.useLightingToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.useLightingToolStripMenuItem.Text = "&Use lighting";
            // 
            // maximizeViewportToolStripMenuItem
            // 
            this.maximizeViewportToolStripMenuItem.Name = "maximizeViewportToolStripMenuItem";
            this.maximizeViewportToolStripMenuItem.Size = new System.Drawing.Size(119, 20);
            this.maximizeViewportToolStripMenuItem.Text = "&Maximize Viewport";
            this.maximizeViewportToolStripMenuItem.Click += new System.EventHandler(this.maximizeViewportToolStripMenuItem_Click);
            // 
            // panel1
            // 
            this.panel1.AutoSize = true;
            this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel1.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(584, 384);
            this.panel1.TabIndex = 1;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // Viewport
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(584, 408);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.menuStrip);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MainMenuStrip = this.menuStrip;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Viewport";
            this.ShowInTaskbar = false;
            this.Text = "Viewport";
            this.Load += new System.EventHandler(this.Viewport_Load);
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem texturedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem rendererToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showPreviewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem useLightingToolStripMenuItem;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ToolStripMenuItem maximizeViewportToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem windowTitleItem;
    }
}