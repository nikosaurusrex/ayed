#pragma once

#include "base.h"

struct Arena
{
   U64 size;
   U64 top;
   U8 *ptr;
};

struct TempArena
{
   Arena *arena;
   U64 reset_top;
};

intern void init_arena(Arena *a, U64 size);
intern void sub_arena(Arena *sub, Arena *a, U64 size);

intern TempArena begin_temp_arena(Arena *a);
intern void end_temp_arena(TempArena ta);

#define push_array(a, ty, n, ...) (ty *) push_size(a, n * sizeof(ty), ## __VA_ARGS__)
#define push_struct(a, ty, ...) (ty *) push_size(a, sizeof(ty), ## __VA_ARGS__)
intern U8 *push_size(Arena *a, U64 size, U64 align=4);
