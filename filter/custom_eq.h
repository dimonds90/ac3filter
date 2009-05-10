#ifndef CUSTOM_EQ
#define CUSTOM_EQ

#include "dlg_base.h"
#include "controls.h"
#include "spectrum_ctrl.h"
#include "buffer.h"
#include "dsp/fft.h"
#include "fir/eq_fir.h"


class CustomEq : public DialogBase
{
protected:
  size_t nbands;
  AutoBuf<EqBand> bands;
  EqFIR eq;

  IntEdit edt_freq[10];
  DoubleEdit edt_gain[10];
  IntEdit edt_length;
  bool log_scale;

  FFT fft;
  Samples buf;
  SpectrumCtrl spectrum;

  void update();

  virtual void on_create();
  virtual BOOL on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  CustomEq(): DialogBase() {}

  size_t get_nbands() const;
  size_t set_bands(const EqBand *bands, size_t nbands);
  size_t get_bands(EqBand *bands, size_t first_band, size_t nbands) const;
  void reset();
};

#endif
