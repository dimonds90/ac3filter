#ifndef SPECTRUM_CTRL_H
#define SPECTRUM_CTRL_H

#include "defs.h"
#include "controls.h"

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
  HPEN   minor_pen;
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

#endif
