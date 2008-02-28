#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_levels.h"

static const int controls[] =
{
  IDC_GRP_LEVELS,
  IDC_LBL_INPUT_LEVELS, IDC_LBL_OUTPUT_LEVELS,
  IDC_GRP_INPUT_LEVELS, IDC_GRP_OUTPUT_LEVELS,

  IDC_LBL_IN_L,  IDC_LBL_IN_C,  IDC_LBL_IN_R,  IDC_LBL_IN_SL,  IDC_LBL_IN_SR,  IDC_LBL_IN_LFE,
  IDC_LBL_OUT_L, IDC_LBL_OUT_C, IDC_LBL_OUT_R, IDC_LBL_OUT_SL, IDC_LBL_OUT_SR, IDC_LBL_OUT_SW,

  IDC_IN_L,  IDC_IN_C,  IDC_IN_R,  IDC_IN_SL,  IDC_IN_SR,  IDC_IN_LFE,
  IDC_OUT_L, IDC_OUT_C, IDC_OUT_R, IDC_OUT_SL, IDC_OUT_SR, IDC_OUT_SW, 

  0
};

static const int idc_level_in[6]  = { IDC_IN_L,  IDC_IN_C,  IDC_IN_R,  IDC_IN_SL,  IDC_IN_SR,  IDC_IN_LFE };
static const int idc_level_out[6] = { IDC_OUT_L, IDC_OUT_C, IDC_OUT_R, IDC_OUT_SL, IDC_OUT_SR, IDC_OUT_SW };
static const double min_level = -50.0;
static const int ticks = 5;

///////////////////////////////////////////////////////////////////////////////

ControlLevels::ControlLevels(HWND _dlg, IAC3Filter *_filter, IAudioProcessor *_proc, bool _invert_levels): 
Controller(_dlg, ::controls), filter(_filter), proc(_proc), invert_levels(_invert_levels)
{
  filter->AddRef();
  proc->AddRef();
}

ControlLevels::~ControlLevels()
{
  filter->Release();
  proc->Release();
}

void ControlLevels::init()
{
  for (int ch = 0; ch < NCHANNELS; ch++)
  {
    SendDlgItemMessage(hdlg, idc_level_in[ch],  PBM_SETBARCOLOR, 0, RGB(0, 128, 0));
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETBARCOLOR, 0, RGB(0, 128, 0));
    SendDlgItemMessage(hdlg, idc_level_in[ch],  PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
  }
}

void ControlLevels::update_dynamic()
{
  vtime_t time;
  sample_t input_levels[NCHANNELS];
  sample_t output_levels[NCHANNELS];

  filter->get_playback_time(&time);
  proc->get_levels(time, input_levels, output_levels);

  for (int ch = 0; ch < NCHANNELS; ch++)
  {
    long in, out;
    if (invert_levels)
    {
      in = input_levels[ch]  > 0? long(-value2db(input_levels[ch]) * ticks): long(-min_level * ticks);
      out = output_levels[ch]  > 0? long(-value2db(output_levels[ch]) * ticks): long(-min_level * ticks);
    }
    else
    {
      in = input_levels[ch]  > 0? long((value2db(input_levels[ch]) - min_level) * ticks): 0;
      out = output_levels[ch]  > 0? long((value2db(output_levels[ch]) - min_level) * ticks): 0;
    }
    SendDlgItemMessage(hdlg, idc_level_in[ch],  PBM_SETPOS, in, 0);
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETPOS, out, 0);
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETBARCOLOR, 0, (output_levels[ch] > 0.99)? RGB(255, 0, 0): RGB(0, 128, 0));
  }
};
