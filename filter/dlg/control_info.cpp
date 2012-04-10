#include <stdio.h>
#include "../ac3filter_intl.h"
#include "../resource_ids.h"
#include "control_info.h"

static const int controls[] = 
{
  IDC_GRP_DECODER_INFO,
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
  memset(info, 0, sizeof(info));
}

ControlInfo::~ControlInfo()
{
  dec->Release();
}

void ControlInfo::update_dynamic()
{
  char buf[128];

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
