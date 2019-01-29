namespace Mitosis
{
    partial class MainForm
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
            this.components = new System.ComponentModel.Container();
            this.tbTime = new System.Windows.Forms.TrackBar();
            this.btStartStop = new System.Windows.Forms.Button();
            this.lbTime = new System.Windows.Forms.Label();
            this.lbSpeed = new System.Windows.Forms.Label();
            this.pRenderArea = new System.Windows.Forms.Panel();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiClose = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiSplitter1 = new System.Windows.Forms.ToolStripSeparator();
            this.tsmiExit = new System.Windows.Forms.ToolStripMenuItem();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiVisual = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiCapture = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.tsmiHelp = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiHowDoI = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmiAbout = new System.Windows.Forms.ToolStripMenuItem();
            this.nudSpeed = new System.Windows.Forms.NumericUpDown();
            this.btScreenshot = new System.Windows.Forms.Button();
            this.btVideoCapturing = new System.Windows.Forms.Button();
            this.btInfo = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.tbTime)).BeginInit();
            this.menuStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudSpeed)).BeginInit();
            this.SuspendLayout();
            // 
            // tbTime
            // 
            this.tbTime.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbTime.Location = new System.Drawing.Point(12, 292);
            this.tbTime.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tbTime.Maximum = 500;
            this.tbTime.Name = "tbTime";
            this.tbTime.Size = new System.Drawing.Size(459, 56);
            this.tbTime.TabIndex = 0;
            this.tbTime.TickStyle = System.Windows.Forms.TickStyle.None;
            // 
            // btStartStop
            // 
            this.btStartStop.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btStartStop.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.btStartStop.Location = new System.Drawing.Point(192, 322);
            this.btStartStop.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btStartStop.Name = "btStartStop";
            this.btStartStop.Size = new System.Drawing.Size(101, 44);
            this.btStartStop.TabIndex = 1;
            this.btStartStop.Text = "Start";
            this.btStartStop.UseVisualStyleBackColor = true;
            // 
            // lbTime
            // 
            this.lbTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.lbTime.AutoSize = true;
            this.lbTime.CausesValidation = false;
            this.lbTime.Font = new System.Drawing.Font("Courier New", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbTime.Location = new System.Drawing.Point(355, 318);
            this.lbTime.Name = "lbTime";
            this.lbTime.Size = new System.Drawing.Size(112, 17);
            this.lbTime.TabIndex = 2;
            this.lbTime.Text = "00:00 / 00:00";
            // 
            // lbSpeed
            // 
            this.lbSpeed.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.lbSpeed.AutoSize = true;
            this.lbSpeed.Location = new System.Drawing.Point(351, 345);
            this.lbSpeed.Name = "lbSpeed";
            this.lbSpeed.Size = new System.Drawing.Size(49, 17);
            this.lbSpeed.TabIndex = 5;
            this.lbSpeed.Text = "Speed";
            // 
            // pRenderArea
            // 
            this.pRenderArea.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pRenderArea.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.pRenderArea.Location = new System.Drawing.Point(12, 62);
            this.pRenderArea.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.pRenderArea.Name = "pRenderArea";
            this.pRenderArea.Size = new System.Drawing.Size(459, 224);
            this.pRenderArea.TabIndex = 7;
            this.pRenderArea.Resize += new System.EventHandler(this.pRenderArea_Resize);
            // 
            // timer
            // 
            this.timer.Interval = 40;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsmiOpen,
            this.tsmiClose,
            this.tsmiSplitter1,
            this.tsmiExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // tsmiOpen
            // 
            this.tsmiOpen.Name = "tsmiOpen";
            this.tsmiOpen.Size = new System.Drawing.Size(114, 24);
            this.tsmiOpen.Text = "Open";
            this.tsmiOpen.Click += new System.EventHandler(this.tsmiOpen_Click);
            // 
            // tsmiClose
            // 
            this.tsmiClose.Name = "tsmiClose";
            this.tsmiClose.Size = new System.Drawing.Size(114, 24);
            this.tsmiClose.Text = "Close";
            this.tsmiClose.Click += new System.EventHandler(this.tsmiClose_Click);
            // 
            // tsmiSplitter1
            // 
            this.tsmiSplitter1.Name = "tsmiSplitter1";
            this.tsmiSplitter1.Size = new System.Drawing.Size(111, 6);
            // 
            // tsmiExit
            // 
            this.tsmiExit.Name = "tsmiExit";
            this.tsmiExit.Size = new System.Drawing.Size(114, 24);
            this.tsmiExit.Text = "Exit";
            this.tsmiExit.Click += new System.EventHandler(this.tsmiExit_Click);
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsmiVisual,
            this.tsmiCapture});
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(74, 24);
            this.settingsToolStripMenuItem.Text = "Settings";
            // 
            // tsmiVisual
            // 
            this.tsmiVisual.Name = "tsmiVisual";
            this.tsmiVisual.Size = new System.Drawing.Size(130, 24);
            this.tsmiVisual.Text = "Visual";
            this.tsmiVisual.Click += new System.EventHandler(this.tsmiVisual_Click);
            // 
            // tsmiCapture
            // 
            this.tsmiCapture.Name = "tsmiCapture";
            this.tsmiCapture.Size = new System.Drawing.Size(130, 24);
            this.tsmiCapture.Text = "Capture";
            this.tsmiCapture.Click += new System.EventHandler(this.tsmiCapture_Click);
            // 
            // menuStrip
            // 
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.settingsToolStripMenuItem,
            this.tsmiHelp});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Padding = new System.Windows.Forms.Padding(5, 2, 0, 2);
            this.menuStrip.Size = new System.Drawing.Size(484, 28);
            this.menuStrip.TabIndex = 3;
            this.menuStrip.Text = "MenuStrip";
            // 
            // tsmiHelp
            // 
            this.tsmiHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsmiHowDoI,
            this.tsmiAbout});
            this.tsmiHelp.Name = "tsmiHelp";
            this.tsmiHelp.Size = new System.Drawing.Size(53, 24);
            this.tsmiHelp.Text = "Help";
            // 
            // tsmiHowDoI
            // 
            this.tsmiHowDoI.Name = "tsmiHowDoI";
            this.tsmiHowDoI.Size = new System.Drawing.Size(141, 24);
            this.tsmiHowDoI.Text = "How Do I";
            this.tsmiHowDoI.Click += new System.EventHandler(this.tsmiHowDoI_Click);
            // 
            // tsmiAbout
            // 
            this.tsmiAbout.Name = "tsmiAbout";
            this.tsmiAbout.Size = new System.Drawing.Size(141, 24);
            this.tsmiAbout.Text = "About";
            this.tsmiAbout.Click += new System.EventHandler(this.tsmiAbout_Click);
            // 
            // nudSpeed
            // 
            this.nudSpeed.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.nudSpeed.DecimalPlaces = 2;
            this.nudSpeed.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.nudSpeed.Location = new System.Drawing.Point(405, 342);
            this.nudSpeed.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.nudSpeed.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.nudSpeed.Name = "nudSpeed";
            this.nudSpeed.Size = new System.Drawing.Size(60, 22);
            this.nudSpeed.TabIndex = 8;
            this.nudSpeed.ThousandsSeparator = true;
            this.nudSpeed.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // btScreenshot
            // 
            this.btScreenshot.Location = new System.Drawing.Point(12, 31);
            this.btScreenshot.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btScreenshot.Name = "btScreenshot";
            this.btScreenshot.Size = new System.Drawing.Size(40, 25);
            this.btScreenshot.TabIndex = 9;
            this.btScreenshot.Text = "SC";
            this.btScreenshot.UseVisualStyleBackColor = true;
            // 
            // btVideoCapturing
            // 
            this.btVideoCapturing.Location = new System.Drawing.Point(59, 31);
            this.btVideoCapturing.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btVideoCapturing.Name = "btVideoCapturing";
            this.btVideoCapturing.Size = new System.Drawing.Size(40, 25);
            this.btVideoCapturing.TabIndex = 10;
            this.btVideoCapturing.Text = "VC";
            this.btVideoCapturing.UseVisualStyleBackColor = true;
            // 
            // btInfo
            // 
            this.btInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btInfo.Enabled = false;
            this.btInfo.Location = new System.Drawing.Point(425, 31);
            this.btInfo.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btInfo.Name = "btInfo";
            this.btInfo.Size = new System.Drawing.Size(45, 25);
            this.btInfo.TabIndex = 11;
            this.btInfo.Text = "Info";
            this.btInfo.UseVisualStyleBackColor = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(484, 378);
            this.Controls.Add(this.btInfo);
            this.Controls.Add(this.btVideoCapturing);
            this.Controls.Add(this.btScreenshot);
            this.Controls.Add(this.nudSpeed);
            this.Controls.Add(this.pRenderArea);
            this.Controls.Add(this.lbSpeed);
            this.Controls.Add(this.lbTime);
            this.Controls.Add(this.btStartStop);
            this.Controls.Add(this.tbTime);
            this.Controls.Add(this.menuStrip);
            this.MainMenuStrip = this.menuStrip;
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.MinimumSize = new System.Drawing.Size(499, 398);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Mitosis visualizer";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.tbTime)).EndInit();
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudSpeed)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TrackBar tbTime;
        private System.Windows.Forms.Button btStartStop;
        private System.Windows.Forms.Label lbTime;
        private System.Windows.Forms.Label lbSpeed;
        private System.Windows.Forms.Panel pRenderArea;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tsmiOpen;
        private System.Windows.Forms.ToolStripMenuItem tsmiClose;
        private System.Windows.Forms.ToolStripSeparator tsmiSplitter1;
        private System.Windows.Forms.ToolStripMenuItem tsmiExit;
        private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tsmiVisual;
        private System.Windows.Forms.ToolStripMenuItem tsmiCapture;
        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.NumericUpDown nudSpeed;
        private System.Windows.Forms.Button btScreenshot;
        private System.Windows.Forms.Button btVideoCapturing;
        private System.Windows.Forms.ToolStripMenuItem tsmiHelp;
        private System.Windows.Forms.ToolStripMenuItem tsmiHowDoI;
        private System.Windows.Forms.ToolStripMenuItem tsmiAbout;
        private System.Windows.Forms.Button btInfo;

    }
}

