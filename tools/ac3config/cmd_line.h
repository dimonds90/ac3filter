class CmdLine
{
protected:
  int argc;
  char **argv;
  char *buf;

public:
  CmdLine(const char *cmd_line);
  ~CmdLine();

  inline int count() const { return argc; }
  inline const char *operator [](int i) const { return argv[i]; }
};
