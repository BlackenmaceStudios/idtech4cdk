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
        private const int CP_NOCLOSE_BUTTON = 0x200;
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams myCp = base.CreateParams;
                myCp.ClassStyle = myCp.ClassStyle | CP_NOCLOSE_BUTTON;
                return myCp;
            }
        }


        private void DebugConsole_Load(object sender, EventArgs e)
        {
            idString.SetupColorTableForRTF(textBox1);
            Printf("ToolsManaged Module Loaded Successfully\n\r");


            textBox1.TextChanged += new EventHandler(textBox1_TextChanged);

            inputTxt.KeyDown += new KeyEventHandler(inputTxt_KeyDown);
        }

        void inputTxt_KeyDown(object sender, KeyEventArgs e)
        {
            // Enter excutes a command.
            if (e.KeyCode == Keys.Enter)
            {
                Printf(inputTxt.Text + "\n");
                NativeAPI.CmdSystem.BufferCommandText(inputTxt.Text);
                inputTxt.Text = "";

                e.Handled = true;
            }

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

        private void inputTxt_TextChanged(object sender, EventArgs e)
        {

        }
/*
 * NativeAPI.CmdSystem.BufferCommandText(inputTxt.Text);

            inputTxt.Text = "";
*/
    }
}
