#include <string.h>
#include "cmd_line.h"

static inline bool is_space(char c)
{ return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == 0; }

CmdLine::CmdLine(const char *cmd_line)
{
  argc = 0;
  argv = 0;
  buf = 0;

  if (!cmd_line) return;

  size_t len = strlen(cmd_line);
  if (!len) return;

  buf = new char[len + 1];
  if (buf)
  {
    memcpy(buf, cmd_line, len + 1); // include ending zero

    size_t i;
    enum { state_space, state_parameter, state_quoted_parameter } state;

    /////////////////////////////////////////////////////
    // Count arguments

    state = state_space;
    for (i = 0; i < len; i++)
      switch (state)
      {
        case state_space:
          if (buf[i] == '\"')
          {
            argc++;
            state = state_quoted_parameter;
          }
          else if (!is_space(buf[i]))
          {
            argc++;
            state = state_parameter;
          }
          continue;

        case state_parameter:
          if (is_space(buf[i]))
            state = state_space;
          continue;

        case state_quoted_parameter:
          if (buf[i] == '\"')
            state = state_space;
          continue;
      }

    argv = new char*[argc];

    /////////////////////////////////////////////////////
    // Fill argv

    int iparam = 0;
    state = state_space;
    for (i = 0; i < len; i++)
      switch (state)
      {
        case state_space:
          if (buf[i] == '\"')
          {
            buf[i] = 0;
            argv[iparam++] = buf + i + 1;
            state = state_quoted_parameter;
          }
          else if (!is_space(buf[i]))
          {
            argv[iparam++] = buf + i;
            state = state_parameter;
          }
          else
            buf[i] = 0;
          continue;

        case state_parameter:
          if (is_space(buf[i]))
          {
            buf[i] = 0;
            state = state_space;
          }
          continue;

        case state_quoted_parameter:
          if (buf[i] == '\"')
          {
            buf[i] = 0;
            state = state_space;
          }
          continue;
      }
  }
}

CmdLine::~CmdLine()
{
  if (buf) delete buf;
  if (argv) delete argv;
}
