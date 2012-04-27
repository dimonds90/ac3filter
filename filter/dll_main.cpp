#include <windows.h>
#include <tchar.h>
#include "guids.h"
#include "error_reporting.h"
#include "ac3filter_intl.h"

///////////////////////////////////////////////////////////////////////////////
// Truncate the last element of the path with the trailing slash
// "c:\dir\dir2\filename.ext" turns into
// "c:\dir\dir2"
// "c:\dir"
// etc..

size_t cut_last_name(char *name, size_t size)
{
  size_t pos = 0;
  while (pos < size && name[pos])
    pos++;

  // skip the trailing slash if it is present
  if (name[pos] == 0) pos--;
  if (name[pos] == '\\') pos--;

  while (pos > 0 && name[pos] != '\\')
    pos--;

  name[pos] = 0;
  return pos;
}

///////////////////////////////////////////////////////////////////////////////
// DllMain

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
  if (reason == DLL_PROCESS_ATTACH)
  {
    ac3filter_instance = hinst;

    // Init NLS
    // Search NLS DLL at the filter's folder
    size_t path_size = MAX_PATH;
    char path[MAX_PATH];

    path_size = GetModuleFileName(hinst, path, (DWORD)path_size);
    if (path_size)
      if (cut_last_name(path, path_size))
        init_nls(path);

    // Init logging
    init_logging();
  }
  if (reason == DLL_PROCESS_DETACH)
  {
    uninit_logging();
  }
  return DllEntryPoint(hinst, reason, reserved);
}
