#include <shlwapi.h>
#include <stdio.h>
#include <streams.h>
#include <tchar.h>
#include "logging.h"
#include "vtime.h"

AC3FilterTrace trace;

AC3FilterTrace::AC3FilterTrace()
{
  TCHAR full_name[MAX_PATH];
  TCHAR file_name[MAX_PATH];
  GetLogFileName(file_name, MAX_PATH);
  GetTempPath(MAX_PATH, full_name);
  PathAppend(full_name, file_name);
  Open(full_name, BTLF_TEXT);
  SetLogFlags(BTLF_SHOWLOGLEVEL | BTLF_SHOWTIMESTAMP);
  SetLogLevel(BTLL_VERBOSE);
}

AC3FilterTrace::~AC3FilterTrace()
{
  // Running AC3Filter many times may create hundreds megabytes of logs.
  // Do not leave logs on disk.
  TCHAR file_name[MAX_PATH];
  _tcscpy_s(file_name, MAX_PATH, GetFileName());

  Close();
#ifndef _DEBUG
  DeleteFile(file_name);
#endif
}

void
AC3FilterTrace::GetLogFileName(LPTSTR lpFilename, DWORD nSize)
{
  // Several applications may load AC3Filter at the same time.
  // To prevent mess in logs append process id to the file name.
  _stprintf_s(lpFilename, nSize, _T("ac3filter_%i.log"), GetCurrentProcessId());
}

static vtime_t time_from_start()
{
  static vtime_t time_start = local_time();
  return local_time() - time_start;
}

void 
AC3FilterTrace::input_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock)
{
  if (chunk.sync)
  {
    vtime_t time = time_from_start();
    REFERENCE_TIME clock_time = 0;
    vtime_t latency = 0;
    if (clock)
      if SUCCEEDED(clock->GetTime(&clock_time))
      {
        clock_time -= start_time;
        latency =  chunk.time - vtime_t(clock_time) / 10000000;
      }

    trace.AppendF(BTLL_VERBOSE,
      "-> time: %ims\tclock: %ims\ttimestamp: %ims\tlatency: %ims\tsize: %i",
      int(time * 1000),
      int(clock_time / 10000),
      int(chunk.time * 1000),
      int(latency * 1000),
      chunk.size);
  }
}

void
AC3FilterTrace::output_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock)
{
  if (chunk.sync)
  {
    vtime_t time = time_from_start();
    REFERENCE_TIME clock_time = 0;
    vtime_t latency = 0;
    if (clock)
      if SUCCEEDED(clock->GetTime(&clock_time))
      {
        clock_time -= start_time;
        latency = chunk.time - vtime_t(clock_time) / 10000000;
      }
    trace.AppendF(BTLL_VERBOSE,
      "<- time: %ims\tclock: %ims\ttimestamp: %ims\tlatency: %ims\tsize: %i",
      int(time * 1000),
      int(clock_time / 10000),
      int(chunk.time * 1000),
      int(latency * 1000),
      chunk.size);
  }
}
