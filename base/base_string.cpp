#include "base_string.h"

#include <stdlib.h>

char *
cstr_from_str8(String8 s)
{
   // hmm
   char *cstr = (char *) malloc(s.len + 1);
   MEM_COPY(cstr, s.ptr, s.len);
   cstr[s.len] = 0;
   return cstr;
}
