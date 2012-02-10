#include <windows.h>
#include <tchar.h>
#include "../BugTrap/BugTrap.h"

struct BugTrap
{
  BugTrap()
  {
    BT_InstallSehFilter();
    BT_SetAppName(_T("AC3Filter"));
    BT_SetSupportEMail(_T("support@ac3filter.net"));
    BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
    BT_SetSupportServer(_T("http://ac3filter.net/bugtrap.php"), 80);
    BT_SetSupportURL(_T("http://ac3filter.net"));
  }

  ~BugTrap()
  {
    // On DLL unload we should uninstall our filter
    BT_UninstallSehFilter();
  }
};

static BugTrap bug_trap;
