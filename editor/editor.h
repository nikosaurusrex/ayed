#pragma once

#include "base/base_inc.h"
#include "buffer.h"
#include "keymaps.h"

enum
{
   ED_INSERT = 0,
   ED_NORMAL,
   ED_VISUAL,
   ED_VISUAL_LINE,
   ED_MODE_COUNT
};

enum
{
   INPUT_EVENT_PRESSED,
   INPUT_EVENT_RELEASED,
};

struct InputEvent
{
   U32 key_comb;
   U8 type;
   U8 ch;
};

struct Editor
{
   Keymap *keymaps[ED_MODE_COUNT];
   InputEvent last_input_event;
   U8 mode;
   Pane pane;
};

struct Edit
{
   U64 pos_before;
   U64 pos_after;
};

enum
{
   TAB_SIZE = 3
};

intern void highlight(Pane *p);
intern void ed_on_text_change(Editor *ed, Edit edit);