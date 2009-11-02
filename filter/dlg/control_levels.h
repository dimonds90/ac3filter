/*
  Input/output levels
*/

#ifndef CONTROL_LEVELS_H
#define CONTROL_LEVELS_H

#include "../guids.h"
#include "../controls.h"

#define NLEVELS 8

class ControlLevels : public Controller
{
protected:
  IAC3Filter *filter;
  IAudioProcessor *proc;

  bool invert_levels;
  bool overflow[NLEVELS];

public:
  ControlLevels(HWND dlg, IAC3Filter *filter, IAudioProcessor *proc, bool invert_levels);
  ~ControlLevels();

  virtual void init();
  virtual void update_dynamic();
};

#endif
