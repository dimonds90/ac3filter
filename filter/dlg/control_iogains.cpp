#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_iogains.h"

#define NGAINS 6

static const int controls[] =
{
  IDC_GRP_INPUT_GAINS, IDC_GRP_OUTPUT_GAINS,
  IDC_SLI_IN_L,  IDC_SLI_IN_C,  IDC_SLI_IN_R,  IDC_SLI_IN_SL,  IDC_SLI_IN_SR,  IDC_SLI_IN_LFE,
  IDC_SLI_OUT_L, IDC_SLI_OUT_C, IDC_SLI_OUT_R, IDC_SLI_OUT_SL, IDC_SLI_OUT_SR, IDC_SLI_OUT_LFE,
  IDC_EDT_IN_L,  IDC_EDT_IN_C,  IDC_EDT_IN_R,  IDC_EDT_IN_SL,  IDC_EDT_IN_SR,  IDC_EDT_IN_LFE,
  IDC_EDT_OUT_L, IDC_EDT_OUT_C, IDC_EDT_OUT_R, IDC_EDT_OUT_SL, IDC_EDT_OUT_SR, IDC_EDT_OUT_LFE,
  0
};

const int idc_slider_in[NGAINS]  = { IDC_SLI_IN_L,  IDC_SLI_IN_C,  IDC_SLI_IN_R,  IDC_SLI_IN_SL,  IDC_SLI_IN_SR,  IDC_SLI_IN_LFE  };
const int idc_slider_out[NGAINS] = { IDC_SLI_OUT_L, IDC_SLI_OUT_C, IDC_SLI_OUT_R, IDC_SLI_OUT_SL, IDC_SLI_OUT_SR, IDC_SLI_OUT_LFE };

const int idc_edt_in[NGAINS]     = { IDC_EDT_IN_L,  IDC_EDT_IN_C,  IDC_EDT_IN_R,  IDC_EDT_IN_SL,  IDC_EDT_IN_SR,  IDC_EDT_IN_LFE  };
const int idc_edt_out[NGAINS]    = { IDC_EDT_OUT_L, IDC_EDT_OUT_C, IDC_EDT_OUT_R, IDC_EDT_OUT_SL, IDC_EDT_OUT_SR, IDC_EDT_OUT_LFE };

static const double min_gain_level = -20.0;
static const double max_gain_level = +20.0;
static const int ticks = 5;

static const int gain_ch[NGAINS] = { CH_L, CH_C, CH_R, CH_SL, CH_SR, CH_LFE };

///////////////////////////////////////////////////////////////////////////////

ControlIOGains::ControlIOGains(HWND _dlg, IAudioProcessor *_proc):
Controller(_dlg, ::controls), proc(_proc)
{
  proc->AddRef();
}

ControlIOGains::~ControlIOGains()
{
  proc->Release();
}

void ControlIOGains::init()
{
  for (int i = 0; i < NGAINS; i++)
  {
    SendDlgItemMessage(hdlg, idc_slider_in[i],  TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
    SendDlgItemMessage(hdlg, idc_slider_out[i], TBM_SETRANGE, TRUE, MAKELONG(min_gain_level, max_gain_level) * ticks);
    SendDlgItemMessage(hdlg, idc_slider_in[i],  TBM_SETTIC, 0, 0);
    SendDlgItemMessage(hdlg, idc_slider_out[i], TBM_SETTIC, 0, 0);
    edt_in_gains[i].link(hdlg, idc_edt_in[i]);
    edt_out_gains[i].link(hdlg, idc_edt_out[i]);
  }
}

void ControlIOGains::update()
{
  proc->get_input_gains(input_gains);
  proc->get_output_gains(output_gains);

  for (int i = 0; i < NGAINS; i++)
  {
    SendDlgItemMessage(hdlg, idc_slider_in[i],  TBM_SETPOS, TRUE, long(-value2db(input_gains[gain_ch[i]])  * ticks));
    SendDlgItemMessage(hdlg, idc_slider_out[i], TBM_SETPOS, TRUE, long(-value2db(output_gains[gain_ch[i]]) * ticks));
    edt_in_gains[i].update_value(value2db(input_gains[gain_ch[i]]));
    edt_out_gains[i].update_value(value2db(output_gains[gain_ch[i]]));
  }
};

ControlIOGains::cmd_result ControlIOGains::command(int control, int message)
{
  switch (control)
  {
    case IDC_SLI_IN_L:
    case IDC_SLI_IN_C:
    case IDC_SLI_IN_R:
    case IDC_SLI_IN_SL:
    case IDC_SLI_IN_SR:
    case IDC_SLI_IN_LFE:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        proc->get_input_gains(input_gains);
        for (int i = 0; i < NGAINS; i++)
          input_gains[gain_ch[i]] = db2value(-double(SendDlgItemMessage(hdlg, idc_slider_in[i], TBM_GETPOS, 0, 0))/ticks);

        proc->set_input_gains(input_gains);
        update();

        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_IN_L:
    case IDC_EDT_IN_C:
    case IDC_EDT_IN_R:
    case IDC_EDT_IN_SL:
    case IDC_EDT_IN_SR:
    case IDC_EDT_IN_LFE:
      if (message == CB_ENTER)
      {
        proc->get_input_gains(input_gains);
        for (int i = 0; i < NGAINS; i++)
          input_gains[gain_ch[i]] = db2value(edt_in_gains[i].value);

        proc->set_input_gains(input_gains);
        update();

        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_SLI_OUT_L:
    case IDC_SLI_OUT_C:
    case IDC_SLI_OUT_R:
    case IDC_SLI_OUT_SL:
    case IDC_SLI_OUT_SR:
    case IDC_SLI_OUT_LFE:
      if (message == TB_THUMBPOSITION || message == TB_ENDTRACK)
      {
        proc->get_output_gains(output_gains);
        for (int i = 0; i < NGAINS; i++)
          output_gains[gain_ch[i]] = db2value(-double(SendDlgItemMessage(hdlg, idc_slider_out[i], TBM_GETPOS, 0, 0))/ticks);

        proc->set_output_gains(output_gains);
        update();

        return cmd_ok;
      }
      return cmd_not_processed;

    case IDC_EDT_OUT_L:
    case IDC_EDT_OUT_C:
    case IDC_EDT_OUT_R:
    case IDC_EDT_OUT_SL:
    case IDC_EDT_OUT_SR:
    case IDC_EDT_OUT_LFE:
      if (message == CB_ENTER)
      {
        proc->get_output_gains(output_gains);
        for (int i = 0; i < NGAINS; i++)
          output_gains[gain_ch[i]] = db2value(edt_out_gains[i].value);

        proc->set_output_gains(output_gains);
        update();

        return cmd_ok;
      }
      return cmd_not_processed;
  }
  return cmd_not_processed;
}
