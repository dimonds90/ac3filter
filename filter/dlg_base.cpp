#include <windows.h>
#include <commctrl.h>
#include "wincomp.h"
#include "dlg_base.h"


INT_PTR CALLBACK
DialogBase::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  DialogBase *dlg;
  if (uMsg == WM_INITDIALOG)
  {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);
    dlg = (DialogBase *)lParam;
    if (!dlg) return TRUE;
  }

  dlg = (DialogBase *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (!dlg) return FALSE;

  switch (uMsg)
  {
    case WM_INITDIALOG:
      dlg->hwnd = hwnd;
      dlg->on_create();
      return TRUE;

    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDOK:
        EndDialog(hwnd, IDOK);
        return TRUE;
    
      case IDCANCEL:
        EndDialog(hwnd, IDCANCEL);
        return TRUE;
      }
      break;

    case WM_DESTROY:
      dlg->on_destroy();
      dlg->hwnd = 0;
      dlg->parent = 0;
      dlg->hinstance = 0;
      return TRUE;

    case WM_SHOWWINDOW:
      if (wParam == 0)
        dlg->on_hide();
      else
        dlg->on_show();
      return TRUE;
  }

  return dlg->on_message(hwnd, uMsg, wParam, lParam);
}

DialogBase::DialogBase()
:hwnd(0), parent(0), hinstance(0)
{}

DialogBase::~DialogBase()
{}

INT_PTR
DialogBase::exec(HINSTANCE _hinstance, LPCSTR _dlg_res, HWND _parent)
{
  hwnd = 0;
  hinstance = _hinstance;
  parent = _parent;

  INT_PTR result = DialogBoxParam(hinstance, _dlg_res, parent, DialogProc, (LPARAM)this);

  hwnd = 0;
  hinstance = 0;
  parent = 0;

  return result;
}
