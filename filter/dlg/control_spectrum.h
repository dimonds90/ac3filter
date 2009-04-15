/*
  Spectrum controls
*/

#ifndef CONTROL_SPECTRUM_H
#define CONTROL_SPECTRUM_H

#include "../guids.h"
#include "../controls.h"
#include "../spectrum_ctrl.h"
#include "buffer.h"

class ControlSpectrum : public Controller
{
protected:
  IAudioProcessor *proc;

  SpectrumCtrl spectrum;

  unsigned spectrum_length;
  Samples spectrum_buf;
  double bin2hz;
  bool log_scale;

public:
  ControlSpectrum(HWND dlg, IAudioProcessor *proc);
  ~ControlSpectrum();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
