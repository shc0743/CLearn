using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DownloadToolUi_NET
{
    public partial class Form_DownloadMdi : Form
    {
        public Form_DownloadMdi()
        {
            InitializeComponent();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void downloadTaskToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_NewDownloadTask form_NewDownloadTask = new Form_NewDownloadTask();
            form_NewDownloadTask.MdiParent = this;
            form_NewDownloadTask.Show();
        }

        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_Options form_Options = new Form_Options();
            form_Options.MdiParent = this;
            form_Options.Show();
        }

        private void simpleWebBrowserToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_SimpleWebBrowser form_SimpleWebBrowser = new Form_SimpleWebBrowser();
            form_SimpleWebBrowser.MdiParent = this;
            form_SimpleWebBrowser.Show();
        }
    }
}
