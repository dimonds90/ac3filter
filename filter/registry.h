/*
  RegistryKey

  Load/save values of different types from/to registry

*/


#ifndef REGISTRY_H
#define REGISTRY_H

#include <windows.h>
#include "defs.h"

///////////////////////////////////////////////////////////////////////////////
// RegistryKey
///////////////////////////////////////////////////////////////////////////////


class Config
{
public:
  virtual bool is_ok() = 0;

  virtual bool get_bool (LPCTSTR name, bool    &value) = 0;
  virtual bool get_int32(LPCTSTR name, int32_t &value) = 0;
  virtual bool get_float(LPCTSTR name, double  &value) = 0;
  virtual bool get_text (LPCTSTR name, char    *value, int size) = 0;

  virtual void set_bool (LPCTSTR name, bool    value) = 0;
  virtual void set_int32(LPCTSTR name, int32_t value) = 0;
  virtual void set_float(LPCTSTR name, double  value) = 0;
  virtual void set_text (LPCTSTR name, const char *value) = 0;

  inline bool get_float(LPCTSTR name, float   &value)
  { 
    double tmp = (double)value;
    bool result = get_float(name, tmp); 
    value = (float)tmp; 
    return result;
  }
  inline void set_float(LPCTSTR name, float   value) 
  { set_float(name, (double)value); }

};



class RegistryKey : public Config
{
protected:
  HKEY key;

public:
  RegistryKey();
  RegistryKey(LPCTSTR key, HKEY hive = HKEY_CURRENT_USER);
  ~RegistryKey();

  bool open_key(LPCTSTR key, HKEY hive = HKEY_CURRENT_USER);
  bool create_key(LPCTSTR key, HKEY hive = HKEY_CURRENT_USER);

  bool is_ok()    { return key != 0; }

  bool get_bool (LPCTSTR name, bool    &value);
  bool get_int32(LPCTSTR name, int32_t &value);
  bool get_float(LPCTSTR name, double  &value);
  bool get_text (LPCTSTR name, char    *value, int size);

  void set_bool (LPCTSTR name, bool    value);
  void set_int32(LPCTSTR name, int32_t value);
  void set_float(LPCTSTR name, double  value);
  void set_text (LPCTSTR name, const char *value);
};


class FileConfig : public Config
{
protected:
  bool ok;
  char filename[MAX_PATH];
  char section[256];

public:
  FileConfig(const char *_filename, const char *_section);
  ~FileConfig();

  bool open(const char *_filename, const char *_section);

  bool is_ok()    { return ok; }

  bool get_bool   (LPCTSTR name, bool    &value);
  bool get_int32  (LPCTSTR name, int32_t &value);
  bool get_double (LPCTSTR name, double  &value);
  bool get_text   (LPCTSTR name, char    *value, int size);

  void set_bool   (LPCTSTR name, bool    value);
  void set_int32  (LPCTSTR name, int32_t value);
  void set_double (LPCTSTR name, double  value);
  void set_text   (LPCTSTR name, const char *value);
};


#endif
