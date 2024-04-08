using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MiCoSi
{
  public partial class AboutForm : Form
  {
    public AboutForm()
    {
      InitializeComponent();

      lbProgramVersionValue.Text = String.Format("{0}.{1}.{2}",
                                                 CurrentVersion.Major,
                                                 CurrentVersion.Minor,
                                                 CurrentVersion.Build);

      lbFileFormatVersionValue.Text = String.Format("v{0}", CurrentVersion.FileFormat);

      lbPublishDateValue.Text = CurrentVersion.PublishDate;
    }

    private void btOK_Click(object sender, EventArgs e)
    { Close(); }
  }
}
