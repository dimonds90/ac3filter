#include "../resource_ids.h"
#include "control_bass.h"

static const int controls[] =
{
  IDC_GRP_BASS,
  IDC_CHK_BASS_ENABLE,
  IDC_LBL_BASS_FREQ,
  IDC_EDT_BASS_FREQ,
  IDC_LBL_BASS_GAIN,
  IDC_EDT_BASS_GAIN,
  IDC_SLI_BASS_GAIN,
  IDC_LBL_BASS_ROUTE,
  IDC_RBT_BASS_FRONT,
  IDC_RBT_BASS_SUB,
  IDC_CHK_BASS_HPF,
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

  CheckDlgButton(hdlg, IDC_CHK_BASS_ENABLE, bass_redir? BST_CHECKED: BST_UNCHECKED);
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
  }
  return cmd_not_processed;
}
