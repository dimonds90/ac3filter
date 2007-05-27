/*
  Internationalization support for AC3Filter
*/

#ifndef AC3FILTER_INTL
#define AC3FILTER_INTL

///////////////////////////////////////////////////////////////////////////////
// gettext standard definitions

#ifdef ENABLE_NLS
#  include "intl\include\libintl.h"
#  define _(s) gettext(s)
#  define gettext_noop(s) s
#  define N_(s) gettext_noop(s)
#else
#  define gettext(s) s
#  define _(s) gettext(s)
#  define gettext_noop(s) s
#  define N_(s) gettext_noop(s)
#endif

///////////////////////////////////////////////////////////////////////////////
// Language codes conversion
//
// iso_langs - table of ISO languages
//   First element of this table (index 0) is known to be English
//
// find_iso6392() - find an index of iso6392 code; -1 if not found; 0 is English
// find_iso6391() - find an index of iso6391 code; -1 if not found; 0 is English
// find_iso_code() - find an index of iso6391/6392 code; -1 if not found; 0 is English
// lang_from_iso6392() - convert 3-character language code to the language name
// lang_from_iso6391() - convert 2-character language code to the language name
// lang_from_iso_code() - convert 2 or 3-character code to the language name

struct iso_lang_s
{ 
  const char *name;
  const char *iso6392;
  const char *iso6391;
};

extern const iso_lang_s iso_langs[];

int find_iso6392(const char *code);
int find_iso6391(const char *code);
int find_iso_code(const char *code);

const char *lang_from_iso6392(const char *code);
const char *lang_from_iso6391(const char *code);
const char *lang_from_iso_code(const char *code);

///////////////////////////////////////////////////////////////////////////////
// Language operations
//
// set_lang() - set global language to the language specified and init the
//   default package. You may omit package and set only language code. Code may
//   be either 2 or 3 character code. You may omit language code to set default
//   package only. set_lang("") cancels translation (now it just switches to
//   English)
//
// get_lang() - return currently selected language. Returns zero if no language
//   selected.

void set_lang(const char *code, const char *package = 0, const char *path = 0);
const char *get_lang();

#endif
