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

  bool     bass_redir;
  int      bass_freq;
  sample_t bass_gain;
  bool     bass_hpf;
  int      bass_channels;

  DoubleEdit edt_bass_freq;
  DoubleEdit edt_bass_gain;

public:
  ControlBass(HWND dlg, IAudioProcessor *proc);
  ~ControlBass();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
