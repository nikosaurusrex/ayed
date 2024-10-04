#pragma once

#include "base.h"

typedef struct String8 String8;
struct String8
{
   U8 *ptr;
   U64 len;

   String8() {
      ptr = 0;
      len = 0;
   }

   String8(const char *cstr) {
      ptr = (U8 *)cstr;
      len = strlen(cstr);
   }

   String8(U8 *_ptr, U64 _len) {
      ptr = _ptr;
      len = _len;
   }

   U8 operator[](U64 index) const {
      ASSERT(index < len);
      return ptr[index];
   }

   U8 &operator[](U64 index) {
      ASSERT(index < len);
      return ptr[index];
   }

   B32 operator==(const char *cstr) {
      if ((const char *) ptr == cstr) return 1;
      if (!ptr) return 0;
      if (!cstr) return 0;

      U64 cstr_len = strlen(cstr);
      if (cstr_len != len) return 0;

      for (U64 i = 0; i < len; ++i) {
         if (ptr[i] != cstr[i]) {
            return 0;
         }
      }

      return 1;
   }

   B32 operator!=(const char *cstr) {
      return !operator==(cstr);
   }

   B32 operator==(String8 s) {
      if (s.ptr == ptr) return 1;
      if (!ptr) return 0;
      if (!s.ptr) return 0;

      if (s.len != len) return 0;

      for (U64 i = 0; i < len; ++i) {
         if (ptr[i] != s.ptr[i]) {
            return 0;
         }
      }

      return 1;
   }

   B32 operator!=(String8 s) {
      return !operator==(s);
   }
};

// returned pointer has to be free'd
intern char *cstr_from_str8(String8 s);

const String8 null_str8 = {0, 0};
