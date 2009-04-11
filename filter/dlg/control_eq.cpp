#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_eq.h"

static const int controls[] =
{
  IDC_GRP_EQ,
  IDC_CHK_EQ,
  IDC_BTN_EQ_RESET,

  IDC_SLI_EQ1, IDC_SLI_EQ2, IDC_SLI_EQ3, IDC_SLI_EQ4, IDC_SLI_EQ5, IDC_SLI_EQ6, IDC_SLI_EQ7, IDC_SLI_EQ8, IDC_SLI_EQ9, IDC_SLI_EQ10,
  IDC_EDT_EQ1, IDC_EDT_EQ2, IDC_EDT_EQ3, IDC_EDT_EQ4, IDC_EDT_EQ5, IDC_EDT_EQ6, IDC_EDT_EQ7, IDC_EDT_EQ8, IDC_EDT_EQ9, IDC_EDT_EQ10,
  IDC_LBL_EQ1, IDC_LBL_EQ2, IDC_LBL_EQ3, IDC_LBL_EQ4, IDC_LBL_EQ5, IDC_LBL_EQ6, IDC_LBL_EQ7, IDC_LBL_EQ8, IDC_LBL_EQ9, IDC_LBL_EQ10,
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

static const int idc_lbl_eq[EQ_BANDS] =
{
  IDC_LBL_EQ1, IDC_LBL_EQ2, IDC_LBL_EQ3, IDC_LBL_EQ4, IDC_LBL_EQ5, IDC_LBL_EQ6, IDC_LBL_EQ7, IDC_LBL_EQ8, IDC_LBL_EQ9, IDC_LBL_EQ10,
};

static EqBand default_bands[EQ_BANDS] = 
{
  { 30, 1.0 }, { 60, 1.0 }, { 125, 1.0 }, { 250, 1.0 }, { 500, 1.0 }, { 1000, 1.0 }, { 2000, 1.0 }, { 4000, 1.0 }, { 8000, 1.0 }, { 16000, 1.0 }
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
  proc->get_eq_master_nbands(&nbands);
  proc->get_eq_master_bands(bands, 0, EQ_BANDS);

  // set the default scale if no bands defined
  if (nbands == 0)
  {
    proc->set_eq_master_bands(default_bands, EQ_BANDS);
    proc->get_eq_master_nbands(&nbands);
    proc->get_eq_master_bands(bands, 0, EQ_BANDS);
  }

  CheckDlgButton(hdlg, IDC_CHK_EQ, eq? BST_CHECKED: BST_UNCHECKED);
  for (size_t band = 0; band < EQ_BANDS; band++)
  {
    if (band < nbands)
    {
      char buf[32];
      if (bands[band].freq >= 1000)
        sprintf(buf, _("%ikHz"), bands[band].freq / 1000);
      else
        sprintf(buf, _("%iHz"), bands[band].freq);

      ShowWindow(GetDlgItem(hdlg, idc_edt_eq[band]), SW_SHOW);
      ShowWindow(GetDlgItem(hdlg, idc_sli_eq[band]), SW_SHOW);
      ShowWindow(GetDlgItem(hdlg, idc_lbl_eq[band]), SW_SHOW);

      edt_gain[band].update_value(value2db(bands[band].gain));
      SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETPOS, TRUE, long(-value2db(bands[band].gain) * ticks));
      SetDlgItemText(hdlg, idc_lbl_eq[band], buf);
    }
    else
    {
      ShowWindow(GetDlgItem(hdlg, idc_edt_eq[band]), SW_HIDE);
      ShowWindow(GetDlgItem(hdlg, idc_sli_eq[band]), SW_HIDE);
      ShowWindow(GetDlgItem(hdlg, idc_lbl_eq[band]), SW_HIDE);
    }
  }
};

ControlEq::cmd_result ControlEq::command(int control, int message)
{
  size_t band;

  if (message == CB_ENTER)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_edt_eq[band])
      {
        proc->get_eq_master_bands(bands, 0, EQ_BANDS);
        bands[band].gain = db2value(edt_gain[band].value);
        proc->set_eq_master_bands(bands, EQ_BANDS);
        update();
        return cmd_ok;
      }

  if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_sli_eq[band])
      {
        proc->get_eq_master_bands(bands, 0, EQ_BANDS);
        bands[band].gain = db2value(-double(SendDlgItemMessage(hdlg, idc_sli_eq[band],TBM_GETPOS, 0, 0))/ticks);
        proc->set_eq_master_bands(bands, EQ_BANDS);
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
      proc->set_eq_master_bands(bands, EQ_BANDS);
      update();
      return cmd_ok;
    }
  }

  return cmd_not_processed;
}
