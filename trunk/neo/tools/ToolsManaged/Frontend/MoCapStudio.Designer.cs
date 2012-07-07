namespace ToolsManaged.Frontend
{
    partial class MoCapStudio
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MoCapStudio));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SkeletalModelListBox = new System.Windows.Forms.ListBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.kinectControlGroupBox = new System.Windows.Forms.GroupBox();
            this.trackTxt = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.depthPicBox = new System.Windows.Forms.PictureBox();
            this.SetTitleAngleBtn = new System.Windows.Forms.Button();
            this.elevationBar = new System.Windows.Forms.HScrollBar();
            this.colorCameraData = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.elevationValueTxt = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newAnimationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAnimationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.modelJointList = new System.Windows.Forms.ListBox();
            this.SaveRemapBtn = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.kinectJointRemapList = new System.Windows.Forms.ListBox();
            this.label5 = new System.Windows.Forms.Label();
            this.boneSearchTxt = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.kinectControlGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.depthPicBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.colorCameraData)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SkeletalModelListBox);
            this.groupBox1.Location = new System.Drawing.Point(0, 27);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(194, 294);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "SkeletalModels";
            // 
            // SkeletalModelListBox
            // 
            this.SkeletalModelListBox.FormattingEnabled = true;
            this.SkeletalModelListBox.Location = new System.Drawing.Point(13, 19);
            this.SkeletalModelListBox.Name = "SkeletalModelListBox";
            this.SkeletalModelListBox.Size = new System.Drawing.Size(166, 264);
            this.SkeletalModelListBox.TabIndex = 0;
            this.SkeletalModelListBox.SelectedIndexChanged += new System.EventHandler(this.SkeletalModelListBox_SelectedIndexChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.listBox1);
            this.groupBox2.Location = new System.Drawing.Point(2, 329);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(191, 173);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Animations";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 26);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(101, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Selected Animation:";
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new System.Drawing.Point(11, 42);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(166, 108);
            this.listBox1.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Location = new System.Drawing.Point(203, 822);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(752, 140);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "MoCap Controls";
            this.groupBox3.Enter += new System.EventHandler(this.groupBox3_Enter);
            // 
            // kinectControlGroupBox
            // 
            this.kinectControlGroupBox.Controls.Add(this.trackTxt);
            this.kinectControlGroupBox.Controls.Add(this.label3);
            this.kinectControlGroupBox.Controls.Add(this.depthPicBox);
            this.kinectControlGroupBox.Controls.Add(this.SetTitleAngleBtn);
            this.kinectControlGroupBox.Controls.Add(this.elevationBar);
            this.kinectControlGroupBox.Controls.Add(this.colorCameraData);
            this.kinectControlGroupBox.Controls.Add(this.label1);
            this.kinectControlGroupBox.Controls.Add(this.elevationValueTxt);
            this.kinectControlGroupBox.Location = new System.Drawing.Point(2, 518);
            this.kinectControlGroupBox.Name = "kinectControlGroupBox";
            this.kinectControlGroupBox.Size = new System.Drawing.Size(191, 450);
            this.kinectControlGroupBox.TabIndex = 3;
            this.kinectControlGroupBox.TabStop = false;
            this.kinectControlGroupBox.Text = "Device";
            this.kinectControlGroupBox.Enter += new System.EventHandler(this.kinectControlGroupBox_Enter);
            // 
            // trackTxt
            // 
            this.trackTxt.AutoSize = true;
            this.trackTxt.BackColor = System.Drawing.Color.Red;
            this.trackTxt.Location = new System.Drawing.Point(10, 93);
            this.trackTxt.Name = "trackTxt";
            this.trackTxt.Size = new System.Drawing.Size(69, 13);
            this.trackTxt.TabIndex = 5;
            this.trackTxt.Text = "Not Tracking";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 280);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(75, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Depth Camera";
            // 
            // depthPicBox
            // 
            this.depthPicBox.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.depthPicBox.Location = new System.Drawing.Point(11, 296);
            this.depthPicBox.Name = "depthPicBox";
            this.depthPicBox.Size = new System.Drawing.Size(174, 148);
            this.depthPicBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.depthPicBox.TabIndex = 3;
            this.depthPicBox.TabStop = false;
            // 
            // SetTitleAngleBtn
            // 
            this.SetTitleAngleBtn.Location = new System.Drawing.Point(13, 49);
            this.SetTitleAngleBtn.Name = "SetTitleAngleBtn";
            this.SetTitleAngleBtn.Size = new System.Drawing.Size(105, 35);
            this.SetTitleAngleBtn.TabIndex = 2;
            this.SetTitleAngleBtn.Text = "Set Tilt Angle";
            this.SetTitleAngleBtn.UseVisualStyleBackColor = true;
            this.SetTitleAngleBtn.Click += new System.EventHandler(this.SetTitleAngleBtn_Click);
            // 
            // elevationBar
            // 
            this.elevationBar.Location = new System.Drawing.Point(11, 29);
            this.elevationBar.Maximum = 27;
            this.elevationBar.Minimum = -27;
            this.elevationBar.Name = "elevationBar";
            this.elevationBar.Size = new System.Drawing.Size(162, 17);
            this.elevationBar.TabIndex = 1;
            this.elevationBar.Scroll += new System.Windows.Forms.ScrollEventHandler(this.elevationBar_Scroll);
            // 
            // colorCameraData
            // 
            this.colorCameraData.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.colorCameraData.Location = new System.Drawing.Point(11, 128);
            this.colorCameraData.Name = "colorCameraData";
            this.colorCameraData.Size = new System.Drawing.Size(174, 148);
            this.colorCameraData.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.colorCameraData.TabIndex = 0;
            this.colorCameraData.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 112);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(70, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Color Camera";
            // 
            // elevationValueTxt
            // 
            this.elevationValueTxt.AutoSize = true;
            this.elevationValueTxt.Location = new System.Drawing.Point(10, 16);
            this.elevationValueTxt.Name = "elevationValueTxt";
            this.elevationValueTxt.Size = new System.Drawing.Size(51, 13);
            this.elevationValueTxt.TabIndex = 0;
            this.elevationValueTxt.Text = "Elevation";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1494, 24);
            this.menuStrip1.TabIndex = 4;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.menuStrip1_ItemClicked);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newAnimationToolStripMenuItem,
            this.saveAnimationToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // newAnimationToolStripMenuItem
            // 
            this.newAnimationToolStripMenuItem.Name = "newAnimationToolStripMenuItem";
            this.newAnimationToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.newAnimationToolStripMenuItem.Text = "&New Animation";
            // 
            // saveAnimationToolStripMenuItem
            // 
            this.saveAnimationToolStripMenuItem.Name = "saveAnimationToolStripMenuItem";
            this.saveAnimationToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.saveAnimationToolStripMenuItem.Text = "&Save Animation";
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ButtonShadow;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Location = new System.Drawing.Point(203, 35);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(1279, 776);
            this.panel1.TabIndex = 5;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.boneSearchTxt);
            this.groupBox4.Controls.Add(this.label5);
            this.groupBox4.Controls.Add(this.modelJointList);
            this.groupBox4.Controls.Add(this.SaveRemapBtn);
            this.groupBox4.Controls.Add(this.label4);
            this.groupBox4.Controls.Add(this.kinectJointRemapList);
            this.groupBox4.Location = new System.Drawing.Point(961, 822);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(521, 140);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Joint Remap";
            this.groupBox4.Enter += new System.EventHandler(this.groupBox4_Enter);
            // 
            // modelJointList
            // 
            this.modelJointList.FormattingEnabled = true;
            this.modelJointList.Location = new System.Drawing.Point(288, 30);
            this.modelJointList.Name = "modelJointList";
            this.modelJointList.SelectionMode = System.Windows.Forms.SelectionMode.MultiSimple;
            this.modelJointList.Size = new System.Drawing.Size(128, 95);
            this.modelJointList.TabIndex = 4;
            this.modelJointList.SelectedIndexChanged += new System.EventHandler(this.modelJointList_SelectedIndexChanged_1);
            // 
            // SaveRemapBtn
            // 
            this.SaveRemapBtn.Location = new System.Drawing.Point(422, 16);
            this.SaveRemapBtn.Name = "SaveRemapBtn";
            this.SaveRemapBtn.Size = new System.Drawing.Size(93, 36);
            this.SaveRemapBtn.TabIndex = 3;
            this.SaveRemapBtn.Text = "Save Remap";
            this.SaveRemapBtn.UseVisualStyleBackColor = true;
            this.SaveRemapBtn.Click += new System.EventHandler(this.SaveRemapBtn_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(284, 16);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "Mapped Joint";
            // 
            // kinectJointRemapList
            // 
            this.kinectJointRemapList.Enabled = false;
            this.kinectJointRemapList.FormattingEnabled = true;
            this.kinectJointRemapList.Location = new System.Drawing.Point(7, 16);
            this.kinectJointRemapList.Name = "kinectJointRemapList";
            this.kinectJointRemapList.Size = new System.Drawing.Size(265, 108);
            this.kinectJointRemapList.TabIndex = 0;
            this.kinectJointRemapList.SelectedIndexChanged += new System.EventHandler(this.kinectJointRemapList_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(422, 55);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(55, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "Find Bone";
            // 
            // boneSearchTxt
            // 
            this.boneSearchTxt.Location = new System.Drawing.Point(422, 71);
            this.boneSearchTxt.Name = "boneSearchTxt";
            this.boneSearchTxt.Size = new System.Drawing.Size(100, 20);
            this.boneSearchTxt.TabIndex = 6;
            this.boneSearchTxt.TextChanged += new System.EventHandler(this.boneSearchTxt_TextChanged);
            // 
            // MoCapStudio
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1494, 980);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.kinectControlGroupBox);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MoCapStudio";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Motion Capture Studio";
            this.Load += new System.EventHandler(this.MoCapStudio_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.kinectControlGroupBox.ResumeLayout(false);
            this.kinectControlGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.depthPicBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.colorCameraData)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ListBox SkeletalModelListBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox kinectControlGroupBox;
        private System.Windows.Forms.HScrollBar elevationBar;
        private System.Windows.Forms.Label elevationValueTxt;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newAnimationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAnimationToolStripMenuItem;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button SetTitleAngleBtn;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox colorCameraData;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.PictureBox depthPicBox;
        private System.Windows.Forms.ListBox kinectJointRemapList;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label trackTxt;
        private System.Windows.Forms.Button SaveRemapBtn;
        private System.Windows.Forms.ListBox modelJointList;
        private System.Windows.Forms.TextBox boneSearchTxt;
        private System.Windows.Forms.Label label5;
    }
}