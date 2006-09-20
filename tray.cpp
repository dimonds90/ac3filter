#include <windows.h>
#include <ocidl.h>
#include <olectl.h>
#include <stdio.h>

#include "guids.h"
#include "tray.h"
#include "win32\thread.h"
#include "resource_ids.h"

#define WM_TRAY_ICON (WM_USER + 10)

#define CMD_CONFIG       (100)
#define CMD_FIRST_PRESET (200)
#define CMD_LAST_PRESET  (300)

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

  hmenu = 0;
  hicon = 0;
  hwnd = 0;

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
  nid.uCallbackMessage = WM_TRAY_ICON;
  nid.hIcon = hicon;
  lstrcpy(nid.szTip, "AC3Filter configuration");
}

AC3FilterTray::~AC3FilterTray()
{
  if (visible)
    hide();

  if (hmenu)  DestroyMenu(hmenu);
  if (hwnd)   DestroyWindow(hwnd);
  if (hicon)  DestroyIcon(hicon);
  if (dialog) delete dialog;
}

HMENU
AC3FilterTray::create_menu() const
{
  HMENU menu = CreatePopupMenu();
  if (!menu) return 0;

  /////////////////////////////////////////////////////////
  // Fill presets

  HKEY key;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PRESET, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    char buf[256];
    DWORD len = sizeof(buf);

    int i = 0;
    int npresets = CMD_LAST_PRESET - CMD_FIRST_PRESET;
    while (i < npresets && RegEnumKeyEx(key, i++, (LPTSTR)buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)
    {
      AppendMenu(menu, MF_BYPOSITION | MF_STRING, CMD_FIRST_PRESET + i, (LPCTSTR)buf);
      len = sizeof(buf);
    }
    RegCloseKey(key);
    AppendMenu(menu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
  }

  /////////////////////////////////////////////////////////
  // Add 'Config' command (default)

  AppendMenu(menu, MF_BYPOSITION | MF_STRING, CMD_CONFIG, (LPCTSTR)"AC3Filter config");
  SetMenuDefaultItem(menu, CMD_CONFIG, FALSE);
  return menu;
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
AC3FilterTray::preset(const char *preset)
{
  if (!filter) return;

  IDecoder *dec;
  if FAILED(filter->QueryInterface(IID_IDecoder, (void **)&dec))
    return;

  char buf[256];
  sprintf(buf, REG_KEY_PRESET"\\%s", preset);
  RegistryKey reg(buf);
  dec->load_params(&reg, AC3FILTER_ALL);
  dec->Release();
}

void
AC3FilterTray::config()
{
  if (!dialog || !filter)
    return;

  if (dialog->is_visible())
    dialog->stop();
  else
    dialog->start(0, filter);
}

void
AC3FilterTray::popup_menu()
{
  POINT mouse;

  if (hmenu) DestroyMenu(hmenu);
  hmenu = create_menu();

  GetCursorPos(&mouse);
  SetForegroundWindow(hwnd);
  TrackPopupMenu(hmenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, mouse.x, mouse.y, 0, hwnd, 0);
  PostMessage(hwnd, WM_NULL, 0, 0);
}


LRESULT CALLBACK 
AC3FilterTray::TrayProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  AC3FilterTray *iam = (AC3FilterTray *)GetWindowLong(hwnd, GWL_USERDATA);
  int cmd_id;

  switch (Msg)
  {
    ///////////////////////////////////
    // Tray icon messages

    case WM_TRAY_ICON:
      switch (lParam)
      {
        case WM_LBUTTONDOWN:
          iam->config();
          return 0;

        case WM_RBUTTONDOWN:
          iam->popup_menu();
          return 0;
      }
      break;

    ///////////////////////////////////
    // Menu

    case WM_COMMAND:
      cmd_id = LOWORD(wParam);
      if (cmd_id >= CMD_FIRST_PRESET && cmd_id < CMD_LAST_PRESET)
      {
        char buf[256];
        if (!GetMenuString(iam->hmenu, cmd_id, (LPTSTR)buf, sizeof(buf), MF_BYCOMMAND))
          return 0;

        iam->preset(buf);
        return 0;
      }

      switch (cmd_id)
      {
        case CMD_CONFIG:
          iam->config();
          return 0;
      }
      break;
  }

  return DefWindowProc(hwnd, Msg, wParam, lParam);
}