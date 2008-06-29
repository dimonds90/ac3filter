#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "../resource_ids.h"
#include "control_spectrum.h"

static const int controls[] =
{
  IDC_SPECTRUM,
  IDC_CHK_EQ_LOG,
  0
};

const double db_range = 100; // 120dB range
const double grid_hz = 1000;  // 1kHz grid step
const double grid_db = 10;    // 10db grid step
const int major_db = 2;  // major line grid step
const int major_hz = 2;  // major line grid step

const DWORD bkg_color = RGB(0, 0, 0);
const DWORD signal_color = RGB(0, 255, 255);
const DWORD grid_color = RGB(32, 32, 32);
const DWORD major_color = RGB(64, 64, 64);
const DWORD label_color = RGB(128, 128, 0);

const int font_size = 12;
const char *font_name = "Arial";

ControlSpectrum::ControlSpectrum(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc), spectrum_length(0), spectrum(0)
{
  hctrl = GetDlgItem(hdlg, IDC_SPECTRUM);
  if (hctrl)
  {
    GetClientRect(hctrl, &client_rect);
    width = client_rect.right - client_rect.left;
    height = client_rect.bottom - client_rect.top;

    HDC ctrl_dc = GetDC(hctrl);
    mem_dc = CreateCompatibleDC(ctrl_dc);
    mem_bitmap = CreateCompatibleBitmap(ctrl_dc, width, height);
    old_bitmap = (HBITMAP)SelectObject(mem_dc, mem_bitmap);
    ReleaseDC(hctrl, ctrl_dc);

    bkg_brush = CreateSolidBrush(bkg_color);
    signal_pen = CreatePen(PS_SOLID, 1, signal_color);
    grid_pen = CreatePen(PS_SOLID, 1, grid_color);
    major_pen = CreatePen(PS_SOLID, 1, major_color);
    grid_font = CreateFont(font_size, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, font_name);

    log_scale = false;
    RegistryKey reg(REG_KEY);
    reg.get_bool("log_scale", log_scale);

    proc->set_spectrum_length(1024);
  }

  proc->AddRef();
}

ControlSpectrum::~ControlSpectrum()
{
  if (hctrl)
  {
    DeleteObject(bkg_brush);
    DeleteObject(signal_pen);
    DeleteObject(grid_pen);
    DeleteObject(major_pen);
    DeleteObject(grid_font);

    SelectObject(mem_dc, old_bitmap);
    DeleteObject(mem_bitmap);
    DeleteDC(mem_dc);

    proc->set_spectrum_length(0);
  }

  proc->Release();
}

void ControlSpectrum::init()
{
  CheckDlgButton(hdlg, IDC_CHK_EQ_LOG, log_scale? BST_CHECKED: BST_UNCHECKED);
}

void ControlSpectrum::update()
{
}

void ControlSpectrum::update_dynamic()
{
  if (hctrl == 0)
    return;

  size_t new_spectrum_length;
  proc->get_spectrum_length(&new_spectrum_length);
  if (spectrum_length != new_spectrum_length)
  {
    safe_delete(spectrum);
    spectrum_length = new_spectrum_length;
    if (spectrum_length)
      spectrum = new sample_t[spectrum_length];
  }

  if (spectrum)
  {
    proc->get_spectrum(spectrum, &bin2hz);
    for (size_t i = 0; i < spectrum_length; i++)
      spectrum[i] = value2db(spectrum[i]);

    if (log_scale)
      paint_log();
    else
      paint_linear();
  }
}

void ControlSpectrum::paint_linear()
{
  char label[32];
  HPEN old_pen;
  HFONT old_font;
  int x, y, max_x, max_y;

  FillRect(mem_dc, &client_rect, bkg_brush);

  /////////////////////////////////////////////////////////
  // Grid

  old_pen = (HPEN)SelectObject(mem_dc, grid_pen);

  max_y = int(db_range / grid_db);
  for (y = 1; y < max_y; y++)
  {
    int pos = y * height / max_y;
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  max_x = int(spectrum_length * bin2hz / grid_hz);
  for (x = 1; x < max_x; x++)
  {
    int pos = x * width / max_x;
    MoveToEx(mem_dc, pos, 0, 0);
    LineTo(mem_dc, pos, height);
  }

  SelectObject(mem_dc, major_pen);

  max_y = int(db_range / grid_db);
  for (y = major_db; y < max_y; y+=major_db)
  {
    int pos = y * height / max_y;
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  max_x = int(spectrum_length * bin2hz / grid_hz);
  for (x = major_hz; x < max_x; x+=major_hz)
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
  max_x = int(spectrum_length * bin2hz / grid_hz);
  for (x = major_hz; x < max_x; x += major_hz)
  {
    int pos = x * width / max_x;
    sprintf(label, "%ikHz", int(x * grid_hz / 1000));
    TextOut(mem_dc, pos, 0, label, strlen(label));
  }

  SetTextAlign(mem_dc, TA_BOTTOM | TA_RIGHT);
  max_y = int(db_range / grid_db);
  for (y = major_db; y <= max_y; y+=major_db)
  {
    int pos = y * height / max_y;
    sprintf(label, "-%idB", int(y * grid_db));
    TextOut(mem_dc, width, pos, label, strlen(label));
  }

  SelectObject(mem_dc, old_font);

  /////////////////////////////////////////////////////////
  // Spectrum

  double xfactor = double(width) / double(spectrum_length);
  double yfactor = double(height) / db_range;

  old_pen = (HPEN)SelectObject(mem_dc, signal_pen);

  MoveToEx(mem_dc, 0, int(-spectrum[0] * yfactor), 0);
  for (size_t i = 1; i < spectrum_length; i++)
    LineTo(mem_dc, int(i * xfactor), int(-spectrum[i] * yfactor));

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Show the result

  HDC ctrl_dc = GetDC(hctrl);
  BitBlt(ctrl_dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(hctrl, ctrl_dc);
}

int scale(int pos, int max, int width)
{
  return int(width + log10(double(pos)/double(max)) * width / 3);
}

void ControlSpectrum::paint_log()
{
  char label[32];
  HPEN old_pen;
  HFONT old_font;
  int y, max_y, delta_hz;
  const int nyquist = int(spectrum_length * bin2hz);

  FillRect(mem_dc, &client_rect, bkg_brush);

  /////////////////////////////////////////////////////////
  // Grid

  old_pen = (HPEN)SelectObject(mem_dc, grid_pen);

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
      TextOut(mem_dc, pos, 0, label, strlen(label));
    }
    delta_hz *= 10;
  }

  SetTextAlign(mem_dc, TA_BOTTOM | TA_RIGHT);
  max_y = int(db_range / grid_db);
  for (y = major_db; y <= max_y; y+=major_db)
  {
    int pos = y * height / max_y;
    sprintf(label, "-%idB", int(y * grid_db));
    TextOut(mem_dc, width, pos, label, strlen(label));
  }

  SelectObject(mem_dc, old_font);

  /////////////////////////////////////////////////////////
  // Spectrum

  double xfactor = double(width) / double(spectrum_length);
  double yfactor = double(height) / db_range;

  old_pen = (HPEN)SelectObject(mem_dc, signal_pen);

  MoveToEx(mem_dc, -1, int(-spectrum[0] * yfactor), 0);
  for (size_t i = 1; i < spectrum_length; i++)
    LineTo(mem_dc, scale(i, spectrum_length, width), int(-spectrum[i] * yfactor));

  SelectObject(mem_dc, old_pen);

  /////////////////////////////////////////////////////////
  // Show the result

  HDC ctrl_dc = GetDC(hctrl);
  BitBlt(ctrl_dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(hctrl, ctrl_dc);
}

ControlSpectrum::cmd_result ControlSpectrum::command(int control, int message)
{
  switch (control)
  {
    case IDC_CHK_EQ_LOG:
    {
      log_scale = IsDlgButtonChecked(hdlg, IDC_CHK_EQ_LOG) == BST_CHECKED;

      RegistryKey reg(REG_KEY);
      reg.set_bool("log_scale", log_scale);
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
