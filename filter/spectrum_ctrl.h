#ifndef SPECTRUM_CTRL_H
#define SPECTRUM_CTRL_H

#include "defs.h"
#include "controls.h"

class SpectrumCtrl : public SubclassedControl
{
protected:
  RECT client_rect;
  int width, height;

  double min_db, max_db, db_range;
  double min_hz, max_hz, hz_range;

  double xfactor, yfactor;
  bool log_scale;

  HDC     mem_dc;
  HBITMAP mem_bitmap;
  HBITMAP old_bitmap;

  HBRUSH bkg_brush;
  HPEN   signal_pen;
  HPEN   minor_pen;
  HPEN   major_pen;

  HFONT  grid_font;
  HFONT  title_font;

  inline int db2y(double db);
  inline int hz2x_lin(double hz);
  inline int hz2x_log(double hz);

  virtual void on_link();
  virtual void on_unlink();
  virtual LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
  SpectrumCtrl();

  void set_range(double min_db, double max_db, double min_hz, double max_hz);
  void draw_lin(sample_t *spectrum, size_t length, double bin2hz, const char *title = 0);
  void draw_log(sample_t *spectrum, size_t length, double bin2hz, const char *title = 0);
};

#endif
