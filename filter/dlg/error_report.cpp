#include "error_report.h"
#include "../resource_ids.h"
#include "../registry.h"
#include "../guids.h"
#include "../ac3filter_intl.h"
#include "../../BugTrap/BugTrap.h"

void
ErrorReportDlg::on_create()
{
  edt_feedback.link(hwnd, IDC_EDT_FEEDBACK);
  edt_desc.link(hwnd, IDC_EDT_ERROR_DESC);
  CheckDlgButton(hwnd, IDC_CHK_SEND_AUDIO_DATA, send_audio_data? BST_CHECKED: BST_UNCHECKED);
}

BOOL
ErrorReportDlg::on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_COMMAND)
  {
    int control = LOWORD(wParam);
    int message = HIWORD(wParam);
    if (control == IDOK && message == BN_CLICKED)
    {
      if (strlen(edt_feedback.get_text()) == 0)
      {
        MessageBox(hwnd, _("Enter you email."), _("Please..."), MB_ICONWARNING | MB_OK);
        SetFocus(GetDlgItem(hwnd, IDC_EDT_FEEDBACK));
        return TRUE;
      }
      if (strlen(edt_desc.get_text()) == 0)
      {
        MessageBox(hwnd, _("Describe your proiblem."), _("Please..."), MB_ICONWARNING | MB_OK);
        SetFocus(GetDlgItem(hwnd, IDC_EDT_ERROR_DESC));
        return TRUE;
      }
      // Close the dialog
      return FALSE;
    }
  }

  return FALSE;
}
