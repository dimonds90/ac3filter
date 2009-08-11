/*
  SPDIF options (not including 'Use SPDIF' checkbox)
*/

#ifndef CONTROL_SPDIF_H
#define CONTROL_SPDIF_H

#include "../guids.h"
#include "../controls.h"

class ControlSPDIF : public Controller
{
protected:
  IDecoder *dec;

  int      spdif_pt;
  bool     spdif_as_pcm;
  bool     spdif_encode;
  bool     spdif_stereo_pt;
  int      spdif_bitrate;

  bool     spdif_check_sr;
  bool     spdif_allow_48;
  bool     spdif_allow_44;
  bool     spdif_allow_32;

  int      dts_mode;
  int      dts_conv;

  bool     use_detector;

public:
  ControlSPDIF(HWND dlg, IDecoder *dec);
  ~ControlSPDIF();

  virtual void init();
  virtual void update();

  virtual cmd_result command(int control, int message);
};

#endif
