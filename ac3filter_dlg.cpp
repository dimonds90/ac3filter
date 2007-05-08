#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include "ac3filter_dlg.h"
#include "resource.h"
#include "resource_ids.h"
#include "dialog_controls.h"
#include "guids.h"
#include "registry.h"
#include "filters\delay.h"
#include "ac3filter_ver.h"
#include "translate.h"


#define SAFE_RELEASE(p) { if (p) p->Release(); p = 0; }

const double min_gain_level  = -20.0;
const double max_gain_level  = +20.0;
const double min_level = -50.0;
const double sonic_speed = 330; // m/s
const int ticks = 5;

const int matrix_controls[6][6] =
{
  { IDC_EDT_L_L,   IDC_EDT_C_L,   IDC_EDT_R_L,   IDC_EDT_SL_L,   IDC_EDT_SR_L,   IDC_EDT_LFE_L },
  { IDC_EDT_L_C,   IDC_EDT_C_C,   IDC_EDT_R_C,   IDC_EDT_SL_C,   IDC_EDT_SR_C,   IDC_EDT_LFE_C },
  { IDC_EDT_L_R,   IDC_EDT_C_R,   IDC_EDT_R_R,   IDC_EDT_SL_R,   IDC_EDT_SR_R,   IDC_EDT_LFE_R },
  { IDC_EDT_L_SL,  IDC_EDT_C_SL,  IDC_EDT_R_SL,  IDC_EDT_SL_SL,  IDC_EDT_SR_SL,  IDC_EDT_LFE_SL },
  { IDC_EDT_L_SR,  IDC_EDT_C_SR,  IDC_EDT_R_SR,  IDC_EDT_SL_SR,  IDC_EDT_SR_SR,  IDC_EDT_LFE_SR },
  { IDC_EDT_L_LFE, IDC_EDT_C_LFE, IDC_EDT_R_LFE, IDC_EDT_SL_LFE, IDC_EDT_SR_LFE, IDC_EDT_LFE_LFE }
};

const int idc_level_in[6]   = { IDC_IN_L,  IDC_IN_C,  IDC_IN_R,  IDC_IN_SL,  IDC_IN_SR,  IDC_IN_LFE };
const int idc_level_out[6]  = { IDC_OUT_L, IDC_OUT_C, IDC_OUT_R, IDC_OUT_SL, IDC_OUT_SR, IDC_OUT_SW };

const int idc_slider_in[6]  = { IDC_SLI_IN_L,  IDC_SLI_IN_C,  IDC_SLI_IN_R,  IDC_SLI_IN_SL,  IDC_SLI_IN_SR,  IDC_SLI_IN_LFE  };
const int idc_slider_out[6] = { IDC_SLI_OUT_L, IDC_SLI_OUT_C, IDC_SLI_OUT_R, IDC_SLI_OUT_SL, IDC_SLI_OUT_SR, IDC_SLI_OUT_LFE };

const int idc_edt_in[6]     = { IDC_EDT_IN_L,  IDC_EDT_IN_C,  IDC_EDT_IN_R,  IDC_EDT_IN_SL,  IDC_EDT_IN_SR,  IDC_EDT_IN_LFE  };
const int idc_edt_out[6]    = { IDC_EDT_OUT_L, IDC_EDT_OUT_C, IDC_EDT_OUT_R, IDC_EDT_OUT_SL, IDC_EDT_OUT_SR, IDC_EDT_OUT_LFE };

const int idc_edt_delay[6]  = { IDC_EDT_DL, IDC_EDT_DC, IDC_EDT_DR, IDC_EDT_DSL, IDC_EDT_DSR, IDC_EDT_DLFE };


#define dlg_printf(dlg, ctrl, format, params)                     \
{                                                                 \
  char buf[255];                                                  \
  sprintf(buf, format, ##params);                                 \
  SendDlgItemMessage(dlg, ctrl, WM_SETTEXT, 0, (LONG)(LPSTR)buf); \
}

///////////////////////////////////////////////////////////////////////////////
// Registry functions
///////////////////////////////////////////////////////////////////////////////

bool delete_reg_key(const char *name, HKEY root)
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

int get_merit(HKEY hive, LPCSTR key)
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


bool set_merit(HKEY hive, LPCSTR key, int merit)
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
// Speakers list definition
///////////////////////////////////////////////////////////////////////////////

const char *spklist[] = 
{
  "AS IS (no change)",
  "1/0 - mono",
  "2/0 - stereo",
  "3/0 - 3 front",
  "2/1 - surround",
  "3/1 - surround",
  "2/2 - quadro",
  "3/2 - 5 channels",
  "1/0+SW 1.1 mono",
  "2/0+SW 2.1 stereo",
  "3/0+SW 3.1 front",
  "2/1+SW 3.1 surround",
  "3/1+SW 4.1 surround",
  "2/2+SW 4.1 quadro",
  "3/2+SW 5.1 channels",
  "Dolby Surround/ProLogic",
  "Dolby ProLogic II",
};

const char *fmtlist[] = 
{
  "PCM 16bit",
  "PCM 24bit",
  "PCM 32bit",
  "PCM Float",
};

Speakers list2spk(int ispk, int ifmt, int sample_rate)
{
  int format = FORMAT_PCM16;
  int mask = MODE_STEREO;
  int relation = NO_RELATION;
  sample_t level = 32767;

  switch (ispk)
  {
    case  0: mask = 0;        break;
    case  1: mask = MODE_1_0; break;
    case  2: mask = MODE_2_0; break;
    case  3: mask = MODE_3_0; break;
    case  4: mask = MODE_2_1; break;
    case  5: mask = MODE_3_1; break;
    case  6: mask = MODE_2_2; break;
    case  7: mask = MODE_3_2; break;
           
    case  8: mask = MODE_1_0 | CH_MASK_LFE; break;
    case  9: mask = MODE_2_0 | CH_MASK_LFE; break;
    case 10: mask = MODE_3_0 | CH_MASK_LFE; break;
    case 11: mask = MODE_2_1 | CH_MASK_LFE; break;
    case 12: mask = MODE_3_1 | CH_MASK_LFE; break;
    case 13: mask = MODE_2_2 | CH_MASK_LFE; break;
    case 14: mask = MODE_3_2 | CH_MASK_LFE; break;

    case 15: mask = MODE_STEREO; relation = RELATION_DOLBY; break;
    case 16: mask = MODE_STEREO, relation = RELATION_DOLBY2; break;
  }

  switch (ifmt)
  {
    case 0: format = FORMAT_PCM16;    level = 32767; break;
    case 1: format = FORMAT_PCM24;    level = 8388607; break;
    case 2: format = FORMAT_PCM32;    level = 2147483647; break;
    case 3: format = FORMAT_PCMFLOAT; level = 1.0; break;
  }

  return Speakers(format, mask, sample_rate, level, relation);
}

int spk2ispk(Speakers spk)
{
  switch (spk.relation)
  {
    case RELATION_DOLBY:   return 15;
    case RELATION_DOLBY2:  return 16;
    default:
      switch (spk.mask)
      {
        case 0:            return 0;
        case MODE_1_0:     return 1;
        case MODE_2_0:     return 2;
        case MODE_3_0:     return 3;
        case MODE_2_1:     return 4;
        case MODE_3_1:     return 5;
        case MODE_2_2:     return 6;
        case MODE_3_2:     return 7;
                                  
        case MODE_1_0 | CH_MASK_LFE: return  8;
        case MODE_2_0 | CH_MASK_LFE: return  9;
        case MODE_3_0 | CH_MASK_LFE: return 10;
        case MODE_2_1 | CH_MASK_LFE: return 11;
        case MODE_3_1 | CH_MASK_LFE: return 12;
        case MODE_2_2 | CH_MASK_LFE: return 13;
        case MODE_3_2 | CH_MASK_LFE: return 14;
      }
  }
  return 0;
}

int spk2ifmt(Speakers spk)
{
  switch (spk.format)
  {
    case FORMAT_PCM16:    return 0;
    case FORMAT_PCM24:    return 1;
    case FORMAT_PCM32:    return 2;
    case FORMAT_PCMFLOAT: return 3;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Delay units
///////////////////////////////////////////////////////////////////////////////

char *units_list[] =
{
  "Samples",
  "Millisecs",
  "Meters",
  "Centimeters",
  "Feet",
  "Inches"
};

int list2units(int list)
{
  switch (list)
  {
    case 0:  return DELAY_SP;
    case 1:  return DELAY_MS;
    case 2:  return DELAY_M;
    case 3:  return DELAY_CM;
    case 4:  return DELAY_FT;
    case 5:  return DELAY_IN;
    default: return DELAY_SP;
  }
}

int units2list(int units)
{
  switch (units)
  {
    case DELAY_SP: return 0;
    case DELAY_MS: return 1;
    case DELAY_M : return 2;
    case DELAY_CM: return 3;
    case DELAY_FT: return 4;
    case DELAY_IN: return 5;
    default:       return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Initialization / Deinitialization
///////////////////////////////////////////////////////////////////////////////

CUnknown * WINAPI AC3FilterDlg::CreateMain(LPUNKNOWN lpunk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "CreateInstance of AC3Filter Main property page"));
  CUnknown *punk = new AC3FilterDlg("AC3Filter Main property page", lpunk, phr, IDD_MAIN, IDS_MAIN, 0);
  if (punk == NULL) 
    *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateMixer(LPUNKNOWN lpunk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "CreateInstance of AC3Filter Mixer property page"));
  CUnknown *punk = new AC3FilterDlg("AC3Filter Mixer property page", lpunk, phr, IDD_MIXER, IDS_MIXER, 0);
  if (punk == NULL) 
    *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateGains(LPUNKNOWN lpunk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "CreateInstance of AC3Filter Gains property page"));
  CUnknown *punk = new AC3FilterDlg("AC3Filter Gains property page", lpunk, phr, IDD_GAINS, IDS_GAINS, 0);
  if (punk == NULL) 
    *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateSPDIF(LPUNKNOWN lpunk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "CreateInstance of AC3Filter SPDIF property page"));
  CUnknown *punk = new AC3FilterDlg("AC3Filter SPDIF property page", lpunk, phr, IDD_SPDIF, IDS_SPDIF, 0);
  if (punk == NULL) 
    *phr = E_OUTOFMEMORY;
  return punk;
}
CUnknown * WINAPI AC3FilterDlg::CreateSystem(LPUNKNOWN lpunk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "CreateInstance of AC3Filter System property page"));
  CUnknown *punk = new AC3FilterDlg("AC3Filter System property page", lpunk, phr, IDD_SYS, IDS_SYS, 0);
  if (punk == NULL) 
    *phr = E_OUTOFMEMORY;
  return punk;
}

CUnknown * WINAPI AC3FilterDlg::CreateAbout(LPUNKNOWN lpunk, HRESULT *phr)
{
  DbgLog((LOG_TRACE, 3, "CreateInstance of AC3Filter System property page"));
  CUnknown *punk = new AC3FilterDlg("AC3Filter About property page", lpunk, phr, IDD_ABOUT, IDS_ABOUT, 0);
  if (punk == NULL) 
    *phr = E_OUTOFMEMORY;
  return punk;
}

AC3FilterDlg::AC3FilterDlg(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr, int DialogId, int TitleId, int _flags) 
:CBasePropertyPage(pName, pUnk, DialogId, TitleId)
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::AC3FilterDlg()"));

  filter = 0;
  proc   = 0;

  flags  = _flags;
  InitCommonControls();
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

  visible = false;
  refresh = true;

  // Init and update controls
  init();
  update();

  // remember langage
  // (do not translate on first dialog show)
  memset(old_lang, 0, sizeof(old_lang));
  get_lang(old_lang, sizeof(old_lang));

  SetTimer(m_hwnd, 1, get_refresh_time(), 0);  // for all dynamic controls
  SetTimer(m_hwnd, 2, 1000, 0);          // for CPU usage (should be averaged)

  return NOERROR;
}

HRESULT 
AC3FilterDlg::OnDeactivate()
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::OnDeactivate()"));

  KillTimer(m_hwnd, 1);
  KillTimer(m_hwnd, 2);

  return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
// Translation
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
  // set_lang(0) cancels translation
  RegistryKey reg(REG_KEY);
  reg.set_text("Language", _lang);
  init();
  update();
  return true;
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
          tooltips_ctl.track();

          // normal update
          switch (wParam)
          {
            case 1:
              reload_state();
              if (in_spk != old_in_spk)
                update_static_controls();
              update_dynamic_controls();
              break;

            case 2:
              update_cpu_usage();
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
          char lang[256];
          memset(lang, 0, sizeof(lang));
          get_lang(lang, sizeof(lang));
          if (strcmp(lang, old_lang))
          {
            memcpy(old_lang, lang, sizeof(old_lang));
            init();
          }

          // update interface
          SetTimer(m_hwnd, 1, get_refresh_time(), 0);
          
          // update all controls
          // (static controls may be changed at other windows)
          update();
          update_cpu_usage();
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
  dec->get_out_spk(&out_spk);
  dec->get_user_spk(&user_spk);

  dec->get_formats(&formats);

  // spdif
  dec->get_use_spdif(&use_spdif);
  dec->get_spdif_pt(&spdif_pt);
  dec->get_spdif_as_pcm(&spdif_as_pcm);
  dec->get_spdif_encode(&spdif_encode);
  dec->get_spdif_stereo_pt(&spdif_stereo_pt);

  dec->get_spdif_check_sr(&spdif_check_sr);
  dec->get_spdif_allow_48(&spdif_allow_48);
  dec->get_spdif_allow_44(&spdif_allow_44);
  dec->get_spdif_allow_32(&spdif_allow_32);

  dec->get_dts_mode(&dts_mode);
  dec->get_dts_conv(&dts_conv);

  dec->get_spdif_status(&spdif_status);
  dec->get_use_detector(&use_detector);

  // DirectShow
  filter->get_reinit(&reinit);
  filter->get_spdif_no_pcm(&spdif_no_pcm);
  dec->get_query_sink(&query_sink);

  // interface
  filter->get_tray(&tray);
  tooltips = get_tooltips();
  invert_levels = get_invert_levels();
  refresh_time = get_refresh_time();

  // syncronization
  dec->get_time_shift(&time_shift);
  dec->get_time_factor(&time_factor);
  dec->get_dejitter(&dejitter);
  dec->get_threshold(&threshold);
  dec->get_jitter(&input_mean, &input_stddev, &output_mean, &output_stddev);

  dec->get_frames(&frames, &errors);

  vtime_t time;
  filter->get_playback_time(&time);
  proc->get_state(this, time);
}

void 
AC3FilterDlg::init_controls()
{
  DbgLog((LOG_TRACE, 3, "AC3FilterDlg::init_controls()"));
  int i, j, ch;

  /////////////////////////////////////
  // Speakers

  SendDlgItemMessage(m_Dlg, IDC_CMB_SPK, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < sizeof(spklist) / sizeof(spklist[0]); i++)
    SendDlgItemMessage(m_Dlg, IDC_CMB_SPK, CB_ADDSTRING, 0, (LONG)spklist[i]);

  /////////////////////////////////////
  // CPU usage

  SendDlgItemMessage(m_Dlg, IDC_CPU, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

  /////////////////////////////////////
  // Formats

  SendDlgItemMessage(m_Dlg, IDC_CMB_FORMAT, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < sizeof(fmtlist) / sizeof(fmtlist[0]); i++)
    SendDlgItemMessage(m_Dlg, IDC_CMB_FORMAT, CB_ADDSTRING, 0, (LONG)fmtlist[i]);

  /////////////////////////////////////
  // Matrix

  for (i = 0; i < 6; i++)
    for (j = 0; j < 6; j++)
      edt_matrix[i][j].link(m_Dlg, matrix_controls[i][j]);

  /////////////////////////////////////
  // Gains

  SendDlgItemMessage(m_Dlg, IDC_SLI_MASTER, TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_MASTER, TBM_SETTIC, 0, 0);
  SendDlgItemMessage(m_Dlg, IDC_SLI_GAIN,   TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_GAIN,   TBM_SETTIC, 0, 0);

  SendDlgItemMessage(m_Dlg, IDC_SLI_LFE,    TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_LFE,    TBM_SETTIC, 0, 0);
  SendDlgItemMessage(m_Dlg, IDC_SLI_VOICE,  TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_VOICE,  TBM_SETTIC, 0, 0);
  SendDlgItemMessage(m_Dlg, IDC_SLI_SUR,    TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_SUR,    TBM_SETTIC, 0, 0);

  edt_master.link(m_Dlg, IDC_EDT_MASTER);
  edt_gain  .link(m_Dlg, IDC_EDT_GAIN);
  edt_voice .link(m_Dlg, IDC_EDT_VOICE);
  edt_sur   .link(m_Dlg, IDC_EDT_SUR);
  edt_lfe   .link(m_Dlg, IDC_EDT_LFE);

  edt_gain.enable(false);

 
  /////////////////////////////////////
  // I/O Gains

  for (ch = 0; ch < NCHANNELS; ch++)
  {
    SendDlgItemMessage(m_Dlg, idc_slider_in[ch],  TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
    SendDlgItemMessage(m_Dlg, idc_slider_out[ch], TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
    SendDlgItemMessage(m_Dlg, idc_slider_in[ch],  TBM_SETTIC, 0, 0);
    SendDlgItemMessage(m_Dlg, idc_slider_out[ch], TBM_SETTIC, 0, 0);
    edt_in_gains[ch].link(m_Dlg, idc_edt_in[ch]);
    edt_out_gains[ch].link(m_Dlg, idc_edt_out[ch]);
  }

  /////////////////////////////////////
  // I/O Levels

  for (ch = 0; ch < NCHANNELS; ch++)
  {
    SendDlgItemMessage(m_Dlg, idc_level_in[ch],  PBM_SETBARCOLOR, 0, RGB(0, 128, 0));
    SendDlgItemMessage(m_Dlg, idc_level_out[ch], PBM_SETBARCOLOR, 0, RGB(0, 128, 0));
    SendDlgItemMessage(m_Dlg, idc_level_in[ch],  PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
    SendDlgItemMessage(m_Dlg, idc_level_out[ch], PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
  }

  /////////////////////////////////////
  // Delay

  for (ch = 0; ch < NCHANNELS; ch++)
    edt_delay[ch].link(m_Dlg, idc_edt_delay[ch]);

  /////////////////////////////////////
  // Syncronization

  edt_time_shift.link(m_Dlg, IDC_EDT_TIME_SHIFT);
  SendDlgItemMessage(m_Dlg, IDC_SLI_TIME_SHIFT, TBM_SETRANGE, TRUE, MAKELONG(-500, 500));
  SendDlgItemMessage(m_Dlg, IDC_SLI_TIME_SHIFT, TBM_SETTIC, 0, 0);

  /////////////////////////////////////
  // AGC

  edt_attack.link(m_Dlg, IDC_EDT_ATTACK);
  edt_release.link(m_Dlg, IDC_EDT_RELEASE);

  /////////////////////////////////////
  // DRC

  SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_POWER, TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_POWER, TBM_SETTIC, 0, 0);
  SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_LEVEL, TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_LEVEL, TBM_SETTIC, 0, 0);
  edt_drc_power.link(m_Dlg, IDC_EDT_DRC_POWER);
  edt_drc_level.link(m_Dlg, IDC_EDT_DRC_LEVEL);

  /////////////////////////////////////
  // Bass redirection

  edt_bass_freq.link(m_Dlg, IDC_EDT_BASS_FREQ);

  /////////////////////////////////////
  // Delay units

  SendDlgItemMessage(m_Dlg, IDC_CMB_UNITS, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < sizeof(units_list) / sizeof(units_list[0]); i++)
    SendDlgItemMessage(m_Dlg, IDC_CMB_UNITS, CB_ADDSTRING, 0, (LONG)units_list[i]);

  /////////////////////////////////////
  // Interface

  edt_refresh_time.link(m_Dlg, IDC_EDT_REFRESH_TIME);

  /////////////////////////////////////
  // Links

  lnk_home.link(m_Dlg, IDC_LNK_HOME);
  lnk_forum.link(m_Dlg, IDC_LNK_FORUM);
  lnk_email.link(m_Dlg, IDC_LNK_EMAIL);
  lnk_donate.link(m_Dlg, IDC_LNK_DONATE);

  /////////////////////////////////////
  // Build and environment info

  char info[1024];

  strncpy(info, valib_build_info(), sizeof(info));
  info[sizeof(info)-1] = 0;
  cr2crlf(info, sizeof(info));
  SetDlgItemText(m_Dlg, IDC_EDT_ENV, info);

  strncpy(info, valib_credits(), sizeof(info));
  info[sizeof(info)-1] = 0;
  cr2crlf(info, sizeof(info));
  SetDlgItemText(m_Dlg, IDC_EDT_CREDITS, info);

  /////////////////////////////////////
  // Version

  char ver1[255];
  char ver2[255];
  GetDlgItemText(m_Dlg, IDC_VER, ver1, array_size(ver1));
  sprintf(ver2, ver1, AC3FILTER_VER);
  SetDlgItemText(m_Dlg, IDC_VER, ver2);
}

void
AC3FilterDlg::translate_controls()
{
  char file[1536];
  char path[1024];
  char lang[256];

  RegistryKey reg(REG_KEY);
  path[0] = 0; reg.get_text("Install_Dir", path, sizeof(path));
  lang[0] = 0; reg.get_text("Language", lang, sizeof(lang));
  sprintf(file, "%s\\lang\\%s.lng", path, lang);

  char buf[1024];
  trans.open(file);
  for (int i = 0; i < array_size(dialog_controls); i++)
  {
    trans.translate(dialog_controls[i].transid, buf, sizeof(buf), dialog_controls[i].label);
    if (buf[0] != 0)
      SetDlgItemText(m_Dlg, dialog_controls[i].id, buf);
  }
}

void
AC3FilterDlg::init_tooltips()
{
  tooltips_ctl.destroy();
  tooltips_ctl.create(ac3filter_instance, m_Dlg);
  tooltips_ctl.set_width(300);
  {
    for (int i = 0; i < array_size(dialog_controls); i++)
    {
      const char *text = trans(dialog_controls[i].tipid, dialog_controls[i].tip);
      if (text[0] != 0)
        tooltips_ctl.add_control(dialog_controls[i].id, text);
    }
  }
}


void 
AC3FilterDlg::update_dynamic_controls()
{
  /////////////////////////////////////////////////////////
  // Input format

  if (old_in_spk != in_spk || refresh)
  {
    char buf[128];
    old_in_spk = in_spk;
    sprintf(buf, "%s %s %iHz", in_spk.format_text(), in_spk.mode_text(), in_spk.sample_rate);
    SetDlgItemText(m_Dlg, IDC_LBL_INPUT, buf);
  }

  /////////////////////////////////////
  // SPDIF mode

  if (spdif_status != old_spdif_status || refresh)
  {
    old_spdif_status = spdif_status;
    switch (old_spdif_status)
    {
      case SPDIF_MODE_NONE:
        SetDlgItemText(m_Dlg, IDC_CHK_USE_SPDIF, "Use SPDIF");
        break;

      case SPDIF_MODE_DISABLED:
        SetDlgItemText(m_Dlg, IDC_CHK_USE_SPDIF, "Use SPDIF (disabled)");
        break;

      case SPDIF_MODE_PASSTHROUGH:
        SetDlgItemText(m_Dlg, IDC_CHK_USE_SPDIF, "Use SPDIF (passthrough)");
        break;

      case SPDIF_MODE_ENCODE:
        SetDlgItemText(m_Dlg, IDC_CHK_USE_SPDIF, "Use SPDIF (AC3 encode)");
        break;

      default:
        SetDlgItemText(m_Dlg, IDC_CHK_USE_SPDIF, "Use SPDIF (Unknown)");
        break;
    }
  }

  /////////////////////////////////////
  // Stream info

  char info[sizeof(old_info)];
  memset(info, 0, sizeof(old_info));
  dec->get_info(info, sizeof(old_info));
  if (memcmp(info, old_info, sizeof(old_info)) || refresh)
  {
    memcpy(old_info, info, sizeof(old_info));
    SendDlgItemMessage(m_Dlg, IDC_EDT_INFO, WM_SETTEXT, 0, (LONG)(LPSTR)info);
  }

  dlg_printf(m_Dlg, IDC_EDT_FRAMES, "%i", frames);
  dlg_printf(m_Dlg, IDC_EDT_ERRORS, "%i", errors);

  /////////////////////////////////////
  // Auto gain control

  SendDlgItemMessage(m_Dlg, IDC_SLI_GAIN, TBM_SETPOS, TRUE, long(-value2db(gain) * ticks));
  edt_gain.update_value(value2db(gain));

  /////////////////////////////////////
  // I/O Levels

  for (int ch = 0; ch < NCHANNELS; ch++)
  {
    long in, out;
    if (invert_levels)
    {
      in = input_levels[ch]  > 0? long(-value2db(input_levels[ch]) * ticks): long(-min_level * ticks);
      out = output_levels[ch]  > 0? long(-value2db(output_levels[ch]) * ticks): long(-min_level * ticks);
    }
    else
    {
      in = input_levels[ch]  > 0? long((value2db(input_levels[ch]) - min_level) * ticks): 0;
      out = output_levels[ch]  > 0? long((value2db(output_levels[ch]) - min_level) * ticks): 0;
    }
    SendDlgItemMessage(m_Dlg, idc_level_in[ch],  PBM_SETPOS, in, 0);
    SendDlgItemMessage(m_Dlg, idc_level_out[ch], PBM_SETPOS, out, 0);
    SendDlgItemMessage(m_Dlg, idc_level_out[ch], PBM_SETBARCOLOR, 0, (output_levels[ch] > 0.99)? RGB(255, 0, 0): RGB(0, 128, 0));
  }

  /////////////////////////////////////
  // DRC

  SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_LEVEL, TBM_SETPOS, TRUE, long(-value2db(drc_level) * ticks));
  edt_drc_level.update_value(value2db(drc_level));

  /////////////////////////////////////
  // Syncronization

  char jitter[sizeof(old_jitter)];
  sprintf(jitter, "Input\tmean: %ims\tstddev: %ims\r\nOutput\tmean: %ims\tstddev: %ims", 
    int(input_mean * 1000), int(input_stddev * 1000), 
    int(output_mean * 1000), int(output_stddev * 1000));

  if (memcmp(jitter, old_jitter, strlen(jitter)) || refresh)
  {
    memcpy(old_jitter, jitter, sizeof(old_jitter));
    SendDlgItemMessage(m_Dlg, IDC_EDT_JITTER, WM_SETTEXT, 0, (LONG)(LPSTR)jitter);
  }

  /////////////////////////////////////
  // Matrix controls

  if (auto_matrix)
    update_matrix_controls();

  refresh = false;
}

void 
AC3FilterDlg::update_static_controls()
{
  int ch;

  m_bDirty = true;
  if(m_pPageSite)
    m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);

  /////////////////////////////////////
  // Speakers

  SendDlgItemMessage(m_Dlg, IDC_CMB_SPK,    CB_SETCURSEL, spk2ispk(user_spk), 0);
  SendDlgItemMessage(m_Dlg, IDC_CMB_FORMAT, CB_SETCURSEL, spk2ifmt(user_spk), 0);

  /////////////////////////////////////
  // SPDIF

  CheckDlgButton(m_Dlg, IDC_CHK_USE_SPDIF, use_spdif? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // SPDIF passthrough

  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_MPA, (spdif_pt & FORMAT_MASK_MPA) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_AC3, (spdif_pt & FORMAT_MASK_AC3) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_DTS, (spdif_pt & FORMAT_MASK_DTS) != 0? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // SPDIF/DTS output mode

  SendDlgItemMessage(m_Dlg, IDC_RBT_DTS_MODE_AUTO,    BM_SETCHECK, dts_mode == DTS_MODE_AUTO? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_RBT_DTS_MODE_WRAPPED, BM_SETCHECK, dts_mode == DTS_MODE_WRAPPED? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_RBT_DTS_MODE_PADDED,  BM_SETCHECK, dts_mode == DTS_MODE_PADDED? BST_CHECKED: BST_UNCHECKED, 1);

  /////////////////////////////////////
  // SPDIF/DTS conversion

  SendDlgItemMessage(m_Dlg, IDC_RBT_DTS_CONV_NONE,    BM_SETCHECK, dts_conv == DTS_CONV_NONE? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_RBT_DTS_CONV_14BIT,   BM_SETCHECK, dts_conv == DTS_CONV_14BIT? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_RBT_DTS_CONV_16BIT,   BM_SETCHECK, dts_conv == DTS_CONV_16BIT? BST_CHECKED: BST_UNCHECKED, 1);

  /////////////////////////////////////
  // SPDIF options

  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_AS_PCM, spdif_as_pcm? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_ENCODE, spdif_encode? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_STEREO_PT, spdif_stereo_pt? BST_CHECKED: BST_UNCHECKED);

  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_CHECK_SR, spdif_check_sr? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_ALLOW_48, spdif_allow_48? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_ALLOW_44, spdif_allow_44? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_ALLOW_32, spdif_allow_32? BST_CHECKED: BST_UNCHECKED);

  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_SPDIF_STEREO_PT), spdif_encode);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_SPDIF_ALLOW_48), spdif_check_sr);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_SPDIF_ALLOW_44), spdif_check_sr);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_SPDIF_ALLOW_32), spdif_check_sr);

  CheckDlgButton(m_Dlg, IDC_CHK_USE_DETECTOR, use_detector? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // Formats

  CheckDlgButton(m_Dlg, IDC_CHK_PCM, (formats & FORMAT_CLASS_PCM_LE) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_MPA, (formats & FORMAT_MASK_MPA) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_AC3, (formats & FORMAT_MASK_AC3) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_DTS, (formats & FORMAT_MASK_DTS) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_PES, (formats & FORMAT_MASK_PES) != 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF, (formats & FORMAT_MASK_SPDIF) != 0? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // Auto gain control

  SendDlgItemMessage(m_Dlg, IDC_SLI_MASTER, TBM_SETPOS, TRUE, long(-value2db(master) * ticks));
  edt_master.update_value(value2db(master));

  SendDlgItemMessage(m_Dlg, IDC_CHK_AUTO_GAIN, BM_SETCHECK, auto_gain? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_CHK_NORMALIZE, BM_SETCHECK, normalize? BST_CHECKED: BST_UNCHECKED, 1);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_NORMALIZE), auto_gain);

  edt_attack.update_value(attack);
  edt_release.update_value(release);

  /////////////////////////////////////
  // Gain controls

  SendDlgItemMessage(m_Dlg, IDC_SLI_VOICE, TBM_SETPOS, TRUE, long(-value2db(clev)   * ticks));
  SendDlgItemMessage(m_Dlg, IDC_SLI_SUR,   TBM_SETPOS, TRUE, long(-value2db(slev)   * ticks));
  SendDlgItemMessage(m_Dlg, IDC_SLI_LFE,   TBM_SETPOS, TRUE, long(-value2db(lfelev) * ticks));

  edt_voice.update_value(value2db(clev));
  edt_sur  .update_value(value2db(slev));
  edt_lfe  .update_value(value2db(lfelev));

  /////////////////////////////////////
  // I/O Gains

  for (ch = 0; ch < NCHANNELS; ch++)
  {
    SendDlgItemMessage(m_Dlg, idc_slider_in[ch],  TBM_SETPOS, TRUE, long(-value2db(input_gains[ch])  * ticks));
    SendDlgItemMessage(m_Dlg, idc_slider_out[ch], TBM_SETPOS, TRUE, long(-value2db(output_gains[ch]) * ticks));
    edt_in_gains[ch].update_value(value2db(input_gains[ch]));
    edt_out_gains[ch].update_value(value2db(output_gains[ch]));
  }

  /////////////////////////////////////
  // Delay

  for (ch = 0; ch < NCHANNELS; ch++)
  {
    edt_delay[ch].update_value(delays[ch]);
    edt_delay[ch].enable(delay);
  }
  SendDlgItemMessage(m_Dlg, IDC_CHK_DELAYS, BM_SETCHECK, delay? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_CMB_UNITS, CB_SETCURSEL, units2list(delay_units), 0);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CMB_UNITS), delay);

  /////////////////////////////////////
  // Interface

  CheckDlgButton(m_Dlg, IDC_CHK_TRAY, tray? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_TOOLTIPS, tooltips? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_INVERT_LEVELS, invert_levels? BST_CHECKED: BST_UNCHECKED);
  edt_refresh_time.update_value(refresh_time);

  /////////////////////////////////////
  // DirectShow

  CheckDlgButton(m_Dlg, IDC_CHK_QUERY_SINK, query_sink? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_REINIT, reinit > 0? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(m_Dlg, IDC_CHK_SPDIF_NO_PCM, spdif_no_pcm? BST_CHECKED: BST_UNCHECKED);

  /////////////////////////////////////
  // Syncronization

  edt_time_shift.update_value(time_shift * 1000);
  SendDlgItemMessage(m_Dlg, IDC_SLI_TIME_SHIFT, TBM_SETPOS, TRUE, int(time_shift * 1000));

  SendDlgItemMessage(m_Dlg, IDC_CHK_JITTER, BM_SETCHECK, dejitter? BST_CHECKED: BST_UNCHECKED, 1);
//  SetDlgItemInt(m_Dlg, IDC_LBL_JITTER, int(jitter * 1000), false);

  /////////////////////////////////////
  // DRC

  SendDlgItemMessage(m_Dlg, IDC_CHK_DRC, BM_SETCHECK, drc? BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_POWER, TBM_SETPOS, TRUE, long(-drc_power * ticks));
  edt_drc_power.update_value(drc_power);

  /////////////////////////////////////
  // Bass redirection

  SendDlgItemMessage(m_Dlg, IDC_CHK_BASS_REDIR, BM_SETCHECK, bass_redir? BST_CHECKED: BST_UNCHECKED, 1);
  edt_bass_freq.update_value(bass_freq);

  /////////////////////////////////////
  // Matrix Flags

  SendDlgItemMessage(m_Dlg, IDC_CHK_AUTO_MATRIX,   BM_SETCHECK, auto_matrix?      BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_CHK_EXPAND_STEREO, BM_SETCHECK, expand_stereo?    BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_CHK_VOICE_CONTROL, BM_SETCHECK, voice_control?    BST_CHECKED: BST_UNCHECKED, 1);
  SendDlgItemMessage(m_Dlg, IDC_CHK_NORM_MATRIX,   BM_SETCHECK, normalize_matrix? BST_CHECKED: BST_UNCHECKED, 1);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_EXPAND_STEREO), auto_matrix);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_VOICE_CONTROL), auto_matrix);
  EnableWindow(GetDlgItem(m_Dlg, IDC_CHK_NORM_MATRIX), auto_matrix);

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
    SendDlgItemMessage(m_Dlg, IDC_RBT_MERIT_PREFERRED, BM_SETCHECK, filter_merit >  MERIT_NORMAL? BST_CHECKED: BST_UNCHECKED, 1);
    SendDlgItemMessage(m_Dlg, IDC_RBT_MERIT_UNLIKELY,  BM_SETCHECK, filter_merit <= MERIT_NORMAL? BST_CHECKED: BST_UNCHECKED, 1);
  }
    
  if (wo_merit && ds_merit)
  {
    SendDlgItemMessage(m_Dlg, IDC_RBT_RENDER_DS, BM_SETCHECK, ds_merit > wo_merit? BST_CHECKED: BST_UNCHECKED, 1);
    SendDlgItemMessage(m_Dlg, IDC_RBT_RENDER_WO, BM_SETCHECK, ds_merit < wo_merit? BST_CHECKED: BST_UNCHECKED, 1);
  }

  /////////////////////////////////////
  // Languages
  
  {
    char file[1536];
    char path[1024];
    char lang[256];
    WIN32_FIND_DATA fd;
    HANDLE          fh;

    RegistryKey reg(REG_KEY);
    path[0] = 0; reg.get_text("Install_Dir", path, sizeof(path));
    lang[0] = 0; reg.get_text("Language", lang, sizeof(lang));
    sprintf(file, "%s\\lang\\*.lng", path);

    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_RESETCONTENT, 0, 0);
    SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)"--- Original ---");
    fh = FindFirstFile(file, &fd);
    if (fh != INVALID_HANDLE_VALUE)
      do
      {
        // cut off .lng extension
        size_t len = strlen(fd.cFileName);
        if (len > 4)
        {
          len = MIN(sizeof(file) - 1, len - 4);
          memcpy(file, fd.cFileName, len);
          file[len] = 0;
          SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_ADDSTRING, 0, (LONG)file);
        }
      } while (FindNextFile(fh, &fd));

    int n = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_FINDSTRINGEXACT, 0, (LONG)lang);
    if (n != CB_ERR)
      SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_SETCURSEL, n, 0);

    char buf[1024];
    sprintf(buf, trans("STR_TRANS_INFO", "Translation author: %s\r\nFilter version: %s\r\nDate of last edit: %s\r\n%s"),
      trans.author(), trans.ver(), trans.date(), trans.comment());
    SendDlgItemMessage(m_Dlg, IDC_EDT_TRANS_INFO, WM_SETTEXT, 0, (LONG)buf);
  }

  /////////////////////////////////////
  // Presets

  #define fill_combobox(control, registry)                                                \
  {                                                                                       \
    HKEY key;                                                                             \
    char preset[256];                                                                     \
    int  n;                                                                               \
                                                                                          \
    SendDlgItemMessage(m_Dlg, control, WM_GETTEXT, 256, (LONG)preset);                    \
    SendDlgItemMessage(m_Dlg, control, CB_RESETCONTENT, 0, 0);                            \
                                                                                          \
    if (RegOpenKeyEx(HKEY_CURRENT_USER, registry, 0, KEY_READ, &key) == ERROR_SUCCESS)    \
    {                                                                                     \
      char buf[256];                                                                      \
      int i = 0;                                                                          \
      DWORD len = 256;                                                                    \
      while (RegEnumKeyEx(key, i++, (LPTSTR)buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)      \
      {                                                                                   \
        if (strcmp(buf, "Default"))                                                       \
          SendDlgItemMessage(m_Dlg, control, CB_ADDSTRING, 0, (LONG)buf);                 \
        len = 256;                                                                        \
      }                                                                                   \
      RegCloseKey(key);                                                                   \
    }                                                                                     \
                                                                                          \
    n = SendDlgItemMessage(m_Dlg, control, CB_FINDSTRINGEXACT, 0, (LONG)preset);          \
    if (n != CB_ERR)                                                                      \
      SendDlgItemMessage(m_Dlg, control, CB_SETCURSEL, n, 0);                             \
    SendDlgItemMessage(m_Dlg, control, WM_SETTEXT, 0, (LONG)preset);                      \
  }

  fill_combobox(IDC_CMB_PRESET, REG_KEY_PRESET);
  fill_combobox(IDC_CMB_MATRIX_PRESET, REG_KEY_MATRIX);

  /////////////////////////////////////
  // Matrix

  update_matrix_controls();
}

void 
AC3FilterDlg::update_matrix_controls()
{
  bool auto_matrix;
  matrix_t matrix;

  proc->get_matrix(&matrix);
  proc->get_auto_matrix(&auto_matrix);

  if (memcmp(old_matrix, matrix, sizeof(matrix_t)) || !auto_matrix || refresh)
  {
    memcpy(old_matrix, matrix, sizeof(matrix_t));
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++)
      {
        edt_matrix[i][j].update_value(matrix[j][i]);
        SendDlgItemMessage(m_Dlg, matrix_controls[j][i], EM_SETREADONLY, auto_matrix, 0);
      }
  }
}

void 
AC3FilterDlg::update_cpu_usage()
{
  /////////////////////////////////////
  // CPU usage

  double cpu_usage;
  filter->get_cpu_usage(&cpu_usage);
  dlg_printf(m_Dlg, IDC_CPU_LABEL, "%i%%", int(cpu_usage*100));
  if (invert_levels)
    SendDlgItemMessage(m_Dlg, IDC_CPU, PBM_SETPOS, 100 - int(cpu_usage * 100),  0);
  else
    SendDlgItemMessage(m_Dlg, IDC_CPU, PBM_SETPOS, int(cpu_usage * 100),  0);

}

///////////////////////////////////////////////////////////////////////////////
// Commands
///////////////////////////////////////////////////////////////////////////////

void 
AC3FilterDlg::command(int control, int message)
{
  /////////////////////////////////////
  // Matrix controls

  if (message == CB_ENTER)
  {
    matrix_t matrix;
    proc->get_matrix(&matrix);
    bool update_matrix = false;

    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++)
        if (control == matrix_controls[i][j])
        {
          matrix[j][i] = edt_matrix[i][j].value;
          update_matrix = true;
        }

    if (update_matrix)
    {
      proc->set_matrix(&matrix);
      update_matrix_controls();
      return;
    }
  }

  switch (control)
  {
    /////////////////////////////////////
    // Speaker selection

    case IDC_CMB_SPK:
    case IDC_CMB_FORMAT:
      if (message == CBN_SELENDOK)
      {
        int ispk = SendDlgItemMessage(m_Dlg, IDC_CMB_SPK, CB_GETCURSEL, 0, 0);
        int ifmt = SendDlgItemMessage(m_Dlg, IDC_CMB_FORMAT, CB_GETCURSEL, 0, 0);

        Speakers spk = list2spk(ispk, ifmt, 0);
        dec->set_user_spk(spk);
        update();
      }
      break;

    /////////////////////////////////////
    // SPDIF if possible

    case IDC_CHK_USE_SPDIF:
    {
      use_spdif = IsDlgButtonChecked(m_Dlg, IDC_CHK_USE_SPDIF) == BST_CHECKED;
      dec->set_use_spdif(use_spdif);
      update();
      break;
    }

    /////////////////////////////////////
    // SPDIF passthrough

    case IDC_CHK_SPDIF_MPA:
    case IDC_CHK_SPDIF_AC3:
    case IDC_CHK_SPDIF_DTS:
    {
      spdif_pt = 0;
      spdif_pt |= IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_MPA) == BST_CHECKED? FORMAT_MASK_MPA: 0;
      spdif_pt |= IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_AC3) == BST_CHECKED? FORMAT_MASK_AC3: 0;
      spdif_pt |= IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_DTS) == BST_CHECKED? FORMAT_MASK_DTS: 0;
      dec->set_spdif_pt(spdif_pt);
      update();
      break;
    }

    /////////////////////////////////////
    // SPDIF options

    case IDC_CHK_USE_DETECTOR:
    {
      use_detector = IsDlgButtonChecked(m_Dlg, IDC_CHK_USE_DETECTOR) == BST_CHECKED;
      dec->set_use_detector(use_detector);
      update();
      break;
    }

    case IDC_CHK_SPDIF_AS_PCM:
    {
      spdif_as_pcm = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_AS_PCM) == BST_CHECKED;
      dec->set_spdif_as_pcm(spdif_as_pcm);
      update();
      break;
    }

    case IDC_CHK_SPDIF_ENCODE:
    {
      spdif_encode = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_ENCODE) == BST_CHECKED;
      dec->set_spdif_encode(spdif_encode);
      update();
      break;
    }

    case IDC_CHK_SPDIF_STEREO_PT:
    {
      spdif_stereo_pt = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_STEREO_PT) == BST_CHECKED;
      dec->set_spdif_stereo_pt(spdif_stereo_pt);
      update();
      break;
    }

    case IDC_CHK_SPDIF_CHECK_SR:
    {
      spdif_check_sr = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_CHECK_SR) == BST_CHECKED;
      dec->set_spdif_check_sr(spdif_check_sr);
      update();
      break;
    }

    case IDC_CHK_SPDIF_ALLOW_48:
    {
      spdif_allow_48 = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_ALLOW_48) == BST_CHECKED;
      dec->set_spdif_allow_48(spdif_allow_48);
      update();
      break;
    }

    case IDC_CHK_SPDIF_ALLOW_44:
    {
      spdif_allow_44 = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_ALLOW_44) == BST_CHECKED;
      dec->set_spdif_allow_44(spdif_allow_44);
      update();
      break;
    }

    case IDC_CHK_SPDIF_ALLOW_32:
    {
      spdif_allow_32 = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_ALLOW_32) == BST_CHECKED;
      dec->set_spdif_allow_32(spdif_allow_32);
      update();
      break;
    }

    /////////////////////////////////////
    // Formats

    case IDC_CHK_PCM:
    case IDC_CHK_MPA:
    case IDC_CHK_AC3:
    case IDC_CHK_DTS:
    case IDC_CHK_PES:
    case IDC_CHK_SPDIF:
    {
      formats = FORMAT_CLASS_PCM_BE; // Always allow DVD LPCM
      formats |= IsDlgButtonChecked(m_Dlg, IDC_CHK_PCM) == BST_CHECKED? FORMAT_CLASS_PCM: 0;
      formats |= IsDlgButtonChecked(m_Dlg, IDC_CHK_MPA) == BST_CHECKED? FORMAT_MASK_MPA: 0;
      formats |= IsDlgButtonChecked(m_Dlg, IDC_CHK_AC3) == BST_CHECKED? FORMAT_MASK_AC3: 0;
      formats |= IsDlgButtonChecked(m_Dlg, IDC_CHK_DTS) == BST_CHECKED? FORMAT_MASK_DTS: 0;
      formats |= IsDlgButtonChecked(m_Dlg, IDC_CHK_PES) == BST_CHECKED? FORMAT_MASK_PES: 0;
      formats |= IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF) == BST_CHECKED? FORMAT_MASK_SPDIF: 0;
      dec->set_formats(formats);
      update();
      break;
    }

    /////////////////////////////////////
    // Query sink

    case IDC_CHK_QUERY_SINK:
    {
      query_sink = IsDlgButtonChecked(m_Dlg, IDC_CHK_QUERY_SINK) == BST_CHECKED;
      dec->set_query_sink(query_sink);
      update();
      break;
    }

    /////////////////////////////////////
    // Force SPDIF reinit

    case IDC_CHK_REINIT:
    {
      reinit = IsDlgButtonChecked(m_Dlg, IDC_CHK_REINIT) == BST_CHECKED? 128: 0;
      filter->set_reinit(reinit);
      update();
      break;
    }

    /////////////////////////////////////
    // Disallow PCM in SPDIF mode

    case IDC_CHK_SPDIF_NO_PCM:
    {
      spdif_no_pcm = IsDlgButtonChecked(m_Dlg, IDC_CHK_SPDIF_NO_PCM) == BST_CHECKED;
      filter->set_spdif_no_pcm(spdif_no_pcm);
      update();
      break;
    }

    /////////////////////////////////////
    // SPDIF/DTS output mode

    case IDC_RBT_DTS_MODE_AUTO:
    {
      dts_mode = DTS_MODE_AUTO;
      dec->set_dts_mode(dts_mode);
      update();
      break;
    }

    case IDC_RBT_DTS_MODE_WRAPPED:
    {
      dts_mode = DTS_MODE_WRAPPED;
      dec->set_dts_mode(dts_mode);
      update();
      break;
    }

    case IDC_RBT_DTS_MODE_PADDED:
    {
      dts_mode = DTS_MODE_PADDED;
      dec->set_dts_mode(dts_mode);
      update();
      break;
    }

    /////////////////////////////////////
    // SPDIF/DTS conversion

    case IDC_RBT_DTS_CONV_NONE:
    {
      dts_conv = DTS_CONV_NONE;
      dec->set_dts_conv(dts_conv);
      update();
      break;
    }

    case IDC_RBT_DTS_CONV_14BIT:
    {
      dts_conv = DTS_CONV_14BIT;
      dec->set_dts_conv(dts_conv);
      update();
      break;
    }

    case IDC_RBT_DTS_CONV_16BIT:
    {
      dts_conv = DTS_CONV_16BIT;
      dec->set_dts_conv(dts_conv);
      update();
      break;
    }

    /////////////////////////////////////
    // Tray icon

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
    // Auto gain control

    case IDC_SLI_MASTER:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        master = db2value(-double(SendDlgItemMessage(m_Dlg, IDC_SLI_MASTER,TBM_GETPOS, 0, 0))/ticks);
        proc->set_master(master);
        update();
      }
      break;

    case IDC_EDT_MASTER:
      if (message == CB_ENTER)
      {
        proc->set_master(db2value(edt_master.value));
        update();
      }
      break;

    case IDC_EDT_ATTACK:
      if (message == CB_ENTER)
      {
        attack = edt_attack.value;
        proc->set_attack(attack);
        update();
      }
      break;

    case IDC_EDT_RELEASE:
      if (message == CB_ENTER)
      {
        release = edt_release.value;
        proc->set_release(release);
        update();
      }
      break;


    /////////////////////////////////////
    // Gain controls

    case IDC_SLI_VOICE:
    case IDC_SLI_SUR:
    case IDC_SLI_LFE:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        clev   = db2value(-double(SendDlgItemMessage(m_Dlg, IDC_SLI_VOICE, TBM_GETPOS, 0, 0))/ticks);
        slev   = db2value(-double(SendDlgItemMessage(m_Dlg, IDC_SLI_SUR,   TBM_GETPOS, 0, 0))/ticks);
        lfelev = db2value(-double(SendDlgItemMessage(m_Dlg, IDC_SLI_LFE,   TBM_GETPOS, 0, 0))/ticks);
        proc->set_clev(clev);
        proc->set_slev(slev);
        proc->set_lfelev(lfelev);
        update();
      }
      break;

    case IDC_EDT_VOICE:
    case IDC_EDT_SUR:
    case IDC_EDT_LFE:
      if (message == CB_ENTER)
      {  
        clev   = db2value(edt_voice.value);
        slev   = db2value(edt_sur.value);
        lfelev = db2value(edt_lfe.value);
        proc->set_clev(clev);
        proc->set_slev(slev);
        proc->set_lfelev(lfelev);
        update();
      }
      break;

    /////////////////////////////////////
    // I/O Gains

    case IDC_SLI_IN_L:
    case IDC_SLI_IN_C:
    case IDC_SLI_IN_R:
    case IDC_SLI_IN_SL:
    case IDC_SLI_IN_SR:
    case IDC_SLI_IN_LFE:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        for (int ch = 0; ch < NCHANNELS; ch++)
          input_gains[ch] = db2value(-double(SendDlgItemMessage(m_Dlg, idc_slider_in[ch], TBM_GETPOS, 0, 0))/ticks);

        proc->set_input_gains(input_gains);
        update();
      }
      break;

    case IDC_EDT_IN_L:
    case IDC_EDT_IN_C:
    case IDC_EDT_IN_R:
    case IDC_EDT_IN_SL:
    case IDC_EDT_IN_SR:
    case IDC_EDT_IN_LFE:
      if (message == CB_ENTER)
      {
        for (int ch = 0; ch < NCHANNELS; ch++)
          input_gains[ch] = db2value(edt_in_gains[ch].value);

        proc->set_input_gains(input_gains);
        update();
      }
      break;

    case IDC_SLI_OUT_L:
    case IDC_SLI_OUT_C:
    case IDC_SLI_OUT_R:
    case IDC_SLI_OUT_SL:
    case IDC_SLI_OUT_SR:
    case IDC_SLI_OUT_LFE:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        for (int ch = 0; ch < NCHANNELS; ch++)
          output_gains[ch] = db2value(-double(SendDlgItemMessage(m_Dlg, idc_slider_out[ch], TBM_GETPOS, 0, 0))/ticks);

        proc->set_output_gains(output_gains);
        update();
      }
      break;

    case IDC_EDT_OUT_L:
    case IDC_EDT_OUT_C:
    case IDC_EDT_OUT_R:
    case IDC_EDT_OUT_SL:
    case IDC_EDT_OUT_SR:
    case IDC_EDT_OUT_LFE:
      if (message == CB_ENTER)
      {
        for (int ch = 0; ch < NCHANNELS; ch++)
          output_gains[ch] = db2value(edt_out_gains[ch].value);

        proc->set_output_gains(output_gains);
        update();
      }
      break;


    /////////////////////////////////////
    // Delay

    case IDC_CHK_DELAYS:
    {
      delay = (SendDlgItemMessage(m_Dlg, IDC_CHK_DELAYS, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_delay(delay);
      update();
      break;
    }

    case IDC_EDT_DL:
    case IDC_EDT_DC:
    case IDC_EDT_DR:
    case IDC_EDT_DSL:
    case IDC_EDT_DSR:
    case IDC_EDT_DLFE:
      if (message == CB_ENTER)
      {
        for (int ch = 0; ch < NCHANNELS; ch++)
          delays[ch] = (float)edt_delay[ch].value;

        proc->set_delays(delays);
        update();
      }
      break;

    case IDC_CMB_UNITS:
      if (message == CBN_SELENDOK)
      {
        delay_units = list2units(SendDlgItemMessage(m_Dlg, IDC_CMB_UNITS, CB_GETCURSEL, 0, 0));
        proc->set_delay_units(delay_units);
        update();
      }
      break;

    /////////////////////////////////////
    // Syncronization

    case IDC_EDT_TIME_SHIFT:
      if (message == CB_ENTER)
      {
        time_shift = vtime_t(edt_time_shift.value) / 1000;
        dec->set_time_shift(time_shift);
        update();
      }
      break;

    case IDC_SLI_TIME_SHIFT:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        time_shift = vtime_t(SendDlgItemMessage(m_Dlg, IDC_SLI_TIME_SHIFT, TBM_GETPOS, 0, 0)) / 1000;
        dec->set_time_shift(time_shift);
        update();
      }
      break;

    case IDC_CHK_JITTER:
    {
      dejitter = (SendDlgItemMessage(m_Dlg, IDC_CHK_JITTER, BM_GETCHECK, 0, 0) == BST_CHECKED);
      dec->set_dejitter(dejitter);
      update();
      break;
    }


    /////////////////////////////////////
    // DRC

    case IDC_CHK_DRC:
    {
      drc = (SendDlgItemMessage(m_Dlg, IDC_CHK_DRC, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_drc(drc);
      update();
      break;
    }

    case IDC_SLI_DRC_POWER:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        drc_power = -double(SendDlgItemMessage(m_Dlg, IDC_SLI_DRC_POWER, TBM_GETPOS, 0, 0)) / ticks;
        proc->set_drc_power(drc_power);
        update();
      }
      break;

    case IDC_EDT_DRC_POWER:
      if (message == CB_ENTER)
      {
        drc_power = edt_drc_power.value;
        proc->set_drc_power(drc_power);
        update();
      }
      break;


    /////////////////////////////////////
    // Bass redirection

    case IDC_CHK_BASS_REDIR:
    {
      bass_redir = (SendDlgItemMessage(m_Dlg, IDC_CHK_BASS_REDIR, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_bass_redir(bass_redir);
      update();
      break;
    }

    case IDC_EDT_BASS_FREQ:
      if (message == CB_ENTER)
      {
        bass_freq = (int)edt_bass_freq.value;
        proc->set_bass_freq(bass_freq);
        update();
      }
      break;


    /////////////////////////////////////
    // Flags

    case IDC_CHK_AUTO_MATRIX:
    {
      auto_matrix = (SendDlgItemMessage(m_Dlg, IDC_CHK_AUTO_MATRIX, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_auto_matrix(auto_matrix);
      refresh = true;
      update();
      break;
    }

    case IDC_CHK_NORM_MATRIX:
    {
      normalize_matrix = (SendDlgItemMessage(m_Dlg, IDC_CHK_NORM_MATRIX, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_normalize_matrix(normalize_matrix);
      update();
      break;
    }

    case IDC_CHK_AUTO_GAIN:
    {
      auto_gain = (SendDlgItemMessage(m_Dlg, IDC_CHK_AUTO_GAIN, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_auto_gain(auto_gain);
      update();
      break;
    }

    case IDC_CHK_NORMALIZE:
    {
      normalize = (SendDlgItemMessage(m_Dlg, IDC_CHK_NORMALIZE, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_normalize(normalize);
      update();
      break;
    }

    case IDC_CHK_EXPAND_STEREO:
    {
      expand_stereo = (SendDlgItemMessage(m_Dlg, IDC_CHK_EXPAND_STEREO, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_expand_stereo(expand_stereo);
      update();
      break;
    }

    case IDC_CHK_VOICE_CONTROL:
    {
      voice_control = (SendDlgItemMessage(m_Dlg, IDC_CHK_VOICE_CONTROL, BM_GETCHECK, 0, 0) == BST_CHECKED);
      proc->set_voice_control(voice_control);
      update();
      break;
    }

    /////////////////////////////////////
    // Preset

    case IDC_CMB_PRESET:
      if (message == CBN_SELENDOK)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, CB_GETLBTEXT, SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, CB_GETCURSEL, 0, 0), (LONG)preset);
        SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, WM_SETTEXT, 0, (LONG)preset);
        sprintf(buf, REG_KEY_PRESET"\\%s", preset);

        RegistryKey reg(buf);
        dec->load_params(&reg, AC3FILTER_ALL);
        update();
      }
      if (message == CB_ENTER)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, WM_GETTEXT, 256, (LONG)preset);
        sprintf(buf, REG_KEY_PRESET"\\%s", preset);

        RegistryKey reg;
        reg.create_key(buf);
        dec->save_params(&reg, AC3FILTER_PRESET);
        update();
      }

      break;

    case IDC_BTN_PRESET_SAVE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, WM_GETTEXT, 256, (LONG)preset);
      sprintf(buf, REG_KEY_PRESET"\\%s", preset);

      RegistryKey reg;
      reg.create_key(buf);
      dec->save_params(&reg, AC3FILTER_PRESET);
      update();
      break;
    }

    case IDC_BTN_PRESET_DELETE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, WM_GETTEXT, 256, (LONG)preset);

      sprintf(buf, "Are you sure you want to delete '%s' preset?", preset);
      if (MessageBox(m_Dlg, buf, "Delete confirmation", MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {     
        sprintf(buf, REG_KEY_PRESET"\\%s", preset);
        delete_reg_key(buf, HKEY_CURRENT_USER);
        SendDlgItemMessage(m_Dlg, IDC_CMB_PRESET, WM_SETTEXT, 0, (LONG)"");
        update();
      }
      break;
    }
/*
    case IDC_BTN_PRESET_FILE:
    {
      char filename[MAX_PATH];

      if FAILED(dec->get_config_file(filename, MAX_PATH))
        filename[0] = 0;

      FileDlg dlg(m_Dlg, filename);
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
    // Matrix combo box 

    case IDC_CMB_MATRIX_PRESET:
      if (message == CBN_SELENDOK)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, CB_GETLBTEXT, SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, CB_GETCURSEL, 0, 0), (LONG)preset);
        SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, WM_SETTEXT, 0, (LONG)preset);
        sprintf(buf, REG_KEY_MATRIX"\\%s", preset);

        proc->set_auto_matrix(false);
        RegistryKey reg(buf);
        dec->load_params(&reg, AC3FILTER_MATRIX);
        update();
      }
      if (message == CB_ENTER)
      {
        char buf[256];
        char preset[256];
        SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, WM_GETTEXT, 256, (LONG)preset);
        sprintf(buf, REG_KEY_MATRIX"\\%s", preset);

        RegistryKey reg;
        reg.create_key(buf);
        dec->save_params(&reg, AC3FILTER_MATRIX);
        update();
      }
      break;

    case IDC_BTN_MATRIX_SAVE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, WM_GETTEXT, 256, (LONG)preset);
      sprintf(buf, REG_KEY_MATRIX"\\%s", preset);

      RegistryKey reg;
      reg.create_key(buf);
      dec->save_params(&reg, AC3FILTER_MATRIX);
      update();
      break;
    }

    case IDC_BTN_MATRIX_DELETE:
    {
      char buf[256];
      char preset[256];
      SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, WM_GETTEXT, 256, (LONG)preset);

      sprintf(buf, "Are you sure you want to delete '%s' matrix?", preset);
      if (MessageBox(m_Dlg, buf, "Delete confirmation", MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {     
        sprintf(buf, REG_KEY_MATRIX"\\%s", preset);
        delete_reg_key(buf, HKEY_CURRENT_USER);
        SendDlgItemMessage(m_Dlg, IDC_CMB_MATRIX_PRESET, WM_SETTEXT, 0, (LONG)"");
        proc->set_auto_matrix(true);
        update();
      }
      break;
    }

    /////////////////////////////////////
    // Language

    case IDC_CMB_LANG:
      if (message == CBN_SELENDOK)
      {
        int ilang;
        char new_lang[256];
        new_lang[0] = 0;

        ilang = SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_GETCURSEL, 0, 0);
        if (ilang != 0)
          SendDlgItemMessage(m_Dlg, IDC_CMB_LANG, CB_GETLBTEXT, ilang, (LONG)new_lang);
        set_lang(new_lang);
        break;
      }
    /////////////////////////////////////
    // Merit 

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
      }
      break;

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
      }
      break;

    case IDC_RBT_MERIT_PREFERRED:
      if (message == BN_CLICKED)
      {
        set_merit(HKEY_CLASSES_ROOT, "CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\{A753A1EC-973E-4718-AF8E-A3F554D45C44}", 0x10000000);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
      }
      break;

    case IDC_RBT_MERIT_UNLIKELY:
      if (message == BN_CLICKED)
      {
        set_merit(HKEY_CLASSES_ROOT, "CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\{A753A1EC-973E-4718-AF8E-A3F554D45C44}", MERIT_UNLIKELY);
        // Clear filter cache
        delete_reg_key("Software\\Microsoft\\Multimedia\\ActiveMovie\\Filter Cache", HKEY_CURRENT_USER);

        update();
      }
      break;
  }
}


