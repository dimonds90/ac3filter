// Windows compatibility fix
// New windows headers use SetWindowsLongPtr funciton instead of SetWindowsLong
// (and some other functions). It is the new portable way to build 32 and 64 bit
// windows applications from the same sources. But old headers do not know this
// functions. To be able to build the filter using old headers (using MSVC6 for
// instance), we need this fix.


#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef GWLP_WNDPROC
#define GWLP_USERDATA GWL_USERDATA
#define GWLP_WNDPROC  GWL_WNDPROC
#endif
