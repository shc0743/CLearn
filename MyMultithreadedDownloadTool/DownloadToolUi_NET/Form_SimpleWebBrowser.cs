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
    public partial class Form_SimpleWebBrowser : Form
    {
        public Form_SimpleWebBrowser()
        {
            InitializeComponent();
        }

        private void OnUrlBoxKeyPress(object sender, KeyPressEventArgs e)
        {
            if(e.KeyChar == '\n' || e.KeyChar == '\r')
            {
                webBrowser_main.Navigate(textBox_url.Text);
            }
        }

        private void OnWebBrowserLocationChanged(object sender, EventArgs e)
        {
            if (textBox_url.Focused) return;
            try {
                textBox_url.Text = webBrowser_main.Url.ToString();
            } catch { };
        }

        private void OnResize(object sender, EventArgs e)
        {
            webBrowser_main.Size = new Size(this.Width - 16, this.Height - 63);
        }
    }
}
