/*
  System options:
  * Allowed formats
  * DirectShow options
  * Waveout/DirectSound switch
  * Filter merit
*/

#ifndef CONTROL_SYSTEM_H
#define CONTROL_SYSTEM_H

#include "../guids.h"
#include "../controls.h"

class ControlSystem : public Controller
{
protected:
  IAC3Filter *filter;
  IDecoder   *dec;

  int  formats;
  int  reinit;
  bool spdif_no_pcm;
  bool query_sink;

public:
  ControlSystem(HWND dlg, IAC3Filter *filter, IDecoder *dec);
  ~ControlSystem();

  virtual void init();
  virtual void update();
  virtual cmd_result command(int control, int message);
  virtual cmd_result notify(int control, int message, LPNMHDR nmhdr, INT_PTR &result);
};

#endif
