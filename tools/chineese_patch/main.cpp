#include <stdio.h>
#include <string.h>

const char *filename = "ac3filter.ax";
const unsigned char seq[]   = { 0x00, 0x90, 0x01, 0xe0, 0x00 };
const unsigned char patch[] = { 0x00, 0x8d, 0x01, 0x08, 0x01 };
size_t patch_size = sizeof(seq);

int main()
{
  size_t i;
  FILE *f = 0;
  fopen_s(&f, filename, "r+b");

  if (!f)
  {
    printf("Cannot open file %s", filename);
    return 1;
  }

  fseek(f, 0, SEEK_END);
  size_t file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buf = new char[file_size];
  if (!buf)
  {
    printf("Not enough memory (%i)", file_size);
    fclose(f);
    return 1;
  }

  if (fread(buf, 1, file_size, f) != file_size)
  {
    printf("Cannot load file");
    fclose(f);
    return 1;
  }

  bool patched = true;
  for (i = 0; i < file_size - patch_size; i++)
  {
    if (!memcmp(buf+i, seq, patch_size))
    {
      patched = false;
      break;
    }
    if (!memcmp(buf+i, patch, patch_size)) 
    {
      patched = true;
      break;
    }
  }

  if (i >= file_size - patch_size)
  {
    printf("Cannot find patch location");
    fclose(f);
    return 1;
  }

  printf("Patch location: 0x%x%s\n", i, patched? " (patched)": "");

  fseek(f, i, SEEK_SET);
  fwrite(patched? seq: patch, 1, patch_size, f);
  printf(patched? "Unpatch done": "Patch done");

  fclose(f);
  return 0;
}
