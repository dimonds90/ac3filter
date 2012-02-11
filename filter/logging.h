#ifndef LOGGING_H
#define LOGGING_H

#include <windows.h>
#include <streams.h>
#include "../bugtrap/bugtrap.h"
#include "../bugtrap/bttrace.h"
#include "chunk.h"

class AC3FilterTrace : public BTTrace
{
public:
  AC3FilterTrace();
  ~AC3FilterTrace();

  static void GetLogFileName(LPTSTR lpFilename, DWORD nSize);

  void input_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock);
  void output_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock);
};

extern AC3FilterTrace trace;

#endif
