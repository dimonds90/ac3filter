#include <stdio.h>
#include <windows.h>
#include "defs.h"
#include "auto_buf.h"
#include "ac3filter_intl.h"

static HMODULE h_dll = 0;
static size_t ref_count = 0;
static bool dll_exists = true;

// Both 32bit and 64bit AC3Filter versions may reside in
// the same folder, therefore we have to have different
// DLL names for both paltforms.

#ifdef _M_X64
static const char *dll_name = "ac3filter64_intl.dll";
#else
static const char *dll_name = "ac3filter_intl.dll";
#endif

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
// NLS control

const char *nls_dll_name()
{
  return dll_name;
}

bool init_nls(const char *path)
{
  if (h_dll)
  {
    ref_count++;
    return true;
  }

  if (dll_exists)
  {
    if (path)
    {
      AutoBuf<char> full_dll_name(strlen(path) + strlen(dll_name) + 2);
      if (full_dll_name.size() != 0)
      {
        sprintf(full_dll_name, "%s\\%s", path, dll_name);
        h_dll = LoadLibrary(full_dll_name);
      }
    }

    if (h_dll == 0)
      h_dll = LoadLibrary(dll_name);

    if (h_dll == 0)
    {
      dll_exists = false;
      return false;
    }
  }

  for (int i = 0; i < nfuncs; i++)
  {
    func_ptrs[i] = GetProcAddress(h_dll, func_names[i]);
    if (func_ptrs[i] == 0)
    {
      FreeLibrary(h_dll);
      h_dll = 0;
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
      FreeLibrary(h_dll);
      h_dll = 0;
    }
  }
}

bool is_nls_available()
{
  return h_dll != 0;
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper functions

const char *gettext_wrapper(const char *str)
{ return h_dll? ((str2str_func)func_ptrs[GETTEXT_WRAPPER])(str): str; }

int lang_index(const char *code)
{ return h_dll? ((str2int_func)func_ptrs[LANG_INDEX])(code): -1; }

const char *lang_by_code(const char *code)
{ return h_dll? ((str2str_func)func_ptrs[LANG_BY_CODE])(code): 0; }

const char *lang_by_index(int index)
{ return h_dll? ((int2str_func)func_ptrs[LANG_BY_INDEX])(index): 0; }

int country_index(const char *code)
{ return h_dll? ((str2int_func)func_ptrs[COUNTRY_INDEX])(code): -1; }

const char *country_by_code(const char *code)
{ return h_dll? ((str2str_func)func_ptrs[COUNTRY_BY_CODE])(code): 0; }

const char *country_by_index(int index)
{ return h_dll? ((int2str_func)func_ptrs[COUNTRY_BY_INDEX])(index): 0; }

void set_lang(const char *code, const char *package, const char *path)
{ if (h_dll) ((set_lang_func)func_ptrs[SET_LANG])(code, package, path); }

const char *get_lang()
{ return h_dll? ((get_lang_func)func_ptrs[GET_LANG])(): 0; }
