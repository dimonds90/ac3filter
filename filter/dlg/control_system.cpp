#include <streams.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_system.h"

static int controls[] =
{
  IDC_GRP_FORMATS,
  IDC_CHK_PCM,
  IDC_CHK_LPCM,
  IDC_CHK_AAC,
  IDC_CHK_AC3,
  IDC_CHK_DTS,
  IDC_CHK_FLAC,
  IDC_CHK_MPA,
  IDC_CHK_PES,
  IDC_CHK_SPDIF,

  IDC_GRP_MERIT,
  IDC_RBT_MERIT_PREFERRED,
  IDC_RBT_MERIT_UNLIKELY,
  IDC_RBT_MERIT_DO_NOT_USE,

  IDC_GRP_RENDERER,
  IDC_RBT_RENDER_DS,
  IDC_RBT_RENDER_WO,

  IDC_GRP_DSHOW,
  IDC_CHK_REINIT,
  IDC_CHK_QUERY_SINK,
  IDC_CHK_SPDIF_NO_PCM,

  0
};

///////////////////////////////////////////////////////////////////////////////
// Registry functions
///////////////////////////////////////////////////////////////////////////////

static bool delete_reg_key(const char *name, HKEY root)
{
  HKEY  key;
  char  buf[256];
  DWORD len;

  if (RegOpenKeyEx(root, name, 0, KEY_READ | KEY_WRITE, &key) != ERROR_SUCCESS)
    return false;

  len = 256;
  while (RegEnumKeyEx(key, 0, (LPTSTR)buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)
  {
    len = 256;
    if (!delete_reg_key(buf, key))
    {
      RegCloseKey(key);
      return false;
    }
  }

  len = 256;
  while (RegEnumValue(key, 0, (LPTSTR)buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)
  {
    len = 256;
    if (RegDeleteValue(key, buf) != ERROR_SUCCESS)
    {
      RegCloseKey(key);
      return false;
    }
  }

  RegCloseKey(key);
  RegDeleteKey(root, name);
  return true;
}

static int get_merit(HKEY hive, LPCSTR key)
{
  HKEY  reg;
  if (RegOpenKeyEx(hive, key, 0, KEY_READ, &reg) != ERROR_SUCCESS)
    return 0;
  
  DWORD buf[256];
  DWORD buf_len = 1024;
  DWORD type;

  if (RegQueryValueEx(reg, "FilterData", NULL, &type, (LPBYTE)&buf, &buf_len) != ERROR_SUCCESS)
  {
    RegCloseKey(reg);
    return 0;
  }

  RegCloseKey(reg);

  if (type != REG_BINARY || buf_len < sizeof(REGFILTER2) || buf[0] != 2)
    return 0;

  return buf[1];
}


static bool set_merit(HKEY hive, LPCSTR key, int merit)
{
  HKEY  reg;
  if (RegOpenKeyEx(hive, key, 0, KEY_READ | KEY_WRITE, &reg) != ERROR_SUCCESS)
    return false;
  
  DWORD buf[256];
  DWORD buf_len = 1024;
  DWORD type;

  if (RegQueryValueEx(reg, "FilterData", NULL, &type, (LPBYTE)&buf, &buf_len) != ERROR_SUCCESS)
  {
    RegCloseKey(reg);
    return false;
  }

  if (type != REG_BINARY || buf_len < sizeof(REGFILTER2) || buf[0] != 2)
  {
    RegCloseKey(reg);
    return false;
  }

  buf[1] = merit;

  if (RegSetValueEx(reg, "FilterData", NULL, REG_BINARY, (LPBYTE)&buf, buf_len) != ERROR_SUCCESS)
    return false;

  RegCloseKey(reg);
  return true;
}


///////////////////////////////////////////////////////////////////////////////

ControlSystem::ControlSystem(HWND _dlg, IAC3Filter *_filter, IDecoder *_dec):
Controller(_dlg, ::controls), filter(_filter), dec(_dec)
{
  filter->AddRef();
  dec->AddRef();
}

ControlSystem::~ControlSystem()
{
  filter->Release();
  dec->Release();
}

void ControlSystem::update()
{
  dec->get_formats(&formats);
  filter->get_reinit(&reinit);
  filter->get_spdif_no_pcm(&spdif_no_pcm);
  dec->get_query_sink(&query_sink);

  /////////////////////////////////////
  // Formats

  CheckDlgButton(hdlg, IDC_CHK_PCM,   (formats & FORMAT_CLASS_PCM_LE) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_LPCM,  (formats & FORMAT_CLASS_LPCM) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_MPA,   (formats & FORMAT_MASK_MPA) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_AAC,   (formats & FORMAT_MASK(FORMAT_AAC_FRAME)) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_AC3,   (formats & FORMAT_MASK(FORMAT_AC3_EAC3)) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_DTS,   (formats & FORMAT_MASK_DTS) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_FLAC,  (formats & FORMAT_MASK_FLAC) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_PES,   (formats & FORMAT_MASK_PES) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF, (formats & FORMAT_MASK_SPDIF) != 0? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // DirectShow

  CheckDlgButton(hdlg, IDC_CHK_QUERY_SINK, query_sink? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_REINIT, reinit > 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_SPDIF_NO_PCM, spdif_no_pcm? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // Merit

  int filter_merit = get_merit(HKEY_CLASSES_ROOT, "CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\{A753A1EC-973E-4718-AF8E-A3F554D45C44}");

  // Win2K/WinXP
  int ds_merit = get_merit(HKEY_CURRENT_USER, "Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default DirectSound Device"); 
  int wo_merit = get_merit(HKEY_CURRENT_USER, "Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default WaveOut Device");
  // Win9x case
  if (!ds_merit) ds_merit = get_merit(HKEY_CLASSES_ROOT, "{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default DirectSound Device");
  if (!wo_merit) wo_merit = get_merit(HKEY_CLASSES_ROOT, "{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default WaveOut Device");

  if (filter_merit)
  {
    SendDlgItemMessage(hdlg, IDC_RBT_MERIT_PREFERRED,  BM_SETCHECK, filter_merit >  MERIT_NORMAL? BST_CHECKED: BST_UNCHECKED, 1);
    SendDlgItemMessage(hdlg, IDC_RBT_MERIT_UNLIKELY,   BM_SETCHECK, filter_merit <= MERIT_NORMAL && filter_merit > MERIT_DO_NOT_USE? BST_CHECKED: BST_UNCHECKED, 1);
    SendDlgItemMessage(hdlg, IDC_RBT_MERIT_DO_NOT_USE, BM_SETCHECK, filter_merit <= MERIT_DO_NOT_USE? BST_CHECKED: BST_UNCHECKED, 1);
  }
    
  if (wo_merit && ds_merit)
  {
    SendDlgItemMessage(hdlg, IDC_RBT_RENDER_DS, BM_SETCHECK, ds_merit > wo_merit? BST_CHECKED: BST_UNCHECKED, 1);
    SendDlgItemMessage(hdlg, IDC_RBT_RENDER_WO, BM_SETCHECK, ds_merit < wo_merit? BST_CHECKED: BST_UNCHECKED, 1);
  }
};

ControlSystem::cmd_result ControlSystem::command(int control, int message)
{
  switch (control)
  {
    /////////////////////////////////////
    // Formats

    case IDC_CHK_PCM:
    case IDC_CHK_LPCM:
    case IDC_CHK_MPA:
    case IDC_CHK_AAC:
    case IDC_CHK_AC3:
    case IDC_CHK_DTS:
    case IDC_CHK_FLAC:
    case IDC_CHK_PES:
    case IDC_CHK_SPDIF:
    {
      formats = 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_PCM)   == BST_CHECKED? FORMAT_CLASS_PCM: 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_LPCM)  == BST_CHECKED? FORMAT_CLASS_LPCM | FORMAT_MASK_PCM16_BE: 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_MPA)   == BST_CHECKED? FORMAT_MASK_MPA: 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_AAC)   == BST_CHECKED? FORMAT_MASK(FORMAT_AAC_FRAME): 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_AC3)   == BST_CHECKED? FORMAT_MASK(FORMAT_AC3_EAC3): 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_DTS)   == BST_CHECKED? FORMAT_MASK_DTS: 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_FLAC)  == BST_CHECKED? FORMAT_MASK_FLAC: 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_PES)   == BST_CHECKED? FORMAT_MASK_PES: 0;
      formats |= IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF) == BST_CHECKED? FORMAT_MASK_SPDIF: 0;
      dec->set_formats(formats);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // Query sink

    case IDC_CHK_QUERY_SINK:
    {
      query_sink = IsDlgButtonChecked(hdlg, IDC_CHK_QUERY_SINK) == BST_CHECKED;
      dec->set_query_sink(query_sink);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // Force sound card reinit

    case IDC_CHK_REINIT:
    {
      reinit = IsDlgButtonChecked(hdlg, IDC_CHK_REINIT) == BST_CHECKED? 128: 0;
      if (reinit)
        reinit = MessageBox(hdlg, _("This option is DANGEROUS! Video may go out of sync with this option enabled. Press 'No' to enable this option."), _("Dangerous option!"), MB_YESNO | MB_ICONWARNING) == IDNO;
      filter->set_reinit(reinit);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // Disallow PCM in SPDIF mode

    case IDC_CHK_SPDIF_NO_PCM:
    {
      spdif_no_pcm = IsDlgButtonChecked(hdlg, IDC_CHK_SPDIF_NO_PCM) == BST_CHECKED;
      filter->set_spdif_no_pcm(spdif_no_pcm);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // Default renderer

    case IDC_RBT_RENDER_DS:
      if (message == BN_CLICKED)
      {
        // DirectSound
        set_merit(HKEY_CLASSES_ROOT, "{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default DirectSound Device", 0x800000);
        set_merit(HKEY_CURRENT_USER, "Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default DirectSound Device", 0x800000); 
        // WaveOut
        set_merit(HKEY_CLASSES_ROOT, "{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default WaveOut Device", 0x200000);
        set_merit(HKEY_CURRENT_USER, "Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default WaveOut Device", 0x200000);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_RBT_RENDER_WO:
      if (message == BN_CLICKED)
      {
        // DirectSound
        set_merit(HKEY_CLASSES_ROOT, "{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default DirectSound Device", 0x200000);
        set_merit(HKEY_CURRENT_USER, "Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default DirectSound Device", 0x200000); 
        // WaveOut
        set_merit(HKEY_CLASSES_ROOT, "{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default WaveOut Device", 0x800000);
        set_merit(HKEY_CURRENT_USER, "Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default WaveOut Device", 0x800000);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    /////////////////////////////////////
    // Merit

    case IDC_RBT_MERIT_PREFERRED:
      if (message == BN_CLICKED)
      {
        set_merit(HKEY_CLASSES_ROOT, "CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\{A753A1EC-973E-4718-AF8E-A3F554D45C44}", 0x10000000);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_RBT_MERIT_UNLIKELY:
      if (message == BN_CLICKED)
      {
        set_merit(HKEY_CLASSES_ROOT, "CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\{A753A1EC-973E-4718-AF8E-A3F554D45C44}", MERIT_UNLIKELY);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_RBT_MERIT_DO_NOT_USE:
      if (message == BN_CLICKED)
      {
        set_merit(HKEY_CLASSES_ROOT, "CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\{A753A1EC-973E-4718-AF8E-A3F554D45C44}", MERIT_DO_NOT_USE);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
        return cmd_ok;
      }
      return cmd_not_processed;
  }
  return cmd_not_processed;
}
