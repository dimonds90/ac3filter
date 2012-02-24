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

#include <string>
#include <vector>
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "log.h"
#include "logging.h"

using std::string;

///////////////////////////////////////////////////////////////////////////////

AC3FilterEventLog event_log;
AC3FilterTraceLog trace_log;

///////////////////////////////////////////////////////////////////////////////

static string log_filename()
{
  TCHAR file_name[MAX_PATH];
  TCHAR temp_name[MAX_PATH];
  GetTempPath(MAX_PATH, file_name);
  GetTempFileName(file_name, "ac3filter_", 0, temp_name);
  PathAppend(file_name, temp_name);
  return string(file_name);
}

///////////////////////////////////////////////////////////////////////////////

AC3FilterEventLog::AC3FilterEventLog()
{}

AC3FilterEventLog::~AC3FilterEventLog()
{
  LogFile::close();
#ifndef _DEBUG
  remove(filename.c_str());
#endif
}

void
AC3FilterEventLog::init(LogDispatcher *source)
{
  filename = log_filename();
  LogFile::open(filename.c_str());
  LogFile::subscribe(source);
}

void
AC3FilterEventLog::receive(const LogEntry &entry)
{
  if (entry.level <= log_event)
    LogFile::receive(entry);
}

///////////////////////////////////////////////////////////////////////////////


class AC3FilterTraceLog::EntryList : public std::vector<LogEntry>
{};

AC3FilterTraceLog::AC3FilterTraceLog():
entries(new AC3FilterTraceLog::EntryList)
{
  pos = 0;
  max_events = 0;
}

AC3FilterTraceLog::~AC3FilterTraceLog()
{
#ifdef _DEBUG
  flush();
#endif
  LogFile::close();
#ifndef _DEBUG
  remove(filename.c_str());
#endif
}

void
AC3FilterTraceLog::init(LogDispatcher *source, size_t max_events_)
{
  pos = 0;
  max_events = max_events_;
  entries->resize(0);
  filename = log_filename();
  LogFile::subscribe(source);
}

void
AC3FilterTraceLog::receive(const LogEntry &entry)
{
  if (entries->size() < max_events)
    entries->push_back(entry);
  else if (max_events != 0)
  {
    (*entries)[pos++] = entry;
    if (pos >= entries->size())
      pos = 0;
  }
}

void
AC3FilterTraceLog::flush()
{
  size_t i;
  if (LogFile::open(filename.c_str()))
  {
    for (i = pos; i < entries->size(); i++)
      LogFile::receive((*entries)[i]);
    for (i = 0; i < pos; i++)
      LogFile::receive((*entries)[i]);
    LogFile::close();
  }
}
