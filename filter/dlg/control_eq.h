/*
  Equalizer controls
*/

#ifndef CONTROL_EQ_H
#define CONTROL_EQ_H

#include "../guids.h"
#include "../controls.h"

#define EQ_BANDS 10

class ControlEq : public Controller
{
protected:
  IAudioProcessor *proc;

  bool eq;
  int eq_ch;
  size_t nbands;
  EqBand bands[EQ_BANDS];
  DoubleEdit edt_gain[EQ_BANDS];
  Speakers out_spk;

  void init_channels_list();

public:
  ControlEq(HWND dlg, IAudioProcessor *proc);
  ~ControlEq();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
