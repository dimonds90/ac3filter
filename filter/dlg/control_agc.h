/*
  Automatic gain control and dynamic range compression
*/

#ifndef CONTROL_AGC_H
#define CONTROL_AGC_H

#include "../guids.h"
#include "../controls.h"

class ControlAGC : public Controller
{
protected:
  IAudioProcessor *proc;

  // AGC
  sample_t master, gain;
  bool auto_gain, normalize;
  sample_t attack, release;

  DoubleEdit  edt_master;
  DoubleEdit  edt_gain;
  DoubleEdit  edt_attack;
  DoubleEdit  edt_release;

  // DRC
  bool drc;
  sample_t drc_power, drc_level;

  DoubleEdit  edt_drc_power;
  DoubleEdit  edt_drc_level;

public:
  ControlAGC(HWND dlg, IAudioProcessor *proc);
  ~ControlAGC();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
