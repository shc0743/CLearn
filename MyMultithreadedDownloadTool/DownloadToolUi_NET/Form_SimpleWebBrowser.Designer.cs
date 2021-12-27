namespace DownloadToolUi_NET
{
    partial class Form_SimpleWebBrowser
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.webBrowser_main = new System.Windows.Forms.WebBrowser();
            this.textBox_url = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // webBrowser_main
            // 
            this.webBrowser_main.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.webBrowser_main.Location = new System.Drawing.Point(0, 24);
            this.webBrowser_main.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser_main.Name = "webBrowser_main";
            this.webBrowser_main.Size = new System.Drawing.Size(484, 337);
            this.webBrowser_main.TabIndex = 0;
            this.webBrowser_main.LocationChanged += new System.EventHandler(this.OnWebBrowserLocationChanged);
            // 
            // textBox_url
            // 
            this.textBox_url.Dock = System.Windows.Forms.DockStyle.Top;
            this.textBox_url.Location = new System.Drawing.Point(0, 0);
            this.textBox_url.Name = "textBox_url";
            this.textBox_url.Size = new System.Drawing.Size(484, 22);
            this.textBox_url.TabIndex = 1;
            this.textBox_url.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.OnUrlBoxKeyPress);
            // 
            // Form_SimpleWebBrowser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(484, 361);
            this.Controls.Add(this.textBox_url);
            this.Controls.Add(this.webBrowser_main);
            this.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form_SimpleWebBrowser";
            this.Text = "Simple Web Browser";
            this.Resize += new System.EventHandler(this.OnResize);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.WebBrowser webBrowser_main;
        private System.Windows.Forms.TextBox textBox_url;
    }
}