#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <windows.h>
#include "controls.h"
#include "wincomp.h"

///////////////////////////////////////////////////////////////////////////////
// SubclassedControl
///////////////////////////////////////////////////////////////////////////////

void
SubclassedControl::link(HWND _dlg, int _item)
{
  if (hwnd) unlink();

  dlg = _dlg;
  item = _item;
  hwnd = GetDlgItem(_dlg, _item);
  if (hwnd)
  {
    next_wndproc = (WNDPROC) SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)SubClassProc);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(SubclassedControl *)this);
  }
  on_link();
}

void 
SubclassedControl::unlink()
{
  on_unlink();
  if (hwnd)
  {
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)next_wndproc);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
  }

  dlg = 0;
  item = 0;
  hwnd = 0;
}

void
SubclassedControl::enable(bool enabled)
{
  EnableWindow(hwnd, enabled);
}

LRESULT CALLBACK
SubclassedControl::SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  SubclassedControl *iam = (SubclassedControl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (iam) return iam->wndproc(hwnd, msg, wParam, lParam);
  else return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Edit, IntEdit, DoubleEdit, TextEdit controls
///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK
Edit::wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) 
  { 
    case WM_GETDLGCODE:
    {
      LRESULT dlgcode = SubclassedControl::wndproc(hwnd, msg, wParam, lParam);
      if (wParam == VK_RETURN || wParam == VK_ESCAPE)
         dlgcode |= DLGC_WANTMESSAGE;
      return dlgcode;
    }

    case WM_KILLFOCUS:
      if (editing)
      {
        editing = false;
        if (read_value())
        {
          write_value();
          SendMessage(dlg, WM_COMMAND, MAKEWPARAM(item, CB_ENTER), 0); 
        }
        else
        {
          restore_value();
          write_value();
          MessageBox(dlg, incorrect_value(), "Error", MB_ICONEXCLAMATION | MB_OK);
        }
      }
      break;

    case WM_KEYDOWN: 
      if (!editing)
      {
        backup_value();
        editing = true;
      } 

      if (editing && wParam == VK_RETURN)
      {
        editing = false;
        if (read_value())
        {
          write_value();
          SendMessage(dlg, WM_COMMAND, MAKEWPARAM(item, CB_ENTER), 0); 
        }
        else
        {
          restore_value();
          write_value();
          MessageBox(dlg, incorrect_value(), "Error", MB_ICONEXCLAMATION | MB_OK);
        }
        return 0; 
      }

      if (editing && wParam == VK_ESCAPE)
      {
        restore_value();
        write_value();
        editing = false;
        return 0;
      }

      break;
                            
    case WM_KEYUP: 
    case WM_CHAR: 
      switch (wParam) 
      { 
        case VK_RETURN: 
        case VK_ESCAPE: 
          return 0; 
      }
      break;

  } // switch  (msg)

  // Call the original window procedure for default processing. 
  return SubclassedControl::wndproc(hwnd, msg, wParam, lParam);
}

void
Edit::on_link()
{
  write_value();
}

bool
IntEdit::read_value()
{
  char buf[256];

  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, 256, (LPARAM) buf))
  {
    value = 0;
    return true;
  }

  int new_value = 0;
  char tmp;
  if (sscanf(buf, "%i%c", &new_value, &tmp) != 1)
    return false;

  value = new_value;
  return true;
}

void
IntEdit::backup_value()
{
  old_value = value;
}

void 
IntEdit::restore_value()
{
  value = old_value;
}

void 
IntEdit::write_value()
{
  char buf[256];
  sprintf(buf, "%i", value);
  SetDlgItemText(dlg, item, buf);
}

bool
DoubleEdit::read_value()
{
  char buf[256];

  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, 256, (LPARAM) buf))
  {
    value = 0;
    return true;
  }

  double new_value = 0.0;
  char tmp;
  if (sscanf(buf, "%lg%c", &new_value, &tmp) != 1)
    return false;

  value = new_value;
  return true;
}

void
DoubleEdit::backup_value()
{
  old_value = value;
}

void 
DoubleEdit::restore_value()
{
  value = old_value;
}

void 
DoubleEdit::write_value()
{
  char buf[256];
  sprintf(buf, "%.4g", value);
  SetDlgItemText(dlg, item, buf);
}

TextEdit::TextEdit(size_t _size)
{
  size = 0;
  value = new char[_size+1];
  old_value = new char[_size+1];
  value[0] = 0;
  old_value[0] = 0;

  if (value && old_value)
    size = _size;
}

TextEdit::~TextEdit()
{
  if (value) delete value;
  if (old_value) delete old_value;
}

bool
TextEdit::read_value()
{
  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, size, (LPARAM) value))
    value[0] = 0;
  else
    value[size] = 0;

  return true;
}

void
TextEdit::backup_value()
{
  strcpy(old_value, value);
}

void 
TextEdit::restore_value()
{
  strcpy(value, old_value);
}

void 
TextEdit::write_value()
{
  SetDlgItemText(dlg, item, value);
}

void 
TextEdit::set_text(const char *_text)
{
  strncpy(value, _text, size);
  value[size] = 0;
  write_value();
}

///////////////////////////////////////////////////////////////////////////////
// LinkButton
///////////////////////////////////////////////////////////////////////////////

void
LinkButton::on_link()
{
  // Create underlined font
  HFONT dlg_font = (HFONT)SendDlgItemMessage(dlg, item, WM_GETFONT, 0, 0);
  LOGFONT logfont;
  GetObject(dlg_font, sizeof(LOGFONT), &logfont);
  logfont.lfUnderline = TRUE;
  font = CreateFontIndirect(&logfont);
}

void 
LinkButton::on_unlink()
{
  DeleteObject(font);
  font = 0;
}

LRESULT CALLBACK 
LinkButton::wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) 
  { 
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC dc = BeginPaint(hwnd, &ps);
      paint(dc);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_LBUTTONDOWN:
    {
      press();
      return 0;
    }
  } 

  // Call the original window procedure for default processing. 
  return SubclassedControl::wndproc(hwnd, msg, wParam, lParam);
}

void 
LinkButton::paint(HDC dc)
{
  RECT client_rect;
  GetClientRect(hwnd, &client_rect);

  int i;
  char link_text[256];
  int link_text_len = GetWindowText(hwnd, link_text, 256);

  // find description text
  for (i = 0; i < link_text_len; i++)
    if (link_text[i] == '|')
      break;

  link_text_len = i;

  // Draw description text or url
  HFONT old_font = (HFONT)SelectObject(dc, font);
  COLORREF old_color = SetTextColor(dc, RGB(0, 0, 255));
  DrawText(dc, link_text, link_text_len, &client_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  SetTextColor(dc, old_color);
  SelectObject(dc, old_font);
}

void 
LinkButton::press()
{
  int i;
  char link_text[256];
  int link_text_len = GetWindowText(hwnd, link_text, 256);

  // find url
  for (i = 0; i < link_text_len; i++)
    if (link_text[i] == '|')
    {
      i++;
      break;
    }

  // execute url
  if (i < link_text_len)
    ShellExecute(hwnd, 0, link_text + i, 0, 0, SW_SHOWMAXIMIZED);
  else
    ShellExecute(hwnd, 0, link_text, 0, 0, SW_SHOWMAXIMIZED);
}
