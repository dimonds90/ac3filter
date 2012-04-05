#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#include "guids.h"
#include "resource_ids.h"
#include "ac3filter_intl.h"
#include "dialog_controls.h"
#include "log.h"

#include "ac3filter_dlg.h"
#include "dlg/control_all.h"

static const string log_module = "AC3FilterDlg";

#define SAFE_RELEASE(p) { if (p) p->Release(); p = 0; }

#define dlg_printf(dlg, ctrl, format, params)                     \
{                                                                 \
  char buf[255];                                                  \
  sprintf(buf, format, ##params);                                 \
  SendDlgItemMessage(dlg, ctrl, WM_SETTEXT, 0, (LPARAM) (LPSTR)buf); \
}


///////////////////////////////////////////////////////////////////////////////
// Specialized controllers
///////////////////////////////////////////////////////////////////////////////

Controller *AC3FilterDlg::ctrl_main(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlSpk(hdlg, dec));
  ctrl->add(new ControlPreset(hdlg, dec, proc));
  ctrl->add(new ControlLevels(hdlg, filter, proc, invert_levels));
  ctrl->add(new ControlAGC(hdlg, proc));
  ctrl->add(new ControlInfo(hdlg, dec));
  ctrl->add(new ControlVer(hdlg));
  return ctrl;
}

Controller *AC3FilterDlg::ctrl_mixer(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlSpk(hdlg, dec));
  ctrl->add(new ControlPreset(hdlg, dec, proc));
  ctrl->add(new ControlLevels(hdlg, filter, proc, invert_levels));
  ctrl->add(new ControlAGC(hdlg, proc));
  ctrl->add(new ControlMatrix(hdlg, proc));
  ctrl->add(new ControlVer(hdlg));
  return ctrl;
}

Controller *AC3FilterDlg::ctrl_gains(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlPreset(hdlg, dec, proc));
  ctrl->add(new ControlLevels(hdlg, filter, proc, invert_levels));
  ctrl->add(new ControlAGC(hdlg, proc));
  ctrl->add(new ControlBass(hdlg, proc, invert_levels));
  ctrl->add(new ControlMatrix(hdlg, proc));
  ctrl->add(new ControlDelay(hdlg, proc));
  ctrl->add(new ControlIOGains(hdlg, proc));
  ctrl->add(new ControlVer(hdlg));
  return ctrl;
}

Controller *AC3FilterDlg::ctrl_eq(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlPreset(hdlg, dec, proc));
  ctrl->add(new ControlEq(hdlg, proc));
  ctrl->add(new ControlSpectrum(hdlg, filter, proc));
  ctrl->add(new ControlVer(hdlg));
  return ctrl;
}

Controller *AC3FilterDlg::ctrl_spdif(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlSpk(hdlg, dec));
  ctrl->add(new ControlPreset(hdlg, dec, proc));
  ctrl->add(new ControlSPDIF(hdlg, dec));
  ctrl->add(new ControlSystem(hdlg, filter, dec));
  ctrl->add(new ControlInfo(hdlg, dec));
  ctrl->add(new ControlVer(hdlg));
  return ctrl;
}

Controller *AC3FilterDlg::ctrl_system(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlSPDIF(hdlg, dec));
  ctrl->add(new ControlSystem(hdlg, filter, dec));
  ctrl->add(new ControlAGC(hdlg, proc));
  ctrl->add(new ControlLang(hdlg));
  ctrl->add(new ControlSync(hdlg, dec));
  ctrl->add(new ControlVer(hdlg));
  ctrl->add(new ControlAbout(hdlg));
  return ctrl;
}

Controller *AC3FilterDlg::ctrl_about(HWND hdlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels)
{
  ControlList *ctrl = new ControlList(hdlg);
  if (ctrl == 0) return 0;

  ctrl->add(new ControlVer(hdlg));
  ctrl->add(new ControlAbout(hdlg));
  ctrl->add(new ControlDebug(hdlg));
  return ctrl;
}

///////////////////////////////////////////////////////////////////////////////
// Dialog creation
///////////////////////////////////////////////////////////////////////////////

CUnknown * WINAPI AC3FilterDlg::CreateMain(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Main */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Main page", lpunk, phr, IDD_MAIN, N_("IDD_MAIN"), "Main", "http://ac3filter.net/wiki/AC3Filter:Main_page", &AC3FilterDlg::ctrl_main);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateMixer(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Mixer */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Mixer page", lpunk, phr, IDD_MIXER, N_("IDD_MIXER"), "Mixer", "http://ac3filter.net/wiki/AC3Filter:Mixer_page", &AC3FilterDlg::ctrl_mixer);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateGains(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Gains */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Gains page", lpunk, phr, IDD_GAINS, N_("IDD_GAINS"), "Gains", "http://ac3filter.net/wiki/AC3Filter:Gains_page", &AC3FilterDlg::ctrl_gains);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateEq(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: Equalizer */
  CUnknown *punk = new AC3FilterDlg("AC3Filter Equzlizer page", lpunk, phr, IDD_EQ, N_("IDD_EQ"), "Equalizer", "http://ac3filter.net/wiki/AC3Filter:Equalizer_page", &AC3FilterDlg::ctrl_eq);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateSPDIF(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: SPDIF */
  CUnknown *punk = new AC3FilterDlg("AC3Filter SPDIF page", lpunk, phr, IDD_SPDIF, N_("IDD_SPDIF"), "SPDIF", "http://ac3filter.net/wiki/AC3Filter:SPDIF_page", &AC3FilterDlg::ctrl_spdif);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateSystem(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: System */
  CUnknown *punk = new AC3FilterDlg("AC3Filter System page", lpunk, phr, IDD_SYSTEM, N_("IDD_SYSTEM"), "System", "http://ac3filter.net/wiki/AC3Filter:System_page", &AC3FilterDlg::ctrl_system);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}

CUnknown * WINAPI AC3FilterDlg::CreateAbout(LPUNKNOWN lpunk, HRESULT *phr)
{
  /* TRANSLATORS: About */
  CUnknown *punk = new AC3FilterDlg("AC3Filter About property page", lpunk, phr, IDD_ABOUT, N_("IDD_ABOUT"), "About", "http://ac3filter.net/wiki/AC3Filter:About_page", &AC3FilterDlg::ctrl_about);
  if (punk == NULL) *phr = E_OUTOFMEMORY;
  return punk;
}

///////////////////////////////////////////////////////////////////////////////
// Dialog class
///////////////////////////////////////////////////////////////////////////////

AC3FilterDlg::AC3FilterDlg(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr, int DialogId, const char *title_id, const char *title_def, const char *help_link_, ctrl_maker maker_)
:CBasePropertyPage(pName, pUnk, DialogId, 0), maker(maker_)
{
  valib_log(log_event, log_module, "AC3FilterDlg(this=%x, pName=%s)", this, pName);

  title = gettext_id(title_id, title_def);
  help_link = help_link_;
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
  valib_log(log_event, log_module, "OnConnect(this=%x)", this);

  pUnknown->QueryInterface(IID_IAC3Filter, (void **)&filter);
  pUnknown->QueryInterface(IID_IDecoder, (void **)&dec);
  pUnknown->QueryInterface(IID_IAudioProcessor, (void **)&proc);
  if (!filter || !dec || !proc)
  {
    valib_log(log_error, log_module, "OnConnect(this=%x): Cannot get required interfaces", this);
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
  valib_log(log_event, log_module, "OnDisconnect(this=%x)", this);

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
  valib_log(log_event, log_module, "OnActivate(this=%x)", this);

  visible = true;
  refresh = true;

  // Init controllers
  invert_levels = get_invert_levels();
  if (maker) ctrl = (*maker)(m_Dlg, filter, dec, proc, invert_levels);
  cpu = new ControlCPU(m_Dlg, filter, invert_levels);

  // Init and update controls
  init();
  update();

  // Update 'old' values
  const char *new_lang = get_lang();
  if (new_lang == 0) new_lang = "";
  strncpy(lang, new_lang, LANG_LEN);
  old_in_spk = in_spk;

  // Run!
  SetTimer(m_hwnd, 1, get_refresh_time(), 0);  // for all dynamic controls
  SetTimer(m_hwnd, 2, 1000, 0); // for CPU usage (should be averaged)

  return NOERROR;
}

HRESULT 
AC3FilterDlg::OnDeactivate()
{
  valib_log(log_event, log_module, "OnDeactivate(this=%x)", this);

  KillTimer(m_hwnd, 1);
  KillTimer(m_hwnd, 2);

  // Destroy controllers
  safe_delete(ctrl);
  safe_delete(cpu);

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
// Handle messages
///////////////////////////////////////////////////////////////////////////////

INT_PTR 
AC3FilterDlg::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_COMMAND:
      if (command(LOWORD(wParam), HIWORD(wParam)))
        // If an application processes this message, it should return zero.
        return 0;
      break;

    case WM_HSCROLL:
    case WM_VSCROLL:
      if (command(GetDlgCtrlID((HWND)lParam), LOWORD(wParam)))
        // If an application processes this message, it should return zero. 
        return 0;
      break;

    case WM_NOTIFY:
    {
      LPNMHDR nmhdr = (LPNMHDR)lParam;
      INT_PTR result = 0;
      if (lParam && notify((int)nmhdr->idFrom, nmhdr->code, nmhdr, result))
        // If message was processed, return result code if it was set
        return result;
      break;
    }

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
          const char *new_lang = get_lang();
          if (new_lang == 0) new_lang = "";
          if (strcmp(lang, new_lang))
          {
            strcpy(lang, new_lang);
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
}

void 
AC3FilterDlg::init_controls()
{
  valib_log(log_event, log_module, "init_controls(this=%x)", this);

  /////////////////////////////////////
  // Init controllers

  if (ctrl) ctrl->init();
  if (cpu) cpu->init();

  /////////////////////////////////////
  // Interface

  edt_refresh_time.link(m_Dlg, IDC_EDT_REFRESH_TIME);
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
  if (ctrl) ctrl->update_dynamic();
}

void 
AC3FilterDlg::update_static_controls()
{
  m_bDirty = true;
  if(m_pPageSite)
    m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);

  /////////////////////////////////////
  // Update controllers

  if (ctrl) ctrl->update();
  if (cpu) cpu->update();

  /////////////////////////////////////
  // Interface

  CheckDlgButton(m_Dlg, IDC_CHK_TRAY, tray? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_TOOLTIPS, tooltips? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_INVERT_LEVELS, invert_levels? BST_CHECKED: BST_UNCHECKED);
  edt_refresh_time.update_value(refresh_time);
}

///////////////////////////////////////////////////////////////////////////////
// Commands
///////////////////////////////////////////////////////////////////////////////

bool
AC3FilterDlg::command(int control, int message)
{
  /////////////////////////////////////
  // Dispatch message to controllers

  if (ctrl && ctrl->own_control(control))
  {
    Controller::cmd_result cmd_result = ctrl->command(control, message);
    if (cmd_result == Controller::cmd_init) { init(); update(); }
    if (cmd_result == Controller::cmd_update) update();
    return cmd_result != Controller::cmd_not_processed;
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
    // Donate

    case IDC_BTN_DONATE:
      if (message == BN_CLICKED)
        ShellExecute(0, 0, "http://order.kagi.com/?6CZJZ", 0, 0, SW_SHOWMAXIMIZED);
      break;

    case IDC_BTN_HELP:
      if (message == BN_CLICKED)
        ShellExecute(0, 0, help_link, 0, 0, SW_SHOWMAXIMIZED);
      break;
  }

  return false;
}

bool
AC3FilterDlg::notify(int control, int message, LPNMHDR nmhdr, INT_PTR &result)
{
  /////////////////////////////////////
  // Dispatch message to controllers

  if (ctrl && ctrl->own_control(control))
  {
    Controller::cmd_result cmd_result = ctrl->notify(control, message, nmhdr, result);
    if (cmd_result == Controller::cmd_init) { init(); update(); }
    if (cmd_result == Controller::cmd_update) update();
    return cmd_result != Controller::cmd_not_processed;
  }

  return false;
}
