#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "dsp/kaiser.h"
#include "../resource_ids.h"
#include "control_spectrum.h"

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

static const int controls[] =
{
  IDC_SPECTRUM,
  IDC_CHK_EQ_LOG,
  0
};

static const double max_db = 0;
static const double min_db = -100;
static const unsigned lin_window_length = 512;
static const unsigned log_window_length = 1024;
static const int sample_rate = 48000;
static const vtime_t max_lag_time = 5.0;

ControlSpectrum::ControlSpectrum(HWND _dlg, IAC3Filter *_filter, IAudioProcessor *_proc):
Controller(_dlg, ::controls), filter(_filter), proc(_proc), length(0)
{
  log_scale = false;
  RegistryKey reg(REG_KEY);
  reg.get_bool("log_scale", log_scale);

  filter->AddRef();
  proc->AddRef();

  spectrum.link(_dlg, IDC_SPECTRUM);
  spectrum.set_range(min_db, max_db, 0, sample_rate / 2);

  init_spectrum(log_scale? log_window_length: lin_window_length);
}

ControlSpectrum::~ControlSpectrum()
{
  proc->Release();
  filter->Release();
}

void
ControlSpectrum::init_spectrum(unsigned new_length)
{
  spk = spk_unknown;
  length = clp2(new_length);

  proc->get_proc_out_spk(&spk);
  fft.set_length(length);
  buf.allocate(spk.nch(), length);
  win.allocate(length);

  if (spk.is_unknown() || !fft.is_ok() || !buf.is_allocated() || !win.is_allocated())
  {
    spk = spk_unknown;
    return;
  }

  double alpha = kaiser_alpha(-min_db);
  int odd_length = length / 2 - 1;
  for (int i = 0; i <= 2 * odd_length; i++)
    win[i] = (sample_t) kaiser_window(i - odd_length, length - 1, alpha);
  win[length-1] = 0;
}

void ControlSpectrum::init()
{
  CheckDlgButton(hdlg, IDC_CHK_EQ_LOG, log_scale? BST_CHECKED: BST_UNCHECKED);
}

void ControlSpectrum::update()
{}

void ControlSpectrum::update_dynamic()
{
  int ch;
  unsigned i;
  size_t out_size;
  Speakers new_spk;
  sample_t *data = buf[0];

  // Format change
  proc->get_proc_out_spk(&new_spk);
  if (spk != new_spk)
    init_spectrum(length);

  // Show nothing when uninitialized
  if (!is_ok()) return;

  // Automatically adjust cache size
  vtime_t playback_time, cache_time, cache_size, lag_time;
  proc->get_output_cache_size(&cache_size);
  proc->get_output_cache_time(&cache_time);
  filter->get_playback_time(&playback_time);
  lag_time = cache_time - playback_time + vtime_t(length) / spk.sample_rate;
  if (lag_time > max_lag_time)
    lag_time = max_lag_time;

  if (lag_time > cache_size)
    proc->set_output_cache_size(lag_time + 1.0);

  // Get data
  playback_time -= vtime_t(length / 2) / spk.sample_rate;
  proc->get_output_cache(playback_time, buf, length, &out_size);
  if (out_size < length)
    for (ch = 0; ch < spk.nch(); ch++)
      memset(buf[ch] + out_size, 0, (length - out_size) * sizeof(sample_t));

  // Sum channels
  for (ch = 1; ch < spk.nch(); ch++)
    for (i = 0; i < length; i++)
      data[i] += buf[ch][i];

  // Normalization and windowing
  double norm = 1.0 / (spk.level * length / 2);
  for (i = 0; i < length; i++)
    data[i] *= win[i] * norm;

  // FFT and amplitude
  fft.rdft(data);
  for (i = 0; i < length / 2; i++)
    data[i] = sqrt(data[i*2]*data[i*2] + data[i*2+1]*data[i*2+1]);

  if (log_scale)
    spectrum.draw_log(data, length/2, double(spk.sample_rate)/length);
  else
    spectrum.draw_lin(data, length/2, double(spk.sample_rate)/length);
}

ControlSpectrum::cmd_result ControlSpectrum::command(int control, int message)
{
  switch (control)
  {
    case IDC_CHK_EQ_LOG:
    {
      log_scale = IsDlgButtonChecked(hdlg, IDC_CHK_EQ_LOG) == BST_CHECKED;
      init_spectrum(log_scale? log_window_length: lin_window_length);

      RegistryKey reg(REG_KEY);
      reg.set_bool("log_scale", log_scale);
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
