#include "../ch_names.h"
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "filters/delay.h"
#include "control_delay.h"

#define NDELAYS 8

static const int controls[] =
{
  IDC_GRP_DELAYS,
  IDC_CHK_DELAYS,
  IDC_BTN_DELAYS_RESET,
  IDC_LBL_DELAY_UNITS,
  IDC_CMB_DELAY_UNITS,
  IDC_LBL_DELAY1, IDC_LBL_DELAY2, IDC_LBL_DELAY3, IDC_LBL_DELAY4, IDC_LBL_DELAY5, IDC_LBL_DELAY6, IDC_LBL_DELAY7, IDC_LBL_DELAY8, 
  IDC_EDT_DELAY1, IDC_EDT_DELAY2, IDC_EDT_DELAY3, IDC_EDT_DELAY4, IDC_EDT_DELAY5, IDC_EDT_DELAY6, IDC_EDT_DELAY7, IDC_EDT_DELAY8, 
  0
};

static const int idc_edt_delay[NDELAYS] = { IDC_EDT_DELAY1, IDC_EDT_DELAY2, IDC_EDT_DELAY3, IDC_EDT_DELAY4, IDC_EDT_DELAY5, IDC_EDT_DELAY6, IDC_EDT_DELAY7, IDC_EDT_DELAY8, };
static const int idc_lbl_delay[NDELAYS] = { IDC_LBL_DELAY1, IDC_LBL_DELAY2, IDC_LBL_DELAY3, IDC_LBL_DELAY4, IDC_LBL_DELAY5, IDC_LBL_DELAY6, IDC_LBL_DELAY7, IDC_LBL_DELAY8, };

static const int ch_map322[NDELAYS] = { CH_LFE, CH_BL, CH_SL, CH_L, CH_C, CH_R, CH_SR, CH_BR };

///////////////////////////////////////////////////////////////////////////////
// Delay units
///////////////////////////////////////////////////////////////////////////////

static struct { int units; const char *name; } units_list[] =
{
  { DELAY_SP, N_("Samples") },
  { DELAY_MS, N_("Millisecs") },
  { DELAY_M,  N_("Meters") },
  { DELAY_CM, N_("Centimeters") },
  { DELAY_FT, N_("Feet") },
  { DELAY_IN, N_("Inches") },
};

static int list2units(LRESULT list)
{
  if (list >= 0 && list < array_size(units_list))
    return units_list[list].units;
  return DELAY_SP;
}

static int units2list(int units)
{
  for (int i = 0; i < array_size(units_list); i++)
    if (units_list[i].units == units)
      return i;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

ControlDelay::ControlDelay(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
{
  proc->AddRef();
}

ControlDelay::~ControlDelay()
{
  proc->Release();
}

void ControlDelay::init()
{
  int i;

  for (i = 0; i < NDELAYS; i++)
    edt_delay[i].link(hdlg, idc_edt_delay[i]);

  SendDlgItemMessage(hdlg, IDC_CMB_DELAY_UNITS, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < array_size(units_list); i++)
    SendDlgItemMessage(hdlg, IDC_CMB_DELAY_UNITS, CB_ADDSTRING, 0, (LPARAM) gettext(units_list[i].name));

  for (int i = 0; i < NDELAYS; i++)
    SetDlgItemText(hdlg, idc_lbl_delay[i], gettext(short_ch_names[ch_map322[i]]));
}

void ControlDelay::update()
{
  proc->get_delay(&delay);
  proc->get_delay_units(&delay_units);
  proc->get_delays(delays);

  for (int i = 0; i < NDELAYS; i++)
  {
    edt_delay[i].update_value(delays[ch_map322[i]]);
    edt_delay[i].enable(delay);
  }

  CheckDlgButton(hdlg, IDC_CHK_DELAYS, delay? BST_CHECKED: BST_UNCHECKED);
  SendDlgItemMessage(hdlg, IDC_CMB_DELAY_UNITS, CB_SETCURSEL, units2list(delay_units), 0);
  EnableWindow(GetDlgItem(hdlg, IDC_CMB_DELAY_UNITS), delay);
};

ControlDelay::cmd_result ControlDelay::command(int control, int message)
{
  switch (control)
  {
    case IDC_CHK_DELAYS:
    {
      delay = IsDlgButtonChecked(hdlg, IDC_CHK_DELAYS) == BST_CHECKED;
      proc->set_delay(delay);
      update();
      break;
    }

    case IDC_BTN_DELAYS_RESET:
    {
      for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
        delays[ch_name] = 0;
      proc->set_delays(delays);
      update();
      break;
    }
      

    case IDC_EDT_DELAY1:
    case IDC_EDT_DELAY2:
    case IDC_EDT_DELAY3:
    case IDC_EDT_DELAY4:
    case IDC_EDT_DELAY5:
    case IDC_EDT_DELAY6:
    case IDC_EDT_DELAY7:
    case IDC_EDT_DELAY8:
      if (message == CB_ENTER)
      {
        proc->get_delays(delays);
        for (int i = 0; i < NDELAYS; i++)
          delays[ch_map322[i]] = (float)edt_delay[i].value;
        proc->set_delays(delays);
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_CMB_DELAY_UNITS:
      if (message == CBN_SELENDOK)
      {
        delay_units = list2units(SendDlgItemMessage(hdlg, IDC_CMB_DELAY_UNITS, CB_GETCURSEL, 0, 0));
        proc->set_delay_units(delay_units);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;
  }

  return cmd_not_processed;
}
