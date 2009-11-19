#include <math.h>
#include "../resource_ids.h"
#include "control_bass.h"

static const int controls[] =
{
  IDC_GRP_BASS,
  IDC_CHK_BASS_ENABLE,
  IDC_LBL_BASS_FREQ,
  IDC_EDT_BASS_FREQ,
  IDC_LBL_BASS_ROUTE,
  IDC_CHK_BASS_FRONT,
  IDC_CHK_BASS_SUB,
  0
};

///////////////////////////////////////////////////////////////////////////////

ControlBass::ControlBass(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
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
}

void ControlBass::update()
{
  proc->get_bass_redir(&bass_redir);
  proc->get_bass_freq(&bass_freq);
  proc->get_bass_channels(&bass_channels);
  bool front = (bass_channels & (CH_MASK_L | CH_MASK_R)) != 0;
  bool sub   = (bass_channels & CH_MASK_LFE) != 0;

  CheckDlgButton(hdlg, IDC_CHK_BASS_ENABLE, bass_redir? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_BASS_FRONT, front? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_BASS_SUB, sub? BST_CHECKED: BST_UNCHECKED);
  edt_bass_freq.update_value(bass_freq);
};

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

    case IDC_CHK_BASS_FRONT:
    case IDC_CHK_BASS_SUB:
    {
      bool front = IsDlgButtonChecked(hdlg, IDC_CHK_BASS_FRONT) == BST_CHECKED;
      bool sub   = IsDlgButtonChecked(hdlg, IDC_CHK_BASS_SUB) == BST_CHECKED;

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
