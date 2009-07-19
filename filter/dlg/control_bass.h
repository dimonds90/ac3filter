/*
  Bass redirection
*/

#ifndef CONTROL_BASS_H
#define CONTROL_BASS_H

#include "../guids.h"
#include "../controls.h"

class ControlBass : public Controller
{
protected:
  IAudioProcessor *proc;

  bool bass_redir;
  int  bass_freq;
  DoubleEdit edt_bass_freq;

public:
  ControlBass(HWND dlg, IAudioProcessor *proc);
  ~ControlBass();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
