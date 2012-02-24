#ifndef LOGGING_H
#define LOGGING_H

#include "log.h"

class AC3FilterEventLog : public LogFile
{
public:
  AC3FilterEventLog();
  ~AC3FilterEventLog();

  void init(LogDispatcher *source);
  std::string get_filename() const
  { return filename; }

  virtual void receive(const LogEntry &entry);

protected:
  std::string filename;
};

class AC3FilterTraceLog : public LogFile
{
public:
  AC3FilterTraceLog();
  ~AC3FilterTraceLog();

  void init(LogDispatcher *source, size_t max_events = 1000);
  std::string get_filename() const
  { return filename; }

  virtual void receive(const LogEntry &entry);
  void flush();

protected:
  std::string filename;
  size_t max_events;
  size_t pos;

  class EntryList;
  EntryList *entries;
};

extern AC3FilterEventLog event_log;
extern AC3FilterTraceLog trace_log;

#endif
