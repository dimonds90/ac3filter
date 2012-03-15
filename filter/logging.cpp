/*
  Good log should be:
  * small enough for fast reporting
  * very detailed to help with debugging

  AC3Filter can make tens and hundreds megabytes trace logs. Of course, it is
  unacceptable for end user. But low-detailed logs may not help at all.\
  
  As a tradeoff TWO log files are produced:
  * low-detailed (log_event and lower).
  * high-detailed (not filtered), but limited in size.
*/

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <DbgHelp.h>
#include "../BugTrap/BugTrap.h"
#include "ac3filter_ver.h"
#include "registry.h"
#include "logging.h"
#include "guids.h"

static const size_t log_size = 1000;

LogMem event_log(log_size);
LogMem trace_log(log_size);

static std::string event_log_file_name()
{
  TCHAR file_name[MAX_PATH];
  GetTempPath(MAX_PATH, file_name);
  PathAppend(file_name, "ac3filter_event.log");
  return std::string(file_name);
}

static std::string trace_log_file_name()
{
  TCHAR file_name[MAX_PATH];
  GetTempPath(MAX_PATH, file_name);
  PathAppend(file_name, "ac3filter_trace.log");
  return std::string(file_name);
}

static void flush_log(LogMem &log, const string &file_name)
{
  static const string endl = "\n";
  AutoFile f(file_name.c_str(), "w");
  for (size_t i = 0; i < log.size(); i++)
  {
    string s = log[i].print() + endl;
    f.write(s.c_str(), s.size());
  }
}

static void CALLBACK pre_error(INT_PTR param)
{
  flush_log(event_log, event_log_file_name());
  flush_log(trace_log, trace_log_file_name());
}

void init_logging()
{
  event_log.subscribe(&valib_log_dispatcher);
  trace_log.subscribe(&valib_log_dispatcher);
  event_log.set_max_log_level(log_event);
  trace_log.set_max_log_level(log_all);

  // Init BugTrap
  BT_SetAppName(_T(APP_NAME));
  BT_SetAppVersion(_T(AC3FILTER_VER));
  BT_SetSupportEMail(_T(SUPPORT_EMAIL));
  BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
  BT_SetDumpType(MiniDumpNormal);
  BT_SetSupportServer(_T(BUG_TRAP_URL), 80);
  BT_SetSupportURL(_T(WEB_SITE_URL));
  BT_AddRegFile(_T("Settings.reg"), _T("HKEY_CURRENT_USER\\"REG_KEY));
  BT_AddLogFile(event_log_file_name().c_str());
  BT_AddLogFile(trace_log_file_name().c_str());
  BT_SetPreErrHandler(pre_error, 0);
  BT_SetModule(ac3filter_instance);

  BT_InstallSehFilter();
}

void uninit_logging()
{
  BT_UninstallSehFilter();

  bool save_logs = false;
  RegistryKey reg(REG_KEY);
  reg.get_bool("save_logs", save_logs);
  if (save_logs)
  {
    flush_log(event_log, event_log_file_name());
    flush_log(trace_log, trace_log_file_name());
  }
}
