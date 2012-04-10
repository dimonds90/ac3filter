#include "control_list.h"

ControlList::ControlList(HWND _dlg):
Controller(_dlg, 0)
{}


ControlList::~ControlList()
{
  drop();
}

void ControlList::add(Controller *new_ctrl)
{
  ctrl.push_back(new_ctrl);
}

void ControlList::drop()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    delete *iter;
}

void ControlList::init()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    (*iter)->init();
};

void ControlList::update()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    (*iter)->update();
};

void ControlList::update_dynamic()
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    (*iter)->update_dynamic();
};

bool ControlList::own_control(int control)
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    if ((*iter)->own_control(control))
      return true;
  return false;
}

ControlList::cmd_result ControlList::command(int control, int message)
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    if ((*iter)->own_control(control))
      return (*iter)->command(control, message);
  return cmd_not_processed;
}

ControlList::cmd_result ControlList::notify(int control, int message, LPNMHDR nmhdr, INT_PTR &result)
{
  for (VCtrl::iterator iter = ctrl.begin(); iter < ctrl.end(); iter++)
    if ((*iter)->own_control(control))
      return (*iter)->notify(control, message, nmhdr, result);
  return cmd_not_processed;
}

