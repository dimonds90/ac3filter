#include <windows.h>
#include <commctrl.h>
#include <streams.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_system.h"

static int controls[] =
{
  IDC_GRP_FORMATS,
  IDC_LST_FORMATS,

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

static struct
{
  int format_mask;
  const char *name;
} format_list[] =
{
  // PCM formats
  { FORMAT_CLASS_PCM,  "PCM" },
  { FORMAT_CLASS_LPCM, "LPCM" },

  // Lossy formats
  { FORMAT_MASK_MPA,   "MPEG Audio" },
  { FORMAT_MASK(FORMAT_DOLBY), "AC3" },
  { FORMAT_MASK(FORMAT_AAC_FRAME), "AAC" },
  { FORMAT_MASK_DTS,   "DTS" },
  { FORMAT_MASK_VORBIS,"Vorbis" },

  // Lossless formats
  { FORMAT_MASK_FLAC,  "Flac" },
  { FORMAT_MASK_TRUEHD,"TrueHD" },

  // Containers
  { FORMAT_MASK_PES,   "MPEG PES" },
  { FORMAT_MASK_SPDIF, "SPDIF" },
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

static DWORD get_merit(HKEY hive, LPCSTR key)
{
  HKEY reg;
  DWORD data_size;
  DWORD type;
  DWORD merit = 0;

  if (RegOpenKeyEx(hive, key, 0, KEY_READ, &reg) != ERROR_SUCCESS)
    return 0;

  if (RegQueryValueEx(reg, "FilterData", NULL, &type, NULL, &data_size) == ERROR_SUCCESS &&
      type == REG_BINARY && data_size >= sizeof(REGFILTER2))
  {
    BYTE *data = new BYTE[data_size];
    if (RegQueryValueEx(reg, "FilterData", NULL, &type, data, &data_size) == ERROR_SUCCESS &&
        type == REG_BINARY && data_size >= sizeof(REGFILTER2))
      merit = ((REGFILTER2 *)data)->dwMerit;
    delete data;
  }

  RegCloseKey(reg);
  return merit;
}


static bool set_merit(HKEY hive, LPCSTR key, DWORD merit)
{
  HKEY reg;
  DWORD data_size;
  DWORD type;
  bool result = false;

  if (RegOpenKeyEx(hive, key, 0, KEY_READ | KEY_WRITE, &reg) != ERROR_SUCCESS)
    return 0;

  if (RegQueryValueEx(reg, "FilterData", NULL, &type, NULL, &data_size) == ERROR_SUCCESS &&
      type == REG_BINARY && data_size >= sizeof(REGFILTER2))
  {
    BYTE *data = new BYTE[data_size];
    if (RegQueryValueEx(reg, "FilterData", NULL, &type, data, &data_size) == ERROR_SUCCESS &&
        type == REG_BINARY && data_size >= sizeof(REGFILTER2))
    {
      ((REGFILTER2 *)data)->dwMerit = merit;
      if (RegSetValueEx(reg, "FilterData", NULL, REG_BINARY, data, data_size) == ERROR_SUCCESS)
        result = true;
    }
    delete data;
  }

  RegCloseKey(reg);
  return result;
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

void ControlSystem::init()
{
  /////////////////////////////////////////////////////////
  // List of formats

  HWND formats_hwnd = GetDlgItem(hdlg, IDC_LST_FORMATS);
  DWORD style = ListView_GetExtendedListViewStyle(formats_hwnd);
  style |= LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT;
  ListView_SetExtendedListViewStyle(formats_hwnd, style);

  LVITEM item;
  item.iItem = 0;
  item.iSubItem = 0;
  item.mask = LVIF_TEXT | LVIF_PARAM;
  item.state = 0;
  item.stateMask = 0;
  for (int i = 0; i < array_size(format_list); i++)
  {
    item.iItem = i + 1;
    item.pszText = (LPSTR)format_list[i].name;
    item.lParam = format_list[i].format_mask;
    ListView_InsertItem(formats_hwnd, &item);
  }

  // Set background color
  ListView_SetBkColor(formats_hwnd, GetSysColor(CTLCOLOR_DLG));
  ListView_SetTextBkColor(formats_hwnd, GetSysColor(CTLCOLOR_DLG));

  // Create main column
  RECT rect;
  GetClientRect(formats_hwnd, &rect);

  LVCOLUMN col;
  col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
  col.iSubItem = 0;
  col.pszText = "Format";
  col.cx = rect.right - rect.top;
  col.fmt = LVCFMT_LEFT;
  ListView_InsertColumn(formats_hwnd, 0, &col);
}

void ControlSystem::update()
{
  dec->get_formats(&formats);
  filter->get_reinit(&reinit);
  filter->get_spdif_no_pcm(&spdif_no_pcm);
  dec->get_query_sink(&query_sink);

  /////////////////////////////////////
  // Formats

  LVITEM item;
  HWND formats_hwnd = GetDlgItem(hdlg, IDC_LST_FORMATS);
  int count = ListView_GetItemCount(formats_hwnd);
  for (int i = 0; i < count; i++)
  {
    item.iItem = i;
    item.iSubItem = 0;
    item.mask = LVIF_PARAM;
    if (ListView_GetItem(formats_hwnd, &item))
    {
      int mask = (int)item.lParam;
      ListView_SetCheckState(formats_hwnd, i, (formats & mask) == mask);
    }
  }

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

ControlSystem::cmd_result
ControlSystem::notify(int control, int message, LPNMHDR nmhdr, INT_PTR &result)
{
  switch (control)
  {
    /////////////////////////////////////
    // Formats

    case IDC_LST_FORMATS:
      if (message == LVN_ITEMCHANGED)
      {
        LPNMLISTVIEW nmlistview = (LPNMLISTVIEW)nmhdr;
        if ((nmlistview->uChanged & LVIF_STATE) == 0 || // State did not change
            (nmlistview->uOldState & LVIS_STATEIMAGEMASK) == 0 || // Item initialize
            (nmlistview->uOldState & LVIS_STATEIMAGEMASK) == (nmlistview->uNewState & LVIS_STATEIMAGEMASK)) // Check state did not change
          return cmd_not_processed;

        LVITEM item;
        HWND formats_hwnd = GetDlgItem(hdlg, IDC_LST_FORMATS);
        item.iItem = nmlistview->iItem;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        if (!ListView_GetItem(formats_hwnd, &item))
          return cmd_not_processed;

        int mask = (int)item.lParam;
        bool checked = ListView_GetCheckState(formats_hwnd, nmlistview->iItem) == TRUE;
        if (checked && (formats & mask) != mask)
        {
          formats |= mask;
          dec->set_formats(formats);
        }
        if (!checked && (formats & mask) != 0)
        {
          formats &= ~mask;
          dec->set_formats(formats);
        }
        return cmd_ok;
      }
      return cmd_not_processed;
  }
  return cmd_not_processed;
}
