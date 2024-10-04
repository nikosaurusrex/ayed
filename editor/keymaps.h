#pragma once

#include "base/base.h"

#define MAX_KEY_COMBINATIONS (1 << (8 + 3))

struct Editor;
typedef void (*ShortcutFn)(Editor *ed);
struct Shortcut {
   const char *name;
   ShortcutFn function;
};

struct Keymap {
   Shortcut shortcuts[MAX_KEY_COMBINATIONS];
};

intern Shortcut *keymap_get_shortcut(Keymap *km, U32 key_comb);

intern Keymap *keymap_create_empty(Arena *a);
intern void    create_default_keymaps(Editor *ed, Arena *a);