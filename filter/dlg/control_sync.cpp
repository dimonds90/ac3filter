#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_sync.h"

static int controls[] =
{
  IDC_GRP_SYNC,
  IDC_SLI_TIME_SHIFT,
  IDC_LBL_TIME_SHIFT,
  IDC_EDT_TIME_SHIFT,
  IDC_LBL_TIME_SHIFT_MINUS,
  IDC_LBL_TIME_SHIFT_PLUS,
  IDC_CHK_JITTER,
  IDC_LBL_JITTER,
  IDC_EDT_JITTER,
  0
};

///////////////////////////////////////////////////////////////////////////////

ControlSync::ControlSync(HWND _dlg, IDecoder *_dec):
Controller(_dlg, ::controls), dec(_dec)
{
  dec->AddRef();
  memset(jitter, 0, sizeof(jitter));
}

ControlSync::~ControlSync()
{
  dec->Release();
}

void ControlSync::init()
{
  edt_time_shift.link(hdlg, IDC_EDT_TIME_SHIFT);
  SendDlgItemMessage(hdlg, IDC_SLI_TIME_SHIFT, TBM_SETRANGE, TRUE, MAKELONG(-500, 500));
  SendDlgItemMessage(hdlg, IDC_SLI_TIME_SHIFT, TBM_SETTIC, 0, 0);
  SendDlgItemMessage(hdlg, IDC_SLI_TIME_SHIFT, TBM_SETPAGESIZE, 0, 100);
}

void ControlSync::update()
{
  dec->get_time_shift(&time_shift);
  dec->get_time_factor(&time_factor);
  dec->get_dejitter(&dejitter);
  dec->get_threshold(&threshold);

  edt_time_shift.update_value(time_shift * 1000);
  SendDlgItemMessage(hdlg, IDC_SLI_TIME_SHIFT, TBM_SETPOS, TRUE, int(time_shift * 1000));
  CheckDlgButton(hdlg, IDC_CHK_JITTER, dejitter? BST_CHECKED: BST_UNCHECKED);

  update_dynamic();
};

void ControlSync::update_dynamic()
{
  char new_jitter[sizeof(jitter)];
  memset(new_jitter, 0, sizeof(jitter));
  dec->get_jitter(&input_mean, &input_stddev, &output_mean, &output_stddev);

  sprintf(new_jitter, _("Input\tmean: %ims\tstddev: %ims\nOutput\tmean: %ims\tstddev: %ims"),
    int(input_mean * 1000), int(input_stddev * 1000), 
    int(output_mean * 1000), int(output_stddev * 1000));

  cr2crlf(new_jitter, sizeof(new_jitter));

  if (memcmp(jitter, new_jitter, sizeof(jitter)))
  {
    memcpy(jitter, new_jitter, sizeof(jitter));
    SendDlgItemMessage(hdlg, IDC_EDT_JITTER, WM_SETTEXT, 0, (LPARAM) jitter);
  }
}

ControlSync::cmd_result ControlSync::command(int control, int message)
{
  switch (control)
  {
    case IDC_EDT_TIME_SHIFT:
      if (message == CB_ENTER)
      {
        time_shift = vtime_t(edt_time_shift.value) / 1000;
        dec->set_time_shift(time_shift);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_SLI_TIME_SHIFT:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        time_shift = vtime_t(SendDlgItemMessage(hdlg, IDC_SLI_TIME_SHIFT, TBM_GETPOS, 0, 0)) / 1000;
        dec->set_time_shift(time_shift);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_CHK_JITTER:
    {
      dejitter = (SendDlgItemMessage(hdlg, IDC_CHK_JITTER, BM_GETCHECK, 0, 0) == BST_CHECKED);
      dec->set_dejitter(dejitter);
      update();
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
