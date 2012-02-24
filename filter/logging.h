#ifndef LOGGING_H
#define LOGGING_H

#include "log.h"

extern LogMem event_log;
extern LogMem trace_log;

void init_logging();
void uninit_logging();

#endif
