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
  HMENU hmenu;
  NOTIFYICONDATA nid;

  HMENU create_menu() const;

  static LRESULT CALLBACK TrayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  AC3FilterTray(IAC3Filter *filter);
  ~AC3FilterTray();

  // tray icon control
  void show();
  void hide();

  // control actions
  void config();
  void popup_menu();
  void preset(const char *preset);
};

#endif
