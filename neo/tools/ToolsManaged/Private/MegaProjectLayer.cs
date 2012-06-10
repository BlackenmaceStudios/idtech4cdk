using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using ToolsManaged.Frontend;

namespace ToolsManaged.Private
{
    class MegaProjectLayer
    {
        private List<MegaProjectChart> _charts = new List<MegaProjectChart>();

        //
        // MegaProjectLayer
        //
        public MegaProjectLayer(int numCharts, int chartDimen)
        {
            ProgressDialog progress = new ProgressDialog();

            progress.Show();
            
            for (int i = 0; i < numCharts; i++)
            {
                progress.SetUpdateMsg("Building chart " + i);
                _charts.Add(new MegaProjectChart(chartDimen));

                float percent = ((float)i / (float)numCharts) * 100f;
                progress.SetProgress((int)percent);
            }

            progress.Hide();
            progress.Dispose();
        }

        //
        // MegaProjectLayer(Read from HD)
        //
        public MegaProjectLayer(NativeAPI.File _megaFile)
        {
            ProgressDialog progress = new ProgressDialog();

            int numCharts = 0;

            _megaFile.ReadInt(ref numCharts);

            progress.Show();

            for (int i = 0; i < numCharts; i++)
            {
                progress.SetUpdateMsg("Loading chart " + i);
                _charts.Add(new MegaProjectChart(_megaFile));

                float percent = ((float)i / (float)numCharts) * 100f;
                progress.SetProgress((int)percent);
            }

            progress.Hide();
            progress.Dispose();
        }

        public void Write(NativeAPI.File _megaFile)
        {
            _megaFile.WriteInt(_charts.Count);
            for (int i = 0; i < _charts.Count; i++)
            {
                _charts[i].Write(_megaFile);
            }
        }
    }
}
