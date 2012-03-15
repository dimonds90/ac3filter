/*
  Debug options
*/

#ifndef CONTROL_DEBUG_H
#define CONTROL_DEBUG_H

#include "../controls.h"

class ControlDebug : public Controller
{
protected:
  TextEdit edt_feedback;

public:
  ControlDebug(HWND dlg);

  virtual void init();
  virtual void update();
  virtual cmd_result command(int control, int message);
};

#endif
