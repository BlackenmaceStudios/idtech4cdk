using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using ToolsManaged.Private;

namespace ToolsManaged.Frontend
{
    public partial class DebugConsole : Form
    {
        public DebugConsole()
        {
            InitializeComponent();
        }

        private void DebugConsole_Load(object sender, EventArgs e)
        {
            Printf("ToolsManaged Module Loaded Successfully\n\r");


            textBox1.TextChanged += new EventHandler(textBox1_TextChanged);

            textBox1.Text += "";
            textBox1.SelectAll();
            textBox1.ScrollToCaret();

        }

        void textBox1_TextChanged(object sender, EventArgs e)
        {
            

        }

        //
        // Printf
        //
        public void Printf(string s)
        {
            s = s.Replace("^3", "");
            s = s.Replace("^1", "");
            textBox1.Text += s;
            textBox1.SelectAll();

            textBox1.ScrollToCaret();
          
           
        }

        private void executeButton_Click(object sender, EventArgs e)
        {
            NativeAPI.CmdSystem.BufferCommandText(inputTxt.Text);

            inputTxt.Text = "";
        }
    }
}
