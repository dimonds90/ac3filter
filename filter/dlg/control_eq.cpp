#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "../ac3filter_intl.h"
#include "../ch_names.h"
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

static const int band_reorder[EQ_BANDS+1][EQ_BANDS] = 
{
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, // 0 bands
  { 4, 0, 1, 2, 3, 5, 6, 7, 8, 9 }, // 1 bands
  { 2, 7, 0, 1, 3, 4, 5, 6, 8, 9 }, // 2 bands
  { 1, 4, 7, 0, 2, 3, 5, 6, 8, 9 }, // 3 bands
  { 1, 3, 5, 7, 0, 2, 4, 6, 8, 9 }, // 4 bands
  { 1, 3, 4, 5, 7, 0, 2, 6, 8, 9 }, // 5 bands
  { 1, 2, 4, 5, 7, 8, 0, 3, 6, 9 }, // 6 bands
  { 1, 2, 3, 4, 5, 6, 7, 0, 8, 9 }, // 7 bands
  { 1, 2, 3, 4, 5, 6, 7, 8, 0, 9 }, // 8 bands
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, // 9 bands
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, // 10 bands
 };

static EqBand default_bands[EQ_BANDS] = 
{
  { 30, 1.0 }, { 60, 1.0 }, { 125, 1.0 }, { 250, 1.0 }, { 500, 1.0 }, { 1000, 1.0 }, { 2000, 1.0 }, { 4000, 1.0 }, { 8000, 1.0 }, { 16000, 1.0 }
};

static const double min_gain_level = -12.0; // dB
static const double max_gain_level = +12.0; // dB
static const double step_size = 1;          // dB
static const double page_size = 1;          // dB
static const int ticks = 10; // steps per dB

static const char *equalized_mark = " (*)";

static inline int db2pos(double db)
{
  return int(-db * ticks);
}

static inline double pos2db(LRESULT pos)
{
  double db = double(-pos) / ticks;
  return floor(db / step_size + 0.5) * step_size;
}

static inline int gain2pos(double gain)
{
  return db2pos(value2db(gain));
}

static inline double pos2gain(LRESULT pos)
{
  return db2value(pos2db(pos));
}

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

void ControlEq::init_channels_list()
{
  proc->get_eq_channel(&eq_ch);
  proc->get_proc_out_spk(&out_spk);

  LRESULT cmb_index;
  SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_RESETCONTENT, 0, 0);

  bool equalized = false;
  proc->get_eq_equalized(CH_NONE, &equalized);
  string s = _("All channels");
  if (equalized)
    s += equalized_mark;

  cmb_index = SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_ADDSTRING, 0, (LPARAM)s.c_str());
  SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_SETITEMDATA, cmb_index, (LPARAM)CH_NONE);
  if (CH_NONE == eq_ch)
    SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_SETCURSEL, cmb_index, 0);

  for (int ch_name = 0; ch_name < CH_NAMES; ch_name++)
    if ((out_spk.format == FORMAT_UNKNOWN) || (out_spk.mask & CH_MASK(ch_name)))
    {
      equalized = false;
      proc->get_eq_equalized(ch_name, &equalized);
      s = gettext(long_ch_names[ch_name]);
      if (equalized)
        s += equalized_mark;

      cmb_index = SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_ADDSTRING, 0, (LPARAM)s.c_str());
      SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_SETITEMDATA, cmb_index, (LPARAM)ch_name);
      if (ch_name == eq_ch)
        SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_SETCURSEL, cmb_index, 0);
    }
}

void ControlEq::init()
{
  eq_ch = CH_NONE;

  init_channels_list();

  for (size_t band = 0; band < EQ_BANDS; band++)
  {
    edt_gain[band].link(hdlg, idc_edt_eq[band]);
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETLINESIZE, 0, LONG(step_size * ticks));
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETPAGESIZE, 0, LONG(page_size * ticks));
    SendDlgItemMessage(hdlg, idc_sli_eq[band], TBM_SETTIC, 0, db2pos(0));
  }
}

void ControlEq::update()
{
  proc->get_eq(&eq);
  proc->get_eq_channel(&eq_ch);
  proc->get_eq_nbands(eq_ch, &nbands);
  proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);

  init_channels_list();

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

      ShowWindow(GetDlgItem(hdlg, idc_edt_eq[band_reorder[nbands][band]]), SW_SHOW);
      ShowWindow(GetDlgItem(hdlg, idc_sli_eq[band_reorder[nbands][band]]), SW_SHOW);
      ShowWindow(GetDlgItem(hdlg, idc_lbl_eq[band_reorder[nbands][band]]), SW_SHOW);

      edt_gain[band_reorder[nbands][band]].update_value(value2db(bands[band].gain));
      SendDlgItemMessage(hdlg, idc_sli_eq[band_reorder[nbands][band]], TBM_SETPOS, TRUE, gain2pos(bands[band].gain));
      SetDlgItemText(hdlg, idc_lbl_eq[band_reorder[nbands][band]], buf);
    }
    else
    {
      bands[band].freq = 0;
      bands[band].gain = 0;
      ShowWindow(GetDlgItem(hdlg, idc_edt_eq[band_reorder[nbands][band]]), SW_HIDE);
      ShowWindow(GetDlgItem(hdlg, idc_sli_eq[band_reorder[nbands][band]]), SW_HIDE);
      ShowWindow(GetDlgItem(hdlg, idc_lbl_eq[band_reorder[nbands][band]]), SW_HIDE);
    }
  }
};

ControlEq::cmd_result ControlEq::command(int control, int message)
{
  size_t band;

  if (message == CB_ENTER)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_edt_eq[band_reorder[nbands][band]])
      {
        proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);
        bands[band].gain = db2value(edt_gain[band_reorder[nbands][band]].value);
        proc->set_eq_bands(eq_ch, bands, EQ_BANDS);
        update();
        return cmd_ok;
      }

  if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
    for (band = 0; band < EQ_BANDS; band++)
      if (control == idc_sli_eq[band_reorder[nbands][band]])
      {
        LRESULT pos = SendDlgItemMessage(hdlg, idc_sli_eq[band_reorder[nbands][band]],TBM_GETPOS, 0, 0);
        proc->get_eq_bands(eq_ch, bands, 0, EQ_BANDS);
        bands[band].gain = pos2gain(pos);
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
      if (message == CBN_DROPDOWN)
        init_channels_list();
      else if (message == CBN_SELENDOK)
      {
        LRESULT idx = SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_GETCURSEL, 0, 0);
        if (idx != CB_ERR)
        {
          eq_ch = (int)SendDlgItemMessage(hdlg, IDC_CMB_EQ_CH, CB_GETITEMDATA, idx, 0);
          proc->set_eq_channel(eq_ch);
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
