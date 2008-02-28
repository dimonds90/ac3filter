/*
  All controllers at once...
*/

#ifndef CONTROL_ALL_H
#define CONTROL_ALL_H

#include <memory>
#include <vector>
#include "../guids.h"
#include "../controls.h"

class ControlAll : public Controller
{
protected:
  typedef std::auto_ptr<Controller> PCtrl;
  typedef std::vector<PCtrl> VCtrl;
  VCtrl ctrl;

public:
  ControlAll(HWND dlg, IAC3Filter *filter, IDecoder *dec, IAudioProcessor *proc, bool invert_levels);

  virtual void init();
  virtual void update();
  virtual void update_dynamic();
  virtual bool own_control(int control);
  virtual cmd_result command(int control, int message);
};

#endif
