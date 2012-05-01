using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// This file uses Syntax highlighting by Parsing the RTF file format by Alun Evans(2006).
namespace ToolsManaged.Private.idLib
{
    static class idString
    {
        // color escape character
        const char C_COLOR_ESCAPE = '^';
        const char C_COLOR_DEFAULT = '0';
        const char C_COLOR_RED = '1';
        const char C_COLOR_GREEN = '2';
        const char C_COLOR_YELLOW = '3';
        const char C_COLOR_BLUE = '4';
        const char C_COLOR_CYAN = '5';
        const char C_COLOR_MAGENTA = '6';
        const char C_COLOR_WHITE = '7';
        const char C_COLOR_GRAY = '8';
        const char C_COLOR_BLACK = '9';

        public static System.Drawing.Color GetColor(char s)
        {
            switch (s)
            {
                case C_COLOR_DEFAULT:
                    return System.Drawing.Color.Aqua;
                case C_COLOR_RED:
                    return System.Drawing.Color.Red;
                case C_COLOR_GREEN:
                    return System.Drawing.Color.Green;
                case C_COLOR_CYAN:
                    return System.Drawing.Color.Cyan;
                case C_COLOR_YELLOW:
                    return System.Drawing.Color.Yellow;

                    
            }

            return System.Drawing.Color.White;
        }

        public static string GetColorFromTable(char s)
        {
            switch (s)
            {
                case C_COLOR_DEFAULT:
                    return "\\cf0";
                case C_COLOR_RED:
                    return "\\cf1";
                case C_COLOR_GREEN:
                    return "\\cf3";
                case C_COLOR_CYAN:
                    return "\\cf3";
                case C_COLOR_YELLOW:
                    return "\\cf4";

                    
            }

            return "\\cf2";
        }
        
        public static void SetupColorTableForRTF(System.Windows.Forms.RichTextBox richTextBox1, string str = " ")
        {
            string s = "";
            for (char c = '0'; c < '9'; c++)
            {
                s += BuildRTFFromColor(GetColor(c));
            }

            //Use the RichTextBox to create the initial RTF code
            richTextBox1.Clear();
            richTextBox1.AppendText("<");
            string strRTF = richTextBox1.Rtf;
            richTextBox1.Clear();

            /* 
             * ADD COLOUR TABLE TO THE HEADER FIRST 
             * */

            // Search for colour table info, if it exists (which it shouldn't)
            // remove it and replace with our one
            int iCTableStart = strRTF.IndexOf("colortbl ;");

            if (iCTableStart != -1) //then colortbl exists
            {
                //find end of colortbl tab by searching
                //forward from the colortbl tab itself
                int iCTableEnd = strRTF.IndexOf('}', iCTableStart);
                strRTF = strRTF.Remove(iCTableStart, iCTableEnd - iCTableStart);

                

                //now insert new colour table at index of old colortbl tag
                strRTF = strRTF.Insert(iCTableStart,
                    // CHANGE THIS STRING TO ALTER COLOUR TABLE
                    "colortbl ;" + s + "}");
            }

            //colour table doesn't exist yet, so let's make one
            else
            {
                // find index of start of header
                int iRTFLoc = strRTF.IndexOf("\\rtf");
                // get index of where we'll insert the colour table
                // try finding opening bracket of first property of header first                
                int iInsertLoc = strRTF.IndexOf('{', iRTFLoc);

                // if there is no property, we'll insert colour table
                // just before the end bracket of the header
                if (iInsertLoc == -1) iInsertLoc = strRTF.IndexOf('}', iRTFLoc) - 1;

                // insert the colour table at our chosen location                
                strRTF = strRTF.Insert(iInsertLoc,
                    // CHANGE THIS STRING TO ALTER COLOUR TABLE
                    "{\\colortbl ;" + s + "}\r\n");
            }


            richTextBox1.Rtf = strRTF.Replace("<", "\\cf0 -- \\cf1 --  \\cf2 -- \\cf3 -- \\cf4 -- \\cf5 -- \\cf6 -- \\cf7 -- \\cf8 -- \\cf9 -- \r "); ;
        }

        public static string BuildRTFFromColor(System.Drawing.Color c)
        {
            return "\\red" + (int)c.R + "\\green" + + (int)c.G + "\\blue" + (int)c.B + ";";
        }

        public static void RTFPrintColorString(System.Windows.Forms.RichTextBox richTextBox1, string msg)
        {
           

           //Use the RichTextBox to create the initial RTF code
             richTextBox1.AppendText(msg);
           string strRTF = richTextBox1.Rtf;
           richTextBox1.Clear();


           for (char c = '0'; c < '9'; c++)
           {
               strRTF = strRTF.Replace("^" + c, GetColorFromTable(c) + " ");
           }

           richTextBox1.Rtf = strRTF;
        }

    }
}
