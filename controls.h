#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>

#define CB_ENTER (WM_USER+1)
/*
class ComboBox
{
protected:
  static LRESULT CALLBACK SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  HWND    dlg;
  HWND    hwnd;
  int     item;
  WNDPROC wndproc;


public:
  COMBOBOXINFO info;
  ComboBox(): dlg(0), hwnd(0), item(0), wndproc(0) {};
  ~ComboBox();

  void link(HWND dlg, int item);
  void unlink();
};
*/
class Edit
{
protected:
  static LRESULT CALLBACK SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  HWND    dlg;
  HWND    hwnd;
  int     item;
  WNDPROC wndproc;
  bool    editing;


public:
  Edit(): dlg(0), hwnd(0), item(0), wndproc(0), editing(false) {};
  ~Edit();

  void link(HWND dlg, int item);
  void unlink();
  void enable(bool enabled);

  virtual bool set_value() = 0;
  virtual void backup_value() = 0;
  virtual void restore_value() = 0;
  virtual void print_value() = 0;
};

class DoubleEdit : public Edit
{
protected:
  double old_value;

public:
  double value;
  DoubleEdit() {};

  bool set_value();
  void backup_value();
  void restore_value();
  void print_value();
  void update_value(double _value) { value = _value; print_value(); };
};

/*
class Slider
{
public:
  Slider();
  ~Slider();


  void link(HWND dlg, int item);
  void unlink();
};
*/
#endif
