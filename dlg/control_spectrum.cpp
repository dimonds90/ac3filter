#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_spectrum.h"

static const int controls[] =
{
  IDC_SPECTRUM,
  0
};

const double db_range = 120; // 120dB range
const double grid_y = 10;    // 10db grid step

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

    bkg_brush = CreateSolidBrush(RGB(0, 0, 0));
    signal_pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
    grid_pen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
    proc->set_spectrum_length(512);
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

    SelectObject(mem_dc, old_bitmap);
    DeleteObject(mem_bitmap);
    DeleteDC(mem_dc);
  }
  proc->Release();
}

void ControlSpectrum::init()
{
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
    proc->get_spectrum(spectrum);
    for (size_t i = 0; i < spectrum_length; i++)
      spectrum[i] = value2db(spectrum[i]);
    paint();
  }
}

void ControlSpectrum::paint()
{
  FillRect(mem_dc, &client_rect, bkg_brush);

  HPEN old_pen = (HPEN)SelectObject(mem_dc, grid_pen);

  for (int y = 1; y < int(db_range / grid_y); y++)
  {
    int pos = int(y * height * grid_y / db_range);
    MoveToEx(mem_dc, 0, pos, 0);
    LineTo(mem_dc, width, pos);
  }

  double xfactor = double(width) / double(spectrum_length);
  double yfactor = double(height) / db_range;

  SelectObject(mem_dc, signal_pen);
  MoveToEx(mem_dc, 0, int(-spectrum[0] * yfactor), 0);
  for (size_t i = 1; i < spectrum_length; i++)
    LineTo(mem_dc, int(i * xfactor), int(-spectrum[i] * yfactor));

  SelectObject(mem_dc, old_pen);

  HDC ctrl_dc = GetDC(hctrl);
  BitBlt(ctrl_dc, client_rect.left, client_rect.top, width, height, mem_dc, 0, 0, SRCCOPY);
  ReleaseDC(hctrl, ctrl_dc);
}

ControlSpectrum::cmd_result ControlSpectrum::command(int control, int message)
{
  return cmd_not_processed;
}
