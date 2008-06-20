/*
  Translation control
*/

#ifndef CONTROL_LANG_H
#define CONTROL_LANG_H

#include "../guids.h"
#include "../controls.h"

class ControlLang : public Controller
{
protected:
  LinkButton  lnk_translate;

public:
  ControlLang(HWND dlg);
  ~ControlLang();

  virtual void init();
  virtual void update();
  virtual cmd_result command(int control, int message);
};

#endif
