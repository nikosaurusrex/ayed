#include "base_string.h"

#include <stdlib.h>

String8
str8(U8 *ptr, U64 len)
{
   String8 s = {};
   s.ptr = ptr;
   s.len = len;
   return s;
}

String8
str8_from_cstr(const char *cstr)
{
   String8 s = {};
   s.ptr = (U8 *)cstr;
   s.len = strlen(cstr);
   return s;
}

char *
cstr_from_str8(String8 s)
{
   // hmm
   char *cstr = (char *) malloc(s.len + 1);
   MEM_COPY(cstr, s.ptr, s.len);
   cstr[s.len] = 0;
   return cstr;
}

B32
str8_eq_cstr(String8 s, const char *cstr)
{
   if ((const char *) s.ptr == cstr) return 1;
   if (!s.ptr) return 0;
   if (!cstr) return 0;

   U64 cstr_len = strlen(cstr);
   if (cstr_len != s.len) return 0;

   for (U64 i = 0; i < s.len; ++i) {
      if (s.ptr[i] != cstr[i]) {
         return 0;
      }
   }

   return 1;
}
