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

static const struct { int mask; int relation; const char *text; } spklist[] = 
{
  { 0,              0, N_("Do not change") },
  // No subwoofer
  { MODE_1_0,       0, N_("Mono") },
  { MODE_2_0,       0, N_("Stereo") },
  { MODE_2_2,       0, N_("4.0 Quadro") },
  { MODE_3_2,       0, N_("5.0 Surround") },
//  { MODE_3_0_2,     0, N_("5.0 Surround (back)") },
  { MODE_3_2_1,     0, N_("6.0 Surround") },
  { MODE_3_2_2,     0, N_("7.0 Surround") },
//  { MODE_5_2,       0, N_("7.0 Surround (wide)") },
  // With subwoofer
  { MODE_2_0_LFE,   0, N_("2.1 Stereo") },
  { MODE_2_2_LFE,   0, N_("4.1 Quadro") },
  { MODE_3_2_LFE,   0, N_("5.1 Surround") },
//  { MODE_3_0_2_LFE, 0, N_("5.1 Surround (back)") },
  { MODE_3_2_1_LFE, 0, N_("6.1 Surround") },
  { MODE_3_2_2_LFE, 0, N_("7.1 Surround") },
//  { MODE_5_2_LFE,   0, N_("7.1 Surround (wide)") },
  // Dolby downmixes
  { MODE_2_0, RELATION_DOLBY,  N_("Dolby Surround/ProLogic") },
  { MODE_2_0, RELATION_DOLBY2, N_("Dolby ProLogic II") },
  // Custom modes
//  { MODE_1_0,       0, "1/0"     },
//  { MODE_2_0,       0, "2/0"     },
  { MODE_2_1,       0, "2/1"     },
//  { MODE_2_2,       0, "2/2"     },
  { MODE_3_0,       0, "3/0"     },
  { MODE_3_1,       0, "3/1"     },
//  { MODE_3_2,       0, "3/2"     },
//  { MODE_3_2_1,     0, "3/2/1"   },
//  { MODE_3_2_2,     0, "3/2/2"   },
  { MODE_3_0_2,     0, "3/0/2"   },
  { MODE_3_0_3,     0, "3/0/3"   },
  { MODE_5_0,       0, "5/0"     },
  { MODE_5_2,       0, "5/2"     },
  { MODE_5_0_2,     0, "5/0/2"   },
  // Custom with subwoofer
//  { MODE_1_0_LFE,   0, "1/0.1"   },
//  { MODE_2_0_LFE,   0, "2/0.1"   },
  { MODE_2_1_LFE,   0, "2/1.1"   },
//  { MODE_2_2_LFE,   0, "2/2.1"   },
  { MODE_3_0_LFE,   0, "3/0.1"   },
  { MODE_3_1_LFE,   0, "3/1.1"   },
//  { MODE_3_2_LFE,   0, "3/2.1"   },
//  { MODE_3_2_1_LFE, 0, "3/2/1.1" },
//  { MODE_3_2_2_LFE, 0, "3/2/2.1" },
  { MODE_3_0_2_LFE, 0, "3/0/2.1" },
  { MODE_3_0_3_LFE, 0, "3/0/3.1" },
  { MODE_5_0_LFE,   0, "5/0.1"   },
  { MODE_5_2_LFE,   0, "5/2.1"   },
  { MODE_5_0_2_LFE, 0, "5/0/2.1" },
};

static const struct {int format; const char *text; } fmtlist[] = 
{
  { FORMAT_PCM16, N_("PCM 16bit") },
  { FORMAT_PCM24, N_("PCM 24bit") },
  { FORMAT_PCM32, N_("PCM 32bit") },
  { FORMAT_PCMFLOAT,  N_("PCM Float") },
  { FORMAT_PCMDOUBLE, N_("PCM Double") },
};

static const int rates_list[] =
{
  0, 8000, 11025, 22050, 24000, 32000, 44100, 48000, 96000, 192000
};

static Speakers list2spk(LRESULT ispk, LRESULT ifmt, int sample_rate)
{
  int format = FORMAT_PCM16;
  int mask = MODE_STEREO;
  int relation = NO_RELATION;

  if (ispk >= 0 && ispk < array_size(spklist))
  {
    mask = spklist[ispk].mask;
    relation = spklist[ispk].relation;
  }

  if (ifmt >= 0 && ifmt < array_size(fmtlist))
    format = fmtlist[ifmt].format;

  return Speakers(format, mask, sample_rate, -1, relation);
}

static int spk2ispk(Speakers spk)
{
  for (int i = 0; i < array_size(spklist); i++)
    if (spk.mask == spklist[i].mask && spk.relation == spklist[i].relation)
      return i;
  return 0;
}

static int spk2ifmt(Speakers spk)
{
  for (int i = 0; i < array_size(fmtlist); i++)
    if (spk.format == fmtlist[i].format)
      return i;
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
    SendDlgItemMessage(hdlg, IDC_CMB_SPK, CB_ADDSTRING, 0, (LPARAM) gettext(spklist[i].text));

  SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < array_size(fmtlist); i++)
    SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_ADDSTRING, 0, (LPARAM) gettext(fmtlist[i].text));

  SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_ADDSTRING, 0, (LPARAM)_("Do not change"));
  for (i = 1; i < array_size(rates_list); i++)
  {
    LRESULT index = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_ADDSTRING, 0, (LPARAM)itoa(rates_list[i], buf, 10));
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
    LRESULT index = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_FINDSTRINGEXACT, -1, (LPARAM)buf);
    if (index != CB_ERR)
      SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_SETCURSEL, index, 0);
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
        LRESULT ispk = SendDlgItemMessage(hdlg, IDC_CMB_SPK, CB_GETCURSEL, 0, 0);
        LRESULT ifmt = SendDlgItemMessage(hdlg, IDC_CMB_FORMAT, CB_GETCURSEL, 0, 0);
        LRESULT irate = SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_GETCURSEL, 0, 0);
        int sample_rate = (int)SendDlgItemMessage(hdlg, IDC_CMB_RATE, CB_GETITEMDATA, irate, 0);

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
