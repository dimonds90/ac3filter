#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#include "../dlg_base.h"
#include "../controls.h"

class ErrorReportDlg : public DialogBase
{
protected:
  virtual void on_create();
  virtual BOOL on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  bool send_audio_data;
  TextEdit edt_feedback;
  TextEdit edt_desc;

  ErrorReportDlg(): DialogBase()
  {}
};

#endif
