namespace DownloadToolUi_NET
{
    partial class Form_Options
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_Options));
            System.Windows.Forms.TabPage tabPage_generic;
            System.Windows.Forms.TabPage tabPage_proxy;
            System.Windows.Forms.TabPage tabPage_advanced;
            this.button_apply = new System.Windows.Forms.Button();
            this.button_cancel = new System.Windows.Forms.Button();
            this.button_ok = new System.Windows.Forms.Button();
            this.tabControl_main = new System.Windows.Forms.TabControl();
            tabPage_generic = new System.Windows.Forms.TabPage();
            tabPage_proxy = new System.Windows.Forms.TabPage();
            tabPage_advanced = new System.Windows.Forms.TabPage();
            this.tabControl_main.SuspendLayout();
            this.SuspendLayout();
            // 
            // button_apply
            // 
            this.button_apply.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_apply, "button_apply");
            this.button_apply.Name = "button_apply";
            this.button_apply.UseVisualStyleBackColor = true;
            // 
            // button_cancel
            // 
            this.button_cancel.Cursor = System.Windows.Forms.Cursors.Hand;
            this.button_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            resources.ApplyResources(this.button_cancel, "button_cancel");
            this.button_cancel.Name = "button_cancel";
            this.button_cancel.UseVisualStyleBackColor = true;
            this.button_cancel.Click += new System.EventHandler(this.button_cancel_Click);
            // 
            // button_ok
            // 
            this.button_ok.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_ok, "button_ok");
            this.button_ok.Name = "button_ok";
            this.button_ok.UseVisualStyleBackColor = true;
            this.button_ok.Click += new System.EventHandler(this.button_ok_Click);
            // 
            // tabControl_main
            // 
            resources.ApplyResources(this.tabControl_main, "tabControl_main");
            this.tabControl_main.Controls.Add(tabPage_generic);
            this.tabControl_main.Controls.Add(tabPage_proxy);
            this.tabControl_main.Controls.Add(tabPage_advanced);
            this.tabControl_main.Name = "tabControl_main";
            this.tabControl_main.SelectedIndex = 0;
            // 
            // tabPage_generic
            // 
            resources.ApplyResources(tabPage_generic, "tabPage_generic");
            tabPage_generic.Name = "tabPage_generic";
            tabPage_generic.UseVisualStyleBackColor = true;
            // 
            // tabPage_proxy
            // 
            resources.ApplyResources(tabPage_proxy, "tabPage_proxy");
            tabPage_proxy.Name = "tabPage_proxy";
            tabPage_proxy.UseVisualStyleBackColor = true;
            // 
            // tabPage_advanced
            // 
            resources.ApplyResources(tabPage_advanced, "tabPage_advanced");
            tabPage_advanced.Name = "tabPage_advanced";
            tabPage_advanced.UseVisualStyleBackColor = true;
            // 
            // Form_Options
            // 
            this.AcceptButton = this.button_ok;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button_cancel;
            this.Controls.Add(this.tabControl_main);
            this.Controls.Add(this.button_ok);
            this.Controls.Add(this.button_cancel);
            this.Controls.Add(this.button_apply);
            this.Name = "Form_Options";
            this.tabControl_main.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button button_apply;
        private System.Windows.Forms.Button button_cancel;
        private System.Windows.Forms.Button button_ok;
        private System.Windows.Forms.TabControl tabControl_main;
    }
}