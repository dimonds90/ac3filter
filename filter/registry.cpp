#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <streams.h>
#include "registry.h"


RegistryKey::RegistryKey()
{
  key = 0;
}

RegistryKey::RegistryKey(LPCTSTR name, HKEY hive)
{
  key = 0;
  if (name)
    if (RegOpenKeyEx(hive, name, 0, KEY_READ | KEY_WRITE, &key) != ERROR_SUCCESS)
      key = 0;
}

RegistryKey::~RegistryKey()
{
  if (key)
    RegCloseKey(key);
}

bool 
RegistryKey::open_key(LPCTSTR name, HKEY hive)
{
  if (key)
    RegCloseKey(key);

  if (RegOpenKeyEx(hive, name, 0, KEY_READ | KEY_WRITE, &key) != ERROR_SUCCESS)
    key = 0;

  return key != 0;
}

bool 
RegistryKey::create_key(LPCTSTR name, HKEY hive)
{
  if (key)
    RegCloseKey(key);

  if (RegOpenKeyEx(hive, name, 0, KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY, &key) == ERROR_SUCCESS)
    return true;
  else
    key = 0;

  // split path and subkey name
  const size_t buf_size = 255;
  CHAR path[buf_size];
  CHAR subkey[buf_size];
  LPCTSTR name_end = name + strlen(name) - 1;
  LPCTSTR pos;
  size_t n;

  // find key name
  pos = name_end;
  while (pos > name && *pos != '\\')
    pos--;

  n = MIN(buf_size - 1, pos - name);
  memcpy(path, name, n);
  path[n] = 0;

  n = MIN(buf_size - 1, name_end - pos);
  memcpy(subkey, pos + 1, n);
  subkey[n] = 0;

  // create parent key
  if (!create_key(path, hive))
  {
    key = 0;
    return false;
  }

  // now key != 0 and it is parent key for required 'name' key

  HKEY new_key;
  DWORD disposition;
  if (RegCreateKeyEx(key, subkey, NULL, "", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY, NULL, &new_key, &disposition))
  {
    RegCloseKey(key);
    key = 0;
    return false;
  }

  RegCloseKey(key);
  key = new_key;
  return true;
}



bool 
RegistryKey::get_bool(LPCTSTR name, bool &value)
{
  if (!key) return false;

  DWORD v, type, buf_len;
  v = 0;
  buf_len = 4;

  if (RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&v, &buf_len) != ERROR_SUCCESS)
    return false;

  if (type != REG_DWORD)
    return false;

  value = v != 0;
  DbgLog((LOG_TRACE, 3, "RegistryKey::get_bool(\"%s\") = %s", name, value? "true": "false"));
  return true;
}

bool
RegistryKey::get_int32(LPCTSTR name, int32_t &value)
{
  if (!key) return false;

  DWORD v, type, buf_len;
  v = 0;
  buf_len = 4;

  if (RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&v, &buf_len) != ERROR_SUCCESS)
    return false;

  if (type != REG_DWORD)
    return false;

  value = v;
  DbgLog((LOG_TRACE, 3, "RegistryKey::get_int32(\"%s\") = %i", name, value));
  return true;
}

bool 
RegistryKey::get_float(LPCTSTR name, double &value)
{
  if (!key) return false;

  DWORD type, buf_len;
  char buf[256];
  buf_len = 256;

  if (RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&buf, &buf_len) != ERROR_SUCCESS)
    return false;

  if (type != REG_SZ)
    return false;

  double v = 0.0;
  char tmp;
  if (sscanf(buf, "%lg%c", &v, &tmp) != 1)
  {
    DbgLog((LOG_TRACE, 3, "RegistryKey::get_float(\"%s\"): cannot convert \"%s\" to float!", name, buf));
    return false;
  }

  value = v;
  DbgLog((LOG_TRACE, 3, "RegistryKey::get_float(\"%s\") = %lg", name, value));
  return true;
}

bool 
RegistryKey::get_text(LPCTSTR name, char *value, int size)
{
  if (!key) return false;

  DWORD type;
  DWORD buf_len = size;

  if (RegQueryValueEx(key, name, NULL, &type, (LPBYTE)value, &buf_len) != ERROR_SUCCESS)
    return false;

  if (type != REG_SZ)
    return false;
  
  DbgLog((LOG_TRACE, 3, "RegistryKey::get_text(\"%s\") = \"%s\"", name, value));
  return true;
}

void
RegistryKey::set_bool(LPCTSTR name, bool _value)
{
  if (!key) return;

  DWORD value = _value;
  RegSetValueEx(key, name, NULL, REG_DWORD, (LPBYTE)&value, 4);
//  DbgLog((LOG_TRACE, 3, "RegistryKey::set_bool(\"%s\", %s", name, _value? "true": "false"));
}

void
RegistryKey::set_int32(LPCTSTR name, int32_t _value)
{
  if (!key) return;

  DWORD value = _value;
  RegSetValueEx(key, name, NULL, REG_DWORD, (LPBYTE)&value, 4);
//  DbgLog((LOG_TRACE, 3, "RegistryKey::set_int32(\"%s\", %i)", name, _value));
}

void
RegistryKey::set_float(LPCTSTR name, double _value)
{
  if (!key) return;

  char buf[256];
  sprintf(buf, "%lg", _value);
  RegSetValueEx(key, name, NULL, REG_SZ, (LPBYTE)&buf, (DWORD)strlen(buf)+1);
//  DbgLog((LOG_TRACE, 3, "RegistryKey::set_float(\"%s\", %lg = \"%s\")", name, _value, buf));
}

void
RegistryKey::set_text(LPCTSTR name, const char *_value)
{
  if (!key) return;

  if (_value)
    RegSetValueEx(key, name, NULL, REG_SZ, (LPBYTE)_value, (DWORD)strlen(_value)+1);
  else
    RegSetValueEx(key, name, NULL, REG_SZ, (LPBYTE)"", 1);
//  DbgLog((LOG_TRACE, 3, "RegistryKey::set_text(\"%s\", \"%s\")", name, _value? _value: ""));
}



FileConfig::FileConfig(const char *_filename, const char *_section)
{
  filename[0] = 0;
  section[0] = 0;
  ok = false;
  open (_filename, _section);
}

FileConfig::~FileConfig()
{
}

bool    
FileConfig::open(const char *_filename, const char *_section)
{
  filename[0] = 0;
  section[0] = 0;
  ok = _filename && _section;
  if (ok)
  {
    memcpy(filename, _filename, min(sizeof(filename), strlen(_filename) + 1));
    memcpy(section, _section, min(sizeof(section), strlen(_section) + 1));
    filename[255] = 0;
    section[255] = 0;
  }

  return ok;
}

bool    
FileConfig::get_bool(LPCTSTR name, bool &value)
{
  if (!ok) return false;
  char buf[256];
  if (!GetPrivateProfileString(section, name, "", buf, 128, filename)) return false;
  if (!strcmp(buf, "1"))     { value = true;  return true; }
  if (!strcmp(buf, "true"))  { value = true;  return true; }
  if (!strcmp(buf, "True"))  { value = true;  return true; }
  if (!strcmp(buf, "TRUE"))  { value = true;  return true; }
  if (!strcmp(buf, "0"))     { value = false; return true; }
  if (!strcmp(buf, "false")) { value = false; return true; }
  if (!strcmp(buf, "False")) { value = false; return true; }
  if (!strcmp(buf, "FALSE")) { value = false; return true; }
  return false;
}

bool
FileConfig::get_int32(LPCTSTR name, int32_t &value)
{
  if (!ok) return false;
  char buf[256];

  if (!GetPrivateProfileString(section, name, "default", buf, 128, filename)) return false;

  int v = 0;
  char tmp;
  if (sscanf(buf, "%i%c", &v, &tmp) != 1)
  {
    DbgLog((LOG_TRACE, 3, "FileConfig::get_int32(\"%s\"): cannot convert \"%s\" to integer!", name, buf));
    return false;
  }

  value = v;
  return true;
}

bool
FileConfig::get_double(LPCTSTR name, double &value)
{
  if (!ok) return false;
  char buf[256];

  if (!GetPrivateProfileString(section, name, "default", buf, 128, filename)) return false;

  double v = 0.0;
  char tmp;
  if (sscanf(buf, "%lg%c", &v, &tmp) != 1)
  {
    DbgLog((LOG_TRACE, 3, "FileConfig::get_float(\"%s\"): cannot convert \"%s\" to float!", name, buf));
    return false;
  }

  value = v;
  return true;
}

bool FileConfig::get_text(LPCTSTR name, char *value, int size)
{
  if (!value) return false;
  if (!ok) { value[0] = 0; return false; }
  if (!GetPrivateProfileString(section, name, "", value, size, filename)) return false;
  return true;
}

void FileConfig::set_bool(LPCTSTR name, bool value)
{
  if (!ok) return;
  WritePrivateProfileString(section, name, value? "true": "false", filename);
}

void FileConfig::set_int32(LPCTSTR name, int32_t value)
{
  if (!ok) return;
  char buf[256];
  sprintf(buf, "%i", value);
  WritePrivateProfileString(section, name, buf, filename);
}

void FileConfig::set_double(LPCTSTR name, double value)
{
  if (!ok) return;
  char buf[256];
  sprintf(buf, "%f", value);
  WritePrivateProfileString(section, name, buf, filename);
}

void FileConfig::set_text(LPCTSTR name, const char *value)
{
  if (!ok) return;
  WritePrivateProfileString(section, name, value, filename);
}
