#include <string.h>
#include "translate.h"
#include "auto_file.h"

Translator::Translator()
: data(0), data_size(0), n(0), id(0), text(0), idhash(0), textlen(0)
{}

Translator::Translator(const char *_file)
: data(0), data_size(0), n(0), id(0), text(0), idhash(0), textlen(0)
{
  open(_file);
}

Translator::~Translator()
{
  close();
}

bool 
Translator::open(const char *_file)
{
  close();

  // read the translation file into memory

  AutoFile f(_file);
  if (!f.is_open())
    return false;

  data = new char[f.size() + 1];
  data_size = f.read(data, f.size());
  f.close();

  char *data_end = data + data_size;

  // count number of strings in the file and allocate buffers

  int count = 0;
  char *pos = data;
  while (pos < data_end)
  {
    if (*pos == '\n')
      count++;
    pos++;
  }

  id = new char *[count];
  text = new char *[count];
  idhash = new int[count];
  textlen = new size_t[count];

  // parse

  char *str_start = data;
  char *str_pos = data;

  while (str_start < data_end)
  {
    // find where translation id ends
    str_pos = str_start;
    while (str_pos < data_end && *str_pos != '=' && *str_pos != '\r' && *str_pos != '\n')
      str_pos++;

    if (*str_pos == '=')
    {
      // translation id found; fill data
      *str_pos = 0;
      id[n] = str_start;
      idhash[n] = hash(str_start);

      // find where translatin string ends
      str_pos++;
      str_start = str_pos;
      while (str_pos < data_end && *str_pos != '\r' && *str_pos != '\n')
        str_pos++;

      *str_pos = 0;
      text[n] = str_start;
      textlen[n] = str_pos - str_start;
      n++;
    }

    // seek to a new string
    while (str_pos < data_end && (*str_pos == '\r' || *str_pos == '\n')) str_pos++;
    str_start = str_pos;

  }

  return true;
}

void
Translator::close()
{
  if (data) delete data;
  if (id) delete id;
  if (text) delete text;
  if (idhash) delete idhash;
  if (textlen) delete textlen;

  data = 0;
  data_size = 0;
  n = 0;
  id = 0;
  text = 0;
  idhash = 0;
  textlen = 0;
}

bool
Translator::translate(const char *_id, char *_str, size_t _size, const char *_def)
{
  int h = hash(_id);
  for (int i = 0; i < n; i++)
    if (idhash[i] == h)
      if (!strcmp(id[i], _id))
      {
        memcpy(_str, text[i], MIN(_size, textlen[i] + 1));
        return true;
      }

  if (_def)
  {
    size_t len = strlen(_def) + 1;
    memcpy(_str, _def, MIN(_size, len));
    return false;
  }

  return false;
}

int
Translator::hash(const char *s)
{
  int h = 0;
  while (*s)
    h += *s++;
  return h;
}
