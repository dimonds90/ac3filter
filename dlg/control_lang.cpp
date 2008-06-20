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
      // Parse the file name into language, country and custom text:
      //   pt_BR -> "Purtuguese (Brazil)"
      //   zh@Simplified -> "Chineese (Simplified)"

      int lang = -1;
      int country = -1;
      const char *custom = 0;
      const char *fn = fd.cFileName;

      lang = lang_index(fn);
      if (lang == -1)
      {
        int len = strlen(fn);
        char ll[3] = { 0, 0, 0 };
        char cc[3] = { 0, 0, 0 };

        if (len >= 2 && fn[2] == '@')
        {
          /////////////////////////////////////////////////
          // ll@custom

          if (len > 2)
            if (fn[2] == '@')
              custom = fn + 3;
            else
              continue;

          ll[0] = fn[0]; ll[1] = fn[1];
          lang = lang_index(ll);
          if (lang == -1)
            continue;
        }
        else if (len >= 5 && fn[2] == '_')
        {
          /////////////////////////////////////////////////
          // ll_cc@custom

          if (len > 5)
            if (fn[5] == '@')
              custom = fn + 6;
            else
              continue;

          ll[0] = fn[0]; ll[1] = fn[1];
          cc[0] = fn[3]; cc[1] = fn[4];
          lang = lang_index(ll);
          country = country_index(cc);
          if (lang == -1 || country == -1)
            continue;
        }
        else
          continue;
      }

      // Ensure that translation data exists
      char file[MAX_PATH + MAX_PATH + 30];
      sprintf(file, "%s\\%s\\LC_MESSAGES\\%s.mo", path, fd.cFileName, package);
      if (GetFileAttributes(file) == -1) continue;

      // Fill the array
      std::string label(lang_name(lang));
      if (country != -1 && custom != 0)
        label = label + " (" + country_name(country) + ", " + custom + ")";
      else if (country != -1)
        label = label + " (" + country_name(country) + ")";
      else if (custom != 0)
        label = label + " (" + custom + ")";

      codes.push_back(fn);
      labels.push_back(label);

    } while (FindNextFile(fh, &fd));
    FindClose(fh);
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
  if (langs && langs->nlangs())
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
