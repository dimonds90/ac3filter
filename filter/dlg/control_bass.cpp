#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_bass.h"

static const int controls[] =
{
  IDC_GRP_BASS,
  IDC_CHK_BASS_ENABLE,
  IDC_LBL_BASS_FREQ,
  IDC_EDT_BASS_FREQ,
  IDC_LBL_BASS_ROUTE,
  IDC_RBT_BASS_FRONT,
  IDC_RBT_BASS_SUB,
  IDC_SLI_BASS_LEVEL,
  0
};

static const COLORREF bass_level_color = RGB(0, 128, 0);
static const double min_level = -50.0;
static const int ticks = 5;

static inline long slider_level(double level, bool invert)
{
  if (invert)
    return level > 0? long(-value2db(level) * ticks): long(-min_level * ticks);
  return level > 0? long((value2db(level) - min_level) * ticks): 0;
}

///////////////////////////////////////////////////////////////////////////////

ControlBass::ControlBass(HWND _dlg, IAudioProcessor *_proc, bool _invert_levels):
Controller(_dlg, ::controls), proc(_proc), invert_levels(_invert_levels)
{
  proc->AddRef();
}

ControlBass::~ControlBass()
{
  proc->Release();
}

void ControlBass::init()
{
  edt_bass_freq.link(hdlg, IDC_EDT_BASS_FREQ);
  SendDlgItemMessage(hdlg, IDC_SLI_BASS_LEVEL,  PBM_SETBARCOLOR, 0, bass_level_color);
  SendDlgItemMessage(hdlg, IDC_SLI_BASS_LEVEL,  PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
}

void ControlBass::update()
{
  proc->get_bass_redir(&bass_redir);
  proc->get_bass_freq(&bass_freq);
  proc->get_bass_channels(&bass_channels);
  bool front = (bass_channels & (CH_MASK_L | CH_MASK_R)) != 0;
  bool sub   = (bass_channels & CH_MASK_LFE) != 0;

  CheckDlgButton(hdlg, IDC_CHK_BASS_ENABLE, bass_redir? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_RBT_BASS_FRONT, front? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_RBT_BASS_SUB, sub? BST_CHECKED: BST_UNCHECKED);
  edt_bass_freq.update_value(bass_freq);
};

void ControlBass::update_dynamic()
{
  sample_t level = 0;
  proc->get_bass_level(&level);
  SendDlgItemMessage(hdlg, IDC_SLI_BASS_LEVEL, PBM_SETPOS, slider_level(level, invert_levels), 0);
}

ControlBass::cmd_result ControlBass::command(int control, int message)
{
  switch (control)
  {
    case IDC_CHK_BASS_ENABLE:
    {
      bass_redir = IsDlgButtonChecked(hdlg, IDC_CHK_BASS_ENABLE) == BST_CHECKED;
      proc->set_bass_redir(bass_redir);
      update();
      return cmd_ok;
    }

    case IDC_EDT_BASS_FREQ:
      if (message == CB_ENTER)
      {
        bass_freq = (int)edt_bass_freq.value;
        proc->set_bass_freq(bass_freq);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_RBT_BASS_FRONT:
    case IDC_RBT_BASS_SUB:
    {
      bool front = IsDlgButtonChecked(hdlg, IDC_RBT_BASS_FRONT) == BST_CHECKED;
      bool sub   = IsDlgButtonChecked(hdlg, IDC_RBT_BASS_SUB) == BST_CHECKED;

      bass_channels = 0;
      if (front) bass_channels |= CH_MASK_L | CH_MASK_R;
      if (sub) bass_channels |= CH_MASK_LFE;

      proc->set_bass_channels(bass_channels);
      update();
      return cmd_ok;
    }
  }
  return cmd_not_processed;
}
