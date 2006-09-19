#ifndef AC3FILTER_TRAY_H
#define AC3FILTER_TRAY_H

#include <windows.h>
#include "guids.h"

class PropThread;

class AC3FilterTray
{
protected:
  IAC3Filter *filter;
  PropThread *dialog;
  bool visible;

  HWND  hwnd;
  HICON hicon;
  NOTIFYICONDATA nid;

  void create_window();

  static LRESULT CALLBACK TrayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  AC3FilterTray(IAC3Filter *_filter);
  ~AC3FilterTray();

  void show();
  void hide();

  void l_click();
  void r_click();
};

#endif
