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

  int hash(const char *s) const;
  size_t unescape(char *str) const;

public:
  Translator();
  Translator(const char *file);
  ~Translator();

  bool open(const char *file);
  void reset();

  bool translate(const char *id, char *str, size_t size, const char *def);

  const char *translate(const char *id, const char *def) const;
  const char *operator()(const char *id, const char *def) const { return translate(id, def); }

  const char *author() const { return translate("#AUTHOR", "-"); }
  const char *ver() const { return translate("#VER", "-"); }
  const char *date() const { return translate("#DATE", "-"); }
  const char *comment() const { return translate("#COMMENT", ""); }
};

#endif
