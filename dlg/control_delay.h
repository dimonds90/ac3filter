/*
  Delay controls
*/

#ifndef CONTROL_DELAY_H
#define CONTROL_DELAY_H

#include "../guids.h"
#include "../controls.h"

class ControlDelay : public Controller
{
protected:
  IAudioProcessor *proc;

  bool  delay;
  int   delay_units;
  float delays[NCHANNELS];

  DoubleEdit edt_delay[NCHANNELS];

public:
  ControlDelay(HWND dlg, IAudioProcessor *proc);
  ~ControlDelay();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
