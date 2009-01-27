/*
  Internationalization support for AC3Filter
*/

#ifndef AC3FILTER_INTL
#define AC3FILTER_INTL

extern "C" {

///////////////////////////////////////////////////////////////////////////////
// gettext wrapper

const char *gettext_wrapper(const char *);

///////////////////////////////////////////////////////////////////////////////
// Language codes conversion
//
// lang_index() - find an index of iso 639 code; -1 if not found; 0 is English
// lang_name() - convert 2 or 3-character code to the language name
//
// country_index() - find an index of iso 3166 code; -1 if not found; 0 is English
// country_name() - convert 2 or 3-character code to the country name

int lang_index(const char *code);
const char *lang_by_code(const char *code);
const char *lang_by_index(int index);

int country_index(const char *code);
const char *country_by_code(const char *code);
const char *country_by_index(int index);

///////////////////////////////////////////////////////////////////////////////
// Language operations
//
// set_lang() - set global language to the language specified and init the
//   default package. You may omit package and set only language code. Code may
//   be either 2 or 3 character code. You may omit language code to set default
//   package only. set_lang("") cancels translation (now it just switches to
//   English)
//
//   set_lang(0, package, path) may be used to change the packade and path
//   without changing the language.
//
// get_lang() - return currently selected language. Returns zero if no language
//   selected.

#define LANG_LEN 256
void set_lang(const char *code, const char *package = 0, const char *path = 0);
const char *get_lang();

}

#endif
