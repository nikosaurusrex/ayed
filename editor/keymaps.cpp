#include "keymaps.h"

#include <ctype.h>

#include <GLFW/glfw3.h>

#include "editor.h"

#define CTRL  (1 << 8)
#define ALT   (1 << 9)
#define SHIFT (1 << 10)

#define MAX_NORMAL_LENGTH 8
global U8  g_normal_buffer[MAX_NORMAL_LENGTH];
global U32 g_normal_index = 0;

#define SHORTCUT(name)                                                                                                                     \
    intern void shortcut_fn_##name(Editor *ed);                                                                                            \
    Shortcut    shortcut_##name = {#name, shortcut_fn_##name};                                                                             \
    void        shortcut_fn_##name(Editor *ed)

intern B32 normal_mode_get_shortcut(Editor *ed, Shortcut *shortcut);

SHORTCUT(null)
{
}

SHORTCUT(insert_char)
{
   InputEvent input_event = ed->last_input_event;

   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U8 ch       = input_event.ch;

   U64 cursor_before = p->cursor;
   p->cursor = insert_char(&p->buffer, ch, p->cursor);
   ed_on_text_change(ed, {cursor_before, p->cursor});

   if (ch == '}') {
      U32 indent = brace_matching_indentation(buf, p->cursor);

      U64 start = cursor_line_begin(buf, p->cursor);
      U32 leading = 0;
      for (U64 i = start; i < p->cursor; ++i) {
         if (is_whitespace((*buf)[i])) {
            leading++;
         } else {
            break;
         }
      }

      if (leading > indent) {
         U32 del = leading - indent;

         U64 end = delete_chars(buf, start, del);
         ed_on_text_change(ed, {start, end});

         pane_set_cursor(p, start + indent + 1);
      }
   }
}

SHORTCUT(cursor_left)
{
   pane_cursor_next(&ed->pane);
}

SHORTCUT(cursor_right)
{
   pane_cursor_next(&ed->pane);
}

SHORTCUT(cursor_up)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
   
   if (p->cursor_store < 0) {
      p->cursor_store = (S64) cursor_column(buf, p->cursor);
   }

   U64 beginning_of_prev_line = cursor_prev_line_begin(buf, p->cursor);
   U64 prev_line_length = line_length(buf, beginning_of_prev_line);

   p->cursor = beginning_of_prev_line + MIN(prev_line_length, (U64)p->cursor_store);
}

SHORTCUT(cursor_down)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   if (p->cursor_store < 0) {
      p->cursor_store = (S64) cursor_column(buf, p->cursor);
   }

   U64 beginning_of_next_line = cursor_next_line_begin(buf, p->cursor);
   U64 next_line_length = line_length(buf, beginning_of_next_line);

   p->cursor = beginning_of_next_line + MIN(next_line_length, (U64)p->cursor_store);
}

SHORTCUT(delete_forwards)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U64 before = p->cursor;
   pane_set_cursor(p, delete_char(buf, p->cursor));
   ed_on_text_change(ed, {before, p->cursor});
}

SHORTCUT(delete_backwards)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   if (p->cursor == 0) {
      return;
   }

   B32 is_newline = (*buf)[p->cursor - 1] == '\n';

   U64 before = p->cursor;
   pane_set_cursor(p, delete_char(buf, p->cursor - 1));
   ed_on_text_change(ed, {before, p->cursor});
}

SHORTCUT(insert_new_line)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U64 before = p->cursor;
   pane_set_cursor(p, insert_line(&p->buffer, p->cursor, 1));
   ed_on_text_change(ed, {before, p->cursor});
}

SHORTCUT(insert_tab)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U64 before = p->cursor;
   pane_set_cursor(p, insert_char(buf, '\t', p->cursor));
   ed_on_text_change(ed, {before, p->cursor});
}

SHORTCUT(normal_mode)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   ed->mode = ED_NORMAL;
   g_normal_index                  = 0;
   g_normal_buffer[g_normal_index] = 0;
}

SHORTCUT(normal_cursor_back)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_back_normal(buf, p->cursor));
}

SHORTCUT(normal_cursor_next)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_next_normal(buf, p->cursor));
}

SHORTCUT(insert_beginning_of_line)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_line_begin(buf, p->cursor));
   ed->mode = ED_INSERT;
}

SHORTCUT(insert_end_of_line)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U64 end = cursor_line_end(buf, p->cursor);
   pane_set_cursor(p, end);
   ed->mode = ED_INSERT;
}

SHORTCUT(insert_mode)
{
   ed->mode = ED_INSERT;
}

SHORTCUT(insert_mode_next)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   ed->mode = ED_INSERT;
   pane_cursor_next(p);
}

SHORTCUT(go_word_next)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_next_word(buf, p->cursor));
}

SHORTCUT(go_word_end)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_end_of_word(buf, p->cursor));
}

SHORTCUT(go_word_prev)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_prev_word(buf, p->cursor));
}

SHORTCUT(goto_buffer_begin)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, 0);
}

SHORTCUT(goto_buffer_end)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, buf->len);
}

SHORTCUT(new_line_before)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U64 before = p->cursor;
   p->cursor = cursor_prev_line_end(buf, p->cursor);
   pane_set_cursor(p, insert_line(buf, p->cursor, 1));
   ed_on_text_change(ed, {before, p->cursor});
   ed->mode = ED_INSERT;
}

SHORTCUT(new_line_after)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   U64 before = p->cursor;
   p->cursor = cursor_line_end(buf, p->cursor);
   pane_set_cursor(p, insert_line(buf, p->cursor, 1));
   ed_on_text_change(ed, {before, p->cursor});
   ed->mode = ED_INSERT;
}

SHORTCUT(skip_paragraph_up)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_paragraph_up(buf, p->cursor));
}

SHORTCUT(skip_paragraph_down)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   pane_set_cursor(p, cursor_paragraph_down(buf, p->cursor));
}

SHORTCUT(normal_mode_clear)
{
   g_normal_index                  = 0;
   g_normal_buffer[g_normal_index] = 0;
}

SHORTCUT(normal_handle)
{
   U8 *normal_buffer = g_normal_buffer;
   U32 normal_index  = g_normal_index;

   if (normal_index + 2 >= MAX_NORMAL_LENGTH) {
      return;
   }

   InputEvent input_event = ed->last_input_event;
   char       ch          = input_event.ch;
   U32        key_comb    = input_event.key_comb;

   if (key_comb & CTRL) {
      normal_buffer[normal_index] = '^';
      normal_buffer[normal_index] = ch;
      g_normal_index += 2;
   } else {
      normal_buffer[normal_index] = ch;
      g_normal_index++;
   }

   normal_buffer[g_normal_index] = 0;

   Shortcut shortcut;
   B32      exists = normal_mode_get_shortcut(ed, &shortcut);
   if (exists) {
      shortcut.function(ed);
      shortcut_fn_normal_mode_clear(ed);
   }
}

SHORTCUT(visual_mode)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   ed->mode = ED_VISUAL;

   p->visual = p->cursor;
}

SHORTCUT(visual_mode_line)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   ed->mode = ED_VISUAL_LINE;
}

SHORTCUT(visual_line_down)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;

   shortcut_fn_cursor_down(ed);
}

SHORTCUT(visual_line_up)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

SHORTCUT(visual_line_buffer_begin)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

SHORTCUT(visual_line_buffer_end)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

SHORTCUT(visual_delete)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

SHORTCUT(visual_yoink)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

SHORTCUT(yoink_selection)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

SHORTCUT(yoink_paste)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
}

/* TODO: definetly rework this! */
B32
normal_mode_get_shortcut(Editor *ed, Shortcut *shortcut)
{
   Pane *p = &ed->pane;
   GapBuffer *buf = &p->buffer;
   U8     *normal_buffer = g_normal_buffer;

   switch (normal_buffer[0]) {
   // one letter shortcuts
   case 'x':
      *shortcut = shortcut_delete_forwards;
      break;
   case 'v':
      *shortcut = shortcut_visual_mode;
      break;
   case 'V':
      *shortcut = shortcut_visual_mode_line;
      break;
   case 'h':
      *shortcut = shortcut_normal_cursor_back;
      break;
   case 'l':
      *shortcut = shortcut_normal_cursor_next;
      break;
   case 'j':
      *shortcut = shortcut_cursor_down;
      break;
   case 'k':
      *shortcut = shortcut_cursor_up;
      break;
   case 'w':
      *shortcut = shortcut_go_word_next;
      break;
   case 'e':
      *shortcut = shortcut_go_word_end;
      break;
   case 'b':
      *shortcut = shortcut_go_word_prev;
      break;
   case 'y':
      *shortcut = shortcut_yoink_selection;
      break;
   case 'p':
      *shortcut = shortcut_yoink_paste;
      break;
   case 'I':
      *shortcut = shortcut_insert_beginning_of_line;
      break;
   case 'A':
      *shortcut = shortcut_insert_end_of_line;
      break;
   case 'i':
      *shortcut = shortcut_insert_mode;
      break;
   case 'a':
      *shortcut = shortcut_insert_mode_next;
      break;
   case 'o':
      *shortcut = shortcut_new_line_after;
      break;
   case 'O':
      *shortcut = shortcut_new_line_before;
      break;
   case 'G':
      *shortcut = shortcut_goto_buffer_end;
      break;
   case '{':
      *shortcut = shortcut_skip_paragraph_up;
      break;
   case '}':
      *shortcut = shortcut_skip_paragraph_down;
      break;
   default: {
      // two letter shortcuts
      char c1 = normal_buffer[0];
      if (g_normal_index == 2 && !isdigit(c1)) {
         char c2 = normal_buffer[1];

         if (c1 == 'g' && c2 == 'g') {
               shortcut_fn_goto_buffer_begin(ed);
               shortcut_fn_normal_mode_clear(ed);
               return 0;
         } else if (c1 == 'd') {
               if (c2 == 'd') {
                  shortcut_fn_normal_mode_clear(ed);
                  return 0;
               } else if (c2 == 'w') {
                  shortcut_fn_normal_mode_clear(ed);
                  return 0;
               } else {
                  return 0;
               }
         } else if (c1 == 'c' && c2 == 'w') {

               shortcut_fn_normal_mode_clear(ed);
               shortcut_fn_insert_mode(ed);
               return 0;
         } else {
               return 0;
         }
      } else {
         return 0;
      }
   }
   }

   return 1;
}

Shortcut *
keymap_get_shortcut(Keymap *km, U32 key_comb)
{
   ASSERT(key_comb <= MAX_KEY_COMBINATIONS);

   return km->shortcuts + key_comb;
}

Keymap *
keymap_create_empty(Arena *a)
{
   Keymap *keymap = push_struct(a, Keymap);

   for (int i = 0; i < MAX_KEY_COMBINATIONS; ++i) {
      keymap->shortcuts[i] = shortcut_null;
   }

   return keymap;
}

void
create_default_keymaps(Editor *ed, Arena *a)
{
   // insert mode keymap
   Keymap *keymap = keymap_create_empty(a);

   for (char ch = ' '; ch <= '~'; ++ch) {
      keymap->shortcuts[ch]         = shortcut_insert_char;
      keymap->shortcuts[ch | SHIFT] = shortcut_insert_char;
   }

   keymap->shortcuts[GLFW_KEY_ENTER]     = shortcut_insert_new_line;
   keymap->shortcuts[GLFW_KEY_ENTER | SHIFT]     = shortcut_insert_new_line;
   keymap->shortcuts[GLFW_KEY_TAB]       = shortcut_insert_tab;
   keymap->shortcuts[GLFW_KEY_BACKSPACE] = shortcut_delete_backwards;
   keymap->shortcuts[GLFW_KEY_BACKSPACE | SHIFT] = shortcut_delete_backwards;
   keymap->shortcuts[GLFW_KEY_ESCAPE]    = shortcut_normal_mode;

   keymap->shortcuts[GLFW_KEY_LEFT]  = shortcut_cursor_left;
   keymap->shortcuts[GLFW_KEY_RIGHT] = shortcut_cursor_right;
   keymap->shortcuts[GLFW_KEY_UP]    = shortcut_cursor_up;
   keymap->shortcuts[GLFW_KEY_DOWN]  = shortcut_cursor_down;

   ed->keymaps[ED_INSERT] = keymap;

   // normal keymap
   keymap = keymap_create_empty(a);

   for (char ch = ' '; ch <= '~'; ++ch) {
      keymap->shortcuts[ch]         = shortcut_normal_handle;
      keymap->shortcuts[ch | SHIFT] = shortcut_normal_handle;
      keymap->shortcuts[ch | CTRL]  = shortcut_normal_handle;
   }

   keymap->shortcuts[GLFW_KEY_ESCAPE] = shortcut_normal_mode_clear;

   ed->keymaps[ED_NORMAL] = keymap;

   // visual keymap
   keymap = keymap_create_empty(a);

   keymap->shortcuts['L']             = shortcut_normal_cursor_next;
   keymap->shortcuts['H']             = shortcut_normal_cursor_back;
   keymap->shortcuts['J']             = shortcut_cursor_down;
   keymap->shortcuts['K']             = shortcut_cursor_up;
   keymap->shortcuts['D']             = shortcut_visual_delete;
   keymap->shortcuts['Y']             = shortcut_visual_yoink;
   keymap->shortcuts['W']             = shortcut_go_word_next;
   keymap->shortcuts['E']             = shortcut_go_word_end;
   keymap->shortcuts['B']             = shortcut_go_word_prev;
   keymap->shortcuts['G']             = shortcut_goto_buffer_begin;
   keymap->shortcuts['G' | SHIFT]     = shortcut_goto_buffer_end;
   keymap->shortcuts[GLFW_KEY_ESCAPE] = shortcut_normal_mode;

   ed->keymaps[ED_VISUAL] = keymap;

   // visual line
   keymap = keymap_create_empty(a);

   keymap->shortcuts['J']             = shortcut_visual_line_down;
   keymap->shortcuts['K']             = shortcut_visual_line_up;
   keymap->shortcuts['D']             = shortcut_visual_delete;
   keymap->shortcuts['Y']             = shortcut_visual_yoink;
   keymap->shortcuts['G']             = shortcut_visual_line_buffer_begin;
   keymap->shortcuts['G' | SHIFT]     = shortcut_visual_line_buffer_end;
   keymap->shortcuts[GLFW_KEY_ESCAPE] = shortcut_normal_mode;

   ed->keymaps[ED_VISUAL_LINE] = keymap;
}