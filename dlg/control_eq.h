/*
  Equalizer controls
*/

#ifndef CONTROL_EQ_H
#define CONTROL_EQ_H

#include "../guids.h"
#include "../controls.h"

class ControlEq : public Controller
{
protected:
  IAudioProcessor *proc;

  bool eq;
  int freq[EQ_BANDS];
  double gain[EQ_BANDS];
  DoubleEdit edt_gain[EQ_BANDS];

public:
  ControlEq(HWND dlg, IAudioProcessor *proc);
  ~ControlEq();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
