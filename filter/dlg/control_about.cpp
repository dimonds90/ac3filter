#include <stdio.h>
#include "../guids.h"
#include "../resource_ids.h"
#include "control_about.h"

static const int controls[] = 
{
  IDC_GRP_ABOUT,
  IDC_LBL_ABOUT1,
  IDC_LBL_ABOUT2,
  IDC_GRP_LICENSE,
  IDC_LBL_LICENSE,
  IDC_GRP_CONTACT,
  IDC_LBL_CONTACT,
  IDC_GRP_CREDITS,
  IDC_EDT_CREDITS,

  IDC_LNK_HOME,
  IDC_LNK_FORUM,
  IDC_LNK_EMAIL,

  0
};

///////////////////////////////////////////////////////////////////////////////

ControlAbout::ControlAbout(HWND _dlg): Controller(_dlg, ::controls)
{}

void ControlAbout::init()
{
  /////////////////////////////////////
  // Links

  lnk_home.link(hdlg, IDC_LNK_HOME);
  lnk_forum.link(hdlg, IDC_LNK_FORUM);
  lnk_email.link(hdlg, IDC_LNK_EMAIL);

  /////////////////////////////////////
  // Build credits and environment info

  char info[1024];

  strncpy(info, valib_credits(), sizeof(info));
  info[sizeof(info)-1] = 0;
  cr2crlf(info, sizeof(info));
  SetDlgItemText(hdlg, IDC_EDT_CREDITS, info);
};
