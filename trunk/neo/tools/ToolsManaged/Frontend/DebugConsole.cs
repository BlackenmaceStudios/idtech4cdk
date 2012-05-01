using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using ToolsManaged.Private;
using System.Runtime.InteropServices;
using ToolsManaged.Private.idLib;
namespace ToolsManaged.Frontend
{
    public partial class DebugConsole : Form
    {
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(
          IntPtr hWnd,
          uint Msg,
          IntPtr wParam,
          IntPtr lParam);

        private const int WM_VSCROLL = 277;
        private const int SB_LINEUP = 0;
        private const int SB_LINEDOWN = 1;
        private const int SB_TOP = 6;
        private const int SB_BOTTOM = 7;

        public DebugConsole()
        {
            InitializeComponent();
        }

        private void DebugConsole_Load(object sender, EventArgs e)
        {
            idString.SetupColorTableForRTF(textBox1);
            Printf("ToolsManaged Module Loaded Successfully\n\r");


            textBox1.TextChanged += new EventHandler(textBox1_TextChanged);

            
        }

        void textBox1_TextChanged(object sender, EventArgs e)
        {
            textBox1.SuspendLayout();

            SendMessage(textBox1.Handle, WM_VSCROLL, new IntPtr(SB_BOTTOM), new IntPtr(0));

            textBox1.ResumeLayout();  

        }



        //
        // Printf
        //
        public void Printf(string s)
        {
            string s2 = s.Replace("\r", " ");
            s2 = s2.Replace("\n", " ");
            s2 = s2.Replace("\r", " ");
            idString.RTFPrintColorString(textBox1, s2 + "\n");
        }

        private void executeButton_Click(object sender, EventArgs e)
        {
            NativeAPI.CmdSystem.BufferCommandText(inputTxt.Text);

            inputTxt.Text = "";
           
        }

        private void textBox1_TextChanged_1(object sender, EventArgs e)
        {
           
        }
    }
}
