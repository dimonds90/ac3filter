#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "controls.h"


Edit::~Edit()
{
  unlink();
}

void
Edit::link(HWND _dlg, int _item)
{
  if (hwnd) unlink();

  dlg = _dlg;
  item = _item;
  hwnd = GetDlgItem(_dlg, _item);
  if (hwnd)
  {
    wndproc = (WNDPROC) SetWindowLong(hwnd, GWL_WNDPROC, (DWORD) SubClassProc);
    SetWindowLong(hwnd, GWL_USERDATA, (DWORD)(Edit *)this);
  }
}

void 
Edit::unlink()
{
  if (hwnd)
  {
    SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)wndproc);
    SetWindowLong(hwnd, GWL_USERDATA, 0);
  }

  dlg = 0;
  item = 0;
  hwnd = 0;
}

void
Edit::enable(bool enabled)
{
  EnableWindow(hwnd, enabled);
}

LRESULT CALLBACK 
Edit::SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Edit *iam = (Edit *)GetWindowLong(hwnd, GWL_USERDATA);

  switch (msg) 
  { 
    case WM_GETDLGCODE:
      return CallWindowProc(iam->wndproc, hwnd, msg, wParam, lParam) | DLGC_WANTALLKEYS;

    case WM_KILLFOCUS:
      if (iam->editing)
      {
        if (iam->read_value())
        {
          iam->write_value();
          SendMessage(iam->dlg, WM_COMMAND, MAKEWPARAM(iam->item, CB_ENTER), 0); 
        }
        else
        {
          iam->restore_value();
          iam->write_value();
          MessageBox(0, "Incorrect value", "Error", MB_ICONEXCLAMATION | MB_OK);
        }
        iam->editing = false;
      }
      break;

    case WM_KEYDOWN: 
      if (!iam->editing)
      {
        iam->backup_value();
        iam->editing = true;
      } 

      if (iam->editing && wParam == VK_RETURN)
      {
        if (iam->read_value())
        {
          iam->editing = false;
          SendMessage(iam->dlg, WM_COMMAND, MAKEWPARAM(iam->item, CB_ENTER), 0); 
        }
        else
          MessageBox(0, "Incorrect value", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0; 
      }

      if (iam->editing && wParam == VK_ESCAPE)
      {
        iam->restore_value();
        iam->write_value();
        iam->editing = false;
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
  return CallWindowProc(iam->wndproc, hwnd, msg, wParam, lParam); 
}

bool
DoubleEdit::read_value()
{
  char buf[256];
  char *stop;

  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, 256, (LONG)buf))
  {
    value = 0;
    return true;
  }

  double new_value = strtod(buf, &stop);
  if (*stop || errno == ERANGE) 
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
  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, size, (LONG)value))
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


LinkButton::~LinkButton()
{
  unlink();
}

void
LinkButton::link(HWND _dlg, int _item)
{
  if (hwnd) unlink();

  dlg = _dlg;
  item = _item;
  hwnd = GetDlgItem(_dlg, _item);
  if (hwnd)
  {
    wndproc = (WNDPROC) SetWindowLong(hwnd, GWL_WNDPROC, (DWORD) SubClassProc);
    SetWindowLong(hwnd, GWL_USERDATA, (DWORD)(Edit *)this);
  }

  // Create underlined font
  HFONT dlg_font = (HFONT)SendDlgItemMessage(dlg, item, WM_GETFONT, 0, 0);
  LOGFONT logfont;
  GetObject(dlg_font, sizeof(LOGFONT), &logfont);
  logfont.lfUnderline = TRUE;
  font = CreateFontIndirect(&logfont);
}

void 
LinkButton::unlink()
{
  if (hwnd)
  {
    SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)wndproc);
    SetWindowLong(hwnd, GWL_USERDATA, 0);
  }
  DeleteObject(font);

  dlg = 0;
  item = 0;
  hwnd = 0;
  font = 0;
}

LRESULT CALLBACK 
LinkButton::SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  LinkButton *iam = (LinkButton *)GetWindowLong(hwnd, GWL_USERDATA);

  switch (msg) 
  { 
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC dc = BeginPaint(hwnd, &ps);
      iam->paint(dc);
      EndPaint(hwnd, &ps);
      return 0;
    }

    case WM_LBUTTONDOWN:
    {
      iam->press();
      return 0;
    }

  } 
        
  // Call the original window procedure for default processing. 
  return CallWindowProc(iam->wndproc, hwnd, msg, wParam, lParam); 
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
//  DeleteObject(font);
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

  if (i < link_text_len)
    ShellExecute(hwnd, 0, link_text + i, 0, 0, SW_SHOWMAXIMIZED);
  else
    ShellExecute(hwnd, 0, link_text, 0, 0, SW_SHOWMAXIMIZED);

  // execute url
}
