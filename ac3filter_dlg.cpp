#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#include "guids.h"
#include "resource_ids.h"
#include "ac3filter_intl.h"
#include "dialog_controls.h"

#include "ac3filter_dlg.h"
#include "dlg/control_all.h"
#include "dlg/control_cpu.h"



#define SAFE_RELEASE(p) { if (p) p->Release(); p = 0; }

#define dlg_printf(dlg, ctrl, format, params)                     \
{                                                                 \
  char buf[255];                                                  \
  sprintf(buf, format, ##params);                                 \
  SendDlgItemMessage(dlg, ctrl, WM_SETTEXT, 0, (LONG)(LPSTR)buf); \
}



///////////////////////////////////////////////////////////////////////////////
// Initialization / Deinitialization
///////////////////////////////////////////////////////////////////////////////

CUnknown * WINAPI AC3FilterDlg::CreateMain(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Main */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Main page", lpunk, phr, IDD_MAIN, N_("IDD_MAIN"), "Main");
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateMixer(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Mixer */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Mixer page", lpunk, phr, IDD_MIXER, N_("IDD_MIXER"), "Mixer");
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateGains(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Gains */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Gains page", lpunk, phr, IDD_GAINS, N_("IDD_GAINS"), "Gains");
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateSPDIF(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: SPDIF */
  CUnknown *punk = new AC3FilterDlg("AC3Filter SPDIF page", lpunk, phr, IDD_SPDIF, N_("IDD_SPDIF"), "SPDIF");
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateSystem(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: System */
  CUnknown *punk = new AC3FilterDlg("AC3Filter System page", lpunk, phr, IDD_SYSTEM, N_("IDD_SYSTEM"), "System");
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}

CUnknown * WINAPI AC3FilterDlg::CreateAbout(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: About */
  CUnknown *punk = new AC3FilterDlg("AC3Filter About property page", lpunk, phr, IDD_ABOUT, N_("IDD_ABOUT"), "About");
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}

AC3FilterDlg::AC3FilterDlg(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr, int DialogId, const char *title_id, const char *title_def)
:CBasePropertyPage(pName, pUnk, DialogId, 0)
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::AC3FilterDlg(%s)", pName));

  memset(lang, 0, sizeof(lang));
  get_lang(lang, sizeof(lang));
  set_lang(lang);

  title = gettext_id(title_id, title_def);
  filter = 0;
  proc   = 0;
  InitCommonControls();
}

STDMETHODIMP
AC3FilterDlg::GetPageInfo(LPPROPPAGEINFO pPageInfo)
{
  CheckPointer(pPageInfo,E_POINTER);
  WCHAR wszTitle[STR_MAX_LENGTH];
  MultiByteToWideChar(CP_ACP, 0, title, -1, wszTitle, STR_MAX_LENGTH);

  // Allocate dynamic memory for the property page title
  LPOLESTR pszTitle;
  HRESULT hr = AMGetWideString(wszTitle, &pszTitle);
  if (FAILED(hr)) {
      NOTE("No caption memory");
      return hr;
  }

  pPageInfo->cb               = sizeof(PROPPAGEINFO);
  pPageInfo->pszTitle         = pszTitle;
  pPageInfo->pszDocString     = NULL;
  pPageInfo->pszHelpFile      = NULL;
  pPageInfo->dwHelpContext    = 0;

  // Set defaults in case GetDialogSize fails
  pPageInfo->size.cx          = 340;
  pPageInfo->size.cy          = 150;

  GetDialogSize(m_DialogId, DialogProc, 0L, &pPageInfo->size);
  return NOERROR;
}

HRESULT 
AC3FilterDlg::OnConnect(IUnknown *pUnknown)
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::OnConnect()"));

  pUnknown->QueryInterface(IID_IAC3Filter, (void **)&filter);
  pUnknown->QueryInterface(IID_IDecoder, (void **)&dec);
  pUnknown->QueryInterface(IID_IAudioProcessor, (void **)&proc);
  if (!filter || !dec || !proc)
  {
    DbgLog((LOG_TRACE, 3, "AC3FilterDlg::OnConnect() Failed!"));
    SAFE_RELEASE(filter);
    SAFE_RELEASE(dec);
    SAFE_RELEASE(proc);
    return E_NOINTERFACE; 
  }

  refresh_time = 100;
  RegistryKey reg(REG_KEY);
  reg.get_int32("refresh", refresh_time);

  return NOERROR;
}

HRESULT 
AC3FilterDlg::OnDisconnect()
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::OnDisconnect()"));

  if (filter)
    dec->save_params(0, AC3FILTER_ALL);

  SAFE_RELEASE(filter);
  SAFE_RELEASE(dec);
  SAFE_RELEASE(proc);

  return NOERROR;
}

HRESULT 
AC3FilterDlg::OnActivate()
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::OnActivate()"));

  visible = true;
  refresh = true;

  // select language
  memset(lang, 0, sizeof(lang));
  get_lang(lang, sizeof(lang));
  set_lang(lang);

  // Init controllers
  invert_levels = get_invert_levels();
  ctrl = new ControlAll(m_Dlg, filter, dec, proc, invert_levels);
  cpu = new ControlCPU(m_Dlg, filter, invert_levels);

  // Init and update controls
  init();
  update();

  SetTimer(m_hwnd, 1, get_refresh_time(), 0);  // for all dynamic controls
  SetTimer(m_hwnd, 2, 1000, 0); // for CPU usage (should be averaged)

  return NOERROR;
}

HRESULT 
AC3FilterDlg::OnDeactivate()
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::OnDeactivate()"));

  KillTimer(m_hwnd, 1);
  KillTimer(m_hwnd, 2);

  // Destroy controllers
  delete ctrl;
  delete cpu;

  return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////

bool
AC3FilterDlg::get_tooltips()
{
  bool result = true;
  RegistryKey reg(REG_KEY);
  reg.get_bool("tooltips", result);
  return result;
}

void
AC3FilterDlg::set_tooltips(bool _tooltips)
{
  RegistryKey reg(REG_KEY);
  reg.set_bool("tooltips", _tooltips);
}

bool
AC3FilterDlg::get_invert_levels()
{
  bool result = false;
  RegistryKey reg(REG_KEY);
  reg.get_bool("invert_levels", result);
  return result;
}

void
AC3FilterDlg::set_invert_levels(bool _invert_levels)
{
  RegistryKey reg(REG_KEY);
  reg.set_bool("invert_levels", _invert_levels);
}

int
AC3FilterDlg::get_refresh_time()
{
  int result = 100;
  RegistryKey reg(REG_KEY);
  reg.get_int32("refresh_time", result);
  return result;
}

void
AC3FilterDlg::set_refresh_time(int _refresh_time)
{
  RegistryKey reg(REG_KEY);
  reg.set_int32("refresh_time", _refresh_time);
  SetTimer(m_hwnd, 1, refresh_time, 0);
}

///////////////////////////////////////////////////////////////////////////////
// Translation
///////////////////////////////////////////////////////////////////////////////

bool
AC3FilterDlg::set_lang(const char *_lang)
{
  // set_lang(0) or set_lang("") cancels translation

  if (!_lang)
    _lang = "";

  if (find_iso6392(_lang) != -1)
  {
    char path[MAX_PATH];
    RegistryKey reg(REG_KEY);
    reg.set_text("Language", _lang);
    reg.get_text("Lang_Dir", path, sizeof(path));

    // do not use localization if language repository does not exists
    DWORD attr = GetFileAttributes(path);
    if (attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY))
    {
      ::set_lang(_lang, "ac3filter", path);
      strncpy(lang, _lang, sizeof(lang));
      return true;
    }
    else
      return false;
  }
  if (_lang[0] == 0)
  {
    RegistryKey reg(REG_KEY);
    reg.set_text("Language", "");
    ::set_lang("");
    lang[0] = 0;
    return true;
  }
  return false;
}

void
AC3FilterDlg::get_lang(char *_buf, size_t _size)
{
  if (_buf && _size)
  {
    _buf[0] = 0;
    RegistryKey reg(REG_KEY);
    reg.get_text("Language", _buf, _size);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Handle messages
///////////////////////////////////////////////////////////////////////////////

BOOL 
AC3FilterDlg::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_COMMAND:
      command(LOWORD(wParam), HIWORD(wParam));
      return 1;

    case WM_HSCROLL:
    case WM_VSCROLL:
      command(GetDlgCtrlID((HWND)lParam), LOWORD(wParam));
      return 1;

    case WM_TIMER:
      /////////////////////////////////////////////////////
      // For some reasons OlePropertyFrame does not send
      // WM_SHOW message to property pages. But we need it
      // to update our ststic controls that may be changed
      // at other pages. Therefore we have to detect show
      // event based on window visibility. Disadvantage of
      // this method is that user can see control updates.
      //
      // Note: WM_SHOW message is not sent when window has
      // WS_MAXIMIZEBOX style. For some reasons dialog page
      // is wrapped in one more dialog page as following:
      //
      //   Dialog tab set
      //     Wrapper page (with WS_MAXIMIZEBOX style)
      //       Page 1
      //     Wrapper page (with WS_MAXIMIZEBOX style)
      //       Page 2
      //     ...

      if (IsWindowVisible(hwnd))
        if (visible)
        {
          tooltip_ctl.track();

          // normal update
          switch (wParam)
          {
            case 1:
              reload_state();
              if (in_spk != old_in_spk)
              {
                update_static_controls();
                old_in_spk = in_spk;
              }
              update_dynamic_controls();
              break;

            case 2:
              cpu->update_dynamic();
              break;
          }
        }
        else
        {
          // show window
          refresh = true;
          visible = true;

          // translate if language was changed
          // do not do this all time because this update is visible!
          char lang_temp[256];
          memset(lang_temp, 0, sizeof(lang));
          get_lang(lang_temp, sizeof(lang));
          if (strcmp(lang_temp, lang))
          {
            memcpy(lang, lang_temp, sizeof(lang));
            init();
          }

          // update interface
          SetTimer(m_hwnd, 1, get_refresh_time(), 0);
          
          // update all controls
          // (static controls may be changed at other windows)
          update();
        }
      else
        // hide
        visible = false;

      return 1;

  }

  return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// Controls initalization/update
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Top-level

void 
AC3FilterDlg::init()
{
  translate_controls();
  init_controls();
  init_tooltips();
}

void 
AC3FilterDlg::update()
{
  reload_state();
  update_dynamic_controls();
  update_static_controls();
}

///////////////////////////////////////////////////////////////////////////////
// Underground

void 
AC3FilterDlg::reload_state()
{
  dec->get_in_spk(&in_spk);

  // interface
  filter->get_tray(&tray);
  tooltips = get_tooltips();
  invert_levels = get_invert_levels();
  refresh_time = get_refresh_time();

  vtime_t time;
  filter->get_playback_time(&time);
  proc->get_state(this, time);
}

void 
AC3FilterDlg::init_controls()
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::init_controls()"));

  /////////////////////////////////////
  // Init controllers

  ctrl->init();
  cpu->init();

  /////////////////////////////////////
  // Interface

  edt_refresh_time.link(m_Dlg, IDC_EDT_REFRESH_TIME);

  /////////////////////////////////////
  // Languages
  
# ifdef ENABLE_NLS
  {
    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)"--- Original ---");
    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_SETITEMDATA, 0, 0);

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
            int iso_index = find_iso6392(fd.cFileName);
            if (iso_index != -1)
            {
              int cb_index = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)iso_langs[iso_index].name);
              SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_SETITEMDATA, cb_index, iso_index);
            }
          }
        } while (FindNextFile(fh, &fd));
    }
    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_SETCURSEL, 0, 0);
  }
# else
  {
    // Disable language selection if NLS is disabled
    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, WM_ENABLE, 0, 0);
  }
# endif

  /////////////////////////////////////
  // Links

  lnk_translate.link(m_Dlg, IDC_LNK_TRANSLATE);
}

void
AC3FilterDlg::translate_controls()
{
  for (int i = 0; i < array_size(dialog_controls); i++)
  {
    if (dialog_controls[i].label)
      if (dialog_controls[i].label[0] != 0)
        SetDlgItemText(m_Dlg, dialog_controls[i].id, gettext_id(dialog_controls[i].strid, dialog_controls[i].label));
  }
}

void
AC3FilterDlg::init_tooltips()
{
/*
  tooltip_ctl.destroy();
  tooltip_ctl.create(ac3filter_instance, m_Dlg);
  tooltip_ctl.set_width(300);
  {
    for (int i = 0; i < array_size(dialog_controls); i++)
    {
      const char *text = trans(dialog_controls[i].tipid, dialog_controls[i].tip);
      if (text)
        if (text[0] != 0)
          tooltip_ctl.add_control(dialog_controls[i].id, text);
    }
  }
*/
}


void 
AC3FilterDlg::update_dynamic_controls()
{
  ctrl->update_dynamic();
}

void 
AC3FilterDlg::update_static_controls()
{
  m_bDirty = true;
  if(m_pPageSite)
    m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);

  /////////////////////////////////////
  // Update controllers

  ctrl->update();
  cpu->update();

  /////////////////////////////////////
  // Interface

  CheckDlgButton(m_Dlg, IDC_CHK_TRAY, tray? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_TOOLTIPS, tooltips? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_INVERT_LEVELS, invert_levels? BST_CHECKED: BST_UNCHECKED);
  edt_refresh_time.update_value(refresh_time);

  /////////////////////////////////////
  // Language

#ifdef ENABLE_NLS
  {
    int current_iso_index = find_iso6392(lang);
    if (current_iso_index != -1)
    {
      int cb_index = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_GETCOUNT, 0, 0);
      if (cb_index != CB_ERR)
      {
        while (cb_index--)
        {
          int iso_index = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_GETITEMDATA, cb_index, 0);
          if (iso_index != CB_ERR && iso_index == current_iso_index)
          {
            SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_SETCURSEL, cb_index, 0);
            break;
          }
        }
      }
    }

    char info[1024];
    strncpy(info, gettext_meta(), sizeof(info));
    info[sizeof(info)-1] = 0;
    cr2crlf(info, sizeof(info));
    SetDlgItemText(m_Dlg, IDC_EDT_TRANS_INFO, info);
  }
#endif

}

///////////////////////////////////////////////////////////////////////////////
// Commands
///////////////////////////////////////////////////////////////////////////////

void 
AC3FilterDlg::command(int control, int message)
{
  /////////////////////////////////////
  // Dispatch message to controllers

  if (ctrl->own_control(control))
  {
    Controller::cmd_result result = ctrl->command(control, message);
    if (result == Controller::cmd_update)
      update();
    return;
  }

  switch (control)
  {
    /////////////////////////////////////
    // Interface

    case IDC_CHK_TRAY:
    {
      tray = IsDlgButtonChecked(m_Dlg, IDC_CHK_TRAY) == BST_CHECKED;
      filter->set_tray(tray);
      update();
      break;
    }

    case IDC_CHK_TOOLTIPS:
    {
      tooltips = IsDlgButtonChecked(m_Dlg, IDC_CHK_TOOLTIPS) == BST_CHECKED;
      set_tooltips(tooltips);
      update();
      break;
    }

    case IDC_CHK_INVERT_LEVELS:
    {
      invert_levels = IsDlgButtonChecked(m_Dlg, IDC_CHK_INVERT_LEVELS) == BST_CHECKED;
      set_invert_levels(invert_levels);
      update();
      break;
    }

    case IDC_EDT_REFRESH_TIME:
      if (message == CB_ENTER)
      {
        refresh_time = int(edt_refresh_time.value);
        set_refresh_time(refresh_time);
        update();
      }
      break;

    /////////////////////////////////////
    // Language

    case IDC_CMB_LANG:
      if (message == CBN_SELENDOK)
      {
        int cb_index, iso_index;
        cb_index = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_GETCURSEL, 0, 0);
        if (cb_index != CB_ERR)
        {
          iso_index = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_GETITEMDATA, cb_index, 0);
          if (iso_index != CB_ERR)
          {
            set_lang(iso_langs[iso_index].iso6392);
            init();
            update();
          }
        }
      }
      break;

    /////////////////////////////////////
    // Donate

    case IDC_BTN_DONATE:
      if (message == BN_CLICKED)
        ShellExecute(0, 0, "http://order.kagi.com/?6CZJZ", 0, 0, SW_SHOWMAXIMIZED);
      break;

  }
}
