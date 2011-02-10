#include <streams.h>
#include "logging.h"
#include "vtime.h"

void log_input_chunk(Chunk &chunk)
{
  static vtime_t time_start = local_time();
  if (_chunk->sync)
  {
    vtime_t time = local_time() - time_start;
    REFERENCE_TIME clock = 0;
    vtime_t latency = 0;
    if (m_pClock)
      if SUCCEEDED(m_pClock->GetTime(&clock))
      {
        clock -= m_tStart;
        latency =  _chunk->time - vtime_t(clock) / 10000000;
      }
    DbgLog((LOG_TRACE, 3, "-> time: %ims\tclock: %ims\ttimestamp: %ims\tlatency: %ims", int(time * 1000), int(clock / 10000), int(_chunk->time * 1000), int(latency * 1000)));
  }
}

void log_output_chunk(Chunk &chunk)
{
  if (chunk.sync)
  {
    vtime_t time = local_time() - time_start;
    REFERENCE_TIME clock = 0;
    vtime_t latency = 0;
    if (m_pClock)
      if SUCCEEDED(m_pClock->GetTime(&clock))
      {
        clock -= m_tStart;
        latency = chunk.time - vtime_t(clock) / 10000000;
      }
    DbgLog((LOG_TRACE, 3, "<- time: %ims\tclock: %ims\ttimestamp: %ims\tlatency: %ims", int(time * 1000), int(clock / 10000), int(chunk.time * 1000), int(latency * 1000)));
  }
}
