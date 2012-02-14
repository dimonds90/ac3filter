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

  bool invert_levels;
  bool bass_redir;
  int  bass_freq;
  int  bass_channels;

  DoubleEdit edt_bass_freq;

public:
  ControlBass(HWND dlg, IAudioProcessor *proc, bool invert_levels);
  ~ControlBass();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
