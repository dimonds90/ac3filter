/*
  Mixing matrix, including:
  * Matrix itself
  * Matrix options
  * Center/Surround/LFE gain levels
*/

#ifndef CONTROL_MATRIX_H
#define CONTROL_MATRIX_H

#include "../guids.h"
#include "../controls.h"

class ControlMatrix : public Controller
{
protected:
  IAudioProcessor *proc;

  matrix_t matrix;
  bool auto_matrix, normalize_matrix, voice_control, expand_stereo;
  sample_t clev, slev, lfelev;

  DoubleEdit edt_voice;
  DoubleEdit edt_sur;
  DoubleEdit edt_lfe;
  DoubleEdit edt_matrix[8][8];

public:
  ControlMatrix(HWND dlg, IAudioProcessor *proc);
  ~ControlMatrix();

  virtual void init();
  virtual void update();
  virtual void update_matrix();
  virtual void update_dynamic();

  virtual cmd_result command(int control, int message);
};

#endif
