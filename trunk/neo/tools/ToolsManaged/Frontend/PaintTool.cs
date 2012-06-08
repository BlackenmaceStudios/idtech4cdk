using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using ToolsManaged.Frontend.WindowAPI;

namespace ToolsManaged.Frontend
{
    public partial class PaintTool : Form
    {
        public PaintTool()
        {
            InitializeComponent();

            this.Resize += new EventHandler(PaintTool_Resize);
        }

        void PaintTool_Resize(object sender, EventArgs e)
        {
            tableLayoutPanel1.Size = this.Size;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void exitPaintToolToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FormManager.handle.SetFullsreenModalWindow(null);
        }
    }
}
