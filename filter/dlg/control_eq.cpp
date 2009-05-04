#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "../ac3filter_intl.h"
#include "../custom_eq.h"
#include "../resource_ids.h"
#include "control_eq.h"

static const int controls[] =
{
  IDC_GRP_EQ,
  IDC_CHK_EQ,
  IDC_CMB_EQ_CH,
  IDC_BTN_EQ_RESET,
  IDC_BTN_EQ_CUSTOM,

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

static const int ch_name[NCHANNELS+1] =
{
  CH_NONE /* master equalizer */, CH_L, CH_C, CH_R, CH_SL, CH_SR, CH_LFE
};

static const char *ch_text[NCHANNELS+1] =
{
  N_("Master"),
  N_("Left"),
  N_("Center"),
  N_("Right"),
  N_("Surround Left"),
  N_("Surround Right"),
  N_("Subwoofer")
};

static EqBand default_bands[EQ_BANDS] = 
{
  { 30, 1.0 }, { 60, 1.0 }, { 125, 1.0 }, { 250, 1.0 }, { 500, 1.0 }, { 1000, 1.0 }, { 2000, 1.0 }, { 4000, 1.0 }, { 8000, 1.0 }, { 16000, 1.0 }
};

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
  eq_ch = -1;
  for (int i = 0; i < array_size(ch_text); i++)
  {
    LRESULT idx = SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_ADDSTRING, 0, (LPARAM)ch_text[i]);
    SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_SETITEMDATA, idx, (LPARAM)ch_name[i]);
  }
  SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_SETCURSEL, 0, 0);

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
  proc->get_eq_nbands(eq_ch, &nbands);
  proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);

  // set the default scale if no bands defined
  if (nbands == 0)
  {
    proc->set_eq_bands(eq_ch, default_bands, EQ_BANDS);
    proc->get_eq_nbands(eq_ch, &nbands);
    proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);
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
      bands[band].freq = 0;
      bands[band].gain = 0;
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
        proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);
        bands[band].gain = db2value(edt_gain[band].value);
        proc->set_eq_bands(eq_ch, bands, EQ_BANDS);
        update();
        return cmd_ok;
      }

  if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_sli_eq[band])
      {
        proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);
        bands[band].gain = db2value(-double(SendDlgItemMessage(hdlg, idc_sli_eq[band],TBM_GETPOS, 0, 0))/ticks);
        proc->set_eq_bands(eq_ch, bands, EQ_BANDS);
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

    case IDC_CMB_EQ_CH:
      if (message == CBN_SELENDOK)
      {
        LRESULT idx = SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_GETCURSEL, 0, 0);
        if (idx != CB_ERR)
        {
          LRESULT data = SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_GETITEMDATA, idx, 0);
          eq_ch = (int)data;
          update();
        }
      }
      break;

    case IDC_BTN_EQ_RESET:
    {
      for (size_t band = 0; band < EQ_BANDS; band++)
        bands[band].gain = 1.0;
      proc->set_eq_bands(eq_ch, bands, EQ_BANDS);
      update();
      return cmd_ok;
    }

    case IDC_BTN_EQ_CUSTOM:
    {
      INT_PTR result;
      CustomEq custom_eq;
      custom_eq.set_bands(bands, EQ_BANDS);
      result = custom_eq.exec(ac3filter_instance, MAKEINTRESOURCE(IDD_EQ_CUSTOM), hdlg);
      if (result == IDOK)
      {
        nbands = custom_eq.get_nbands();
        if (nbands > EQ_BANDS) nbands = EQ_BANDS;
        custom_eq.get_bands(bands, 0, nbands);
        proc->set_eq_bands(eq_ch, bands, nbands);
        update();
      }
      return cmd_ok;
    }
  }

  return cmd_not_processed;
}
