/*
  Represent a list of controllers as a single controller
*/

#ifndef CONTROL_LIST_H
#define CONTROL_LIST_H

#include <memory>
#include <vector>
#include "../guids.h"
#include "../controls.h"

class ControlList : public Controller
{
protected:
  typedef std::vector<Controller *> VCtrl;
  VCtrl ctrl;

public:
  ControlList(HWND dlg);
  ~ControlList();

  void add(Controller *ctrl);
  void drop();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();
  virtual bool own_control(int control);
  virtual cmd_result command(int control, int message);
  virtual cmd_result notify(int control, int message, LPNMHDR nmhdr, INT_PTR &result);
};

#endif
