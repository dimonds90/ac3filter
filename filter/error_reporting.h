#ifndef ERROR_REPORTING_H
#define ERROR_REPORTING_H

#include "log.h"
#include "spk.h"
#include "chunk.h"

// Log raw audio data
// It must be a separate logger for each audio stream.

class AudioLog
{
public:
  AudioLog();
  ~AudioLog();

  bool open(Speakers spk);
  void close();
  bool is_open() const;

  void log(const Chunk &chunk);
  void reset();

protected:
  class Impl;
  Impl *pimpl;
};

extern LogMem event_log;
extern LogMem trace_log;

void init_logging();
void uninit_logging();

#endif
