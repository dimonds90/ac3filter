/*
  Syncronization and jitter correction
*/

#ifndef CONTROL_SYNC_H
#define CONTROL_SYNC_H

#include "../guids.h"
#include "../controls.h"

class ControlSync : public Controller
{
protected:
  IDecoder *dec;

  vtime_t  time_shift;
  vtime_t  time_factor;
  bool     dejitter;
  vtime_t  threshold;

  vtime_t  input_mean;
  vtime_t  input_stddev;
  vtime_t  output_mean;
  vtime_t  output_stddev;
  char     jitter[128];

  DoubleEdit  edt_time_shift;

public:
  ControlSync(HWND dlg, IDecoder *dec);
  ~ControlSync();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
