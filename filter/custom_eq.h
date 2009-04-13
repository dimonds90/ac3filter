#include "dlg_base.h"
#include "controls.h"
#include "buffer.h"
#include "dsp/fft.h"
#include "fir/eq_fir.h"


class SpectrumCtrl : public SubclassedControl
{
protected:
  RECT client_rect;
  int width, height;

  HDC     mem_dc;
  HBITMAP mem_bitmap;
  HBITMAP old_bitmap;

  HBRUSH bkg_brush;
  HPEN   signal_pen;
  HPEN   grid_pen;
  HPEN   major_pen;

  HFONT  grid_font;

  virtual void on_link();
  virtual void on_unlink();
  virtual LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
  SpectrumCtrl();

  void draw_lin(sample_t *spectrum, size_t length, double bin2hz);
  void draw_log(sample_t *spectrum, size_t length, double bin2hz);
};

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
