#include <math.h>
#include <stdio.h>
#include "custom_eq.h"
#include "resource_ids.h"

#define EQ_BANDS 10

static const int idc_edt_freq[EQ_BANDS] =
{
  IDC_EDT_EQ_FREQ1, IDC_EDT_EQ_FREQ2, IDC_EDT_EQ_FREQ3, IDC_EDT_EQ_FREQ4, IDC_EDT_EQ_FREQ5, IDC_EDT_EQ_FREQ6, IDC_EDT_EQ_FREQ7, IDC_EDT_EQ_FREQ8, IDC_EDT_EQ_FREQ9, IDC_EDT_EQ_FREQ10,
};

static const int idc_edt_gain[EQ_BANDS] =
{
  IDC_EDT_EQ_GAIN1, IDC_EDT_EQ_GAIN2, IDC_EDT_EQ_GAIN3, IDC_EDT_EQ_GAIN4, IDC_EDT_EQ_GAIN5, IDC_EDT_EQ_GAIN6, IDC_EDT_EQ_GAIN7, IDC_EDT_EQ_GAIN8, IDC_EDT_EQ_GAIN9, IDC_EDT_EQ_GAIN10,
};

static EqBand default_bands[EQ_BANDS] = 
{
  { 30, 1.0 }, { 60, 1.0 }, { 125, 1.0 }, { 250, 1.0 }, { 500, 1.0 }, { 1000, 1.0 }, { 2000, 1.0 }, { 4000, 1.0 }, { 8000, 1.0 }, { 16000, 1.0 }
};

static const double default_ripple = 0.1;

inline unsigned int clp2(unsigned int x)
{
  // smallest power-of-2 >= x
  x = x - 1;
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
  return x + 1;
}

///////////////////////////////////////////////////////////////////////////////

void
CustomEq::on_create()
{
  log_scale = true;
  bands.allocate(EQ_BANDS);
  for (int i = 0; i < EQ_BANDS; i++)
  {
    edt_freq[i].link(hwnd, idc_edt_freq[i]);
    edt_gain[i].link(hwnd, idc_edt_gain[i]);
  }
  edt_ripple.link(hwnd, IDC_EDT_EQ_RIPPLE);
  edt_length.link(hwnd, IDC_EDT_EQ_LEN);

  spectrum.link(hwnd, IDC_SPECTRUM);
  update();
}

void
CustomEq::update()
{
  nbands = eq.get_nbands();
  ripple = eq.get_ripple();
  eq.get_bands(bands, 0, EQ_BANDS);

  for (int i = 0; i < EQ_BANDS; i++)
  {
    edt_freq[i].update_value(bands[i].freq);
    edt_gain[i].update_value(value2db(bands[i].gain));
  }
  edt_ripple.update_value(ripple);
  CheckDlgButton(hwnd, IDC_CHK_EQ_LOG, log_scale? BST_CHECKED: BST_UNCHECKED);

  const int sample_rate = 48000;
  const FIRInstance *fir = eq.make(sample_rate);

  if (fir)
  {
    int i;
    int length = MAX(8192, clp2(fir->length));
    edt_length.update_value(fir->length);

    fft.set_length(length);
    buf.allocate(length);
    buf.zero();

    for (i = 0; i < fir->length; i++)
      buf[i] = fir->data[i];
    fft.rdft(buf);

    for (i = 0; i < length / 2; i++)
      buf[i] = sqrt(buf[i*2]*buf[i*2] + buf[i*2+1]*buf[i*2+1]);

    if (log_scale)
      spectrum.draw_log(buf, length/2, double(sample_rate)/length);
    else
      spectrum.draw_lin(buf, length/2, double(sample_rate)/length);
    delete fir;
  }
  else
    edt_length.update_value(0);

}

BOOL
CustomEq::on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_COMMAND)
  {
    int control = LOWORD(wParam);
    int message = HIWORD(wParam);

    if (message == CB_ENTER)
      for (int i = 0; i < EQ_BANDS; i++)
        if (control == idc_edt_freq[i] || control == idc_edt_gain[i])
        {
          eq.get_bands(bands, 0, EQ_BANDS);
          bands[i].freq = edt_freq[i].value;
          bands[i].gain = db2value(edt_gain[i].value);
          eq.set_bands(bands, EQ_BANDS);

          update();
          return TRUE;
        }

    switch (control)
    {
      case IDC_EDT_EQ_RIPPLE:
        if (message == CB_ENTER)
        {
          ripple = edt_ripple.value;
          eq.set_ripple(ripple);
          update();
        }
        return TRUE;

      case IDC_BTN_EQ_RESET:
        eq.set_bands(default_bands, EQ_BANDS);
        eq.set_ripple(default_ripple);
        update();


      case IDC_CHK_EQ_LOG:
        log_scale = IsDlgButtonChecked(hwnd, IDC_CHK_EQ_LOG) == BST_CHECKED;
        update();
        return TRUE;
    }
  }

  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

size_t
CustomEq::get_nbands() const
{ return nbands; }

size_t
CustomEq::set_bands(const EqBand *new_bands, size_t new_nbands)
{
  eq.set_bands(new_bands, new_nbands);

  nbands = eq.get_nbands();
  eq.get_bands(bands, 0, EQ_BANDS);
  return nbands;
}

size_t
CustomEq::get_bands(EqBand *out_bands, size_t first_band, size_t out_nbands) const
{
  return eq.get_bands(out_bands, first_band, out_nbands);
}

double
CustomEq::get_ripple() const
{ return ripple; }

void
CustomEq::set_ripple(double ripple_db)
{
  eq.set_ripple(ripple_db);
  ripple = eq.get_ripple();
}

void
CustomEq::reset()
{
  eq.set_bands(default_bands, EQ_BANDS);
  eq.set_ripple(default_ripple);

  nbands = eq.get_nbands();
  eq.get_bands(bands, 0, EQ_BANDS);
  ripple = eq.get_ripple();
}

///////////////////////////////////////////////////////////////////////////////

static const double max_db = 12;
static const double min_db = -12;
static const double db_range = max_db - min_db;

static const double grid_hz = 1000;  // 1kHz grid step
static const double grid_db = 2;    //  2db grid step

static const int major_db = 2;  // major line grid step
static const int major_hz = 2;  // major line grid step

static const DWORD bkg_color = RGB(0, 0, 0);
static const DWORD signal_color = RGB(0, 255, 255);
static const DWORD grid_color = RGB(32, 32, 32);
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
  grid_pen = CreatePen(PS_SOLID, 1, grid_color);
  major_pen = CreatePen(PS_SOLID, 1, major_color);
  grid_font = CreateFont(font_size, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, font_name);
}

void
SpectrumCtrl::on_unlink()
{
  if (!hwnd) return;
  DeleteObject(bkg_brush);
  DeleteObject(signal_pen);
  DeleteObject(grid_pen);
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

  old_pen = (HPEN)SelectObject(mem_dc, grid_pen);

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

static int scale(int pos, int max, int width)
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
  for (unsigned i = 1; i < length; i++)
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
