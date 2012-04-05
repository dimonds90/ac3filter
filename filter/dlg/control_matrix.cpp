#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../ch_names.h"
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_matrix.h"

#define MATRIX_CHANNELS 8

static const int controls[] =
{
  IDC_GRP_MATRIX,
  IDC_EDT_MATRIX11, IDC_EDT_MATRIX12, IDC_EDT_MATRIX13, IDC_EDT_MATRIX14, IDC_EDT_MATRIX15, IDC_EDT_MATRIX16, IDC_EDT_MATRIX17, IDC_EDT_MATRIX18,
  IDC_EDT_MATRIX21, IDC_EDT_MATRIX22, IDC_EDT_MATRIX23, IDC_EDT_MATRIX24, IDC_EDT_MATRIX25, IDC_EDT_MATRIX26, IDC_EDT_MATRIX27, IDC_EDT_MATRIX28,
  IDC_EDT_MATRIX31, IDC_EDT_MATRIX32, IDC_EDT_MATRIX33, IDC_EDT_MATRIX34, IDC_EDT_MATRIX35, IDC_EDT_MATRIX36, IDC_EDT_MATRIX37, IDC_EDT_MATRIX38,
  IDC_EDT_MATRIX41, IDC_EDT_MATRIX42, IDC_EDT_MATRIX43, IDC_EDT_MATRIX44, IDC_EDT_MATRIX45, IDC_EDT_MATRIX46, IDC_EDT_MATRIX47, IDC_EDT_MATRIX48,
  IDC_EDT_MATRIX51, IDC_EDT_MATRIX52, IDC_EDT_MATRIX53, IDC_EDT_MATRIX54, IDC_EDT_MATRIX55, IDC_EDT_MATRIX56, IDC_EDT_MATRIX57, IDC_EDT_MATRIX58,
  IDC_EDT_MATRIX61, IDC_EDT_MATRIX62, IDC_EDT_MATRIX63, IDC_EDT_MATRIX64, IDC_EDT_MATRIX65, IDC_EDT_MATRIX66, IDC_EDT_MATRIX67, IDC_EDT_MATRIX68,
  IDC_EDT_MATRIX71, IDC_EDT_MATRIX72, IDC_EDT_MATRIX73, IDC_EDT_MATRIX74, IDC_EDT_MATRIX75, IDC_EDT_MATRIX76, IDC_EDT_MATRIX77, IDC_EDT_MATRIX78,
  IDC_EDT_MATRIX81, IDC_EDT_MATRIX82, IDC_EDT_MATRIX83, IDC_EDT_MATRIX84, IDC_EDT_MATRIX85, IDC_EDT_MATRIX86, IDC_EDT_MATRIX87, IDC_EDT_MATRIX88,

  IDC_LBL_MATRIX_IN1, IDC_LBL_MATRIX_IN2, IDC_LBL_MATRIX_IN3, IDC_LBL_MATRIX_IN4, IDC_LBL_MATRIX_IN5, IDC_LBL_MATRIX_IN6, IDC_LBL_MATRIX_IN7, IDC_LBL_MATRIX_IN8, 
  IDC_LBL_MATRIX_OUT1, IDC_LBL_MATRIX_OUT2, IDC_LBL_MATRIX_OUT3, IDC_LBL_MATRIX_OUT4, IDC_LBL_MATRIX_OUT5, IDC_LBL_MATRIX_OUT6, IDC_LBL_MATRIX_OUT7, IDC_LBL_MATRIX_OUT8, 

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

static const int matrix_controls[MATRIX_CHANNELS][MATRIX_CHANNELS] =
{
  { IDC_EDT_MATRIX11, IDC_EDT_MATRIX12, IDC_EDT_MATRIX13, IDC_EDT_MATRIX14, IDC_EDT_MATRIX15, IDC_EDT_MATRIX16, IDC_EDT_MATRIX17, IDC_EDT_MATRIX18, },
  { IDC_EDT_MATRIX21, IDC_EDT_MATRIX22, IDC_EDT_MATRIX23, IDC_EDT_MATRIX24, IDC_EDT_MATRIX25, IDC_EDT_MATRIX26, IDC_EDT_MATRIX27, IDC_EDT_MATRIX28, },
  { IDC_EDT_MATRIX31, IDC_EDT_MATRIX32, IDC_EDT_MATRIX33, IDC_EDT_MATRIX34, IDC_EDT_MATRIX35, IDC_EDT_MATRIX36, IDC_EDT_MATRIX37, IDC_EDT_MATRIX38, },
  { IDC_EDT_MATRIX41, IDC_EDT_MATRIX42, IDC_EDT_MATRIX43, IDC_EDT_MATRIX44, IDC_EDT_MATRIX45, IDC_EDT_MATRIX46, IDC_EDT_MATRIX47, IDC_EDT_MATRIX48, },
  { IDC_EDT_MATRIX51, IDC_EDT_MATRIX52, IDC_EDT_MATRIX53, IDC_EDT_MATRIX54, IDC_EDT_MATRIX55, IDC_EDT_MATRIX56, IDC_EDT_MATRIX57, IDC_EDT_MATRIX58, },
  { IDC_EDT_MATRIX61, IDC_EDT_MATRIX62, IDC_EDT_MATRIX63, IDC_EDT_MATRIX64, IDC_EDT_MATRIX65, IDC_EDT_MATRIX66, IDC_EDT_MATRIX67, IDC_EDT_MATRIX68, },
  { IDC_EDT_MATRIX71, IDC_EDT_MATRIX72, IDC_EDT_MATRIX73, IDC_EDT_MATRIX74, IDC_EDT_MATRIX75, IDC_EDT_MATRIX76, IDC_EDT_MATRIX77, IDC_EDT_MATRIX78, },
  { IDC_EDT_MATRIX81, IDC_EDT_MATRIX82, IDC_EDT_MATRIX83, IDC_EDT_MATRIX84, IDC_EDT_MATRIX85, IDC_EDT_MATRIX86, IDC_EDT_MATRIX87, IDC_EDT_MATRIX88, },
};

static const int in_labels[MATRIX_CHANNELS] =
{
  IDC_LBL_MATRIX_IN1, IDC_LBL_MATRIX_IN2, IDC_LBL_MATRIX_IN3, IDC_LBL_MATRIX_IN4, IDC_LBL_MATRIX_IN5, IDC_LBL_MATRIX_IN6, IDC_LBL_MATRIX_IN7, IDC_LBL_MATRIX_IN8,
};

static const int out_labels[MATRIX_CHANNELS] =
{
  IDC_LBL_MATRIX_OUT1, IDC_LBL_MATRIX_OUT2, IDC_LBL_MATRIX_OUT3, IDC_LBL_MATRIX_OUT4, IDC_LBL_MATRIX_OUT5, IDC_LBL_MATRIX_OUT6, IDC_LBL_MATRIX_OUT7, IDC_LBL_MATRIX_OUT8,
};


static const int matrix_ch[MATRIX_CHANNELS] = { CH_L, CH_C, CH_R, CH_SL, CH_SR, CH_BL, CH_BR, CH_LFE };

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

ControlMatrix::ControlMatrix(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
{
  proc->AddRef();
}

ControlMatrix::~ControlMatrix()
{
  proc->Release();
}

void ControlMatrix::init()
{
  int i, j;
  for (i = 0; i < MATRIX_CHANNELS; i++)
    for (j = 0; j < MATRIX_CHANNELS; j++)
      edt_matrix[i][j].link(hdlg, matrix_controls[i][j]);

  for (i = 0; i < MATRIX_CHANNELS; i++)
  {
    SetDlgItemText(hdlg, in_labels[i], gettext(short_ch_names[matrix_ch[i]]));
    SetDlgItemText(hdlg, out_labels[i], gettext(short_ch_names[matrix_ch[i]]));
  }

  SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_SETLINESIZE, 0, LONG(step_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_SETPAGESIZE, 0, LONG(page_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_SETTIC, 0, db2pos(0));
  SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_SETLINESIZE, 0, LONG(step_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_SETPAGESIZE, 0, LONG(page_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_SETTIC, 0, db2pos(0));
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_SETRANGE, TRUE, MAKELONG(db2pos(max_gain_level), db2pos(min_gain_level)));
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_SETLINESIZE, 0, LONG(step_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_SETPAGESIZE, 0, LONG(page_size * ticks));
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_SETTIC, 0, db2pos(0));

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
  proc->get_clev(&clev);
  proc->get_slev(&slev);
  proc->get_lfelev(&lfelev);

  update_matrix();

  CheckDlgButton(hdlg, IDC_CHK_AUTO_MATRIX,   auto_matrix?      BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_NORM_MATRIX,   normalize_matrix? BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_VOICE_CONTROL, voice_control?    BST_CHECKED: BST_UNCHECKED);
  CheckDlgButton(hdlg, IDC_CHK_EXPAND_STEREO, expand_stereo?    BST_CHECKED: BST_UNCHECKED);

  EnableWindow(GetDlgItem(hdlg, IDC_CHK_EXPAND_STEREO), auto_matrix);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_VOICE_CONTROL), auto_matrix);
  EnableWindow(GetDlgItem(hdlg, IDC_CHK_NORM_MATRIX), auto_matrix);

  SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_SETPOS, TRUE, gain2pos(clev));
  SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_SETPOS, TRUE, gain2pos(slev));
  SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_SETPOS, TRUE, gain2pos(lfelev));

  edt_voice.update_value(value2db(clev));
  edt_sur  .update_value(value2db(slev));
  edt_lfe  .update_value(value2db(lfelev));
};

void ControlMatrix::update_matrix()
{
  proc->get_matrix(&matrix);
  for (int i = 0; i < MATRIX_CHANNELS; i++)
    for (int j = 0; j < MATRIX_CHANNELS; j++)
    {
      edt_matrix[i][j].update_value(matrix[matrix_ch[j]][matrix_ch[i]]);
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
    if (matrix != new_matrix)
      update_matrix();
  }
};

ControlMatrix::cmd_result ControlMatrix::command(int control, int message)
{
  if (message == CB_ENTER)
  {
    proc->get_matrix(&matrix);
    bool update_matrix = false;

    for (int i = 0; i < MATRIX_CHANNELS; i++)
      for (int j = 0; j < MATRIX_CHANNELS; j++)
        if (control == matrix_controls[i][j])
        {
          matrix[matrix_ch[j]][matrix_ch[i]] = edt_matrix[i][j].value;
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
        clev   = pos2gain(SendDlgItemMessage(hdlg, IDC_SLI_VOICE, TBM_GETPOS, 0, 0));
        slev   = pos2gain(SendDlgItemMessage(hdlg, IDC_SLI_SUR,   TBM_GETPOS, 0, 0));
        lfelev = pos2gain(SendDlgItemMessage(hdlg, IDC_SLI_LFE,   TBM_GETPOS, 0, 0));
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
