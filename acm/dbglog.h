#ifndef AC3FILTER_ACM_DBGLOG_H
#define AC3FILTER_ACM_DBGLOG_H

#ifdef _DEBUG

#include "windows.h"

class DbgLog
{
protected:
  HANDLE f;

public:
  DbgLog();
  DbgLog(const char *logfile);
  ~DbgLog();

  void open(const char *filename);
  void close();
  bool is_open();

  void printf(const char *format, ...);
  void log(const char *format, ...);

  void printf(const char *format, va_list args);
  void log(const char *format, va_list args);
};

extern DbgLog dbg;

inline void dbglog(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  dbg.log(format, args); 
}

#else // _DEBUG

inline void dbglog(const char *format, ...) {};

#endif // _DEBUG

#endif
