#include <stdio.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_info.h"

static const int controls[] = 
{
  IDC_GRP_DECODER_INFO,
  IDC_LBL_INPUT,
  IDC_EDT_INFO,
  IDC_LBL_FRAMES_ERRORS,
  IDC_EDT_FRAMES,
  IDC_EDT_ERRORS,
  0
};

///////////////////////////////////////////////////////////////////////////////

ControlInfo::ControlInfo(HWND _dlg, IDecoder *_dec):
Controller(_dlg, ::controls), dec(_dec)
{
  dec->AddRef();
  dec->get_in_spk(&in_spk);
}

ControlInfo::~ControlInfo()
{
  dec->Release();
}

void ControlInfo::update_dynamic()
{
  char buf[128];

  /////////////////////////////////////////////////////////
  // Input format

  Speakers new_in_spk;
  dec->get_in_spk(&new_in_spk);
  if (in_spk != new_in_spk)
  {
    in_spk = new_in_spk;
    sprintf(buf, _("%s %s %iHz"), in_spk.format_text(), in_spk.mode_text(), in_spk.sample_rate);
    SetDlgItemText(hdlg, IDC_LBL_INPUT, buf);
  }

  /////////////////////////////////////
  // Stream info

  char new_info[sizeof(info)];
  memset(new_info, 0, sizeof(info));
  dec->get_info(new_info, sizeof(info));
  if (memcmp(new_info, info, sizeof(info)))
  {
    memcpy(info, new_info, sizeof(info));
    SetDlgItemText(hdlg, IDC_EDT_INFO, info);
  }

  /////////////////////////////////////
  // Frames/errors

  dec->get_frames(&frames, &errors);

  sprintf(buf, "%i", frames);
  SetDlgItemText(hdlg, IDC_EDT_FRAMES, buf);

  sprintf(buf, "%i", errors);
  SetDlgItemText(hdlg, IDC_EDT_ERRORS, buf);
};
