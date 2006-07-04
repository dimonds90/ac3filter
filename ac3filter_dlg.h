#ifndef AC3FILTER_DLG_H
#define AC3FILTER_DLG_H

#include <streams.h>
#include "controls.h"
#include "guids.h"


          
class AC3FilterDlg : public CBasePropertyPage, public AudioProcessorState
{
public:
  static CUnknown * WINAPI CreateMain  (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateMixer (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateGains (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateSystem(LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateAbout (LPUNKNOWN lpunk, HRESULT *phr);

  int flags;

  void reload_state();

private:
  IAC3Filter      *filter;
  IDecoder        *dec;
  IAudioProcessor *proc;

  bool     visible;
  bool     refresh;
  int      refresh_time;

  Speakers in_spk;
  Speakers out_spk;

  Speakers user_spk;
  bool     use_spdif;
  int      spdif_pt;
  bool     spdif_stereo_pt;
  bool     spdif_as_pcm;
  int      spdif_status;

  int      formats;

  int      frames;
  int      errors;

  Speakers old_in_spk;
  int      old_spdif_status;
  matrix_t old_matrix;
  char     old_info[1024];
  char     old_jitter[128];

  // Matrix
  DoubleEdit  edt_matrix[NCHANNELS][NCHANNELS];
  // AGC
  DoubleEdit  edt_attack;
  DoubleEdit  edt_release;
  // Gain control
  DoubleEdit  edt_master;
  DoubleEdit  edt_gain;
  DoubleEdit  edt_voice;
  DoubleEdit  edt_sur;
  DoubleEdit  edt_lfe;
  // I/O Gains
  DoubleEdit  edt_in_gains[NCHANNELS];
  DoubleEdit  edt_out_gains[NCHANNELS];
  // DRC
  DoubleEdit  edt_drc_power;
  DoubleEdit  edt_drc_level;
  // Bass redirection
  DoubleEdit  edt_bass_freq;
  // Delay
  DoubleEdit  edt_delay[NCHANNELS];
  DoubleEdit  edt_time_shift;
  // Links
  LinkButton  lnk_home;
  LinkButton  lnk_forum;
  LinkButton  lnk_email;
  LinkButton  lnk_donate;
  // Images
  HANDLE      logo;

  AC3FilterDlg(TCHAR *pName, LPUNKNOWN lpunk, HRESULT *phr, int DialogId, int TitleId, int flags);

  BOOL OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  HRESULT OnConnect(IUnknown *pUnknown);
  HRESULT OnDisconnect();
  HRESULT OnActivate();
  HRESULT OnDeactivate();

  void update();
  void init_controls();
  void set_dynamic_controls();
  void set_controls();
  void set_matrix_controls();
  void set_cpu_usage();
  void set_logo();

  void command(int control, int message);
};

#endif
