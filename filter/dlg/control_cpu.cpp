#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "control_cpu.h"

static const int controls[] = 
{
  IDC_GRP_CPU,
  IDC_CPU,
  IDC_CPU_LABEL,
  0
};

///////////////////////////////////////////////////////////////////////////////

ControlCPU::ControlCPU(HWND _dlg, IAC3Filter *_filter, bool _invert_levels):
Controller(_dlg, ::controls), filter(_filter), invert_levels(_invert_levels)
{
  filter->AddRef();
}

ControlCPU::~ControlCPU()
{
  filter->Release();
}

void ControlCPU::init()
{
  SendDlgItemMessage(hdlg, IDC_CPU, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
}

void ControlCPU::update()
{
  update_dynamic();
};

void ControlCPU::update_dynamic()
{
  double cpu_usage;
  filter->get_cpu_usage(&cpu_usage);

  char buf[128];
  sprintf(buf, "%i%%", int(cpu_usage * 100));
  SetDlgItemText(hdlg, IDC_CPU_LABEL, buf);
  if (invert_levels)
    SendDlgItemMessage(hdlg, IDC_CPU, PBM_SETPOS, 100 - int(cpu_usage * 100),  0);
  else
    SendDlgItemMessage(hdlg, IDC_CPU, PBM_SETPOS, int(cpu_usage * 100),  0);
}
