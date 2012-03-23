/*
  Decoder info
*/

#ifndef CONTROL_INFO_H
#define CONTROL_INFO_H

#include "../guids.h"
#include "../controls.h"

class ControlInfo : public Controller
{
protected:
  IDecoder *dec;

  int      frames;
  int      errors;
  char     info[4096];

public:
  ControlInfo(HWND dlg, IDecoder *dec);
  ~ControlInfo();

  virtual void update_dynamic();
};

#endif
