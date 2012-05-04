/*
  Controls used at config dialog.

  SubclassedControl
  -----------------
  Use subclassing to alter control behavior.

  Edit
  ----
  Base for edit controls. Subclasses an edit control to make:
  * 'enter' checks the value and either accepts it or reports an error and
    restores old value.
  * 'escape' just restores an old value.
  * when control looses focus it acts as 'enter'.

  DoubleEdit
  ----------
  Numeric edit control.

  IntEdit
  ----------
  Integer edit control.

  TextEdit
  --------
  String edit control.

  LinkButton
  ----------
  A simple link control

*/

#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>

#define CB_ENTER (WM_USER+1)


class Controller
{
protected:
  HWND hdlg;
  const int *controls;

public:
  enum cmd_result { cmd_ok, cmd_fail, cmd_not_processed, cmd_update, cmd_init };

  Controller(HWND _hdlg, const int *_controls): hdlg(_hdlg), controls(_controls) {}
  virtual ~Controller() {}

  virtual void init() {};
  virtual void update() {};
  virtual void update_dynamic() {};

  virtual bool own_control(int control)
  {
    int i = 0;
    if (controls)
      while (controls[i] != 0)
        if (control == controls[i++])
          return true;
    return false;
  }

  virtual cmd_result command(int control, int message) { return cmd_not_processed; }
  virtual cmd_result notify(int control, int message, LPNMHDR nmhdr, INT_PTR &result) { return cmd_not_processed; }
};


class SubclassedControl
{
protected:
  static LRESULT CALLBACK SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  HWND    dlg;
  HWND    hwnd;
  int     item;
  WNDPROC next_wndproc;

  virtual void on_link() {};
  virtual void on_unlink() {};
  virtual LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  { return CallWindowProc(next_wndproc, hwnd, msg, wParam, lParam); }

public:
  SubclassedControl(): dlg(0), hwnd(0), item(0), next_wndproc(0) {};
  virtual ~SubclassedControl() { unlink(); }

  void link(HWND dlg, int item);
  void unlink();
  void enable(bool enabled);
};


class Edit : public SubclassedControl
{
protected:
  bool editing;
  virtual bool read_value() = 0;
  virtual void backup_value() = 0;
  virtual void restore_value() = 0;
  virtual void write_value() = 0;
  virtual const char *incorrect_value() { return "Incorrect value"; }
  virtual LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  virtual void on_link();

public:
  Edit(): editing(false) {};
};

class IntEdit : public Edit
{
protected:
  int old_value;

  bool read_value();
  void backup_value();
  void restore_value();
  void write_value();
  const char *incorrect_value() { return "Incorrect value: must be an integer number"; }

public:
  int value;
  IntEdit(): value(0) {};

  void update_value(int _value) { value = _value; write_value(); };
};

class DoubleEdit : public Edit
{
protected:
  double old_value;

  bool read_value();
  void backup_value();
  void restore_value();
  void write_value();
  const char *incorrect_value() { return "Incorrect value: must be number"; }

public:
  double value;
  DoubleEdit(): value(0) {};

  void update_value(double _value) { value = _value; write_value(); };
};

class TextEdit : public Edit
{
protected:
  size_t size;
  char *old_value;
  char *value;

  bool read_value();
  void backup_value();
  void restore_value();
  void write_value();

public:
  TextEdit(size_t size = 256);
  ~TextEdit();

  void set_text(const char *text);
  const char *get_text() { return value; };
};

class LinkButton : public SubclassedControl
{
protected:
  HFONT   font;

  virtual void on_link();
  virtual void on_unlink();
  virtual LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  virtual void paint(HDC dc);
  virtual void press();

public:
  LinkButton(): font(0) {};
};

#endif
