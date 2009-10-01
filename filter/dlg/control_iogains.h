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

  sample_t input_gains[CH_NAMES];
  sample_t output_gains[CH_NAMES];

  DoubleEdit edt_in_gains[8];
  DoubleEdit edt_out_gains[8];

public:
  ControlIOGains(HWND dlg, IAudioProcessor *proc);
  ~ControlIOGains();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
