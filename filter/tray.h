#ifndef AC3FILTER_TRAY_H
#define AC3FILTER_TRAY_H

#include <windows.h>
#include "guids.h"

class AC3FilterTrayImpl;

class AC3FilterTray
{
protected:
  AC3FilterTrayImpl *pimpl;

public:
  AC3FilterTray();
  ~AC3FilterTray();

  // tray icon control
  void show();
  void hide();

  // Filter registration
  void register_filter(IAC3Filter *filter);
  void unregister_filter(IAC3Filter *filter);

  // Filter state
  void play(IAC3Filter *filter);
  void pause(IAC3Filter *filter);
  void stop(IAC3Filter *filter);

  // control actions
  void preset(const char *preset);
};

extern AC3FilterTray ac3filter_tray;

#endif
