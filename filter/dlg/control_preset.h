/*
  Preset handling
  * Global presets
  * Matrix presets
  * Equalizer preset
*/

#ifndef CONTROL_PRESET_H
#define CONTROL_PRESET_H

#include "../guids.h"
#include "../controls.h"

class ControlPreset : public Controller
{
protected:
  IDecoder *dec;
  IAudioProcessor *proc;

public:
  ControlPreset(HWND dlg, IDecoder *dec, IAudioProcessor *proc);
  ~ControlPreset();

  virtual void update();
  virtual cmd_result command(int control, int message);
};

#endif
