#include <windows.h>
#include <ocidl.h>
#include <olectl.h>

#include "tray.h"
#include "win32\thread.h"
#include "resource_ids.h"

///////////////////////////////////////////////////////////////////////////////
// PropThread
//
// Displays property page to configure a filter if it supports 
// ISpecifyPropertyPages interface.
//
// We use OleCreatePropertyFrame() to diaplay property page. It is modal dialog
// and blocks the thread it was started from. Because of this we have to create
// a separate thread for each property page (muiltiple modal property pages at
// the same thread work incorrectly).

class PropThread : public Thread
{
protected:
  HWND      hwnd_parent;
  IUnknown *filter;

  DWORD process()
  {
    ISpecifyPropertyPages *spp;
    CAUUID cauuid;

    if FAILED(filter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&spp))
      return 1;

    if FAILED(spp->GetPages(&cauuid))
      return 2;

    spp->Release();

    if FAILED(OleCreatePropertyFrame(hwnd_parent, 30, 30, NULL, 1, &filter, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL))
    {
      CoTaskMemFree(cauuid.pElems);
      return 3;
    }

    CoTaskMemFree(cauuid.pElems);
    return 0;
  }

public:
  PropThread()
  {
    hwnd_parent = 0;
    filter = 0;
  }

  bool start(HWND _hwnd_parent, IUnknown *_filter)
  {
    if (!_filter)
      return false;

    if (thread_exists())
      return false;

    hwnd_parent = _hwnd_parent;
    filter = _filter;
    create(false);
    return true;
  }

  void stop()
  {
    if (thread_exists())
    {
      PostThreadMessage(thread_id(), WM_QUIT, 0, 0);
      terminate();
    }
  }

  bool is_visible()
  {
    return thread_exists();
  }
};

///////////////////////////////////////////////////////////////////////////////
// AC3Filter tray
//
// Tray icon to display for each AC3Filter instance.

AC3FilterTray::AC3FilterTray(IAC3Filter *_filter)
{
  filter = _filter;
  dialog = new PropThread();
  visible = false;

  /////////////////////////////////////////////////////////
  // Window to receive messages

  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = TrayProc;
  wc.cbClsExtra  = 0;
  wc.cbWndExtra  = 0;
  wc.hInstance   = GetModuleHandle(0);
  wc.hIcon       = 0;
  wc.hCursor     = 0;
  wc.hbrBackground = 0;
  wc.lpszMenuName  = 0;
  wc.lpszClassName = "AC3Filter control";

  RegisterClass(&wc);
  hwnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    wc.lpszClassName,
    0,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    0,
    0,
    GetModuleHandle(0),
    0);

  if (hwnd)
    SetWindowLong(hwnd, GWL_USERDATA, (DWORD)this); 

  /////////////////////////////////////////////////////////
  // Tray icon itself

  hicon = LoadIcon(GetModuleHandle("ac3filter.ax"), MAKEINTRESOURCE(IDI_AC3FILTER));

  /////////////////////////////////////////////////////////
  // NOTIFYICONDATA used to control tray icon

  memset(&nid, 0, sizeof(nid));
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hwnd;
  nid.uID = 1;
  nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  nid.uCallbackMessage = WM_USER;
  nid.hIcon = hicon;
  lstrcpy(nid.szTip, "AC3Filter configuration");
}

AC3FilterTray::~AC3FilterTray()
{
  if (visible)
    hide();

  if (hwnd)
    DestroyWindow(hwnd);

  if (hicon)
    DestroyIcon(hicon);

  if (dialog)
    delete dialog;
}

void
AC3FilterTray::show()
{
  if (visible)
    return;

  Shell_NotifyIcon(NIM_ADD, &nid);
  visible = true;
}

void
AC3FilterTray::hide()
{
  if (!visible)
    return;

  if (dialog)
    dialog->stop();

  Shell_NotifyIcon(NIM_DELETE, &nid);
  visible = false;
}

void
AC3FilterTray::l_click()
{
  if (!dialog || !filter)
    return;

  if (dialog->is_visible())
    dialog->stop();
  else
    dialog->start(0, filter);
}

void
AC3FilterTray::r_click()
{
}


LRESULT CALLBACK 
AC3FilterTray::TrayProc(HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
  AC3FilterTray *iam = (AC3FilterTray *)GetWindowLong(_hwnd, GWL_USERDATA);

  switch (_lParam)
  {
    case WM_LBUTTONDOWN:
      iam->l_click();
      return 0;

    case WM_RBUTTONDOWN:
      iam->r_click();
      return 0;
  }
  return DefWindowProc(_hwnd, _uMsg, _wParam, _lParam);
}