namespace Mitosis
{
    partial class AboutForm
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
            this.lbCaption = new System.Windows.Forms.Label();
            this.lbProgramVersion = new System.Windows.Forms.Label();
            this.lbFileFormatVersion = new System.Windows.Forms.Label();
            this.btOK = new System.Windows.Forms.Button();
            this.lbProgramVersionValue = new System.Windows.Forms.Label();
            this.lbFileFormatVersionValue = new System.Windows.Forms.Label();
            this.lbPublishDateValue = new System.Windows.Forms.Label();
            this.lbPublishDate = new System.Windows.Forms.Label();
            this.lbDescription1 = new System.Windows.Forms.Label();
            this.lbDescription2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lbCaption
            // 
            this.lbCaption.AutoSize = true;
            this.lbCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbCaption.Location = new System.Drawing.Point(55, 12);
            this.lbCaption.Name = "lbCaption";
            this.lbCaption.Size = new System.Drawing.Size(163, 25);
            this.lbCaption.TabIndex = 0;
            this.lbCaption.Text = "Mitosis Visualizer";
            // 
            // lbProgramVersion
            // 
            this.lbProgramVersion.AutoSize = true;
            this.lbProgramVersion.Location = new System.Drawing.Point(12, 104);
            this.lbProgramVersion.Name = "lbProgramVersion";
            this.lbProgramVersion.Size = new System.Drawing.Size(116, 17);
            this.lbProgramVersion.TabIndex = 1;
            this.lbProgramVersion.Text = "Program version:";
            // 
            // lbFileFormatVersion
            // 
            this.lbFileFormatVersion.AutoSize = true;
            this.lbFileFormatVersion.Location = new System.Drawing.Point(12, 121);
            this.lbFileFormatVersion.Name = "lbFileFormatVersion";
            this.lbFileFormatVersion.Size = new System.Drawing.Size(128, 17);
            this.lbFileFormatVersion.TabIndex = 2;
            this.lbFileFormatVersion.Text = "File format version:";
            // 
            // btOK
            // 
            this.btOK.Location = new System.Drawing.Point(99, 180);
            this.btOK.Name = "btOK";
            this.btOK.Size = new System.Drawing.Size(75, 23);
            this.btOK.TabIndex = 3;
            this.btOK.Text = "OK";
            this.btOK.UseVisualStyleBackColor = true;
            this.btOK.Click += new System.EventHandler(this.btOK_Click);
            // 
            // lbProgramVersionValue
            // 
            this.lbProgramVersionValue.AutoSize = true;
            this.lbProgramVersionValue.Location = new System.Drawing.Point(146, 104);
            this.lbProgramVersionValue.Name = "lbProgramVersionValue";
            this.lbProgramVersionValue.Size = new System.Drawing.Size(36, 17);
            this.lbProgramVersionValue.TabIndex = 4;
            this.lbProgramVersionValue.Text = "NaN";
            // 
            // lbFileFormatVersionValue
            // 
            this.lbFileFormatVersionValue.AutoSize = true;
            this.lbFileFormatVersionValue.Location = new System.Drawing.Point(146, 121);
            this.lbFileFormatVersionValue.Name = "lbFileFormatVersionValue";
            this.lbFileFormatVersionValue.Size = new System.Drawing.Size(36, 17);
            this.lbFileFormatVersionValue.TabIndex = 5;
            this.lbFileFormatVersionValue.Text = "NaN";
            // 
            // lbPublishDateValue
            // 
            this.lbPublishDateValue.AutoSize = true;
            this.lbPublishDateValue.Location = new System.Drawing.Point(146, 138);
            this.lbPublishDateValue.Name = "lbPublishDateValue";
            this.lbPublishDateValue.Size = new System.Drawing.Size(36, 17);
            this.lbPublishDateValue.TabIndex = 7;
            this.lbPublishDateValue.Text = "NaN";
            // 
            // lbPublishDate
            // 
            this.lbPublishDate.AutoSize = true;
            this.lbPublishDate.Location = new System.Drawing.Point(12, 138);
            this.lbPublishDate.Name = "lbPublishDate";
            this.lbPublishDate.Size = new System.Drawing.Size(90, 17);
            this.lbPublishDate.TabIndex = 6;
            this.lbPublishDate.Text = "Publish date:";
            // 
            // lbDescription1
            // 
            this.lbDescription1.AutoSize = true;
            this.lbDescription1.Location = new System.Drawing.Point(10, 49);
            this.lbDescription1.Name = "lbDescription1";
            this.lbDescription1.Size = new System.Drawing.Size(250, 17);
            this.lbDescription1.TabIndex = 8;
            this.lbDescription1.Text = "Small utility that shows mitosis process";
            // 
            // lbDescription2
            // 
            this.lbDescription2.AutoSize = true;
            this.lbDescription2.Location = new System.Drawing.Point(10, 66);
            this.lbDescription2.Name = "lbDescription2";
            this.lbDescription2.Size = new System.Drawing.Size(237, 17);
            this.lbDescription2.TabIndex = 9;
            this.lbDescription2.Text = "and creates screenshots and videos";
            // 
            // AboutForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(274, 222);
            this.Controls.Add(this.lbDescription2);
            this.Controls.Add(this.lbDescription1);
            this.Controls.Add(this.lbPublishDateValue);
            this.Controls.Add(this.lbPublishDate);
            this.Controls.Add(this.lbFileFormatVersionValue);
            this.Controls.Add(this.lbProgramVersionValue);
            this.Controls.Add(this.btOK);
            this.Controls.Add(this.lbFileFormatVersion);
            this.Controls.Add(this.lbProgramVersion);
            this.Controls.Add(this.lbCaption);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximumSize = new System.Drawing.Size(280, 250);
            this.MinimumSize = new System.Drawing.Size(280, 250);
            this.Name = "AboutForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "About";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbCaption;
        private System.Windows.Forms.Label lbProgramVersion;
        private System.Windows.Forms.Label lbFileFormatVersion;
        private System.Windows.Forms.Button btOK;
        private System.Windows.Forms.Label lbProgramVersionValue;
        private System.Windows.Forms.Label lbFileFormatVersionValue;
        private System.Windows.Forms.Label lbPublishDateValue;
        private System.Windows.Forms.Label lbPublishDate;
        private System.Windows.Forms.Label lbDescription1;
        private System.Windows.Forms.Label lbDescription2;
    }
}