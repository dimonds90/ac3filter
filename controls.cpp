#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "controls.h"


Edit::~Edit()
{
  unlink();
}

void
Edit::link(HWND _dlg, int _item)
{
  if (hwnd) unlink();

  dlg = _dlg;
  item = _item;
  hwnd = GetDlgItem(_dlg, _item);
  if (hwnd)
  {
    wndproc = (WNDPROC) SetWindowLong(hwnd, GWL_WNDPROC, (DWORD) SubClassProc);
    SetWindowLong(hwnd, GWL_USERDATA, (DWORD)(Edit *)this);
  }
}

void 
Edit::unlink()
{
  if (hwnd)
  {
    SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)wndproc);
    SetWindowLong(hwnd, GWL_USERDATA, 0);
  }

  dlg = 0;
  item = 0;
  hwnd = 0;
}

void
Edit::enable(bool enabled)
{
  EnableWindow(hwnd, enabled);
}

LRESULT CALLBACK 
Edit::SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Edit *iam = (Edit *)GetWindowLong(hwnd, GWL_USERDATA);

  switch (msg) 
  { 
    case WM_GETDLGCODE:
      return CallWindowProc(iam->wndproc, hwnd, msg, wParam, lParam) | DLGC_WANTALLKEYS;

    case WM_KILLFOCUS:
      if (iam->editing)
      {
        if (iam->set_value())
        {
          iam->print_value();
          SendMessage(iam->dlg, WM_COMMAND, MAKEWPARAM(iam->item, CB_ENTER), 0); 
        }
        else
        {
          iam->restore_value();
          iam->print_value();
          MessageBox(0, "Not a floating point value", "Error", MB_ICONEXCLAMATION | MB_OK);
        }
        iam->editing = false;
      }
      break;

    case WM_KEYDOWN: 
      if (!iam->editing)
      {
        iam->backup_value();
        iam->editing = true;
      } 

      if (iam->editing && wParam == VK_RETURN)
      {
        if (iam->set_value())
        {
          iam->editing = false;
          SendMessage(iam->dlg, WM_COMMAND, MAKEWPARAM(iam->item, CB_ENTER), 0); 
        }
        else
          MessageBox(0, "Not a floating point value", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0; 
      }

      if (iam->editing && wParam == VK_ESCAPE)
      {
        iam->restore_value();
        iam->print_value();
        iam->editing = false;
        return 0;
      }

      break;
                            
    case WM_KEYUP: 
    case WM_CHAR: 
      switch (wParam) 
      { 
        case VK_RETURN: 
        case VK_ESCAPE: 
          return 0; 
      }
      break;

  } // switch  (msg)

  // Call the original window procedure for default processing. 
  return CallWindowProc(iam->wndproc, hwnd, msg, wParam, lParam); 
}

bool
DoubleEdit::set_value()
{
  char buf[256];
  char *stop;

  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, 256, (LONG)buf))
  {
    value = 0;
    return true;
  }

  double new_value = strtod(buf, &stop);
  if (*stop || errno == ERANGE) 
    return false;

  value = new_value;
  return true;
}

void
DoubleEdit::backup_value()
{
  old_value = value;
}

void 
DoubleEdit::restore_value()
{
  value = old_value;
}


void 
DoubleEdit::print_value()
{
  char buf[256];
  sprintf(buf, "%.4g", value);
  SetDlgItemText(dlg, item, buf);
}
