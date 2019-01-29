namespace Mitosis
{
    partial class VisualSettingsForm
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
            this.lbStyle = new System.Windows.Forms.Label();
            this.cbStyle = new System.Windows.Forms.ComboBox();
            this.btApply = new System.Windows.Forms.Button();
            this.cbAntiAliasing = new System.Windows.Forms.CheckBox();
            this.cbTextures = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // lbStyle
            // 
            this.lbStyle.AutoSize = true;
            this.lbStyle.Location = new System.Drawing.Point(12, 28);
            this.lbStyle.Name = "lbStyle";
            this.lbStyle.Size = new System.Drawing.Size(39, 17);
            this.lbStyle.TabIndex = 0;
            this.lbStyle.Text = "Style";
            // 
            // cbStyle
            // 
            this.cbStyle.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbStyle.FormattingEnabled = true;
            this.cbStyle.Location = new System.Drawing.Point(57, 25);
            this.cbStyle.Name = "cbStyle";
            this.cbStyle.Size = new System.Drawing.Size(188, 24);
            this.cbStyle.TabIndex = 1;
            // 
            // btApply
            // 
            this.btApply.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btApply.Location = new System.Drawing.Point(87, 129);
            this.btApply.Name = "btApply";
            this.btApply.Size = new System.Drawing.Size(75, 30);
            this.btApply.TabIndex = 2;
            this.btApply.Text = "Apply";
            this.btApply.UseVisualStyleBackColor = true;
            this.btApply.Click += new System.EventHandler(this.btApply_Click);
            // 
            // cbAntiAliasing
            // 
            this.cbAntiAliasing.AutoSize = true;
            this.cbAntiAliasing.Enabled = false;
            this.cbAntiAliasing.Location = new System.Drawing.Point(15, 73);
            this.cbAntiAliasing.Name = "cbAntiAliasing";
            this.cbAntiAliasing.Size = new System.Drawing.Size(103, 21);
            this.cbAntiAliasing.TabIndex = 3;
            this.cbAntiAliasing.Text = "AntiAliasing";
            this.cbAntiAliasing.UseVisualStyleBackColor = true;
            // 
            // cbTextures
            // 
            this.cbTextures.AutoSize = true;
            this.cbTextures.Enabled = false;
            this.cbTextures.Location = new System.Drawing.Point(15, 101);
            this.cbTextures.Name = "cbTextures";
            this.cbTextures.Size = new System.Drawing.Size(85, 21);
            this.cbTextures.TabIndex = 4;
            this.cbTextures.Text = "Textures";
            this.cbTextures.UseVisualStyleBackColor = true;
            // 
            // VisualSettingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(257, 170);
            this.Controls.Add(this.cbTextures);
            this.Controls.Add(this.cbAntiAliasing);
            this.Controls.Add(this.btApply);
            this.Controls.Add(this.cbStyle);
            this.Controls.Add(this.lbStyle);
            this.MinimumSize = new System.Drawing.Size(275, 215);
            this.Name = "VisualSettingsForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Visual Settings";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbStyle;
        private System.Windows.Forms.ComboBox cbStyle;
        private System.Windows.Forms.Button btApply;
        private System.Windows.Forms.CheckBox cbAntiAliasing;
        private System.Windows.Forms.CheckBox cbTextures;
    }
}