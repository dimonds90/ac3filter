#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_matrix.h"

static const int controls[] =
{
  IDC_GRP_MATRIX,
  IDC_EDT_L_L,   IDC_EDT_C_L,   IDC_EDT_R_L,   IDC_EDT_SL_L,   IDC_EDT_SR_L,   IDC_EDT_LFE_L,
  IDC_EDT_L_C,   IDC_EDT_C_C,   IDC_EDT_R_C,   IDC_EDT_SL_C,   IDC_EDT_SR_C,   IDC_EDT_LFE_C,
  IDC_EDT_L_R,   IDC_EDT_C_R,   IDC_EDT_R_R,   IDC_EDT_SL_R,   IDC_EDT_SR_R,   IDC_EDT_LFE_R,
  IDC_EDT_L_SL,  IDC_EDT_C_SL,  IDC_EDT_R_SL,  IDC_EDT_SL_SL,  IDC_EDT_SR_SL,  IDC_EDT_LFE_SL,
  IDC_EDT_L_SR,  IDC_EDT_C_SR,  IDC_EDT_R_SR,  IDC_EDT_SL_SR,  IDC_EDT_SR_SR,  IDC_EDT_LFE_SR,
  IDC_EDT_L_LFE, IDC_EDT_C_LFE, IDC_EDT_R_LFE, IDC_EDT_SL_LFE, IDC_EDT_SR_LFE, IDC_EDT_LFE_LFE,

  IDC_LBL_MATRIX_IN_L,  IDC_LBL_MATRIX_IN_C,  IDC_LBL_MATRIX_IN_R,  IDC_LBL_MATRIX_IN_SL,  IDC_LBL_MATRIX_IN_SR,  IDC_LBL_MATRIX_IN_LFE,
  IDC_LBL_MATRIX_OUT_L, IDC_LBL_MATRIX_OUT_C, IDC_LBL_MATRIX_OUT_R, IDC_LBL_MATRIX_OUT_SL, IDC_LBL_MATRIX_OUT_SR, IDC_LBL_MATRIX_OUT_SW,

  IDC_GRP_MIXER_OPTIONS,
  IDC_CHK_AUTO_MATRIX,
  IDC_CHK_NORM_MATRIX,
  IDC_CHK_VOICE_CONTROL,
  IDC_CHK_EXPAND_STEREO,

  IDC_LBL_LFE, IDC_LBL_VOICE, IDC_LBL_SUR,
  IDC_SLI_LFE, IDC_SLI_VOICE, IDC_SLI_SUR,
  IDC_EDT_LFE, IDC_EDT_VOICE, IDC_EDT_SUR,

  0
};

static const int matrix_controls[NCHANNELS][NCHANNELS] =
{
  { IDC_EDT_L_L,   IDC_EDT_C_L,   IDC_EDT_R_L,   IDC_EDT_SL_L,   IDC_EDT_SR_L,   IDC_EDT_LFE_L },
  { IDC_EDT_L_C,   IDC_EDT_C_C,   IDC_EDT_R_C,   IDC_EDT_SL_C,   IDC_EDT_SR_C,   IDC_EDT_LFE_C },
  { IDC_EDT_L_R,   IDC_EDT_C_R,   IDC_EDT_R_R,   IDC_EDT_SL_R,   IDC_EDT_SR_R,   IDC_EDT_LFE_R },
  { IDC_EDT_L_SL,  IDC_EDT_C_SL,  IDC_EDT_R_SL,  IDC_EDT_SL_SL,  IDC_EDT_SR_SL,  IDC_EDT_LFE_SL },
  { IDC_EDT_L_SR,  IDC_EDT_C_SR,  IDC_EDT_R_SR,  IDC_EDT_SL_SR,  IDC_EDT_SR_SR,  IDC_EDT_LFE_SR },
  { IDC_EDT_L_LFE, IDC_EDT_C_LFE, IDC_EDT_R_LFE, IDC_EDT_SL_LFE, IDC_EDT_SR_LFE, IDC_EDT_LFE_LFE }
};

static const double min_gain_level = -20.0;
static const double max_gain_level = +20.0;
static const int ticks = 5;

///////////////////////////////////////////////////////////////////////////////

ControlMatrix::ControlMatrix(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
{
  proc->AddRef();
  proc->get_matrix(&matrix);
}

ControlMatrix::~ControlMatrix()
{
  proc->Release();
}

void ControlMatrix::init()
{
  for (int i = 0; i < NCHANNELS; i++)
    for (int j = 0; j < NCHANNELS; j++)
      edt_matrix[i][j].link(hdlg, matrix_controls[i][j]);

  SendDlgItemMessage(hdlg, IDC_SLI_LFE,    TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(hdlg, IDC_SLI_LFE,    TBM_SETTIC, 0, 0);
  SendDlgItemMessage(hdlg, IDC_SLI_VOICE,  TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(hdlg, IDC_SLI_VOICE,  TBM_SETTIC, 0, 0);
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,    TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,    TBM_SETTIC, 0, 0);

  edt_voice .link(hdlg, IDC_EDT_VOICE);
  edt_sur   .link(hdlg, IDC_EDT_SUR);
  edt_lfe   .link(hdlg, IDC_EDT_LFE);
}

void ControlMatrix::update()
{
  proc->get_auto_matrix(&auto_matrix);
  proc->get_normalize_matrix(&normalize_matrix);
  proc->get_voice_control(&voice_control);
  proc->get_expand_stereo(&expand_stereo);

  update_matrix();

  CheckDlgButton(hdlg, IDC_CHK_AUTO_MATRIX,   auto_matrix?      BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_NORM_MATRIX,   normalize_matrix? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_VOICE_CONTROL, voice_control?    BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_EXPAND_STEREO, expand_stereo?    BST_CHECKED: BST_UNCHECKED);

  EnableWindow(GetDlgItem(hdlg, IDC_CHK_EXPAND_STEREO), auto_matrix);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_VOICE_CONTROL), auto_matrix);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_NORM_MATRIX), auto_matrix);

  SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_SETPOS, TRUE, long(-value2db(clev)   * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_SETPOS, TRUE, long(-value2db(slev)   * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_SETPOS, TRUE, long(-value2db(lfelev) * ticks));

  edt_voice.update_value(value2db(clev));
  edt_sur  .update_value(value2db(slev));
  edt_lfe  .update_value(value2db(lfelev));
};

void ControlMatrix::update_matrix()
{
  proc->get_matrix(&matrix);
  for (int i = 0; i < NCHANNELS; i++)
    for (int j = 0; j < NCHANNELS; j++)
    {
      edt_matrix[i][j].update_value(matrix[j][i]);
      SendDlgItemMessage(hdlg, matrix_controls[j][i], EM_SETREADONLY, auto_matrix, 0);
    }
}

void ControlMatrix::update_dynamic()
{
  proc->get_auto_matrix(&auto_matrix);
  if (auto_matrix)
  {
    matrix_t new_matrix;
    proc->get_matrix(&new_matrix);
    if (memcmp(matrix, new_matrix, sizeof(matrix_t)))
      update_matrix();
  }
};

ControlMatrix::cmd_result ControlMatrix::command(int control, int message)
{
  if (message == CB_ENTER)
  {
    proc->get_matrix(&matrix);
    bool update_matrix = false;

    for (int i = 0; i < NCHANNELS; i++)
      for (int j = 0; j < NCHANNELS; j++)
        if (control == matrix_controls[i][j])
        {
          matrix[j][i] = edt_matrix[i][j].value;
          update_matrix = true;
        }

    if (update_matrix)
    {
      proc->set_matrix(&matrix);
      update();
      return cmd_ok;
    }
  }

  switch (control)
  {
    case IDC_CHK_AUTO_MATRIX:
    {
      auto_matrix = IsDlgButtonChecked(hdlg, IDC_CHK_AUTO_MATRIX) == BST_CHECKED;
      proc->set_auto_matrix(auto_matrix);
      update();
      return cmd_ok;
    }

    case IDC_CHK_NORM_MATRIX:
    {
      normalize_matrix = IsDlgButtonChecked(hdlg, IDC_CHK_NORM_MATRIX) == BST_CHECKED;
      proc->set_normalize_matrix(normalize_matrix);
      update();
      return cmd_ok;
    }

    case IDC_CHK_EXPAND_STEREO:
    {
      expand_stereo = IsDlgButtonChecked(hdlg, IDC_CHK_EXPAND_STEREO) == BST_CHECKED;
      proc->set_expand_stereo(expand_stereo);
      update();
      return cmd_ok;
    }

    case IDC_CHK_VOICE_CONTROL:
    {
      voice_control = IsDlgButtonChecked(hdlg, IDC_CHK_VOICE_CONTROL) == BST_CHECKED;
      proc->set_voice_control(voice_control);
      update();
      return cmd_ok;
    }

    case IDC_SLI_VOICE:
    case IDC_SLI_SUR:
    case IDC_SLI_LFE:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        clev   = db2value(-double(SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_GETPOS, 0, 0))/ticks);
        slev   = db2value(-double(SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_GETPOS, 0, 0))/ticks);
        lfelev = db2value(-double(SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_GETPOS, 0, 0))/ticks);
        proc->set_clev(clev);
        proc->set_slev(slev);
        proc->set_lfelev(lfelev);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_VOICE:
    case IDC_EDT_SUR:
    case IDC_EDT_LFE:
      if (message == CB_ENTER)
      {  
        clev   = db2value(edt_voice.value);
        slev   = db2value(edt_sur.value);
        lfelev = db2value(edt_lfe.value);
        proc->set_clev(clev);
        proc->set_slev(slev);
        proc->set_lfelev(lfelev);
        update();
        return cmd_ok;
      }
      return cmd_not_processed;
  }
  return cmd_not_processed;
}
