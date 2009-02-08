#include <windows.h>
#include <new>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Override memory allocation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void *operator new(size_t size)
{
  void *ptr = LocalAlloc(LPTR, size);

#if _MSC_VER >= 1400
  // Looks like Visual C++ CRT relies on std::bad_alloc exception,
  // because it crashes without it.
  //
  // To reproduce the crach we need:
  // * Visual Studio 2008
  // * Multithreaded static release CRT (debug CRT works well)
  // * Compile and install the ACM
  // * Run Windows media player and choose Help->About->Technical support information
  if (ptr == 0) throw std::bad_alloc();
#endif

  return ptr;
}
extern void operator delete(void *block)
{
  LocalFree(block);
}
