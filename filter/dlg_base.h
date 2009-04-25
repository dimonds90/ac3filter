#ifndef DLG_H
#define DLG_H

#include <windows.h>

class DialogBase
{
private:
  static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
  HWND         hwnd;
  HWND         parent;
  HINSTANCE    hinstance;

  /////////////////////////////////////////////////////////
  // Placeholders
  
  virtual void on_create() {}
  virtual void on_destroy() {}
  virtual void on_show() {}
  virtual void on_hide() {}
  virtual void translate();

  virtual BOOL on_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return FALSE; }

public:
  DialogBase();
  virtual ~DialogBase();

  INT_PTR exec(HINSTANCE hinstance, LPCSTR dlg_res, HWND parent);
  void show() { if (hwnd) ShowWindow(hwnd, SW_SHOW); }
  void hide() { if (hwnd) ShowWindow(hwnd, SW_HIDE); }
};

#endif
