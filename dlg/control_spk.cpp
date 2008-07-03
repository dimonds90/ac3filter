#include <stdio.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_spk.h"

static int controls[] =
{
  IDC_GRP_OUTPUT,
  IDC_CMB_SPK,
  IDC_CMB_FORMAT,
  IDC_CMB_RATE,
  IDC_CHK_USE_SPDIF,
  IDC_LBL_INPUT,
  IDC_LBL_SPDIF_STATUS,
  0
};

///////////////////////////////////////////////////////////////////////////////
// Speakers list definition
///////////////////////////////////////////////////////////////////////////////

static const char *spklist[] = 
{
  N_("AS IS (no change)"),
  N_("1/0 - mono"),
  N_("2/0 - stereo"),
  N_("3/0 - 3 front"),
  N_("2/1 - surround"),
  N_("3/1 - surround"),
  N_("2/2 - quadro"),
  N_("3/2 - 5 channels"),
  N_("1/0+SW 1.1 mono"),
  N_("2/0+SW 2.1 stereo"),
  N_("3/0+SW 3.1 front"),
  N_("2/1+SW 3.1 surround"),
  N_("3/1+SW 4.1 surround"),
  N_("2/2+SW 4.1 quadro"),
  N_("3/2+SW 5.1 channels"),
  N_("Dolby Surround/ProLogic"),
  N_("Dolby ProLogic II"),
};

static const char *fmtlist[] = 
{
  N_("PCM 16bit"),
  N_("PCM 24bit"),
  N_("PCM 32bit"),
  N_("PCM Float"),
};

static const int rates_list[] =
{
  0, 8000, 11025, 22050, 24000, 32000, 44100, 48000, 96000, 192000
};

static Speakers list2spk(int ispk, int ifmt, int sample_rate)
{
  int format = FORMAT_PCM16;
  int mask = MODE_STEREO;
  int relation = NO_RELATION;
  sample_t level = 32767;

  switch (ispk)
  {
    case  0: mask = 0;        break;
    case  1: mask = MODE_1_0; break;
    case  2: mask = MODE_2_0; break;
    case  3: mask = MODE_3_0; break;
    case  4: mask = MODE_2_1; break;
    case  5: mask = MODE_3_1; break;
    case  6: mask = MODE_2_2; break;
    case  7: mask = MODE_3_2; break;
           
    case  8: mask = MODE_1_0 | CH_MASK_LFE; break;
    case  9: mask = MODE_2_0 | CH_MASK_LFE; break;
    case 10: mask = MODE_3_0 | CH_MASK_LFE; break;
    case 11: mask = MODE_2_1 | CH_MASK_LFE; break;
    case 12: mask = MODE_3_1 | CH_MASK_LFE; break;
    case 13: mask = MODE_2_2 | CH_MASK_LFE; break;
    case 14: mask = MODE_3_2 | CH_MASK_LFE; break;

    case 15: mask = MODE_STEREO; relation = RELATION_DOLBY; break;
    case 16: mask = MODE_STEREO, relation = RELATION_DOLBY2; break;
  }

  switch (ifmt)
  {
    case 0: format = FORMAT_PCM16;    level = 32767; break;
    case 1: format = FORMAT_PCM24;    level = 8388607; break;
    case 2: format = FORMAT_PCM32;    level = 2147483647; break;
    case 3: format = FORMAT_PCMFLOAT; level = 1.0; break;
  }

  return Speakers(format, mask, sample_rate, level, relation);
}

static int spk2ispk(Speakers spk)
{
  switch (spk.relation)
  {
    case RELATION_DOLBY:   return 15;
    case RELATION_DOLBY2:  return 16;
    default:
      switch (spk.mask)
      {
        case 0:            return 0;
        case MODE_1_0:     return 1;
        case MODE_2_0:     return 2;
        case MODE_3_0:     return 3;
        case MODE_2_1:     return 4;
        case MODE_3_1:     return 5;
        case MODE_2_2:     return 6;
        case MODE_3_2:     return 7;
                                  
        case MODE_1_0 | CH_MASK_LFE: return  8;
        case MODE_2_0 | CH_MASK_LFE: return  9;
        case MODE_3_0 | CH_MASK_LFE: return 10;
        case MODE_2_1 | CH_MASK_LFE: return 11;
        case MODE_3_1 | CH_MASK_LFE: return 12;
        case MODE_2_2 | CH_MASK_LFE: return 13;
        case MODE_3_2 | CH_MASK_LFE: return 14;
      }
  }
  return 0;
}

static int spk2ifmt(Speakers spk)
{
  switch (spk.format)
  {
    case FORMAT_PCM16:    return 0;
    case FORMAT_PCM24:    return 1;
    case FORMAT_PCM32:    return 2;
    case FORMAT_PCMFLOAT: return 3;
  }
  return 0;
}



///////////////////////////////////////////////////////////////////////////////

ControlSpk::ControlSpk(HWND _dlg, IDecoder *_dec):
Controller(_dlg, ::controls), dec(_dec)
{
  dec->AddRef();
  spdif_status = SPDIF_MODE_NONE;
}

ControlSpk::~ControlSpk()
{
  dec->Release();
}

void ControlSpk::init()
{
  int i;
  char buf[128];

  SendDlgItemMessage(hdlg, IDC_CMB_SPK, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < array_size(spklist); i++)
    SendDlgItemMessage(hdlg, IDC_CMB_SPK, CB_ADDSTRING, 0, (LONG) gettext(spklist[i]));

  SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < array_size(fmtlist); i++)
    SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_ADDSTRING, 0, (LONG) gettext(fmtlist[i]));

  SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_ADDSTRING, 0, (LONG)_("AS IS (no change)"));
  for (i = 1; i < array_size(rates_list); i++)
  {
    int index = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_ADDSTRING, 0, (LONG)itoa(rates_list[i], buf, 10));
    SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_SETITEMDATA, index, rates_list[i]);
  }
}

void ControlSpk::update()
{
  dec->get_user_spk(&user_spk);
  dec->get_use_spdif(&use_spdif);

  SendDlgItemMessage(hdlg, IDC_CMB_SPK,    CB_SETCURSEL, spk2ispk(user_spk), 0);
  SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_SETCURSEL, spk2ifmt(user_spk), 0);
  CheckDlgButton(hdlg, IDC_CHK_USE_SPDIF, use_spdif? BST_CHECKED: BST_UNCHECKED);

  if (user_spk.sample_rate)
  {
    char buf[128];
    sprintf(buf, "%i", user_spk.sample_rate);
    int i = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_FINDSTRINGEXACT, -1, (LPARAM)buf);
    if (i != CB_ERR)
      SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_SETCURSEL, i, 0);
  }
  else
    SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_SETCURSEL, 0, 0);

};

void ControlSpk::update_dynamic()
{
  /////////////////////////////////////////////////////////
  // Input format

  Speakers new_in_spk;
  dec->get_in_spk(&new_in_spk);
  if (in_spk != new_in_spk)
  {
    in_spk = new_in_spk;
    if (in_spk.format == FORMAT_UNKNOWN)
      SetDlgItemText(hdlg, IDC_LBL_INPUT, "");
    else
    {
      char buf[128];
      sprintf(buf, _("%s %s %iHz"), in_spk.format_text(), in_spk.mode_text(), in_spk.sample_rate);
      SetDlgItemText(hdlg, IDC_LBL_INPUT, buf);
    }
  }

  /////////////////////////////////////////////////////////
  // SPDIF status

  int new_spdif_status;
  dec->get_spdif_status(&new_spdif_status);

  if (spdif_status != new_spdif_status)
  {
    spdif_status = new_spdif_status;
    switch (new_spdif_status)
    {
      case SPDIF_MODE_NONE:
        SetDlgItemText(hdlg, IDC_CHK_USE_SPDIF, _("Use SPDIF"));
        break;

      case SPDIF_MODE_DISABLED:
        SetDlgItemText(hdlg, IDC_CHK_USE_SPDIF, _("Use SPDIF (disabled)"));
        break;

      case SPDIF_MODE_PASSTHROUGH:
        SetDlgItemText(hdlg, IDC_CHK_USE_SPDIF, _("Use SPDIF (passthrough)"));
        break;

      case SPDIF_MODE_ENCODE:
        SetDlgItemText(hdlg, IDC_CHK_USE_SPDIF, _("Use SPDIF (AC3 encode)"));
        break;

      default:
        SetDlgItemText(hdlg, IDC_CHK_USE_SPDIF, _("Use SPDIF (Unknown)"));
        break;
    }
  }
}

ControlSpk::cmd_result ControlSpk::command(int control, int message)
{
  switch (control)
  {
    case IDC_CMB_SPK:
    case IDC_CMB_RATE:
    case IDC_CMB_FORMAT:
      if (message == CBN_SELENDOK)
      {
        int ispk = SendDlgItemMessage(hdlg, IDC_CMB_SPK, CB_GETCURSEL, 0, 0);
        int ifmt = SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_GETCURSEL, 0, 0);
        int irate = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_GETCURSEL, 0, 0);
        int sample_rate = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_GETITEMDATA, irate, 0);

        user_spk = list2spk(ispk, ifmt, sample_rate);
        dec->set_user_spk(user_spk);
        return cmd_update;
      }
      return cmd_not_processed;

    case IDC_CHK_USE_SPDIF:
    {
      use_spdif = IsDlgButtonChecked(hdlg, IDC_CHK_USE_SPDIF) == BST_CHECKED;
      dec->set_use_spdif(use_spdif);
      return cmd_update;
    }
    return cmd_not_processed;
  }
  return cmd_not_processed;
}
