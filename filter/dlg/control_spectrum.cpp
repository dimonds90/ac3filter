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

static const double max_db = 0;
static const double min_db = -100;
static const unsigned lin_window_length = 2048;
static const unsigned log_window_length = 8192;
static const int sample_rate = 48000;

ControlSpectrum::ControlSpectrum(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc), spectrum_length(0)
{
  log_scale = false;
  RegistryKey reg(REG_KEY);
  reg.get_bool("log_scale", log_scale);

  proc->AddRef();

  spectrum.link(_dlg, IDC_SPECTRUM);

  if (log_scale)
  {
    spectrum.set_range(min_db, max_db, 0, sample_rate / 2);
    proc->set_spectrum_length(log_window_length);
  }
  else
  {
    spectrum.set_range(min_db, max_db, 0, sample_rate / 2);
    proc->set_spectrum_length(lin_window_length);
  }
}

ControlSpectrum::~ControlSpectrum()
{
  proc->set_spectrum_length(0);
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
  unsigned new_spectrum_length;
  proc->get_spectrum_length(&new_spectrum_length);
  if (spectrum_length != new_spectrum_length)
  {
    spectrum_length = new_spectrum_length;
    spectrum_buf.allocate(new_spectrum_length);
  }

  if (spectrum_buf.is_allocated())
  {
    proc->get_spectrum(-1, spectrum_buf, &bin2hz);

    if (log_scale)
      spectrum.draw_log(spectrum_buf, spectrum_length, bin2hz);
    else
      spectrum.draw_lin(spectrum_buf, spectrum_length, bin2hz);
  }
}

ControlSpectrum::cmd_result ControlSpectrum::command(int control, int message)
{
  switch (control)
  {
    case IDC_CHK_EQ_LOG:
    {
      log_scale = IsDlgButtonChecked(hdlg, IDC_CHK_EQ_LOG) == BST_CHECKED;
      if (log_scale)
      {
        spectrum.set_range(min_db, max_db, 0, sample_rate / 2);
        proc->set_spectrum_length(log_window_length);
      }
      else
      {
        spectrum.set_range(min_db, max_db, 0, sample_rate / 2);
        proc->set_spectrum_length(lin_window_length);
      }

      RegistryKey reg(REG_KEY);
      reg.set_bool("log_scale", log_scale);
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
