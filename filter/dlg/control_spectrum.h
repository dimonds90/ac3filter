/*
  Spectrum controls
*/

#ifndef CONTROL_SPECTRUM_H
#define CONTROL_SPECTRUM_H

#include "../guids.h"
#include "../controls.h"
#include "../spectrum_ctrl.h"
#include "dsp/fft.h"
#include "buffer.h"

class ControlSpectrum : public Controller
{
protected:
  IAC3Filter *filter;
  IAudioProcessor *proc;

  FFT fft;
  Samples buf;
  Samples win;

  SpectrumCtrl spectrum;

  Speakers spk;
  unsigned length;
  bool log_scale;

  void init_spectrum(unsigned length);
  bool is_ok() const { return !spk.is_unknown(); }

public:
  ControlSpectrum(HWND dlg, IAC3Filter *filter, IAudioProcessor *proc);
  ~ControlSpectrum();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
