#include <windows.h>
#include <tchar.h>
#include <DbgHelp.h>
#include "../BugTrap/BugTrap.h"
#include "guids.h"
#include "logging.h"

struct BugTrap
{
  BugTrap()
  {
    TCHAR file_name[MAX_PATH];
    AC3FilterTrace::GetLogFileName(file_name, MAX_PATH);

    BT_InstallSehFilter();
    BT_SetAppName(_T(APP_NAME));
    BT_SetSupportEMail(_T(SUPPORT_EMAIL));
    BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
    BT_SetDumpType(MiniDumpNormal);
    BT_SetSupportServer(_T(BUG_TRAP_URL), 80);
    BT_SetSupportURL(_T(WEB_SITE_URL));
    BT_AddRegFile(_T("Settings.reg"), _T("HKEY_CURRENT_USER\\"REG_KEY));
    BT_AddLogFile(file_name);
  }

  ~BugTrap()
  {
    // On DLL unload we should uninstall our filter
    BT_UninstallSehFilter();
  }
};

static BugTrap bug_trap;
