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

// gettext_id(const char *id, const char *str);
// Translate the string that have an id, return str if no translation found
// Used to translate dialog controls by an id, instead of translating by the
// text stored. This allows to translate different controls with the same text
// differently.

inline const char *gettext_id(const char *id, const char *str)
{
  const char *translated = gettext(id);
  return translated == id? str: translated;
}

// gettext_meta()
// Returns current translation info

inline const char *gettext_meta()
{
  return gettext("");
}

#else
#  define gettext(s) s
#  define _(s) gettext(s)
#  define gettext_noop(s) s
#  define N_(s) gettext_noop(s)
#  define gettext_id(id, s) s
#endif

///////////////////////////////////////////////////////////////////////////////
// Language codes conversion
//
// iso_langs - table of languages (ISO 639)
// First element of this table (index 0) is known to be English
//
// iso_countries - table of countries (ISO 3166)
//
// find_iso_lang() - find an index of iso 639 code; -1 if not found; 0 is English
// lang_from_iso_code() - convert 2 or 3-character code to the language name
//
// find_iso_country() - find an index of iso 3166 code; -1 if not found; 0 is English
// country_from_code() - convert 2 or 3-character code to the country name
//
// All functions support ll_CC codes, where ll is a language code and CC is a
// country code. So find_iso_lang("pt_BR") will return the index of Portuguese
// language and country_from_code("pt_BR") will return "Brasil".
//
// Also, ll_CC string may be converted to llcc code with find_llcc() function.
// This code contains info about both language and country, and may be used instead
// of the string value. (Note, that country may not be specified.)

struct iso_lang_s
{ 
  const char *name;
  const char *iso6392;
  const char *iso6391;
};

struct iso_country_s
{
  const char *name;
  const char *alpha2;
  const char *alpha3;
  int code;
};

extern const iso_lang_s iso_langs[];
extern const iso_country_s iso_countries[];

#define MAX_LANG_LEN 64
#define MAX_COUNTRY_LEN 64

int find_llcc(const char *code);

int lang_index(const char *code);
int lang_index(int llcc);
const char *lang_name(const char *code);
const char *lang_name(int llcc);

int country_index(const char *code);
int country_index(int llcc);
const char *country_name(const char *code);
const char *country_name(int llcc);

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
