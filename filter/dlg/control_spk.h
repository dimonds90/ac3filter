/*
  User format selection, including 'Use SPDIF' checkbox.
*/

#ifndef CONTROL_SPK_H
#define CONTROL_SPK_H

#include "../guids.h"
#include "../controls.h"

class ControlSpk : public Controller
{
protected:
  IDecoder *dec;

  Speakers in_spk;
  Speakers user_spk;
  bool     use_spdif;
  int      spdif_status;

public:
  ControlSpk(HWND dlg, IDecoder *dec);
  ~ControlSpk();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
