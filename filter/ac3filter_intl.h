/*
  Internationalization support for AC3Filter

  Load and use ac3filter_intl.dll dynamically. This allows to redistribute the
  filter without translation dll.
*/

#ifndef AC3FILTER_INTL_DLL
#define AC3FILTER_INTL_DLL



///////////////////////////////////////////////////////////////////////////////
// Use the same interface as DLL uses.
// If DLL interface changes we can notice and accomodate the changes immediately.

#include "../intl/ac3filter_intl.h"



///////////////////////////////////////////////////////////////////////////////
// Load/unload the DLL
// Reference counting is used to actually unload the DLL.

bool init_nls();
bool is_nls_available();
void free_nls();



///////////////////////////////////////////////////////////////////////////////
// gettext standard definitions

#ifndef DISABLE_NLS
#  define _(s) gettext_wrapper(s)
#  define gettext_noop(s) s
#  define N_(s) gettext_noop(s)

// gettext_id(const char *id, const char *str);
// Translate the string that have an id, return str if no translation found
// Used to translate dialog controls by an id, instead of translating by the
// text stored. This allows to translate different controls with the same text
// differently.

inline const char *gettext_id(const char *id, const char *str)
{
  const char *translated = gettext_wrapper(id);
  return translated == id? str: translated;
}

// gettext_meta()
// Returns current translation info

inline const char *gettext_meta()
{
  return gettext_wrapper("");
}

#else
#  define gettext(s) s
#  define _(s) gettext(s)
#  define gettext_noop(s) s
#  define N_(s) gettext_noop(s)
#  define gettext_id(id, s) s
#  define gettext_meta() ""
#endif


#endif
