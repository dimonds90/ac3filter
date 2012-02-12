#include <math.h>
#include <stdio.h>
#include "ac3filter_intl.h"
#include "spectrum_ctrl.h"

static const DWORD bkg_color = RGB(0, 0, 0);
static const DWORD signal_color = RGB(0, 255, 255);
static const DWORD minor_color = RGB(32, 32, 32);
static const DWORD major_color = RGB(64, 64, 64);
static const DWORD label_color = RGB(128, 128, 0);

static const int grid_font_size = 12;
static const char *grid_font_name = "Arial";

static const int title_font_size = 16;
static const char *title_font_name = "Arial";

///////////////////////////////////////////////////////////////////////////////

SpectrumCtrl::SpectrumCtrl():
  min_db(-12), max_db(12), db_range(24),
  min_hz(0), max_hz(24000), hz_range(24000),
  xfactor(0), yfactor(0)
{}

inline int SpectrumCtrl::db2y(double db)
{ return (int)((max_db - db) * yfactor); }

inline int SpectrumCtrl::hz2x_lin(double hz)
{ return (int)((hz - min_hz) * xfactor); }

inline int SpectrumCtrl::hz2x_log(double hz)
{ return (int)(log(hz / min_hz) * width / log(max_hz/min_hz)); }

void
SpectrumCtrl::set_range(double _min_db, double _max_db, double _min_hz, double _max_hz)
{
  min_db = _min_db;
  max_db = _max_db;
  db_range = max_db - min_db;

  min_hz = _min_hz;
  max_hz = _max_hz;
  hz_range = max_hz - min_hz;

  xfactor = hz_range > 0? double(width) / hz_range: 0;
  yfactor = db_range > 0? double(height) / db_range: 0;
}

void
SpectrumCtrl::on_link()
{
  if (!hwnd) return;

  GetClientRect(hwnd, &client_rect);
  width = client_rect.right - client_rect.left;
  height = client_rect.bottom - client_rect.top;

  xfactor = hz_range > 0? double(width) / hz_range: 0;
  yfactor = db_range > 0? double(height) / db_range: 0;

  HDC wnd_dc = GetDC(hwnd);
  mem_dc = CreateCompatibleDC(wnd_dc);
  mem_bitmap = CreateCompatibleBitmap(wnd_dc, width, height);
  old_bitmap = (HBITMAP)SelectObject(mem_dc, mem_bitmap);
  ReleaseDC(hwnd, wnd_dc);

  bkg_brush = CreateSolidBrush(bkg_color);
  signal_pen = CreatePen(PS_SOLID, 1, signal_color);
  minor_pen = CreatePen(PS_SOLID, 1, minor_color);
  major_pen = CreatePen(PS_SOLID, 1, major_color);
  grid_font = CreateFont(grid_font_size, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, grid_font_name);
  title_font = CreateFont(title_font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, title_font_name);
}

void
SpectrumCtrl::on_unlink()
{
  if (!hwnd) return;
  DeleteObject(bkg_brush);
  DeleteObject(signal_pen);
  DeleteObject(minor_pen);
  DeleteObject(major_pen);
  DeleteObject(grid_font);
  DeleteObject(title_font);
  SelectObject(mem_dc, old_bitmap);
  DeleteObject(mem_bitmap);
  DeleteDC(mem_dc);
}


void
SpectrumCtrl::draw_lin(sample_t *spectrum, size_t length, double bin2hz, const char *title)
{
  if (!hwnd)
    return;

  char label[32];
  HPEN old_pen;
  HFONT old_font;
  int x, y;
  double db, hz;

  FillRect(mem_dc, &client_rect, bkg_brush);

  int minor_db_height = 6;
  int minor_db_lines = 3;

  double minor_db = int(db_range * minor_db_height + height - 1) / height;
  double major_db = minor_db * minor_db_lines;

  double minor_hz = 500;
  double major_hz = 2000;

  /////////////////////////////////////////////////////////
  // Grid

  old_pen = (HPEN)SelectObject(mem_dc, minor_pen);

  for (db = int(min_db / minor_db) * minor_db; db < max_db; db += minor_db)
  {
    y = db2y(db);
    MoveToEx(mem_dc, 0, y, 0);
    LineTo(mem_dc, width, y);
  }

  for (hz = int(min_hz / minor_hz) * minor_hz; hz < max_hz; hz += minor_hz)
  {
    x = hz2x_lin(hz);
    MoveToEx(mem_dc, x, 0, 0);
    LineTo(mem_dc, x, height);
  }

  SelectObject(mem_dc, major_pen);

  for (db = int(min_db / major_db) * major_db; db < max_db; db += major_db)
  {
    y = db2y(db);
    MoveToEx(mem_dc, 0, y, 0);
    LineTo(mem_dc, width, y);
  }

  for (hz = int(min_hz / major_hz) * major_hz; hz < max_hz; hz += major_hz)
  {
    x = hz2x_lin(hz);
    MoveToEx(mem_dc, x, 0, 0);
    LineTo(mem_dc, x, height);
  }

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Labels

  SetTextColor(mem_dc, label_color);
  SetBkColor(mem_dc, bkg_color);
  SetBkMode(mem_dc, OPAQUE);

  old_font = (HFONT)SelectObject(mem_dc, title_font);
  SetTextAlign(mem_dc, TA_TOP | TA_LEFT);

  if (title)
    TextOut(mem_dc, 10, 12, title, (int)strlen(title));

  SelectObject(mem_dc, grid_font);
  SetTextAlign(mem_dc, TA_TOP | TA_CENTER);

  for (hz = int(min_hz / major_hz + 1) * major_hz; hz < max_hz; hz += major_hz)
  {
    x = hz2x_lin(hz);
    sprintf(label, "%i%s", int(hz / 1000), _("kHz"));
    TextOut(mem_dc, x, 0, label, (int)strlen(label));
  }

  SetTextAlign(mem_dc, TA_BOTTOM | TA_RIGHT);
  for (db = int(min_db / major_db) * major_db; db < max_db; db += major_db)
  {
    y = db2y(db);
    sprintf(label, "%+i%s", int(db), _("dB"));
    TextOut(mem_dc, width, y, label, (int)strlen(label));
  }

  SelectObject(mem_dc, old_font);

  /////////////////////////////////////////////////////////
  // Spectrum

  if (spectrum && length)
  {
    old_pen = (HPEN)SelectObject(mem_dc, signal_pen);

    db = spectrum[1] > 0? value2db(spectrum[1]): min_db;
    MoveToEx(mem_dc, 0, db2y(db), 0);
    for (size_t i = 2; i < length; i++)
    {
      x = hz2x_lin(i * bin2hz);
      y = db2y(spectrum[i] > 0? value2db(spectrum[i]): min_db);
      LineTo(mem_dc, x, y);
    }

    SelectObject(mem_dc, old_pen);
  }

  /////////////////////////////////////////////////////////
  // Show the result

  HDC wnd_dc = GetDC(hwnd);
  BitBlt(wnd_dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(hwnd, wnd_dc);
}

void
SpectrumCtrl::draw_log(sample_t *spectrum, size_t length, double bin2hz, const char *title)
{
  if (!hwnd)
    return;

  char label[32];
  HPEN old_pen;
  HFONT old_font;
  int x, y;
  double db, hz;

  FillRect(mem_dc, &client_rect, bkg_brush);

  int minor_db_height = 6;
  int minor_db_lines = 3;

  double minor_db = int(db_range * minor_db_height + height - 1) / height;
  double major_db = minor_db * minor_db_lines;

  if (min_hz <= 0) min_hz = max_hz / 1000;
  int minor_hz_lines = 10;
  int minor_hz_start = (int)pow(
    (double)minor_hz_lines, 
    (int)(log(min_hz) / log((double)minor_hz_lines)));

  /////////////////////////////////////////////////////////
  // Grid

  old_pen = (HPEN)SelectObject(mem_dc, minor_pen);

  for (db = int(min_db / minor_db) * minor_db; db < max_db; db += minor_db)
  {
    y = db2y(db);
    MoveToEx(mem_dc, 0, y, 0);
    LineTo(mem_dc, width, y);
  }

  for (hz = minor_hz_start; hz < max_hz; hz *= minor_hz_lines)
    for (int i = 0; i < minor_hz_lines; i++)
    {
      x = hz2x_log(hz + i * hz);
      MoveToEx(mem_dc, x, 0, 0);
      LineTo(mem_dc, x, height);
    }

  SelectObject(mem_dc, major_pen);

  for (db = int(min_db / major_db) * major_db; db < max_db; db += major_db)
  {
    y = db2y(db);
    MoveToEx(mem_dc, 0, y, 0);
    LineTo(mem_dc, width, y);
  }

  for (hz = minor_hz_start; hz < max_hz; hz *= minor_hz_lines)
  {
    x = hz2x_log(hz);
    MoveToEx(mem_dc, x, 0, 0);
    LineTo(mem_dc, x, height);
  }

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Labels

  SetTextColor(mem_dc, label_color);
  SetBkColor(mem_dc, bkg_color);
  SetBkMode(mem_dc, OPAQUE);

  old_font = (HFONT)SelectObject(mem_dc, title_font);
  SetTextAlign(mem_dc, TA_TOP | TA_LEFT);

  if (title)
    TextOut(mem_dc, 10, 12, title, (int)strlen(title));

  SelectObject(mem_dc, grid_font);
  SetTextAlign(mem_dc, TA_TOP | TA_CENTER);

  for (hz = minor_hz_start; hz < max_hz; hz *= minor_hz_lines)
  {
    x = hz2x_log(hz);
    if (hz >= 1000)
      sprintf(label, "%i%s", int(hz / 1000), _("kHz"));
    else
      sprintf(label, "%i%s", int(hz), _("Hz"));
    TextOut(mem_dc, x, 0, label, (int)strlen(label));
  }

  SetTextAlign(mem_dc, TA_BOTTOM | TA_RIGHT);
  for (db = int(min_db / major_db) * major_db; db < max_db; db += major_db)
  {
    y = db2y(db);
    sprintf(label, "%+i%s", int(db), _("dB"));
    TextOut(mem_dc, width, y, label, (int)strlen(label));
  }

  SelectObject(mem_dc, old_font);

  /////////////////////////////////////////////////////////
  // Spectrum

  if (spectrum && length)
  {
    old_pen = (HPEN)SelectObject(mem_dc, signal_pen);

    db = spectrum[1] > 0? value2db(spectrum[1]): min_db;
    MoveToEx(mem_dc, hz2x_log(1 * bin2hz), db2y(db), 0);
    for (size_t i = 2; i < length; i++)
    {
      x = hz2x_log(i * bin2hz);
      y = db2y(spectrum[i] > 0? value2db(spectrum[i]): min_db);
      LineTo(mem_dc, x, y);
    }

    SelectObject(mem_dc, old_pen);
  }

  /////////////////////////////////////////////////////////
  // Show the result

  HDC wnd_dc = GetDC(hwnd);
  BitBlt(wnd_dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(hwnd, wnd_dc);
}

LRESULT CALLBACK
SpectrumCtrl::wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (msg == WM_PAINT)
  {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);
    BitBlt(dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
    EndPaint(hwnd, &ps);
  }
  return SubclassedControl::wndproc(hwnd, msg, wParam, lParam);
}
