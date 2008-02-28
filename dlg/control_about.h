/*
  Build info, credits, links
*/

#ifndef CONTROL_ABOUT_H
#define CONTROL_ABOUT_H

#include "../controls.h"

class ControlAbout : public Controller
{
protected:
  LinkButton  lnk_home;
  LinkButton  lnk_forum;
  LinkButton  lnk_email;

public:
  ControlAbout(HWND dlg);
  virtual void init();
};

#endif
