#pragma once

#include "base.h"

typedef struct String8 String8;
struct String8
{
   U8 *ptr;
   U64 len;
};

intern String8 str8(U8 *ptr, U64 len);
intern String8 str8_from_cstr(const char *cstr);

intern char *cstr_from_str8(String8 s);

intern B32 str8_eq_cstr(String8 s, const char *cstr);

read_only String8 null_str8 = {0, 0};
