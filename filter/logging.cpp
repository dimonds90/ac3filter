#include <streams.h>
#include "logging.h"
#include "vtime.h"

static vtime_t time_from_start()
{
  static vtime_t time_start = local_time();
  return local_time() - time_start;
}

void log_input_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock)
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
    DbgLog((LOG_TRACE, 3, "-> time: %ims\tclock: %ims\ttimestamp: %ims\tlatency: %ims", int(time * 1000), int(clock_time / 10000), int(chunk.time * 1000), int(latency * 1000)));
  }
}

void log_output_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock)
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
    DbgLog((LOG_TRACE, 3, "<- time: %ims\tclock: %ims\ttimestamp: %ims\tlatency: %ims", int(time * 1000), int(clock_time / 10000), int(chunk.time * 1000), int(latency * 1000)));
  }
}
