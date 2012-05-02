namespace ToolsManaged.Frontend
{
    partial class DebugConsole
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
            this.inputTxt = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // inputTxt
            // 
            this.inputTxt.Location = new System.Drawing.Point(7, 349);
            this.inputTxt.Name = "inputTxt";
            this.inputTxt.Size = new System.Drawing.Size(624, 20);
            this.inputTxt.TabIndex = 1;
            this.inputTxt.TextChanged += new System.EventHandler(this.inputTxt_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.label1.Location = new System.Drawing.Point(7, 331);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(334, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Type a command here and than press ENTER to execute.";
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.textBox1.Font = new System.Drawing.Font("Rockwell", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.ForeColor = System.Drawing.SystemColors.MenuText;
            this.textBox1.Location = new System.Drawing.Point(7, 9);
            this.textBox1.MaxLength = 483647;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(624, 320);
            this.textBox1.TabIndex = 4;
            this.textBox1.Text = "";
            // 
            // DebugConsole
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.ClientSize = new System.Drawing.Size(638, 385);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.inputTxt);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "DebugConsole";
            this.Text = "Development Debug Console";
            this.Load += new System.EventHandler(this.DebugConsole_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox inputTxt;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RichTextBox textBox1;
    }
}