namespace MiCoSi
{
    partial class CaptureSettingsForm
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
            this.gbImages = new System.Windows.Forms.GroupBox();
            this.cbImageSize = new System.Windows.Forms.ComboBox();
            this.lbImageSize = new System.Windows.Forms.Label();
            this.cbImageContainer = new System.Windows.Forms.ComboBox();
            this.lbImageContainer = new System.Windows.Forms.Label();
            this.gbVideos = new System.Windows.Forms.GroupBox();
            this.cbVideoFPS = new System.Windows.Forms.ComboBox();
            this.lbVideoFPS = new System.Windows.Forms.Label();
            this.cbVideoSize = new System.Windows.Forms.ComboBox();
            this.lbVideoSize = new System.Windows.Forms.Label();
            this.cbVideoCompression = new System.Windows.Forms.ComboBox();
            this.lbVideoCompression = new System.Windows.Forms.Label();
            this.lbPutFolder = new System.Windows.Forms.Label();
            this.tbOutFolder = new System.Windows.Forms.TextBox();
            this.btSelectFolder = new System.Windows.Forms.Button();
            this.btApply = new System.Windows.Forms.Button();
            this.lbNamePrefix = new System.Windows.Forms.Label();
            this.tbNamePrefix = new System.Windows.Forms.TextBox();
            this.gbImages.SuspendLayout();
            this.gbVideos.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbImages
            // 
            this.gbImages.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gbImages.Controls.Add(this.cbImageSize);
            this.gbImages.Controls.Add(this.lbImageSize);
            this.gbImages.Controls.Add(this.cbImageContainer);
            this.gbImages.Controls.Add(this.lbImageContainer);
            this.gbImages.Location = new System.Drawing.Point(12, 12);
            this.gbImages.Name = "gbImages";
            this.gbImages.Size = new System.Drawing.Size(278, 88);
            this.gbImages.TabIndex = 0;
            this.gbImages.TabStop = false;
            this.gbImages.Text = "Images";
            // 
            // cbImageSize
            // 
            this.cbImageSize.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbImageSize.FormattingEnabled = true;
            this.cbImageSize.Location = new System.Drawing.Point(114, 57);
            this.cbImageSize.Name = "cbImageSize";
            this.cbImageSize.Size = new System.Drawing.Size(142, 24);
            this.cbImageSize.TabIndex = 3;
            // 
            // lbImageSize
            // 
            this.lbImageSize.AutoSize = true;
            this.lbImageSize.Location = new System.Drawing.Point(20, 60);
            this.lbImageSize.Name = "lbImageSize";
            this.lbImageSize.Size = new System.Drawing.Size(35, 17);
            this.lbImageSize.TabIndex = 2;
            this.lbImageSize.Text = "Size";
            // 
            // cbImageContainer
            // 
            this.cbImageContainer.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbImageContainer.FormattingEnabled = true;
            this.cbImageContainer.Location = new System.Drawing.Point(114, 25);
            this.cbImageContainer.Name = "cbImageContainer";
            this.cbImageContainer.Size = new System.Drawing.Size(142, 24);
            this.cbImageContainer.TabIndex = 1;
            // 
            // lbImageContainer
            // 
            this.lbImageContainer.AutoSize = true;
            this.lbImageContainer.Location = new System.Drawing.Point(20, 28);
            this.lbImageContainer.Name = "lbImageContainer";
            this.lbImageContainer.Size = new System.Drawing.Size(69, 17);
            this.lbImageContainer.TabIndex = 0;
            this.lbImageContainer.Text = "Container";
            // 
            // gbVideos
            // 
            this.gbVideos.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gbVideos.Controls.Add(this.cbVideoFPS);
            this.gbVideos.Controls.Add(this.lbVideoFPS);
            this.gbVideos.Controls.Add(this.cbVideoSize);
            this.gbVideos.Controls.Add(this.lbVideoSize);
            this.gbVideos.Controls.Add(this.cbVideoCompression);
            this.gbVideos.Controls.Add(this.lbVideoCompression);
            this.gbVideos.Location = new System.Drawing.Point(12, 106);
            this.gbVideos.Name = "gbVideos";
            this.gbVideos.Size = new System.Drawing.Size(278, 130);
            this.gbVideos.TabIndex = 1;
            this.gbVideos.TabStop = false;
            this.gbVideos.Text = "Videos";
            // 
            // cbVideoFPS
            // 
            this.cbVideoFPS.FormattingEnabled = true;
            this.cbVideoFPS.Location = new System.Drawing.Point(114, 91);
            this.cbVideoFPS.Name = "cbVideoFPS";
            this.cbVideoFPS.Size = new System.Drawing.Size(142, 24);
            this.cbVideoFPS.TabIndex = 5;
            // 
            // lbVideoFPS
            // 
            this.lbVideoFPS.AutoSize = true;
            this.lbVideoFPS.Location = new System.Drawing.Point(23, 94);
            this.lbVideoFPS.Name = "lbVideoFPS";
            this.lbVideoFPS.Size = new System.Drawing.Size(34, 17);
            this.lbVideoFPS.TabIndex = 4;
            this.lbVideoFPS.Text = "FPS";
            // 
            // cbVideoSize
            // 
            this.cbVideoSize.FormattingEnabled = true;
            this.cbVideoSize.Location = new System.Drawing.Point(114, 58);
            this.cbVideoSize.Name = "cbVideoSize";
            this.cbVideoSize.Size = new System.Drawing.Size(142, 24);
            this.cbVideoSize.TabIndex = 3;
            // 
            // lbVideoSize
            // 
            this.lbVideoSize.AutoSize = true;
            this.lbVideoSize.Location = new System.Drawing.Point(23, 61);
            this.lbVideoSize.Name = "lbVideoSize";
            this.lbVideoSize.Size = new System.Drawing.Size(77, 17);
            this.lbVideoSize.TabIndex = 2;
            this.lbVideoSize.Text = "Frame size";
            // 
            // cbVideoCompression
            // 
            this.cbVideoCompression.FormattingEnabled = true;
            this.cbVideoCompression.Location = new System.Drawing.Point(114, 24);
            this.cbVideoCompression.Name = "cbVideoCompression";
            this.cbVideoCompression.Size = new System.Drawing.Size(142, 24);
            this.cbVideoCompression.TabIndex = 1;
            // 
            // lbVideoCompression
            // 
            this.lbVideoCompression.AutoSize = true;
            this.lbVideoCompression.Location = new System.Drawing.Point(20, 27);
            this.lbVideoCompression.Name = "lbVideoCompression";
            this.lbVideoCompression.Size = new System.Drawing.Size(90, 17);
            this.lbVideoCompression.TabIndex = 0;
            this.lbVideoCompression.Text = "Compression";
            // 
            // lbPutFolder
            // 
            this.lbPutFolder.AutoSize = true;
            this.lbPutFolder.Location = new System.Drawing.Point(13, 282);
            this.lbPutFolder.Name = "lbPutFolder";
            this.lbPutFolder.Size = new System.Drawing.Size(75, 17);
            this.lbPutFolder.TabIndex = 2;
            this.lbPutFolder.Text = "Out Folder";
            // 
            // tbOutFolder
            // 
            this.tbOutFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbOutFolder.Location = new System.Drawing.Point(105, 279);
            this.tbOutFolder.Name = "tbOutFolder";
            this.tbOutFolder.ReadOnly = true;
            this.tbOutFolder.Size = new System.Drawing.Size(151, 22);
            this.tbOutFolder.TabIndex = 3;
            // 
            // btSelectFolder
            // 
            this.btSelectFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btSelectFolder.Location = new System.Drawing.Point(262, 279);
            this.btSelectFolder.Name = "btSelectFolder";
            this.btSelectFolder.Size = new System.Drawing.Size(28, 23);
            this.btSelectFolder.TabIndex = 4;
            this.btSelectFolder.Text = "...";
            this.btSelectFolder.UseVisualStyleBackColor = true;
            this.btSelectFolder.Click += new System.EventHandler(this.btSelectFolder_Click);
            // 
            // btApply
            // 
            this.btApply.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btApply.Location = new System.Drawing.Point(112, 311);
            this.btApply.Name = "btApply";
            this.btApply.Size = new System.Drawing.Size(78, 30);
            this.btApply.TabIndex = 5;
            this.btApply.Text = "Apply";
            this.btApply.UseVisualStyleBackColor = true;
            this.btApply.Click += new System.EventHandler(this.btApply_Click);
            // 
            // lbNamePrefix
            // 
            this.lbNamePrefix.AutoSize = true;
            this.lbNamePrefix.Location = new System.Drawing.Point(13, 247);
            this.lbNamePrefix.Name = "lbNamePrefix";
            this.lbNamePrefix.Size = new System.Drawing.Size(83, 17);
            this.lbNamePrefix.TabIndex = 6;
            this.lbNamePrefix.Text = "Name prefix";
            // 
            // tbNamePrefix
            // 
            this.tbNamePrefix.Location = new System.Drawing.Point(105, 244);
            this.tbNamePrefix.Name = "tbNamePrefix";
            this.tbNamePrefix.Size = new System.Drawing.Size(151, 22);
            this.tbNamePrefix.TabIndex = 7;
            // 
            // CaptureSettingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(302, 355);
            this.Controls.Add(this.tbNamePrefix);
            this.Controls.Add(this.lbNamePrefix);
            this.Controls.Add(this.btApply);
            this.Controls.Add(this.btSelectFolder);
            this.Controls.Add(this.tbOutFolder);
            this.Controls.Add(this.lbPutFolder);
            this.Controls.Add(this.gbVideos);
            this.Controls.Add(this.gbImages);
            this.MaximumSize = new System.Drawing.Size(320, 400);
            this.MinimumSize = new System.Drawing.Size(320, 400);
            this.Name = "CaptureSettingsForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Capture Settings";
            this.gbImages.ResumeLayout(false);
            this.gbImages.PerformLayout();
            this.gbVideos.ResumeLayout(false);
            this.gbVideos.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox gbImages;
        private System.Windows.Forms.GroupBox gbVideos;
        private System.Windows.Forms.ComboBox cbImageSize;
        private System.Windows.Forms.Label lbImageSize;
        private System.Windows.Forms.ComboBox cbImageContainer;
        private System.Windows.Forms.Label lbImageContainer;
        private System.Windows.Forms.ComboBox cbVideoFPS;
        private System.Windows.Forms.Label lbVideoFPS;
        private System.Windows.Forms.ComboBox cbVideoSize;
        private System.Windows.Forms.Label lbVideoSize;
        private System.Windows.Forms.ComboBox cbVideoCompression;
        private System.Windows.Forms.Label lbVideoCompression;
        private System.Windows.Forms.Label lbPutFolder;
        private System.Windows.Forms.TextBox tbOutFolder;
        private System.Windows.Forms.Button btSelectFolder;
        private System.Windows.Forms.Button btApply;
        private System.Windows.Forms.Label lbNamePrefix;
        private System.Windows.Forms.TextBox tbNamePrefix;
    }
}