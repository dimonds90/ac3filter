#include <streams.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_lang.h"

static int controls[] =
{
  IDC_GRP_TRANS,
  IDC_CMB_LANG,
  IDC_LBL_LANG,
  IDC_EDT_TRANS_INFO,
  IDC_LNK_TRANSLATE,
  0
};

///////////////////////////////////////////////////////////////////////////////

ControlLang::ControlLang(HWND _dlg):
Controller(_dlg, ::controls)
{}

ControlLang::~ControlLang()
{}

void ControlLang::init()
{
# ifdef ENABLE_NLS
  {
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)"--- Original ---");
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETITEMDATA, 0, 0);

    char path[MAX_PATH - 2];
    RegistryKey reg(REG_KEY);
    path[0] = 0; 
    reg.get_text("Lang_Dir", path, sizeof(path));

    if (path[0] != 0)
    {
      char file[MAX_PATH];
      sprintf(file, "%s\\*", path);

      WIN32_FIND_DATA fd;
      HANDLE fh = FindFirstFile(file, &fd);
      if (fh != INVALID_HANDLE_VALUE)
        do
        {
          char file[MAX_PATH + MAX_PATH + 2];
          sprintf(file, "%s\\%s", path, fd.cFileName);
          DWORD attr = GetFileAttributes(file);
          if (attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY))
          { 
            int iso_index = lang_index(fd.cFileName);
            if (iso_index != -1)
            {
              int cb_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)iso_langs[iso_index].name);
              SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETITEMDATA, cb_index, iso_index);
            }
          }
        } while (FindNextFile(fh, &fd));
    }
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETCURSEL, 0, 0);
  }
# else
  {
    // Disable language selection if NLS is disabled
    EnableWindow(GetDlgItem(hdlg, IDC_CMB_LANG), FALSE);
    SetDlgItemText(hdlg, IDC_EDT_TRANS_INFO, "No localization support.");
  }
# endif
  lnk_translate.link(hdlg, IDC_LNK_TRANSLATE);
}

void ControlLang::update()
{
#ifdef ENABLE_NLS
  {
    int current_iso_index = lang_index(get_lang());
    if (current_iso_index != -1)
    {
      int cb_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_GETCOUNT, 0, 0);
      if (cb_index != CB_ERR)
      {
        while (cb_index--)
        {
          int iso_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_GETITEMDATA, cb_index, 0);
          if (iso_index != CB_ERR && iso_index == current_iso_index)
          {
            SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETCURSEL, cb_index, 0);
            break;
          }
        }
      }
    }

    char info[1024];
    strncpy(info, gettext_meta(), sizeof(info));
    info[sizeof(info)-1] = 0;
    cr2crlf(info, sizeof(info));
    SetDlgItemText(hdlg, IDC_EDT_TRANS_INFO, info);
  }
#endif
}

ControlLang::cmd_result ControlLang::command(int control, int message)
{
  switch (control)
  {
    case IDC_CMB_LANG:
      if (message == CBN_SELENDOK)
      {
        int cb_index, iso_index;
        cb_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_GETCURSEL, 0, 0);
        if (cb_index != CB_ERR)
        {
          iso_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_GETITEMDATA, cb_index, 0);
          if (iso_index != CB_ERR)
            set_lang(iso_langs[iso_index].iso6392);

          RegistryKey reg(REG_KEY);
          reg.set_text("Language", iso_langs[iso_index].iso6392);
        }
      }
      return cmd_init;
  }
  return cmd_not_processed;
}
