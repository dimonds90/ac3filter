#include <stdio.h>
#include "../../ac3filter_ver.h"
#include "../resource_ids.h"
#include "control_ver.h"

static const int controls[] = 
{
  IDC_VER, 0
};

///////////////////////////////////////////////////////////////////////////////

ControlVer::ControlVer(HWND _dlg): Controller(_dlg, ::controls)
{}

void ControlVer::init()
{
  char ver1[255];
  char ver2[255];
  GetDlgItemText(hdlg, IDC_VER, ver1, sizeof(ver1) / sizeof(ver1[0]));
  sprintf(ver2, ver1, AC3FILTER_VER);
  SetDlgItemText(hdlg, IDC_VER, ver2);
};
