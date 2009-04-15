#include <math.h>
#include <stdio.h>
#include "spectrum_ctrl.h"

static const double max_db = 12;
static const double min_db = -12;
static const double db_range = max_db - min_db;

static const double grid_hz = 1000;  // 1kHz grid step
static const double grid_db = 2;    //  2db grid step

static const int major_db = 2;  // major line grid step
static const int major_hz = 2;  // major line grid step

static const DWORD bkg_color = RGB(0, 0, 0);
static const DWORD signal_color = RGB(0, 255, 255);
static const DWORD minor_color = RGB(32, 32, 32);
static const DWORD major_color = RGB(64, 64, 64);
static const DWORD label_color = RGB(128, 128, 0);

static const int font_size = 12;
static const char *font_name = "Arial";

///////////////////////////////////////////////////////////////////////////////

SpectrumCtrl::SpectrumCtrl()
{}

void
SpectrumCtrl::on_link()
{
  if (!hwnd) return;

  GetClientRect(hwnd, &client_rect);
  width = client_rect.right - client_rect.left;
  height = client_rect.bottom - client_rect.top;

  HDC wnd_dc = GetDC(hwnd);
  mem_dc = CreateCompatibleDC(wnd_dc);
  mem_bitmap = CreateCompatibleBitmap(wnd_dc, width, height);
  old_bitmap = (HBITMAP)SelectObject(mem_dc, mem_bitmap);
  ReleaseDC(hwnd, wnd_dc);

  bkg_brush = CreateSolidBrush(bkg_color);
  signal_pen = CreatePen(PS_SOLID, 1, signal_color);
  minor_pen = CreatePen(PS_SOLID, 1, minor_color);
  major_pen = CreatePen(PS_SOLID, 1, major_color);
  grid_font = CreateFont(font_size, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, font_name);
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
  SelectObject(mem_dc, old_bitmap);
  DeleteObject(mem_bitmap);
  DeleteDC(mem_dc);
}

void
SpectrumCtrl::draw_lin(sample_t *spectrum, size_t length, double bin2hz)
{
  if (!hwnd || !spectrum || !length)
    return;

  char label[32];
  HPEN old_pen;
  HFONT old_font;
  int x, y, max_x, max_y;

  FillRect(mem_dc, &client_rect, bkg_brush);

  /////////////////////////////////////////////////////////
  // Grid

  old_pen = (HPEN)SelectObject(mem_dc, minor_pen);

  max_y = int(db_range / grid_db);
  for (y = 1; y < max_y; y++)
  {
    int pos = y * height / max_y;
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  max_x = int(length * bin2hz / grid_hz);
  for (x = 1; x < max_x; x++)
  {
    int pos = x * width / max_x;
    MoveToEx(mem_dc, pos, 0, 0);
    LineTo(mem_dc, pos, height);
  }

  SelectObject(mem_dc, major_pen);

  max_y = int(db_range / grid_db);
  for (y = major_db; y < max_y; y += major_db)
  {
    int pos = y * height / max_y;
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  max_x = int(length * bin2hz / grid_hz);
  for (x = major_hz; x < max_x; x += major_hz)
  {
    int pos = x * width / max_x;
    MoveToEx(mem_dc, pos, 0, 0);
    LineTo(mem_dc, pos, height);
  }

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Labels

  SetTextColor(mem_dc, label_color);
  SetBkColor(mem_dc, bkg_color);
  SetBkMode(mem_dc, OPAQUE);
  old_font = (HFONT)SelectObject(mem_dc, grid_font);

  SetTextAlign(mem_dc, TA_TOP | TA_CENTER);
  max_x = int(length * bin2hz / grid_hz);
  for (x = major_hz; x < max_x; x += major_hz)
  {
    int pos = x * width / max_x;
    sprintf(label, "%ikHz", int(x * grid_hz / 1000));
    TextOut(mem_dc, pos, 0, label, (int)strlen(label));
  }

  SetTextAlign(mem_dc, TA_BOTTOM | TA_RIGHT);
  max_y = int(db_range / grid_db);
  for (y = major_db; y <= max_y; y += major_db)
  {
    int pos = y * height / max_y;
    sprintf(label, "%+idB", int(-y * grid_db + max_db));
    TextOut(mem_dc, width, pos, label, (int)strlen(label));
  }

  SelectObject(mem_dc, old_font);

  /////////////////////////////////////////////////////////
  // Spectrum

  double xfactor = double(width) / double(length);
  double yfactor = double(height) / db_range;
  double val;

  old_pen = (HPEN)SelectObject(mem_dc, signal_pen);

  val = spectrum[0] > 0? -value2db(spectrum[0]) + max_db: db_range;
  MoveToEx(mem_dc, 0, int(val * yfactor), 0);
  for (size_t i = 1; i < length; i++)
  {
    val = spectrum[i] > 0? -value2db(spectrum[i]) + max_db: db_range;
    LineTo(mem_dc, int(i * xfactor), int(val * yfactor));
  }

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Show the result

  HDC wnd_dc = GetDC(hwnd);
  BitBlt(wnd_dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(hwnd, wnd_dc);
}

static int scale(size_t pos, size_t max, int width)
{
  return int(width + log10(double(pos)/double(max)) * width / 3);
}

void
SpectrumCtrl::draw_log(sample_t *spectrum, size_t length, double bin2hz)
{
  if (!hwnd || !spectrum || !length)
    return;

  char label[32];
  HPEN old_pen;
  HFONT old_font;
  int y, max_y, delta_hz;
  const int nyquist = int(length * bin2hz);

  FillRect(mem_dc, &client_rect, bkg_brush);

  /////////////////////////////////////////////////////////
  // Grid

  old_pen = (HPEN)SelectObject(mem_dc, minor_pen);

  max_y = int(db_range / grid_db);
  for (y = 1; y < max_y; y++)
  {
    int pos = y * height / max_y;
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  delta_hz = 1;
  while (delta_hz < 100000)
  {
    if (scale(delta_hz * 9, nyquist, width) > 0)
      for (int i = 1; i < 10; i++)
      {
        int pos = scale(i * delta_hz, nyquist, width);
        MoveToEx(mem_dc, pos, 0, 0);
        LineTo(mem_dc, pos, height);
      }
    delta_hz *= 10;
  }

  SelectObject(mem_dc, major_pen);

  max_y = int(db_range / grid_db);
  for (y = major_db; y < max_y; y+=major_db)
  {
    int pos = y * height / max_y;
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  delta_hz = 1;
  while (delta_hz < 100000)
  {
    if (scale(delta_hz, nyquist, width) > 0)
    {
      int pos = scale(delta_hz, nyquist, width);
      MoveToEx(mem_dc, pos, 0, 0);
      LineTo(mem_dc, pos, height);
    }
    delta_hz *= 10;
  }

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Labels

  SetTextColor(mem_dc, label_color);
  SetBkColor(mem_dc, bkg_color);
  SetBkMode(mem_dc, OPAQUE);
  old_font = (HFONT)SelectObject(mem_dc, grid_font);

  SetTextAlign(mem_dc, TA_TOP | TA_CENTER);
  delta_hz = 1;
  while (delta_hz < 100000)
  {
    if (scale(delta_hz, nyquist, width) > 0)
    {
      int pos = scale(delta_hz, nyquist, width);
      if (delta_hz >= 1000)
        sprintf(label, "%ikHz", delta_hz / 1000);
      else
        sprintf(label, "%iHz", delta_hz);
      TextOut(mem_dc, pos, 0, label, (int)strlen(label));
    }
    delta_hz *= 10;
  }

  SetTextAlign(mem_dc, TA_BOTTOM | TA_RIGHT);
  max_y = int(db_range / grid_db);
  for (y = major_db; y <= max_y; y+=major_db)
  {
    int pos = y * height / max_y;
    sprintf(label, "%+idB", int(-y * grid_db + max_db));
    TextOut(mem_dc, width, pos, label, (int)strlen(label));
  }

  SelectObject(mem_dc, old_font);

  /////////////////////////////////////////////////////////
  // Spectrum

  double xfactor = double(width) / double(length);
  double yfactor = double(height) / db_range;
  double val;

  old_pen = (HPEN)SelectObject(mem_dc, signal_pen);

  val = spectrum[0] > 0? -value2db(spectrum[0]) + max_db: db_range;
  MoveToEx(mem_dc, -1, int(val * yfactor), 0);
  for (size_t i = 1; i < length; i++)
  {
    val = spectrum[i] > 0? -value2db(spectrum[i]) + max_db: db_range;
    LineTo(mem_dc, scale(i, length, width), int(val * yfactor));
  }

  SelectObject(mem_dc, old_pen);

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
