#include <windows.h>
#include "../../BugTrap/BugTrap.h"
#include "../resource_ids.h"
#include "../registry.h"
#include "../guids.h"
#include "control_debug.h"
#include "error_report.h"

static const int controls[] =
{
  IDC_GRP_DEBUG,
  IDC_LBL_FEEDBACK,
  IDC_EDT_FEEDBACK,
  IDC_CHK_SAVE_LOGS,
  IDC_BTN_ERROR_REPORT,
  IDC_LNK_ERROR_REPORT,
  IDC_CHK_SEND_AUDIO_DATA,

  0
};

///////////////////////////////////////////////////////////////////////////////

ControlDebug::ControlDebug(HWND _dlg):
Controller(_dlg, ::controls)
{}

void ControlDebug::init()
{
  edt_feedback.link(hdlg, IDC_EDT_FEEDBACK);
  lnk_error_report.link(hdlg, IDC_LNK_ERROR_REPORT);
}

void ControlDebug::update()
{
  bool save_logs = false;
  bool send_audio_data = false;
  char feedback[256];
  feedback[0] = 0;

  RegistryKey reg(REG_KEY);
  reg.get_bool("save_logs", save_logs);
  reg.get_bool("send_audio_data", send_audio_data);
  reg.get_text("feedback", feedback, sizeof(feedback));

  CheckDlgButton(hdlg, IDC_CHK_SAVE_LOGS, save_logs? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SEND_AUDIO_DATA, send_audio_data? BST_CHECKED: BST_UNCHECKED);
  edt_feedback.set_text(feedback);
};

ControlDebug::cmd_result ControlDebug::command(int control, int message)
{
  switch (control)
  {
    case IDC_CHK_SAVE_LOGS:
    {
      bool save_logs = IsDlgButtonChecked(hdlg, IDC_CHK_SAVE_LOGS) == BST_CHECKED;
      RegistryKey reg(REG_KEY);
      reg.set_bool("save_logs", save_logs);
      return cmd_ok;
    }

    case IDC_CHK_SEND_AUDIO_DATA:
    {
      bool send_audio_data = IsDlgButtonChecked(hdlg, IDC_CHK_SEND_AUDIO_DATA) == BST_CHECKED;
      RegistryKey reg(REG_KEY);
      reg.set_bool("send_audio_data", send_audio_data);
      return cmd_ok;
    }

    case IDC_EDT_FEEDBACK:
      if (message == CB_ENTER)
      {
        RegistryKey reg(REG_KEY);
        reg.set_text("feedback", edt_feedback.get_text());
        return cmd_ok;
      }
      break;

    case IDC_BTN_ERROR_REPORT:
      if (message == BN_CLICKED)
      {
        ErrorReportDlg dlg;
        dlg.send_audio_data = (IsDlgButtonChecked(hdlg, IDC_CHK_SEND_AUDIO_DATA) == BST_CHECKED);
        dlg.edt_feedback.set_text(edt_feedback.get_text());
        if (dlg.exec(ac3filter_instance, MAKEINTRESOURCE(IDD_ERROR_REPORT), hdlg) == IDOK)
        {
          RegistryKey reg(REG_KEY);
          reg.set_text("feedback", dlg.edt_feedback.get_text());
          reg.set_bool("send_audio_data", dlg.send_audio_data);

          BT_SetUserMessage(dlg.edt_desc.get_text());
          BT_CallSehFilter();
        }
        return cmd_ok;
      }
      break;
  }
  return cmd_not_processed;
}
