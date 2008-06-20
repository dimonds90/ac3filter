#pragma warning(disable: 4786)

#include <vector>
#include <string>
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

static const char *package = "ac3filter";

///////////////////////////////////////////////////////////////////////////////

class EnumLanguages
{
protected:
  typedef std::vector<std::string> svector;
  typedef std::vector<std::string>::iterator iter;

  svector codes;
  svector labels;

public:
  EnumLanguages() {}
  EnumLanguages(const char *path, const char *package) { open(path, package); }

  bool open(const char *path, const char *package)
  {
    char file[MAX_PATH + 2];
    sprintf(file, "%s\\*", path);

    codes.clear();
    labels.clear();

    WIN32_FIND_DATA fd;
    HANDLE fh = FindFirstFile(file, &fd);
    if (fh != INVALID_HANDLE_VALUE) do
    {
      int i = lang_index(fd.cFileName);
      if (i == -1) continue;

      char file[MAX_PATH + MAX_PATH + 30];
      sprintf(file, "%s\\%s\\LC_MESSAGES\\%s.mo", path, fd.cFileName, package);
      if (GetFileAttributes(file) == -1) continue;

      i = lang_index(fd.cFileName);
      codes.push_back(fd.cFileName);
      labels.push_back(iso_langs[i].name);

    } while (FindNextFile(fh, &fd));
    return true;
  }

  void close()
  {
    codes.clear();
    labels.clear();
  }

  int nlangs()
  {
    return codes.size();
  }

  const char *code(int i)
  {
    return codes[i].c_str();
  }

  const char *label(int i)
  {
    return labels[i].c_str();
  }

  int find_code(const char *code)
  {
    if (code == 0) return -1;
    std::string c(code);
    for(int i = 0; i < codes.size(); i++)
      if (codes[i] == c)
        return i;
    return -1;
  }
};

///////////////////////////////////////////////////////////////////////////////

ControlLang::ControlLang(HWND _dlg):
Controller(_dlg, ::controls)
{
  langs = 0;
  path[0] = 0; 

#ifdef ENABLE_NLS
  RegistryKey reg(REG_KEY);
  reg.get_text("Lang_Dir", path, sizeof(path));
  langs = new EnumLanguages(path, package);
#endif
}

ControlLang::~ControlLang()
{
  safe_delete(langs);
}

void ControlLang::init()
{
#ifdef ENABLE_NLS
  if (langs) if (langs->nlangs())
  {
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)"--- Original ---");
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETITEMDATA, 0, 0);

    for (int i = 0; i < langs->nlangs(); i++)
    {
      int cb_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)langs->label(i));
      SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETITEMDATA, cb_index, i+1);
    }
    SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETCURSEL, 0, 0);
  }
  else
  {
    EnableWindow(GetDlgItem(hdlg, IDC_CMB_LANG), FALSE);
    SetDlgItemText(hdlg, IDC_EDT_TRANS_INFO, "Cannot find localization files");
  }
#else
  // Disable language selection if NLS is disabled
  EnableWindow(GetDlgItem(hdlg, IDC_CMB_LANG), FALSE);
  SetDlgItemText(hdlg, IDC_EDT_TRANS_INFO, "No localization support.");
#endif
  lnk_translate.link(hdlg, IDC_LNK_TRANSLATE);
}

void ControlLang::update()
{
#ifdef ENABLE_NLS
  if (langs)
  {
    const char *lang = get_lang();
    int i = langs->find_code(lang);
    if (i == -1)
    {
      SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETCURSEL, 0, 0);
      SetDlgItemText(hdlg, IDC_EDT_TRANS_INFO, "");
    }
    else
    {
      SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_SETCURSEL, i+1, 0);

      char info[1024];
      strncpy(info, gettext_meta(), sizeof(info));
      info[sizeof(info)-1] = 0;
      cr2crlf(info, sizeof(info));
      SetDlgItemText(hdlg, IDC_EDT_TRANS_INFO, info);
    }
  }
#endif
}

ControlLang::cmd_result ControlLang::command(int control, int message)
{
  switch (control)
  {
    case IDC_CMB_LANG:
      if (langs && message == CBN_SELENDOK)
      {
        int cb_index, i;
        const char *lang = "";
        cb_index = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_GETCURSEL, 0, 0);
        if (cb_index != CB_ERR)
        {
          i = SendDlgItemMessage(hdlg, IDC_CMB_LANG, CB_GETITEMDATA, cb_index, 0);
          if (i != CB_ERR && i > 0)
            lang = langs->code(i-1);

          set_lang(lang, package, path);
          RegistryKey reg(REG_KEY);
          reg.set_text("Language", lang);
          return cmd_init;
        }
      }
      break;
  }
  return cmd_not_processed;
}
