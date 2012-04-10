/*
  Internationalization support for AC3Filter

  Load and use language support dll dynamically. This allows to redistribute
  the filter without NLS support.
*/

#ifndef AC3FILTER_INTL_DLL
#define AC3FILTER_INTL_DLL



///////////////////////////////////////////////////////////////////////////////
// Use the same interface as DLL uses.
// If DLL interface changes we can notice and accomodate the changes immediately.

#include "../intl/ac3filter_intl.h"



///////////////////////////////////////////////////////////////////////////////
// Load/unload the language support DLL
// Reference counting is used to actually unload the DLL
// Path and alternate DLL name can be specified.

const char *nls_dll_name();
bool init_nls(const char *path = 0);
bool is_nls_available();
void free_nls();



///////////////////////////////////////////////////////////////////////////////
// gettext standard definitions

#ifndef DISABLE_NLS
#  define gettext(s) gettext_wrapper(s)
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
