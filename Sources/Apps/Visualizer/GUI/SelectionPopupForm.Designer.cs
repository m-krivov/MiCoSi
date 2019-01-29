namespace Mitosis
{
    partial class SelectionPopupForm
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
            this.gbProperties = new System.Windows.Forms.GroupBox();
            this.pnProperties = new System.Windows.Forms.Panel();
            this.cbPin = new System.Windows.Forms.CheckBox();
            this.gbProperties.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbProperties
            // 
            this.gbProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gbProperties.Controls.Add(this.pnProperties);
            this.gbProperties.Location = new System.Drawing.Point(12, 39);
            this.gbProperties.Name = "gbProperties";
            this.gbProperties.Size = new System.Drawing.Size(114, 62);
            this.gbProperties.TabIndex = 0;
            this.gbProperties.TabStop = false;
            this.gbProperties.Text = "Properties";
            // 
            // pnProperties
            // 
            this.pnProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pnProperties.AutoScroll = true;
            this.pnProperties.Location = new System.Drawing.Point(6, 21);
            this.pnProperties.Name = "pnProperties";
            this.pnProperties.Size = new System.Drawing.Size(90, 35);
            this.pnProperties.TabIndex = 0;
            // 
            // cbPin
            // 
            this.cbPin.AutoSize = true;
            this.cbPin.Location = new System.Drawing.Point(12, 12);
            this.cbPin.Name = "cbPin";
            this.cbPin.Size = new System.Drawing.Size(50, 21);
            this.cbPin.TabIndex = 1;
            this.cbPin.Text = "Pin";
            this.cbPin.UseVisualStyleBackColor = true;
            this.cbPin.CheckedChanged += new System.EventHandler(this.cbPin_CheckedChanged);
            // 
            // SelectionPopupForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(138, 113);
            this.Controls.Add(this.cbPin);
            this.Controls.Add(this.gbProperties);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "SelectionPopupForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Selected item";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SelectionPopupForm_FormClosing);
            this.gbProperties.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox gbProperties;
        private System.Windows.Forms.CheckBox cbPin;
        private System.Windows.Forms.Panel pnProperties;
    }
}