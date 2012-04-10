#include <windows.h>
#include <ocidl.h>
#include <olectl.h>
#include <stdio.h>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "guids.h"
#include "tray.h"
#include "win32\thread.h"
#include "resource_ids.h"
#include "wincomp.h"
#include "crc.h"
#include "ac3filter_intl.h"

#define CMD_FIRST_TRACK  (100)
#define CMD_LAST_TRACK   (200)
#define CMD_FIRST_PRESET (200)
#define CMD_LAST_PRESET  (300)

static Speakers get_input_format(IAC3Filter *filter)
{
  IDecoder *dec;
  if (filter->QueryInterface(IID_IDecoder, (void **)&dec) == S_OK)
  {
    Speakers result;
    dec->get_in_spk(&result);
    dec->Release();
    return result;
  }
  return Speakers();
}

///////////////////////////////////////////////////////////////////////////////
// PropThread
//
// Displays property page to configure a filter if it supports 
// ISpecifyPropertyPages interface.
//
// We use OleCreatePropertyFrame() to diaplay property page. It is modal dialog
// and blocks the thread it was started from. Because of this we have to create
// a separate thread for each property page (muiltiple modal property pages at
// the same thread work incorrectly).

class PropThread : public Thread
{
protected:
  HWND      hwnd_parent;
  IUnknown *filter;

  DWORD process()
  {
    ISpecifyPropertyPages *spp;
    CAUUID cauuid;

    if FAILED(filter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&spp))
      return 1;

    if FAILED(spp->GetPages(&cauuid))
      return 2;

    spp->Release();

    if FAILED(OleCreatePropertyFrame(hwnd_parent, 30, 30, NULL, 1, &filter, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL))
    {
      CoTaskMemFree(cauuid.pElems);
      return 3;
    }

    CoTaskMemFree(cauuid.pElems);
    return 0;
  }

public:
  PropThread()
  {
    hwnd_parent = 0;
    filter = 0;
  }

  bool start(HWND _hwnd_parent, IUnknown *_filter)
  {
    if (!_filter)
      return false;

    if (thread_exists())
      return false;

    hwnd_parent = _hwnd_parent;
    filter = _filter;
    create(false);
    return true;
  }

  void stop()
  {
    if (thread_exists())
    {
      PostThreadMessage(thread_id(), WM_QUIT, 0, 0);
      terminate(1000);
    }
  }

  bool is_visible()
  {
    return thread_exists();
  }

  const IUnknown *get_filter() const
  {
    return filter;
  }
};

///////////////////////////////////////////////////////////////////////////////
// AC3Filter tray

class AC3FilterTrayImpl
{
protected:
  enum state_t { state_stop, state_play, state_pause };

  struct FilterData
  {
    FilterData(IAC3Filter *filter_ = 0, int id_ = 0)
    {
      id = id_;
      filter = filter_;
      state = state_stop;
    }

    int id;
    IAC3Filter *filter;
    state_t state;
    string desc;
  };

  std::vector<FilterData> filters;
  PropThread *dialog;
  bool visible;
  int id;

  HWND  hwnd;
  HICON hicon;
  HMENU hmenu;
  NOTIFYICONDATA nid;
  HBITMAP icon_stop;
  HBITMAP icon_pause;
  HBITMAP icon_play;

  void init();
  HMENU create_menu() const;

  static LRESULT CALLBACK TrayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  AC3FilterTrayImpl();
  ~AC3FilterTrayImpl();

  // tray icon control
  void show();
  void hide();

  // Filter registration
  void register_filter(IAC3Filter *filter);
  void unregister_filter(IAC3Filter *filter);

  // Filter state
  void play(IAC3Filter *filter);
  void pause(IAC3Filter *filter);
  void stop(IAC3Filter *filter);

  // control actions
  void popup_menu();
  void preset(const char *preset);
  void preset(int hash);
};

AC3FilterTrayImpl::AC3FilterTrayImpl()
{
  dialog = new PropThread();
  visible = false;
  id = 1;

  hmenu = 0;
  hicon = 0;
  hwnd = 0;
}

AC3FilterTrayImpl::~AC3FilterTrayImpl()
{
  if (visible)
    hide();

  if (hmenu)  DestroyMenu(hmenu);
  if (hwnd)   DestroyWindow(hwnd);
  if (hicon)  DestroyIcon(hicon);
  if (icon_stop)  DeleteObject(icon_stop);
  if (icon_pause) DeleteObject(icon_pause);
  if (icon_play)  DeleteObject(icon_play);
  if (dialog) delete dialog;
}

void
AC3FilterTrayImpl::init()
{
  /////////////////////////////////////////////////////////
  // Window to receive messages

  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = TrayProc;
  wc.cbClsExtra  = 0;
  wc.cbWndExtra  = 0;
  wc.hInstance   = ac3filter_instance;
  wc.hIcon       = 0;
  wc.hCursor     = 0;
  wc.hbrBackground = 0;
  wc.lpszMenuName  = 0;
  wc.lpszClassName = "AC3Filter control";

  RegisterClass(&wc);
  hwnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    wc.lpszClassName,
    0,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    0,
    0,
    ac3filter_instance,
    0);

  if (hwnd)
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this); 

  /////////////////////////////////////////////////////////
  // Resources

  hicon      = LoadIcon(ac3filter_instance, MAKEINTRESOURCE(IDI_AC3FILTER));
  icon_stop  = LoadBitmap(ac3filter_instance, MAKEINTRESOURCE(IDB_STOP_ICON));
  icon_pause = LoadBitmap(ac3filter_instance, MAKEINTRESOURCE(IDB_PAUSE_ICON));
  icon_play  = LoadBitmap(ac3filter_instance, MAKEINTRESOURCE(IDB_PLAY_ICON));

  /////////////////////////////////////////////////////////
  // NOTIFYICONDATA used to control tray icon

  memset(&nid, 0, sizeof(nid));
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hwnd;
  nid.uID = 1;
  nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  nid.uCallbackMessage = WM_TRAY_ICON;
  nid.hIcon = hicon;
  lstrcpy(nid.szTip, _("AC3Filter configuration"));
}

HMENU
AC3FilterTrayImpl::create_menu() const
{
  HMENU menu = CreatePopupMenu();
  if (!menu) return 0;

  /////////////////////////////////////////////////////////
  // Fill presets

  HKEY key;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PRESET, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    char buf[256];
    DWORD len = sizeof(buf);

    int i = 0;
    int npresets = CMD_LAST_PRESET - CMD_FIRST_PRESET;
    while (i < npresets && RegEnumKeyEx(key, i++, (LPTSTR)buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)
    {
      if (strcmp(buf, "Default"))
        AppendMenu(menu, MF_STRING, CMD_FIRST_PRESET + i, (LPCTSTR)buf);
      len = sizeof(buf);
    }
    RegCloseKey(key);
    AppendMenu(menu, MF_SEPARATOR, 0, 0);
  }

  for (size_t i = 0; i < filters.size(); i++)
  {
    AppendMenu(menu, MF_STRING, 
      CMD_FIRST_TRACK + filters[i].id, 
      (LPCTSTR)filters[i].desc.c_str());

    switch (filters[i].state)
    {
      case state_pause:
        SetMenuItemBitmaps(menu, CMD_FIRST_TRACK + filters[i].id, MF_BITMAP | MF_BYCOMMAND, icon_pause, icon_pause);
        break;
      case state_play:
        SetMenuItemBitmaps(menu, CMD_FIRST_TRACK + filters[i].id, MF_BITMAP | MF_BYCOMMAND, icon_play, icon_play);
        break;
      default:
        SetMenuItemBitmaps(menu, CMD_FIRST_TRACK + filters[i].id, MF_BITMAP | MF_BYCOMMAND, icon_stop, icon_stop);
        break;
    }
  }

  return menu;
}

void
AC3FilterTrayImpl::show()
{
  if (visible)
    return;

  if (!hwnd)
    init();

  Shell_NotifyIcon(NIM_ADD, &nid);
  visible = true;
}

void
AC3FilterTrayImpl::hide()
{
  if (!visible)
    return;

  if (dialog)
    dialog->stop();

  Shell_NotifyIcon(NIM_DELETE, &nid);
  visible = false;
}

void
AC3FilterTrayImpl::register_filter(IAC3Filter *filter)
{
  if (id >= CMD_LAST_TRACK - CMD_FIRST_TRACK)
    // no more ids
    return;

  // Check for duplicates
  for (size_t i = 0; i < filters.size(); i++)
    if (filters[i].filter == filter)
      return;

  FilterData data(filter);
  data.id = id++;
  data.desc = string(_("Adjust track")) + string(" ") + boost::lexical_cast<string>(data.id);
  Speakers format = get_input_format(filter);
  if (!format.is_unknown())
    data.desc += string(" (") + format.print() + string(")");
  filters.push_back(data);

  if (!hwnd)
    init();

  if (!visible)
  {
    bool tray = false;
    RegistryKey reg(REG_KEY);
    reg.get_bool("tray", tray);
    if (tray)
      show();
  }
}

void
AC3FilterTrayImpl::unregister_filter(IAC3Filter *filter)
{
  if (filter == dialog->get_filter())
    dialog->stop();

  for (size_t i = 0; i < filters.size(); i++)
    if (filters[i].filter == filter)
    {
      filters.erase(filters.begin() + i);
      break;
    }

  // Hide tray icon after removing the last filter
  if (visible && filters.size() == 0)
    hide();
}

void
AC3FilterTrayImpl::play(IAC3Filter *filter)
{
  for (size_t i = 0; i < filters.size(); i++)
    if (filters[i].filter == filter)
    {
      filters[i].state = state_play;
      break;
    }
}

void
AC3FilterTrayImpl::pause(IAC3Filter *filter)
{
  for (size_t i = 0; i < filters.size(); i++)
    if (filters[i].filter == filter)
    {
      filters[i].state = state_pause;
      break;
    }
}

void
AC3FilterTrayImpl::stop(IAC3Filter *filter)
{
  for (size_t i = 0; i < filters.size(); i++)
    if (filters[i].filter == filter)
    {
      filters[i].state = state_stop;
      break;
    }
}

void
AC3FilterTrayImpl::preset(const char *preset)
{
  char buf[256];
  sprintf(buf, REG_KEY_PRESET"\\%s", preset);
  RegistryKey reg(buf);

  for (size_t i = 0; i < filters.size(); i++)
  {
    if (!filters[i].filter) return;

    IDecoder *dec = 0;
    if FAILED(filters[i].filter->QueryInterface(IID_IDecoder, (void **)&dec))
      continue;

    dec->load_params(&reg, AC3FILTER_ALL);
    dec->Release();
  }
}

void
AC3FilterTrayImpl::preset(int hash)
{
  HKEY key;
  const int preset_size = 256;
  char preset_buf[preset_size];
  uint32_t preset_hash = 0;

  if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_PRESET, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    int i = 0;
    DWORD len = preset_size;
    while (RegEnumKeyEx(key, i++, (LPTSTR)preset_buf, &len, 0, 0, 0, 0) == ERROR_SUCCESS)
    {
      preset_hash = crc32.calc(0, (uint8_t *)preset_buf, len);
      len = preset_size;
      if (preset_hash == hash)
        break;
    }
    RegCloseKey(key);
  }

  if (preset_hash && preset_hash == hash)
    preset(preset_buf);
}

void
AC3FilterTrayImpl::popup_menu()
{
  POINT mouse;

  if (hmenu) DestroyMenu(hmenu);
  hmenu = create_menu();

  GetCursorPos(&mouse);
  SetForegroundWindow(hwnd);
  TrackPopupMenu(hmenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, mouse.x, mouse.y, 0, hwnd, 0);
  PostMessage(hwnd, WM_NULL, 0, 0);
}


LRESULT CALLBACK 
AC3FilterTrayImpl::TrayProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  AC3FilterTrayImpl *self = (AC3FilterTrayImpl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  int cmd_id;

  switch (Msg)
  {
    ///////////////////////////////////
    // Tray icon messages

    case WM_TRAY_ICON:
      switch (lParam)
      {
        case WM_LBUTTONDOWN:
        {
          // If only one filter is working, show the config dialog
          // for this filter. Show popup menu otherwise.
          int playing = 0;
          IAC3Filter *filter = 0;

          if (self->filters.size() == 1)
          {
            // Single track:
            // Always show config dialog for this track even when it is inactive
            playing = 1;
            filter = self->filters[0].filter;
          }
          else
          {
            // Multiple tracks:
            // Count active tracks.
            for (size_t i = 0; i < self->filters.size(); i++)
              if (self->filters[i].state == state_play)
              {
                playing++;
                filter = self->filters[i].filter;
              }
          }

          if (playing == 1)
            self->dialog->start(0, filter);
          else
            self->popup_menu();

          return 0;
        }

        case WM_RBUTTONDOWN:
          self->popup_menu();
          return 0;
      }
      break;

    case WM_PRESET:
      self->preset(int(lParam));
      return 0;

    ///////////////////////////////////
    // Menu

    case WM_COMMAND:
      cmd_id = LOWORD(wParam);
      if (cmd_id >= CMD_FIRST_PRESET && cmd_id < CMD_LAST_PRESET)
      {
        char buf[256];
        if (!GetMenuString(self->hmenu, cmd_id, (LPTSTR)buf, sizeof(buf), MF_BYCOMMAND))
          return 0;

        self->preset(buf);
        return 0;
      }

      if (cmd_id >= CMD_FIRST_TRACK && cmd_id < CMD_LAST_TRACK)
      {
        for (size_t i = 0; i < self->filters.size(); i++)
          if (self->filters[i].id == cmd_id - CMD_FIRST_TRACK && self->filters[i].filter && self->dialog)
          {
            if (self->dialog->is_visible())
              self->dialog->stop();
            else
              self->dialog->start(0, self->filters[i].filter);
          }
        return 0;
      }
      break;
  }

  return DefWindowProc(hwnd, Msg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////

AC3FilterTray::AC3FilterTray()
{ pimpl = new AC3FilterTrayImpl(); }

AC3FilterTray::~AC3FilterTray()
{ delete pimpl; }

void AC3FilterTray::show()
{ pimpl->show(); }

void AC3FilterTray::hide()
{ pimpl->hide(); }

void AC3FilterTray::register_filter(IAC3Filter *filter)
{ pimpl->register_filter(filter); }

void AC3FilterTray::unregister_filter(IAC3Filter *filter)
{ pimpl->unregister_filter(filter); }

void AC3FilterTray::play(IAC3Filter *filter)
{ pimpl->play(filter); }

void AC3FilterTray::pause(IAC3Filter *filter)
{ pimpl->pause(filter); }

void AC3FilterTray::stop(IAC3Filter *filter)
{ pimpl->stop(filter); }

void AC3FilterTray::preset(const char *preset)
{ pimpl->preset(preset); }

///////////////////////////////////////////////////////////////////////////////

AC3FilterTray ac3filter_tray;
