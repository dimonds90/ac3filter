#include <windows.h>
#include <ocidl.h>
#include <olectl.h>
#include "..\registry.h"
#include "..\guids.h"

INT APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, 
                      INT nCmdShow )
{
  ISpecifyPropertyPages *spp;
  CAUUID cauuid;
  HRESULT hr;

  CoInitialize(0);
  if FAILED(CoCreateInstance(CLSID_AC3Filter, NULL, CLSCTX_INPROC_SERVER, IID_ISpecifyPropertyPages, (LPVOID *)&spp))
  {
    MessageBox(0, "Cannot create AC3Filter instance (AC3Filter is not installed?)", 
      "ERROR!", MB_OK | MB_ICONSTOP);
    return 1;
  }

  spp->GetPages(&cauuid);

  bool warning = true;
  RegistryKey reg(REG_KEY);
  reg.get_bool("offline_warning", warning);

  if (warning)
    if (MessageBox(0, 
      "This utility is for OFFLINE filter configuration only!"
      "This means that you will not see any activity of "
      "currently playing applications and configuration "
      "settings will not affect them. You need to restart "
      "your player for changes take in effect!\n\n" 
      "Press cancel to skip this dialog in future",
      "WARNING!", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
      reg.set_bool("offline_warning", false);

  hr = OleCreatePropertyFrame(0, 30, 30, NULL, 1, (IUnknown **)&spp, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
  CoTaskMemFree(cauuid.pElems);
  spp->Release();
  CoUninitialize();
  return 0;
};