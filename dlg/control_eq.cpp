#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_eq.h"

static const int controls[] =
{
  IDC_GRP_EQ,
  IDC_CHK_EQ,
  IDC_BTN_EQ_RESET,

  IDC_SLI_EQ1, IDC_SLI_EQ2, IDC_SLI_EQ3, IDC_SLI_EQ4, IDC_SLI_EQ5, IDC_SLI_EQ6, IDC_SLI_EQ7, IDC_SLI_EQ8, IDC_SLI_EQ9, IDC_SLI_EQ10,
  IDC_EDT_EQ1, IDC_EDT_EQ2, IDC_EDT_EQ3, IDC_EDT_EQ4, IDC_EDT_EQ5, IDC_EDT_EQ6, IDC_EDT_EQ7, IDC_EDT_EQ8, IDC_EDT_EQ9, IDC_EDT_EQ10,
  0
};

static const int idc_edt_eq[EQ_BANDS] =
{
  IDC_EDT_EQ1, IDC_EDT_EQ2, IDC_EDT_EQ3, IDC_EDT_EQ4, IDC_EDT_EQ5, IDC_EDT_EQ6, IDC_EDT_EQ7, IDC_EDT_EQ8, IDC_EDT_EQ9, IDC_EDT_EQ10,
};

static const int idc_sli_eq[EQ_BANDS] =
{
  IDC_SLI_EQ1, IDC_SLI_EQ2, IDC_SLI_EQ3, IDC_SLI_EQ4, IDC_SLI_EQ5, IDC_SLI_EQ6, IDC_SLI_EQ7, IDC_SLI_EQ8, IDC_SLI_EQ9, IDC_SLI_EQ10,
};

static const int band_freq[EQ_BANDS] =
{ 30, 60, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };

static const double band_gain[EQ_BANDS] =
{ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

static const double min_gain_level = -12.0;
static const double max_gain_level = +12.0;
static const int ticks = 5;

///////////////////////////////////////////////////////////////////////////////

ControlEq::ControlEq(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
{
  proc->AddRef();
}

ControlEq::~ControlEq()
{
  proc->Release();
}

void ControlEq::init()
{
  for (size_t band = 0; band < EQ_BANDS; band++)
  {
    edt_gain[band].link(hdlg, idc_edt_eq[band]);
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETTIC, 0, 0);
  }
}

void ControlEq::update()
{
  proc->get_eq(&eq);
  proc->get_eq_bands(freq, gain);

  CheckDlgButton(hdlg, IDC_CHK_EQ, eq? BST_CHECKED: BST_UNCHECKED);
  for (size_t band = 0; band < EQ_BANDS; band++)
  {
    edt_gain[band].update_value(value2db(gain[band]));
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETPOS, TRUE, long(-value2db(gain[band]) * ticks));
  }
};

ControlEq::cmd_result ControlEq::command(int control, int message)
{
  size_t band;

  if (message == CB_ENTER)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_edt_eq[band])
      {
        proc->get_eq_bands(freq, gain);
        gain[band] = db2value(edt_gain[band].value);
        proc->set_eq_bands(band_freq, gain);
        update();
        return cmd_ok;
      }

  if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_sli_eq[band])
      {
        proc->get_eq_bands(freq, gain);
        gain[band] = db2value(-double(SendDlgItemMessage(hdlg, idc_sli_eq[band],TBM_GETPOS, 0, 0))/ticks);
        proc->set_eq_bands(band_freq, gain);
        update();
        return cmd_ok;
      }

  switch (control)
  {
    case IDC_CHK_EQ:
    {
      eq = IsDlgButtonChecked(hdlg, IDC_CHK_EQ) == BST_CHECKED;
      proc->set_eq(eq);
      return cmd_ok;
    }

    case IDC_BTN_EQ_RESET:
    {
      proc->set_eq_bands(band_freq, band_gain);
      update();
      return cmd_ok;
    }
  }

  return cmd_not_processed;
}
