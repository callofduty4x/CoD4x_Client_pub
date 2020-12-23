#include <stdint.h>

FILE *__cdecl _fopen(const char *, const char *);
int __cdecl fclose(FILE *File);
size_t __cdecl fread(void *, size_t, size_t, FILE *);
size_t __cdecl fwrite(const void *, size_t, size_t, FILE *);
__int32 __cdecl ftell(FILE *);
int __cdecl fseek(FILE *, __int32, int);