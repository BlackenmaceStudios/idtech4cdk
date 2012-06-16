namespace ToolsManaged.Frontend
{
    partial class PaintTool
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
            System.Windows.Forms.ToolStripMenuItem virtualTexturePaintToolToolStripMenuItem;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PaintTool));
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.brushList = new System.Windows.Forms.ListBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.brushSizeTxt = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.brushImage = new System.Windows.Forms.PictureBox();
            this.label2 = new System.Windows.Forms.Label();
            this.brushTrackBar = new System.Windows.Forms.TrackBar();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.StampImg = new System.Windows.Forms.PictureBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.mtrListBox = new System.Windows.Forms.ListBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.LayersBox = new System.Windows.Forms.ListBox();
            this.button2 = new System.Windows.Forms.Button();
            this.AddNewLayerBtn = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolOptionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveMegaProjectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitPaintToolToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stencilsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.brushesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.renderChartToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.updateStencilsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.updateBrushesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            virtualTexturePaintToolToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tableLayoutPanel1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.brushImage)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.brushTrackBar)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.StampImg)).BeginInit();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // virtualTexturePaintToolToolStripMenuItem
            // 
            virtualTexturePaintToolToolStripMenuItem.Enabled = false;
            virtualTexturePaintToolToolStripMenuItem.Font = new System.Drawing.Font("Bernard MT Condensed", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            virtualTexturePaintToolToolStripMenuItem.Name = "virtualTexturePaintToolToolStripMenuItem";
            virtualTexturePaintToolToolStripMenuItem.Size = new System.Drawing.Size(165, 20);
            virtualTexturePaintToolToolStripMenuItem.Text = "Virtual Texture Paint Tool";
            virtualTexturePaintToolToolStripMenuItem.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.AutoSize = true;
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 13.17664F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 86.82336F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 213F));
            this.tableLayoutPanel1.Controls.Add(this.panel1, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.groupBox1, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.groupBox3, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.groupBox5, 2, 1);
            this.tableLayoutPanel1.Controls.Add(this.groupBox4, 1, 1);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 27);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 76.32552F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23.67448F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(1427, 815);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // panel1
            // 
            this.panel1.AutoSize = true;
            this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel1.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(162, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(1048, 616);
            this.panel1.TabIndex = 3;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.brushList);
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Location = new System.Drawing.Point(1216, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(197, 598);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Brushes";
            // 
            // brushList
            // 
            this.brushList.FormattingEnabled = true;
            this.brushList.Location = new System.Drawing.Point(11, 19);
            this.brushList.Name = "brushList";
            this.brushList.Size = new System.Drawing.Size(171, 199);
            this.brushList.TabIndex = 4;
            this.brushList.SelectedIndexChanged += new System.EventHandler(this.brushList_SelectedIndexChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.brushSizeTxt);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.brushImage);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.brushTrackBar);
            this.groupBox2.Location = new System.Drawing.Point(11, 236);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(180, 356);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "PaintBrush Attributes";
            // 
            // brushSizeTxt
            // 
            this.brushSizeTxt.Location = new System.Drawing.Point(128, 207);
            this.brushSizeTxt.Name = "brushSizeTxt";
            this.brushSizeTxt.Size = new System.Drawing.Size(46, 20);
            this.brushSizeTxt.TabIndex = 7;
            this.brushSizeTxt.TextChanged += new System.EventHandler(this.brushSizeTxt_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 28);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Current Brush";
            // 
            // brushImage
            // 
            this.brushImage.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.brushImage.Location = new System.Drawing.Point(9, 44);
            this.brushImage.Name = "brushImage";
            this.brushImage.Size = new System.Drawing.Size(138, 135);
            this.brushImage.TabIndex = 5;
            this.brushImage.TabStop = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 193);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(57, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Brush Size";
            // 
            // brushTrackBar
            // 
            this.brushTrackBar.Location = new System.Drawing.Point(3, 209);
            this.brushTrackBar.Maximum = 3000;
            this.brushTrackBar.Minimum = 1;
            this.brushTrackBar.Name = "brushTrackBar";
            this.brushTrackBar.Size = new System.Drawing.Size(129, 45);
            this.brushTrackBar.TabIndex = 2;
            this.brushTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.brushTrackBar.Value = 500;
            this.brushTrackBar.Scroll += new System.EventHandler(this.brushTrackBar_Scroll);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.StampImg);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Controls.Add(this.mtrListBox);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox3.Location = new System.Drawing.Point(3, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(153, 616);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Stencils";
            // 
            // StampImg
            // 
            this.StampImg.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.StampImg.Location = new System.Drawing.Point(9, 418);
            this.StampImg.Name = "StampImg";
            this.StampImg.Size = new System.Drawing.Size(138, 173);
            this.StampImg.TabIndex = 4;
            this.StampImg.TabStop = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 397);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Size:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 377);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(76, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Current Stencil";
            // 
            // mtrListBox
            // 
            this.mtrListBox.FormattingEnabled = true;
            this.mtrListBox.Location = new System.Drawing.Point(6, 16);
            this.mtrListBox.Name = "mtrListBox";
            this.mtrListBox.Size = new System.Drawing.Size(142, 342);
            this.mtrListBox.TabIndex = 0;
            this.mtrListBox.SelectedIndexChanged += new System.EventHandler(this.mtrListBox_SelectedIndexChanged);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.textBox1);
            this.groupBox5.Location = new System.Drawing.Point(1216, 625);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(205, 170);
            this.groupBox5.TabIndex = 7;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Controls";
            // 
            // textBox1
            // 
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Location = new System.Drawing.Point(8, 20);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(189, 140);
            this.textBox1.TabIndex = 0;
            this.textBox1.Text = resources.GetString("textBox1.Text");
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.LayersBox);
            this.groupBox4.Controls.Add(this.button2);
            this.groupBox4.Controls.Add(this.AddNewLayerBtn);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox4.Location = new System.Drawing.Point(162, 625);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(1048, 187);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Layers";
            // 
            // LayersBox
            // 
            this.LayersBox.FormattingEnabled = true;
            this.LayersBox.Location = new System.Drawing.Point(17, 17);
            this.LayersBox.Name = "LayersBox";
            this.LayersBox.Size = new System.Drawing.Size(184, 108);
            this.LayersBox.TabIndex = 0;
            this.LayersBox.SelectedIndexChanged += new System.EventHandler(this.LayersBox_SelectedIndexChanged);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(117, 129);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(84, 34);
            this.button2.TabIndex = 2;
            this.button2.Text = "Delete Layer";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // AddNewLayerBtn
            // 
            this.AddNewLayerBtn.Location = new System.Drawing.Point(17, 130);
            this.AddNewLayerBtn.Name = "AddNewLayerBtn";
            this.AddNewLayerBtn.Size = new System.Drawing.Size(94, 34);
            this.AddNewLayerBtn.TabIndex = 1;
            this.AddNewLayerBtn.Text = "Add Layer";
            this.AddNewLayerBtn.UseVisualStyleBackColor = true;
            this.AddNewLayerBtn.Click += new System.EventHandler(this.button1_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            virtualTexturePaintToolToolStripMenuItem,
            this.toolOptionsToolStripMenuItem,
            this.stencilsToolStripMenuItem,
            this.brushesToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1425, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.menuStrip1_ItemClicked);
            // 
            // toolOptionsToolStripMenuItem
            // 
            this.toolOptionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.updateStencilsToolStripMenuItem,
            this.updateBrushesToolStripMenuItem,
            this.toolStripSeparator1,
            this.saveMegaProjectToolStripMenuItem,
            this.exitPaintToolToolStripMenuItem});
            this.toolOptionsToolStripMenuItem.Name = "toolOptionsToolStripMenuItem";
            this.toolOptionsToolStripMenuItem.Size = new System.Drawing.Size(88, 20);
            this.toolOptionsToolStripMenuItem.Text = "&Tool Options";
            // 
            // saveMegaProjectToolStripMenuItem
            // 
            this.saveMegaProjectToolStripMenuItem.Name = "saveMegaProjectToolStripMenuItem";
            this.saveMegaProjectToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.saveMegaProjectToolStripMenuItem.Text = "&Save MegaProject";
            this.saveMegaProjectToolStripMenuItem.Click += new System.EventHandler(this.saveMegaProjectToolStripMenuItem_Click);
            // 
            // exitPaintToolToolStripMenuItem
            // 
            this.exitPaintToolToolStripMenuItem.Name = "exitPaintToolToolStripMenuItem";
            this.exitPaintToolToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.exitPaintToolToolStripMenuItem.Text = "Exit Paint Tool";
            this.exitPaintToolToolStripMenuItem.Click += new System.EventHandler(this.exitPaintToolToolStripMenuItem_Click);
            // 
            // stencilsToolStripMenuItem
            // 
            this.stencilsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.renderChartToolStripMenuItem});
            this.stencilsToolStripMenuItem.Name = "stencilsToolStripMenuItem";
            this.stencilsToolStripMenuItem.Size = new System.Drawing.Size(54, 20);
            this.stencilsToolStripMenuItem.Text = "&Debug";
            // 
            // brushesToolStripMenuItem
            // 
            this.brushesToolStripMenuItem.Name = "brushesToolStripMenuItem";
            this.brushesToolStripMenuItem.Size = new System.Drawing.Size(60, 20);
            this.brushesToolStripMenuItem.Text = "Brushes";
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // renderChartToolStripMenuItem
            // 
            this.renderChartToolStripMenuItem.Name = "renderChartToolStripMenuItem";
            this.renderChartToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.renderChartToolStripMenuItem.Text = "Render &Chart";
            this.renderChartToolStripMenuItem.Click += new System.EventHandler(this.renderChartToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(165, 6);
            // 
            // updateStencilsToolStripMenuItem
            // 
            this.updateStencilsToolStripMenuItem.Name = "updateStencilsToolStripMenuItem";
            this.updateStencilsToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.updateStencilsToolStripMenuItem.Text = "&Update Stencils";
            this.updateStencilsToolStripMenuItem.Click += new System.EventHandler(this.updateStencilsToolStripMenuItem_Click);
            // 
            // updateBrushesToolStripMenuItem
            // 
            this.updateBrushesToolStripMenuItem.Name = "updateBrushesToolStripMenuItem";
            this.updateBrushesToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.updateBrushesToolStripMenuItem.Text = "&Update Brushes";
            this.updateBrushesToolStripMenuItem.Click += new System.EventHandler(this.updateBrushesToolStripMenuItem_Click);
            // 
            // PaintTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1425, 812);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "PaintTool";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "PaintTool";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.brushImage)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.brushTrackBar)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.StampImg)).EndInit();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ListBox LayersBox;
        private System.Windows.Forms.Button AddNewLayerBtn;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListBox mtrListBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolOptionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem stencilsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem brushesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitPaintToolToolStripMenuItem;
        private System.Windows.Forms.PictureBox StampImg;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.ToolStripMenuItem saveMegaProjectToolStripMenuItem;
        private System.Windows.Forms.TrackBar brushTrackBar;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ListBox brushList;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.PictureBox brushImage;
        private System.Windows.Forms.TextBox brushSizeTxt;
        private System.Windows.Forms.ToolStripMenuItem renderChartToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem updateStencilsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem updateBrushesToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    }
}