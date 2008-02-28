/*
  Input/output gains
*/

#ifndef CONTROL_IOGAINS_H
#define CONTROL_IOGAINS_H

#include "../guids.h"
#include "../controls.h"

class ControlIOGains : public Controller
{
protected:
  IAudioProcessor *proc;

  sample_t input_gains[NCHANNELS];
  sample_t output_gains[NCHANNELS];

  DoubleEdit edt_in_gains[NCHANNELS];
  DoubleEdit edt_out_gains[NCHANNELS];

public:
  ControlIOGains(HWND dlg, IAudioProcessor *proc);
  ~ControlIOGains();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
