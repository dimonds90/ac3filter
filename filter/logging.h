#ifndef LOGGING_H
#define LOGGING_H

#include "chunk.h"

void log_input_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock);
void log_output_chunk(const Chunk &chunk, CRefTime start_time, IReferenceClock *clock);

#endif
