/*
  CPU usage
*/

#ifndef CONTROL_CPU_H
#define CONTROL_CPU_H

#include "../guids.h"
#include "../controls.h"

class ControlCPU : public Controller
{
protected:
  IAC3Filter *filter;
  bool invert_levels;

public:
  ControlCPU(HWND dlg, IAC3Filter *filter, bool invert_levels);
  ~ControlCPU();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();
};

#endif
