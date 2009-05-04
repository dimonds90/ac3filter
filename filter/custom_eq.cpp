#include <math.h>
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
  edt_length.link(hwnd, IDC_EDT_EQ_LEN);

  spectrum.link(hwnd, IDC_SPECTRUM);
  update();
}

void
CustomEq::update()
{
  int i;
  size_t band;

  nbands = eq.get_nbands();
  eq.get_bands(bands, 0, EQ_BANDS);

  for (band = 0; band < EQ_BANDS; band++)
  {
    if (band < nbands)
    {
      edt_freq[band].update_value(bands[band].freq);
      edt_gain[band].update_value(value2db(bands[band].gain));
      edt_gain[band].enable(true);
    }
    else
    {
      bands[band].freq = 0;
      bands[band].gain = 0;
      edt_freq[band].update_value(0);
      edt_gain[band].update_value(0);
      edt_gain[band].enable(false);
    }
  }
  CheckDlgButton(hwnd, IDC_CHK_EQ_LOG, log_scale? BST_CHECKED: BST_UNCHECKED);

  const int sample_rate = 48000;
  const FIRInstance *fir = eq.make(sample_rate);

  if (fir)
  {
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

    double min_gain = -12;
    double max_gain = 12;
    for (band = 0; band < nbands; band++)
    {
      if (value2db(bands[band].gain) * 1.05 < min_gain) min_gain = value2db(bands[band].gain) * 1.05;
      if (value2db(bands[band].gain) * 1.05 > max_gain) max_gain = value2db(bands[band].gain) * 1.05;
    }

    spectrum.set_range(min_gain, max_gain, 0, sample_rate / 2);
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
      case IDC_BTN_EQ_RESET:
        eq.set_bands(default_bands, EQ_BANDS);
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

void
CustomEq::reset()
{
  eq.set_bands(default_bands, EQ_BANDS);

  nbands = eq.get_nbands();
  eq.get_bands(bands, 0, EQ_BANDS);
}
