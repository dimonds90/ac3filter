#ifndef TRANSLATE_H
#define TRANSLATE_H
        
#include "auto_file.h"

class Translator
{
protected:
  char *data;
  size_t data_size;

  int n;
  char **id;
  char **text;
  int *idhash;
  size_t *textlen;

  int hash(const char *s);

public:
  Translator();
  Translator(const char *file);
  ~Translator();

  bool open(const char *file);
  void close();

  bool translate(const char *id, char *str, size_t size, const char *def);
};

#endif
