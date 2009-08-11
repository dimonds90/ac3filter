/*
  Translation control
*/

#ifndef CONTROL_LANG_H
#define CONTROL_LANG_H

#include "../guids.h"
#include "../controls.h"

class EnumLanguages;
class ControlLang : public Controller
{
protected:
  LinkButton lnk_translate;
  char path[MAX_PATH];
  EnumLanguages *langs;

public:
  ControlLang(HWND dlg);
  ~ControlLang();

  virtual void init();
  virtual void update();
  virtual cmd_result command(int control, int message);
};

#endif
