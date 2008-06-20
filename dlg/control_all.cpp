#include "control_all.h"
#include "control_spk.h"
#include "control_matrix.h"
#include "control_delay.h"
#include "control_eq.h"
#include "control_spectrum.h"
#include "control_agc.h"
#include "control_spdif.h"
#include "control_iogains.h"
#include "control_levels.h"
#include "control_preset.h"
#include "control_system.h"
#include "control_lang.h"
#include "control_sync.h"
#include "control_bass.h"
#include "control_info.h"
#include "control_ver.h"
#include "control_about.h"

///////////////////////////////////////////////////////////////////////////////

ControlAll::ControlAll(HWND _dlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels):
Controller(_dlg, 0)
{
  ctrl.push_back(PCtrl(new ControlSpk(hdlg, dec)));
  ctrl.push_back(PCtrl(new ControlMatrix(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlDelay(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlIOGains(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlPreset(hdlg, dec, proc)));
  ctrl.push_back(PCtrl(new ControlLevels(hdlg, filter, proc, invert_levels)));
  ctrl.push_back(PCtrl(new ControlAGC(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlEq(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlSpectrum(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlSPDIF(hdlg, dec)));
  ctrl.push_back(PCtrl(new ControlSystem(hdlg, filter, dec)));
  ctrl.push_back(PCtrl(new ControlLang(hdlg)));
  ctrl.push_back(PCtrl(new ControlSync(hdlg, dec)));
  ctrl.push_back(PCtrl(new ControlBass(hdlg, proc)));
  ctrl.push_back(PCtrl(new ControlInfo(hdlg, dec)));
  ctrl.push_back(PCtrl(new ControlVer(hdlg)));
  ctrl.push_back(PCtrl(new ControlAbout(hdlg)));
}

void ControlAll::init()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    (*iter)->init();
};

void ControlAll::update()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    (*iter)->update();
};

void ControlAll::update_dynamic()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    (*iter)->update_dynamic();
};

bool ControlAll::own_control(int control)
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    if ((*iter)->own_control(control))
      return true;
  return false;
}

ControlAll::cmd_result ControlAll::command(int control, int message)
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    if ((*iter)->own_control(control))
      return (*iter)->command(control, message);
  return cmd_not_processed;
}
