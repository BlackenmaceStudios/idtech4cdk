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
    public partial class ProgressDialog : Form
    {
        public ProgressDialog()
        {
            InitializeComponent();
        }

        private void ProgressDialog_Load(object sender, EventArgs e)
        {

        }

        public void SetUpdateMsg(string txtMsg)
        {
            statusUpdateTxt.Text = txtMsg;
            Invalidate();
        }

        public void SetProgress(int percent)
        {
            this.Text = "" + percent + "%";
            progressBar1.Value = percent;
            label2.Text = "" + percent + "%";
            Invalidate();
        }
    }
}
