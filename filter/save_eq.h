#ifndef SAVE_EQ
#define SAVE_EQ

#include "dlg_base.h"

class SaveEq : public DialogBase
{
protected:
  int preset;

  virtual void on_create();
  virtual BOOL on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  SaveEq(int _preset): preset(_preset) {}
  int get_preset() const { return preset; }
};

#endif
