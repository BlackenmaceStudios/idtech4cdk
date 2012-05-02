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
    public partial class ModelStudio : Form
    {
        public IEditorWindow window;
        public ModelStudio()
        {
            InitializeComponent();
        }

        public IntPtr GetPanelHandle()
        {
            return panel1.Handle;
        }

        private void ModelStudio_Load(object sender, EventArgs e)
        {
            FormClosing += new FormClosingEventHandler(ModelStudio_FormClosing);

            // Force a refresh.
            panel1.Focus();
            Invalidate();
        }

        void ModelStudio_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            Hide();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // This feels wrong
            if (timer1.Enabled == false)
                return;

            window.Redraw();
            base.OnPaint(e);

            AllowDrop = true;
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }


   

        private void sToolStripMenuItem_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            ((EditorWindow)window).OnNamedEvent("ImportStatic");
            timer1.Enabled = true;
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (Visible)
            {
                Invalidate();
            }
        }
    }
}
