#include <stdio.h>

// Sequence to find:
// 00 8d 01 c9 00
// -- -- -- ef --

const int           addr[] = { 0x23768 };
const unsigned char from[] = { 0xc9    };
const unsigned char to[]   = { 0xef    };
const char *filename = "ac3filter.ax";
const n = sizeof(addr) / sizeof(int);


int main()
{
  int i;
  unsigned char b;
  bool patch = true;
  FILE *f = fopen(filename, "r+b");
  if (!f)
  {
    printf("Cannot open file ac3filter.ax");
    return 0;
  }

  for(i = 0; i < n; i++)
  {
    fseek(f, addr[i], SEEK_SET);
    fread(&b, 1, 1, f);
    if (from[i] != b)
    {
      patch = false;
      for (i = 0; i < n; i++)
      {
        fseek(f, addr[i], SEEK_SET);
        fread(&b, 1, 1, f);
        if (to[i] != b)
        {
          printf("Incorrect file");
          fclose(f);
          return 0;
        }
      }
      break;
    }
  }

  if (patch)
    printf("Patching dialog size");
  else
    printf("Unpatching dialog size");

  for(i = 0; i < n; i++)
  {
    b = patch? to[i]: from[i];
    fseek(f, addr[i], SEEK_SET);
    fwrite(&b, 1, 1, f);
  }

  fclose(f);
  return 1;
}
