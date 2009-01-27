#include <windows.h>
#include "defs.h"
#include "ac3filter_intl.h"

static HMODULE h_intl = 0;
static size_t ref_count = 0;
static bool dll_exists = true;

// function types

typedef const char *(*str2str_func)(const char *);
typedef const char *(*int2str_func)(int);
typedef int (*str2int_func)(const char *);

typedef void (*set_lang_func)(const char *, const char *, const char *);
typedef const char *(*get_lang_func)();

// function names, indexes and pointers

#define GETTEXT_WRAPPER  0
#define LANG_INDEX       1
#define LANG_BY_CODE     2
#define LANG_BY_INDEX    3
#define COUNTRY_INDEX    4
#define COUNTRY_BY_CODE  5
#define COUNTRY_BY_INDEX 6
#define SET_LANG         7
#define GET_LANG         8

static const char *func_names[] = {
  "gettext_wrapper",
  "lang_index",
  "lang_by_code",
  "lang_by_index",
  "country_index",
  "country_by_code",
  "country_by_index",
  "set_lang",
  "get_lang"
};
static const int nfuncs = array_size(func_names);
static FARPROC func_ptrs[nfuncs];

///////////////////////////////////////////////////////////////////////////////

bool init_nls()
{
  if (h_intl)
  {
    ref_count++;
    return true;
  }

  if (dll_exists)
  {
    h_intl = LoadLibrary("ac3filter_intl.dll");
    DWORD err = GetLastError();
    if (h_intl == 0)
    {
      dll_exists = false;
      return false;
    }
  }

  for (int i = 0; i < nfuncs; i++)
  {
    func_ptrs[i] = GetProcAddress(h_intl, func_names[i]);
    if (func_ptrs[i] == 0)
    {
      FreeLibrary(h_intl);
      h_intl = 0;
      dll_exists = false;
      return false;
    }
  }

  ref_count++;
  return true;
}

void free_nls()
{
  if (ref_count > 0)
  {
    ref_count--;
    if (ref_count == 0)
    {
      FreeLibrary(h_intl);
      h_intl = 0;
    }
  }
}

bool is_nls_available()
{
  return h_intl != 0;
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper functions

const char *gettext_wrapper(const char *str)
{ return h_intl? ((str2str_func)func_ptrs[GETTEXT_WRAPPER])(str): str; }

int lang_index(const char *code)
{ return h_intl? ((str2int_func)func_ptrs[LANG_INDEX])(code): -1; }

const char *lang_by_code(const char *code)
{ return h_intl? ((str2str_func)func_ptrs[LANG_BY_CODE])(code): 0; }

const char *lang_by_index(int index)
{ return h_intl? ((int2str_func)func_ptrs[LANG_BY_INDEX])(index): 0; }

int country_index(const char *code)
{ return h_intl? ((str2int_func)func_ptrs[COUNTRY_INDEX])(code): -1; }

const char *country_by_code(const char *code)
{ return h_intl? ((str2str_func)func_ptrs[COUNTRY_BY_CODE])(code): 0; }

const char *country_by_index(int index)
{ return h_intl? ((int2str_func)func_ptrs[COUNTRY_BY_INDEX])(index): 0; }

void set_lang(const char *code, const char *package, const char *path)
{ if (h_intl) ((set_lang_func)func_ptrs[SET_LANG])(code, package, path); }

const char *get_lang()
{ return h_intl? ((get_lang_func)func_ptrs[GET_LANG])(): 0; }
