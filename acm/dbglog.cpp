#ifdef _DEBUG
#include <stdio.h>
#include "DbgLog.h"

extern DbgLog dbg("c:\\ac3filter_acm.log");

DbgLog::DbgLog(): 
f(0) 
{}

DbgLog::DbgLog(const char *logfile): 
f(0) 
{ 
  open(logfile); 
}

DbgLog::~DbgLog() 
{ 
  close(); 
};

void
DbgLog::open(const char *filename)
{
  close();
  f = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
  if (f == INVALID_HANDLE_VALUE)
    f = 0;
  else
    SetFilePointer(f, 0, 0, FILE_END);
}

void
DbgLog::close()
{
  if (f) 
  {
    CloseHandle(f);
    f = 0;
  }
}

void 
DbgLog::printf(const char *format, va_list args)
{
  if (f)
  {
    char str[1024];
    vsprintf(str, format, args);

    DWORD written;
    SetFilePointer(f, 0, 0, FILE_END);
    WriteFile(f, str, strlen(str), &written, 0);
  }
}

void
DbgLog::printf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printf(format, args);
}

void 
DbgLog::log(const char *format, va_list args)
{
  printf(format, args);
  printf("\n");
}
void 
DbgLog::log(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  log(format, args);
}

#endif // _DEBUG
