#include <windows.h>
#include <stdio.h>
#include <ocidl.h>
#include <olectl.h>
#include "registry.h"
#include "..\..\filter\guids.h"
#include "cmd_line.h"

const char *usage_title = "Usage";
const char *usage_text = 
"AC3Filter configuration utility:\n"
"\n"
"Usage:\n"
"> ac3config [/?] [/preset PRESET_NAME] [/load]\n"
"\n"
"Where:\n"
"  /?      - show this help\n"
"  /preset - load the specified preset\n"
"  /load   - just load the preset, do not show the dialog\n"
"\n"
"Example:\n"
"> ac3config /preset \"my preset\" /load\n"
"Load 'my preset' preset and exit (do not show dialog).\n";

const char *offline_title = "WARNING!";
const char *offline_text =
"This utility is for OFFLINE filter configuration only!"
"This means that you will not see any activity of "
"currently playing applications and configuration "
"settings will not affect them. You need to restart "
"your player for changes take in effect!\n\n" 
"Press cancel to skip this dialog in future";

const char *err_ac3filter_title = "ERROR!";
const char *err_ac3filter_text = "Cannot create AC3Filter instance (AC3Filter is not installed?)";

const char *err_dialog_title = "ERROR!";
const char *err_dialog_text = "Cannot create dialog.";

void usage()
{ MessageBox(0, usage_text, usage_title, MB_OK); }

INT APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, 
                      INT nCmdShow )
{
  IDecoder *dec;
  ISpecifyPropertyPages *spp;
  CAUUID cauuid;
  HRESULT hr;

  CmdLine cmd_line(pCmdLine);
  LPCSTR preset = 0;
  bool load_preset = false;


  /////////////////////////////////////////////////////////
  // Parse command line

  for (int i = 0; i < cmd_line.count(); i++)
  {
    if (strcmp(cmd_line[i], "/preset") == 0)
    {
      if (cmd_line.count() <= i + 1)
      {
        usage();
        return -1;
      }

      i++;
      preset = cmd_line[i];
      continue;
    }

    if (strcmp(cmd_line[i], "/load") == 0)
    {
      load_preset = true;
      continue;
    }

    usage();
    return -1;
  }

  /////////////////////////////////////////////////////////
  // Create filter

  CoInitialize(0);

  if FAILED(CoCreateInstance(CLSID_AC3Filter, NULL, CLSCTX_INPROC_SERVER, IID_IDecoder, (LPVOID *)&dec))
  {
    MessageBox(0, err_ac3filter_text, err_ac3filter_title, MB_ICONSTOP | MB_OK);
    CoUninitialize();
    return 1;
  }

  /////////////////////////////////////////////////////////
  // Load preset

  if (preset)
  {
    char buf[256];
    sprintf(buf, REG_KEY_PRESET"\\%s", preset);
    RegistryKey reg(buf);

    dec->load_params(&reg, AC3FILTER_ALL);
    if (load_preset)
      dec->save_params(0, AC3FILTER_ALL);
  }

  if (load_preset)
  {
    dec->Release();
    CoUninitialize();
    return 0;
  }

  /////////////////////////////////////////////////////////
  // Offline warning

  bool warning = true;
  RegistryKey reg(REG_KEY);
  reg.get_bool("offline_warning", warning);

  if (warning)
    if (MessageBox(0, offline_text, offline_title, MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
      reg.set_bool("offline_warning", false);

  /////////////////////////////////////////////////////////
  // Show the dialog

  if FAILED(dec->QueryInterface(IID_ISpecifyPropertyPages, (LPVOID *)&spp))
  {
    MessageBox(0, err_dialog_text, err_dialog_title, MB_OK | MB_ICONSTOP);

    dec->Release();
    CoUninitialize();
    return -1;
  }
  spp->GetPages(&cauuid);

  hr = OleCreatePropertyFrame(0, 30, 30, NULL, 1, (IUnknown **)&spp, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);

  CoTaskMemFree(cauuid.pElems);
  spp->Release();
  dec->Release();
  CoUninitialize();
  return 0;
};
