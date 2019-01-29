using System.Drawing;
using System.Windows.Forms;

namespace Mitosis
{
  internal class SelectionPopupFormAction : IOnTickAction
  {
    private class Context : IOnTickActionContext
    {
      public Form OwnerForm { get; private set; }
      public SelectionPopupForm SelectionPopupForm { get; private set; }
      public bool FormCreationRequest { get; set; }
      private readonly Button _showPropsButton;

      public Context(Form ownerForm, Button showPropsButton)
      {
        OwnerForm = ownerForm;
        SelectionPopupForm = null;
        FormCreationRequest = false;
        _showPropsButton = showPropsButton;
        _showPropsButton.Enabled = true;
        _showPropsButton.Click += ShowPropsButtonPressed;

        if (SettingsManager.GetRef().GuiSettings.ShowProperties)
        { FormCreationRequest = true; }
      }

      private void ShowPropsButtonPressed(System.Object sender, System.EventArgs args)
      { FormCreationRequest = true; }

      private void PopupFormClosing(System.Object sender, System.EventArgs args)
      {
        SelectionPopupForm = null;
        _showPropsButton.Enabled = true;
      }

      public void CreatePopupForm()
      {
        if (SelectionPopupForm == null)
        {
          _showPropsButton.Enabled = false;
          SelectionPopupForm = new SelectionPopupForm(PopupFormClosing);
          SelectionPopupForm.Show();
        }
      }

      public void Dispose()
      {
        if (SelectionPopupForm != null)
        {
          SelectionPopupForm.Close();
          SettingsManager.GetRef().GuiSettings.ShowProperties = true;
        }
        else
        { SettingsManager.GetRef().GuiSettings.ShowProperties = false; }
        SelectionPopupForm = null;
      }
    }

    private readonly States _states;
    private Context _context;

    public SelectionPopupFormAction(States states, Form ownerForm, Button showPropsButton)
      : this(states, new Context(ownerForm, showPropsButton))
    { /*nothing*/ }

    public SelectionPopupFormAction(States states, IOnTickActionContext context)
    {
      _states = states;
      _context = context as Context;
    }

    public void Dispose()
    {
      if (_context != null)
      { _context.Dispose(); }
      _context = null;
    }

    public void Perform()
    {
      if (_context.SelectionPopupForm == null && _context.FormCreationRequest)
      {
        _context.CreatePopupForm();
        _context.FormCreationRequest = false;
      }

      if (_context.SelectionPopupForm != null)
      {
        if (SettingsManager.GetRef().GuiSettings.PinProperties)
        {
          _context.SelectionPopupForm.Location = new Point(_context.OwnerForm.Right, _context.OwnerForm.Top);
        }

        _context.SelectionPopupForm.CurrentObject = _states.SelectedObject;
      }
    }

    public IOnTickActionContext DisposeAndExportContext()
    {
      Context res = _context;
      _context = null;
      Dispose();
      return res;
    }
  }
}
