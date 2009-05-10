#include <stdio.h>
#include "../save_eq.h"
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_preset.h"

static const int controls[] = 
{
  IDC_GRP_PRESET,
  IDC_CMB_PRESET,
  IDC_BTN_PRESET_FILE,
  IDC_BTN_PRESET_SAVE,
  IDC_BTN_PRESET_DELETE,

  IDC_GRP_MATRIX_PRESET,
  IDC_CMB_MATRIX_PRESET,
  IDC_BTN_MATRIX_SAVE,
  IDC_BTN_MATRIX_DELETE,

  IDC_GRP_EQ_PRESET,
  IDC_CMB_EQ_PRESET,
  IDC_BTN_EQ_SAVE,
  IDC_BTN_EQ_DELETE,

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

///////////////////////////////////////////////////////////////////////////////

ControlPreset::ControlPreset(HWND _dlg, IDecoder *_dec, IAudioProcessor *_proc):
Controller(_dlg, ::controls), dec(_dec), proc(_proc)
{
  dec->AddRef();
  proc->AddRef();
}

ControlPreset::~ControlPreset()
{
  dec->Release();
  proc->Release();
}

void ControlPreset::update()
{
  #define fill_combobox(control, registry)                                                \
  {                                                                                       \
    HKEY key;                                                                             \
    char preset[256];                                                                     \
    LRESULT n;                                                                            \
                                                                                          \
    SendDlgItemMessage(hdlg, control, WM_GETTEXT, 256, (LPARAM) preset);                  \
    SendDlgItemMessage(hdlg, control, CB_RESETCONTENT, 0, 0);                             \
                                                                                          \
    if (RegOpenKeyEx(HKEY_CURRENT_USER, registry, 0, KEY_READ, &key) == ERROR_SUCCESS)    \
    {                                                                                     \
      char buf[256];                                                                      \
      int i = 0;                                                                          \
      DWORD len = 256;                                                                    \
      while (RegEnumKeyEx(key, i++, (LPTSTR)buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)      \
      {                                                                                   \
        if (strcmp(buf, "Default"))                                                       \
          SendDlgItemMessage(hdlg, control, CB_ADDSTRING, 0, (LPARAM) buf);               \
        len = 256;                                                                        \
      }                                                                                   \
      RegCloseKey(key);                                                                   \
    }                                                                                     \
                                                                                          \
    n = SendDlgItemMessage(hdlg, control, CB_FINDSTRINGEXACT, 0, (LPARAM) preset);        \
    if (n != CB_ERR)                                                                      \
      SendDlgItemMessage(hdlg, control, CB_SETCURSEL, n, 0);                              \
    SendDlgItemMessage(hdlg, control, WM_SETTEXT, 0, (LPARAM) preset);                    \
  }

  fill_combobox(IDC_CMB_PRESET, REG_KEY_PRESET);
  fill_combobox(IDC_CMB_MATRIX_PRESET, REG_KEY_MATRIX);
  fill_combobox(IDC_CMB_EQ_PRESET, REG_KEY_EQ);
};

ControlPreset::cmd_result ControlPreset::command(int control, int message)
{
  switch (control)
  {
    /////////////////////////////////////
    // Global preset

    case IDC_CMB_PRESET:
      if (message == CBN_SELENDOK)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(hdlg, IDC_CMB_PRESET, CB_GETLBTEXT, SendDlgItemMessage(hdlg, IDC_CMB_PRESET, CB_GETCURSEL, 0, 0), (LPARAM) preset);
        SendDlgItemMessage(hdlg, IDC_CMB_PRESET, WM_SETTEXT, 0, (LPARAM) preset);
        sprintf(buf, REG_KEY_PRESET"\\%s", preset);

        RegistryKey reg(buf);
        dec->load_params(&reg, AC3FILTER_ALL);
        return cmd_update;
      }
      if (message == CB_ENTER)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(hdlg, IDC_CMB_PRESET, WM_GETTEXT, 256, (LPARAM) preset);
        sprintf(buf, REG_KEY_PRESET"\\%s", preset);

        RegistryKey reg;
        reg.create_key(buf);
        dec->save_params(&reg, AC3FILTER_PRESET);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_BTN_PRESET_SAVE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(hdlg, IDC_CMB_PRESET, WM_GETTEXT, 256, (LPARAM) preset);
      sprintf(buf, REG_KEY_PRESET"\\%s", preset);

      RegistryKey reg;
      reg.create_key(buf);
      dec->save_params(&reg, AC3FILTER_PRESET);
      update();
      return cmd_ok;
    }

    case IDC_BTN_PRESET_DELETE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(hdlg, IDC_CMB_PRESET, WM_GETTEXT, 256, (LPARAM) preset);

      sprintf(buf, _("Are you sure you want to delete '%s' preset?"), preset);
      if (MessageBox(hdlg, buf, _("Delete confirmation"), MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {     
        sprintf(buf, REG_KEY_PRESET"\\%s", preset);
        delete_reg_key(buf, HKEY_CURRENT_USER);
        SendDlgItemMessage(hdlg, IDC_CMB_PRESET, WM_SETTEXT, 0, (LPARAM) "");
        update();
      }
      return cmd_ok;
    }
/*
    case IDC_BTN_PRESET_FILE:
    {
      char filename[MAX_PATH];

      if FAILED(dec->get_config_file(filename, MAX_PATH))
        filename[0] = 0;

      FileDlg dlg(hdlg, filename);
      switch (dlg.exec())
      {
      case IDC_BTN_FILE_SAVE:
        if (dlg.preset) dec->save_params(dlg.filename, true);
        if (dlg.matrix) dec->save_matrix(dlg.filename, true);
        if (dlg.delay)  dec->save_delay (dlg.filename, true);
        if (dlg.eq)     dec->save_eq9(dlg.filename, true);
        break;

      case IDC_BTN_FILE_LOAD:
        if (dlg.preset) dec->load_params(dlg.filename, true);
        if (dlg.matrix) dec->load_matrix(dlg.filename, true);
        if (dlg.delay)  dec->load_delay (dlg.filename, true);
        if (dlg.eq)     dec->load_eq9(dlg.filename, true);
        break;
      }
      break;
    }
*/
    /////////////////////////////////////
    // Matrix preset

    case IDC_CMB_MATRIX_PRESET:
      if (message == CBN_SELENDOK)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, CB_GETLBTEXT, SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, CB_GETCURSEL, 0, 0), (LPARAM) preset);
        SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, WM_SETTEXT, 0, (LPARAM) preset);
        sprintf(buf, REG_KEY_MATRIX"\\%s", preset);

        proc->set_auto_matrix(false);
        RegistryKey reg(buf);
        dec->load_params(&reg, AC3FILTER_MATRIX);
        return cmd_update;
      }
      if (message == CB_ENTER)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, WM_GETTEXT, 256, (LPARAM) preset);
        sprintf(buf, REG_KEY_MATRIX"\\%s", preset);

        RegistryKey reg;
        reg.create_key(buf);
        dec->save_params(&reg, AC3FILTER_MATRIX);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_BTN_MATRIX_SAVE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, WM_GETTEXT, 256, (LPARAM) preset);
      sprintf(buf, REG_KEY_MATRIX"\\%s", preset);

      RegistryKey reg;
      reg.create_key(buf);
      dec->save_params(&reg, AC3FILTER_MATRIX);
      update();
      return cmd_ok;
    }

    case IDC_BTN_MATRIX_DELETE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, WM_GETTEXT, 256, (LPARAM) preset);

      sprintf(buf, _("Are you sure you want to delete '%s' matrix?"), preset);
      if (MessageBox(hdlg, buf, _("Delete confirmation"), MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {     
        sprintf(buf, REG_KEY_MATRIX"\\%s", preset);
        delete_reg_key(buf, HKEY_CURRENT_USER);
        SendDlgItemMessage(hdlg, IDC_CMB_MATRIX_PRESET, WM_SETTEXT, 0, (LPARAM) "");
        proc->set_auto_matrix(true);
        update();
      }
      return cmd_ok;
    }

    /////////////////////////////////////
    // Equalizer preset

    case IDC_CMB_EQ_PRESET:
      if (message == CBN_SELENDOK)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, CB_GETLBTEXT, SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, CB_GETCURSEL, 0, 0), (LPARAM) preset);
        SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, WM_SETTEXT, 0, (LPARAM) preset);
        sprintf(buf, REG_KEY_EQ"\\%s", preset);

        RegistryKey reg(buf);
        dec->load_params(&reg, AC3FILTER_EQ_ALL);
        return cmd_update;
      }
      if (message == CB_ENTER)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, WM_GETTEXT, 256, (LPARAM) preset);
        sprintf(buf, REG_KEY_EQ"\\%s", preset);

        RegistryKey reg;
        reg.create_key(buf);
        SaveEq save_eq(AC3FILTER_EQ_CUR);
        if (save_eq.exec(ac3filter_instance, MAKEINTRESOURCE(IDD_EQ_SAVE), hdlg) == IDOK)
        {
          int preset = save_eq.get_preset();
          dec->save_params(&reg, preset);
          update();
        }
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_BTN_EQ_SAVE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, WM_GETTEXT, 256, (LPARAM) preset);
      sprintf(buf, REG_KEY_EQ"\\%s", preset);

      RegistryKey reg;
      reg.create_key(buf);
      SaveEq save_eq(AC3FILTER_EQ_CUR);
      if (save_eq.exec(ac3filter_instance, MAKEINTRESOURCE(IDD_EQ_SAVE), hdlg) == IDOK)
      {
        int preset = save_eq.get_preset();
        dec->save_params(&reg, preset);
        update();
      }
      return cmd_ok;
    }

    case IDC_BTN_EQ_DELETE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, WM_GETTEXT, 256, (LPARAM) preset);

      sprintf(buf, _("Are you sure you want to delete '%s' equalizer preset?"), preset);
      if (MessageBox(hdlg, buf, _("Delete confirmation"), MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {     
        sprintf(buf, REG_KEY_EQ"\\%s", preset);
        delete_reg_key(buf, HKEY_CURRENT_USER);
        SendDlgItemMessage(hdlg, IDC_CMB_EQ_PRESET, WM_SETTEXT, 0, (LPARAM) "");
        update();
      }
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
