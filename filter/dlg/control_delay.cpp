#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "filters/delay.h"
#include "control_delay.h"

static const int controls[] =
{
  IDC_GRP_DELAYS,
  IDC_CHK_DELAYS,
  IDC_LBL_DELAY_UNITS,
  IDC_CMB_DELAY_UNITS,
  IDC_LBL_DELAY_L, IDC_LBL_DELAY_C, IDC_LBL_DELAY_R, IDC_LBL_DELAY_SL, IDC_LBL_DELAY_SR, IDC_LBL_DELAY_SW,
  IDC_EDT_DELAY_L, IDC_EDT_DELAY_C, IDC_EDT_DELAY_R, IDC_EDT_DELAY_SL, IDC_EDT_DELAY_SR, IDC_EDT_DELAY_LFE,
  0
};

static const int idc_edt_delay[NCHANNELS] =
{ 
  IDC_EDT_DELAY_L,
  IDC_EDT_DELAY_C,
  IDC_EDT_DELAY_R,
  IDC_EDT_DELAY_SL,
  IDC_EDT_DELAY_SR,
  IDC_EDT_DELAY_LFE
};

///////////////////////////////////////////////////////////////////////////////
// Delay units
///////////////////////////////////////////////////////////////////////////////

static char *units_list[] =
{
  N_("Samples"),
  N_("Millisecs"),
  N_("Meters"),
  N_("Centimeters"),
  N_("Feet"),
  N_("Inches")
};

static int list2units(LRESULT list)
{
  switch (list)
  {
    case 0:  return DELAY_SP;
    case 1:  return DELAY_MS;
    case 2:  return DELAY_M;
    case 3:  return DELAY_CM;
    case 4:  return DELAY_FT;
    case 5:  return DELAY_IN;
    default: return DELAY_SP;
  }
}

static int units2list(int units)
{
  switch (units)
  {
    case DELAY_SP: return 0;
    case DELAY_MS: return 1;
    case DELAY_M : return 2;
    case DELAY_CM: return 3;
    case DELAY_FT: return 4;
    case DELAY_IN: return 5;
    default:       return 0;
  }
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
  for (int ch = 0; ch < NCHANNELS; ch++)
    edt_delay[ch].link(hdlg, idc_edt_delay[ch]);

  SendDlgItemMessage(hdlg, IDC_CMB_DELAY_UNITS, CB_RESETCONTENT, 0, 0);
  for (int i = 0; i < array_size(units_list); i++)
    SendDlgItemMessage(hdlg, IDC_CMB_DELAY_UNITS, CB_ADDSTRING, 0, (LPARAM) gettext(units_list[i]));
}

void ControlDelay::update()
{
  proc->get_delay(&delay);
  proc->get_delay_units(&delay_units);
  proc->get_delays(delays);

  for (int ch = 0; ch < NCHANNELS; ch++)
  {
    edt_delay[ch].update_value(delays[ch]);
    edt_delay[ch].enable(delay);
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

    case IDC_EDT_DELAY_L:
    case IDC_EDT_DELAY_C:
    case IDC_EDT_DELAY_R:
    case IDC_EDT_DELAY_SL:
    case IDC_EDT_DELAY_SR:
    case IDC_EDT_DELAY_LFE:
      if (message == CB_ENTER)
      {
        for (int ch = 0; ch < NCHANNELS; ch++)
          delays[ch] = (float)edt_delay[ch].value;
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
