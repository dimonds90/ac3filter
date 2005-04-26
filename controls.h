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

  virtual bool read_value() = 0;
  virtual void backup_value() = 0;
  virtual void restore_value() = 0;
  virtual void write_value() = 0;

public:
  Edit(): dlg(0), hwnd(0), item(0), wndproc(0), editing(false) {};
  ~Edit();

  void link(HWND dlg, int item);
  void unlink();
  void enable(bool enabled);
};

class DoubleEdit : public Edit
{
protected:
  double old_value;

  bool read_value();
  void backup_value();
  void restore_value();
  void write_value();

public:
  double value;
  DoubleEdit() {};

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

class LinkButton
{
protected:
  HWND    dlg;
  HWND    hwnd;
  int     item;
  WNDPROC wndproc;
  HFONT   font;

  static LRESULT CALLBACK SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
  LinkButton(): dlg(0), hwnd(0), item(0), wndproc(0) {};
  ~LinkButton();

  void link(HWND dlg, int item);
  void unlink();  

  virtual void paint(HDC dc);
  virtual void press();
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
