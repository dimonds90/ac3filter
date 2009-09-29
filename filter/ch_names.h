#ifndef CH_NAMES_H
#define CH_NAMES_H

#include "defs.h"

extern const char *short_ch_names[CH_NAMES];
extern const char *long_ch_names[CH_NAMES];
inline bool validate_ch_name(int ch) { return ch >= 0 && ch < CH_NAMES; }

#endif
