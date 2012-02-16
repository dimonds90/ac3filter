#ifndef AC3FILTER_VER

// Major version changess after major changes in code
#define AC3FILTER_VER_MAJOR 2

// Minor version changes after:
// * Adding new features
// * COM interface change
// * Significant user unterface change
#define AC3FILTER_VER_MINOR 0

// Release changes after:
// * Bug fix
// * Minor behavior change
// * Translation update
// * Slight user interface change
// Release increases each public release, and does not fall to zero with minor
// version change. So it can be used to compare versions of major branch:
// 2.x.10 < 2.x.15 < 2.x.136
#define AC3FILTER_VER_RELEASE 0

// State signs:
// a - alfa stage, lots of changes without public testing
// b - beta stage, bug fixes after public testing
// none - stable version
#define AC3FILTER_VER_STATE "a"

// Text representation of the version
#define AC3FILTER_VER_QUOTE(x) #x
#define AC3FILTER_VER_(major, minor, release, state) \
  AC3FILTER_VER_QUOTE(major) "." \
  AC3FILTER_VER_QUOTE(minor) "." \
  AC3FILTER_VER_QUOTE(release) \
  AC3FILTER_VER_STATE
#define AC3FILTER_VER AC3FILTER_VER_(AC3FILTER_VER_MAJOR, AC3FILTER_VER_MINOR, AC3FILTER_VER_RELEASE, AC3FILTER_VER_STATE)

#endif
