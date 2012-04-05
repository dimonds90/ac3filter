#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_agc.h"

static const int controls[] =
{
  IDC_LBL_MASTER, IDC_LBL_GAIN,
  IDC_SLI_MASTER, IDC_SLI_GAIN,
  IDC_EDT_MASTER, IDC_EDT_GAIN,

  IDC_GRP_AGC,
  IDC_CHK_AUTO_GAIN,
  IDC_CHK_NORMALIZE,
  IDC_LBL_ATTACK_RELEASE,
  IDC_EDT_ATTACK,
  IDC_EDT_RELEASE,

  IDC_GRP_DRC,
  IDC_CHK_DRC,
  IDC_SLI_DRC_POWER, IDC_SLI_DRC_LEVEL,
  IDC_EDT_DRC_POWER, IDC_EDT_DRC_LEVEL,

  0
};

static const double min_gain_level = -20.0; // dB
static const double max_gain_level = +20.0; // dB
static const double step_size = 1;          // dB
static const double page_size = 1;          // dB
static const int ticks = 10; // steps per dB

static inline int db2pos(double db)
{
  return int(-db * ticks);
}

static inline double pos2db(LRESULT pos)
{
  // round to the nearest step_size
  double db = double(-pos) / ticks;
  return floor(db / step_size + 0.5) * step_size;
}

static inline int gain2pos(double gain)
{
  return db2pos(value2db(gain));
}

static inline double pos2gain(LRESULT pos)
{
  return db2value(pos2db(pos));
}

///////////////////////////////////////////////////////////////////////////////

ControlAGC::ControlAGC(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
{
  proc->AddRef();
}

ControlAGC::~ControlAGC()
{
  proc->Release();
}

void ControlAGC::init()
{
  // AGC

  SendDlgItemMessage(hdlg, IDC_SLI_MASTER, TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_MASTER, TBM_SETLINESIZE, 0, LONG(step_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_MASTER, TBM_SETPAGESIZE, 0, LONG(page_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_MASTER, TBM_SETTIC, 0, db2pos(0));
  SendDlgItemMessage(hdlg, IDC_SLI_GAIN,   TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_GAIN,   TBM_SETTIC, 0, db2pos(0));

  edt_master.link(hdlg, IDC_EDT_MASTER);
  edt_gain  .link(hdlg, IDC_EDT_GAIN);
  edt_gain.enable(false);

  edt_attack.link(hdlg, IDC_EDT_ATTACK);
  edt_release.link(hdlg, IDC_EDT_RELEASE);

  // DRC

  SendDlgItemMessage(hdlg, IDC_SLI_DRC_POWER, TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_DRC_POWER, TBM_SETLINESIZE, 0, LONG(step_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_DRC_POWER, TBM_SETPAGESIZE, 0, LONG(page_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_DRC_POWER, TBM_SETTIC, 0, db2pos(0));
  SendDlgItemMessage(hdlg, IDC_SLI_DRC_LEVEL, TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_DRC_LEVEL, TBM_SETTIC, 0, db2pos(0));

  edt_drc_power.link(hdlg, IDC_EDT_DRC_POWER);
  edt_drc_level.link(hdlg, IDC_EDT_DRC_LEVEL);
}

void ControlAGC::update()
{
  proc->get_master(&master);
  proc->get_auto_gain(&auto_gain);
  proc->get_normalize(&normalize);
  proc->get_attack(&attack);
  proc->get_release(&release);
  proc->get_drc(&drc);
  proc->get_drc_power(&drc_power);

  // AGC

  SendDlgItemMessage(hdlg, IDC_SLI_MASTER, TBM_SETPOS, TRUE, gain2pos(master));
  edt_master.update_value(value2db(master));

  CheckDlgButton(hdlg, IDC_CHK_AUTO_GAIN, auto_gain? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_NORMALIZE, normalize? BST_CHECKED: BST_UNCHECKED);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_NORMALIZE), auto_gain);

  edt_attack.update_value(attack);
  edt_release.update_value(release);

  // DRC

  CheckDlgButton(hdlg, IDC_CHK_DRC, drc? BST_CHECKED: BST_UNCHECKED);
  SendDlgItemMessage(hdlg, IDC_SLI_DRC_POWER, TBM_SETPOS, TRUE, db2pos(drc_power));
  edt_drc_power.update_value(drc_power);

  update_dynamic();
};

void ControlAGC::update_dynamic()
{
  proc->get_gain(&gain);
  proc->get_drc_level(&drc_level);

  // AGC

  SendDlgItemMessage(hdlg, IDC_SLI_GAIN, TBM_SETPOS, TRUE, gain2pos(gain));
  edt_gain.update_value(value2db(gain));

  // DRC

  SendDlgItemMessage(hdlg, IDC_SLI_DRC_LEVEL, TBM_SETPOS, TRUE, db2pos(drc_level));
  edt_drc_level.update_value(value2db(drc_level));
};

ControlAGC::cmd_result ControlAGC::command(int control, int message)
{
  switch (control)
  {
    /////////////////////////////////////
    // Auto gain control

    case IDC_SLI_MASTER:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        LRESULT pos = SendDlgItemMessage(hdlg, IDC_SLI_MASTER,TBM_GETPOS, 0, 0);
        master = pos2gain(pos);
        proc->set_master(master);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_MASTER:
      if (message == CB_ENTER)
      {
        proc->set_master(db2value(edt_master.value));
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_ATTACK:
      if (message == CB_ENTER)
      {
        attack = edt_attack.value;
        proc->set_attack(attack);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_RELEASE:
      if (message == CB_ENTER)
      {
        release = edt_release.value;
        proc->set_release(release);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_CHK_AUTO_GAIN:
    {
      auto_gain = IsDlgButtonChecked(hdlg, IDC_CHK_AUTO_GAIN) == BST_CHECKED;
      proc->set_auto_gain(auto_gain);
      update();
      break;
    }

    case IDC_CHK_NORMALIZE:
    {
      normalize = IsDlgButtonChecked(hdlg, IDC_CHK_NORMALIZE) == BST_CHECKED;
      proc->set_normalize(normalize);
      update();
      return cmd_ok;
    }

    /////////////////////////////////////
    // DRC

    case IDC_CHK_DRC:
    {
      drc = IsDlgButtonChecked(hdlg, IDC_CHK_DRC) == BST_CHECKED;
      proc->set_drc(drc);
      update();
      return cmd_ok;
    }

    case IDC_SLI_DRC_POWER:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        LRESULT pos = SendDlgItemMessage(hdlg, IDC_SLI_DRC_POWER, TBM_GETPOS, 0, 0);
        drc_power = pos2db(pos);
        proc->set_drc_power(drc_power);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_DRC_POWER:
      if (message == CB_ENTER)
      {
        drc_power = edt_drc_power.value;
        proc->set_drc_power(drc_power);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

  }
  return cmd_not_processed;
}
