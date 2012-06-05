#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_spdif.h"

static int controls[] =
{
  IDC_GRP_SPDIF_PT,
  IDC_CHK_SPDIF_MPA,
  IDC_CHK_SPDIF_AC3,
  IDC_CHK_SPDIF_DTS,

  IDC_GRP_DTS_MODE,
  IDC_RBT_DTS_MODE_AUTO,
  IDC_RBT_DTS_MODE_WRAPPED,
  IDC_RBT_DTS_MODE_PADDED,

  IDC_GRP_DTS_CONV,
  IDC_RBT_DTS_CONV_NONE,
  IDC_RBT_DTS_CONV_14BIT,
  IDC_RBT_DTS_CONV_16BIT,

  IDC_GRP_SPDIF,
  IDC_CHK_USE_DETECTOR,
  IDC_CHK_SPDIF_ENCODE,
  IDC_CMB_SPDIF_BITRATE,
  IDC_LBL_SPDIF_BITRATE,
  IDC_CHK_SPDIF_STEREO_PT,
  IDC_CHK_SPDIF_AS_PCM,
  IDC_CHK_SPDIF_CHECK_SR,
  IDC_CHK_SPDIF_ALLOW_48,
  IDC_CHK_SPDIF_ALLOW_44,
  IDC_CHK_SPDIF_ALLOW_32,

  0
};

static const int mask_mpa = FORMAT_MASK_MPA;
static const int mask_dts = FORMAT_MASK_DTS;
static const int mask_ac3 = FORMAT_MASK_AC3 | FORMAT_MASK_EAC3 | FORMAT_MASK_DOLBY;

///////////////////////////////////////////////////////////////////////////////
// Encoder bitrate
///////////////////////////////////////////////////////////////////////////////

static const int bitrate_tbl[19] =
{
   32,  40,  48,  56,  64,  80,  96, 112, 128, 
  160, 192, 224, 256, 320, 384, 448, 512, 576, 640 
};

static int bitrate2list(int bitrate)
{
  bitrate /= 1000;
  for (int i = 0; i < array_size(bitrate_tbl); i++)
    if (bitrate_tbl[i] >= bitrate)
      return i;

  return array_size(bitrate_tbl) - 1;
}

static int list2bitrate(LRESULT index)
{
  if (index >= 0 && index < array_size(bitrate_tbl))
    return bitrate_tbl[index] * 1000;
  else
    return 448000; // default bitrate
}

///////////////////////////////////////////////////////////////////////////////

ControlSPDIF::ControlSPDIF(HWND _dlg, IDecoder *_dec):
Controller(_dlg, ::controls), dec(_dec)
{
  dec->AddRef();
}

ControlSPDIF::~ControlSPDIF()
{
  dec->Release();
}

void ControlSPDIF::init()
{
  char buf[128];
  for (int i = 0; i < array_size(bitrate_tbl); i++)
  {
    LRESULT index = SendDlgItemMessage(hdlg, IDC_CMB_SPDIF_BITRATE, CB_ADDSTRING, 0, (LPARAM) itoa(bitrate_tbl[i], buf, 10));
    SendDlgItemMessage(hdlg, IDC_CMB_SPDIF_BITRATE, CB_SETITEMDATA, index, bitrate_tbl[i]);
  }
}

void ControlSPDIF::update()
{
  dec->get_spdif_pt(&spdif_pt);
  dec->get_spdif_as_pcm(&spdif_as_pcm);
  dec->get_spdif_encode(&spdif_encode);
  dec->get_spdif_stereo_pt(&spdif_stereo_pt);
  dec->get_spdif_bitrate(&spdif_bitrate);

  dec->get_spdif_check_sr(&spdif_check_sr);
  dec->get_spdif_allow_48(&spdif_allow_48);
  dec->get_spdif_allow_44(&spdif_allow_44);
  dec->get_spdif_allow_32(&spdif_allow_32);

  dec->get_dts_mode(&dts_mode);
  dec->get_dts_conv(&dts_conv);

  dec->get_use_detector(&use_detector);

  /////////////////////////////////////
  // SPDIF passthrough

  CheckDlgButton(hdlg, IDC_CHK_SPDIF_MPA, (spdif_pt & mask_mpa) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_AC3, (spdif_pt & mask_ac3) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_DTS, (spdif_pt & mask_dts) != 0? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // SPDIF/DTS output mode

  SendDlgItemMessage(hdlg, IDC_RBT_DTS_MODE_AUTO,    BM_SETCHECK, dts_mode == DTS_MODE_AUTO? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(hdlg, IDC_RBT_DTS_MODE_WRAPPED, BM_SETCHECK, dts_mode == DTS_MODE_WRAPPED? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(hdlg, IDC_RBT_DTS_MODE_PADDED,  BM_SETCHECK, dts_mode == DTS_MODE_PADDED? BST_CHECKED: BST_UNCHECKED, 1);

  /////////////////////////////////////
  // SPDIF/DTS conversion

  SendDlgItemMessage(hdlg, IDC_RBT_DTS_CONV_NONE,    BM_SETCHECK, dts_conv == DTS_CONV_NONE? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(hdlg, IDC_RBT_DTS_CONV_14BIT,   BM_SETCHECK, dts_conv == DTS_CONV_14BIT? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(hdlg, IDC_RBT_DTS_CONV_16BIT,   BM_SETCHECK, dts_conv == DTS_CONV_16BIT? BST_CHECKED: BST_UNCHECKED, 1);

  /////////////////////////////////////
  // SPDIF options

  CheckDlgButton(hdlg, IDC_CHK_SPDIF_AS_PCM, spdif_as_pcm? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_ENCODE, spdif_encode? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_STEREO_PT, spdif_stereo_pt? BST_CHECKED: BST_UNCHECKED);

  SendDlgItemMessage(hdlg, IDC_CMB_SPDIF_BITRATE, CB_SETCURSEL, bitrate2list(spdif_bitrate), 0);

  CheckDlgButton(hdlg, IDC_CHK_SPDIF_CHECK_SR, spdif_check_sr? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_ALLOW_48, spdif_allow_48? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_ALLOW_44, spdif_allow_44? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_ALLOW_32, spdif_allow_32? BST_CHECKED: BST_UNCHECKED);

  EnableWindow(GetDlgItem(hdlg, IDC_CHK_SPDIF_STEREO_PT), spdif_encode);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_SPDIF_ALLOW_48), spdif_check_sr);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_SPDIF_ALLOW_44), spdif_check_sr);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_SPDIF_ALLOW_32), spdif_check_sr);

  CheckDlgButton(hdlg, IDC_CHK_USE_DETECTOR, use_detector? BST_CHECKED: BST_UNCHECKED);

};

ControlSPDIF::cmd_result ControlSPDIF::command(int control, int message)
{
  switch (control)
  {
    /////////////////////////////////////
    // SPDIF passthrough

    case IDC_CHK_SPDIF_MPA:
    case IDC_CHK_SPDIF_AC3:
    case IDC_CHK_SPDIF_DTS:
    {
      spdif_pt = 0;
      spdif_pt |= IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_MPA) == BST_CHECKED? mask_mpa: 0;
      spdif_pt |= IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_AC3) == BST_CHECKED? mask_ac3: 0;
      spdif_pt |= IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_DTS) == BST_CHECKED? mask_dts: 0;
      dec->set_spdif_pt(spdif_pt);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // SPDIF/DTS output mode

    case IDC_RBT_DTS_MODE_AUTO:
    {
      dts_mode = DTS_MODE_AUTO;
      dec->set_dts_mode(dts_mode);
      update();
      return cmd_ok;
    }

    case IDC_RBT_DTS_MODE_WRAPPED:
    {
      dts_mode = DTS_MODE_WRAPPED;
      dec->set_dts_mode(dts_mode);
      update();
      return cmd_ok;
    }

    case IDC_RBT_DTS_MODE_PADDED:
    {
      dts_mode = DTS_MODE_PADDED;
      dec->set_dts_mode(dts_mode);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // SPDIF/DTS conversion

    case IDC_RBT_DTS_CONV_NONE:
    {
      dts_conv = DTS_CONV_NONE;
      dec->set_dts_conv(dts_conv);
      update();
      return cmd_ok;
    }

    case IDC_RBT_DTS_CONV_14BIT:
    {
      dts_conv = DTS_CONV_14BIT;
      dec->set_dts_conv(dts_conv);
      update();
      return cmd_ok;
    }

    case IDC_RBT_DTS_CONV_16BIT:
    {
      dts_conv = DTS_CONV_16BIT;
      dec->set_dts_conv(dts_conv);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // SPDIF options

    case IDC_CHK_USE_DETECTOR:
    {
      use_detector = IsDlgButtonChecked(hdlg, IDC_CHK_USE_DETECTOR) == BST_CHECKED;
      dec->set_use_detector(use_detector);
      return cmd_ok;
    }

    case IDC_CHK_SPDIF_AS_PCM:
    {
      spdif_as_pcm = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_AS_PCM) == BST_CHECKED;
      if (spdif_as_pcm)
        spdif_as_pcm = MessageBox(hdlg, _("This option is DANGEROUS! Filter may make very loud noise with this option enabled. Press 'No' to enable this option."), _("Dangerous option!"), MB_YESNO | MB_ICONWARNING) == IDNO;
      dec->set_spdif_as_pcm(spdif_as_pcm);
      update();
      return cmd_ok;
    }

    case IDC_CHK_SPDIF_ENCODE:
    {
      spdif_encode = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_ENCODE) == BST_CHECKED;
      dec->set_spdif_encode(spdif_encode);
      update();
      return cmd_ok;
    }

    case IDC_CHK_SPDIF_STEREO_PT:
    {
      spdif_stereo_pt = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_STEREO_PT) == BST_CHECKED;
      dec->set_spdif_stereo_pt(spdif_stereo_pt);
      update();
      return cmd_ok;
    }

    case IDC_CMB_SPDIF_BITRATE:
      if (message == CBN_SELENDOK)
      {
        LRESULT ibitrate = SendDlgItemMessage(hdlg, IDC_CMB_SPDIF_BITRATE, CB_GETCURSEL, 0, 0);
        if (ibitrate != CB_ERR)
        {
          spdif_bitrate = list2bitrate(ibitrate);
          dec->set_spdif_bitrate(spdif_bitrate);
        }
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_CHK_SPDIF_CHECK_SR:
    {
      spdif_check_sr = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_CHECK_SR) == BST_CHECKED;
      dec->set_spdif_check_sr(spdif_check_sr);
      update();
      return cmd_ok;
    }

    case IDC_CHK_SPDIF_ALLOW_48:
    {
      spdif_allow_48 = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_ALLOW_48) == BST_CHECKED;
      dec->set_spdif_allow_48(spdif_allow_48);
      update();
      return cmd_ok;
    }

    case IDC_CHK_SPDIF_ALLOW_44:
    {
      spdif_allow_44 = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_ALLOW_44) == BST_CHECKED;
      dec->set_spdif_allow_44(spdif_allow_44);
      update();
      return cmd_ok;
    }

    case IDC_CHK_SPDIF_ALLOW_32:
    {
      spdif_allow_32 = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_ALLOW_32) == BST_CHECKED;
      dec->set_spdif_allow_32(spdif_allow_32);
      update();
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
