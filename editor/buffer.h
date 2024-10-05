#pragma once

#include "base/base_inc.h"

struct GapBuffer
{
   U8 *ptr;
   U64 cap;
   U64 start; // gap start
   U64 end; // gap end
   U64 len;

   U8 operator[](U64 index) const {
      ASSERT(index < len);

      if (index < start) {
         return ptr[index];
      } else {
         return ptr[index + (end - start)];
      }
   }

   U8 &operator[](U64 index) {
      ASSERT(index < len + (end - start));

      if (index < start) {
         return ptr[index];
      } else {
         return ptr[index + (end - start)];
      }
   }
};

struct TSParser;
struct TSTree;
struct TSQuery;
struct SyntaxHighlighter
{
   TSParser *parser;
   TSTree *tree;
   TSQuery *query;
};

struct Pane
{
   GapBuffer buffer;
   SyntaxHighlighter highlighter;
   Arena arena;

   U64 prev_cursor; // for treesitter
   U64 cursor;
   U64 visual; // visual cursor position
   S64 cursor_store; // cursor column position to restore after moving up/down

   U32 scroll_offset;
   U32 cols;
   U32 rows;
};

intern NKINLINE B32 is_whitespace(U8 c);

intern GapBuffer gap_buffer_from_arena(Arena a);
intern void load_source_file(GapBuffer *buf, String8 path, Arena *a);

intern U64 insert_char(GapBuffer *buf, U8 c, U64 pos);
intern U64 insert_string(GapBuffer *buf, String8 s, U64 pos);
intern U64 insert_line(GapBuffer *buf, U64 pos, B32 auto_indent);
intern U64 delete_char(GapBuffer *buf, U64 pos);
intern U64 delete_char_back(GapBuffer *buf, U64 pos);
intern U64 delete_chars(GapBuffer *buf, U64 pos, U64 n);

intern String8 str8_from_gap_buffer(GapBuffer *buf, Arena *a);

intern U64 line_length(GapBuffer *buf, U64 crs);

intern Pane create_pane(U64 cap, U32 cols, U32 rows);
intern void destroy_pane(Pane pane);

intern SyntaxHighlighter create_syntax_highlighter();
intern void destroy_syntax_highlighter(SyntaxHighlighter hl);
intern void update_syntax_highlighting(Pane *p, Arena *a);

// they not only move the cursor but also reset cursor_store
intern NKINLINE void pane_cursor_back(Pane *p);
intern NKINLINE void pane_cursor_next(Pane *p);
intern NKINLINE void _pane_set_cursor(Pane *p, U64 c);
intern NKINLINE void pane_set_cursor(Pane *p, U64 crs);
intern NKINLINE void pane_reset_col_store(Pane *p);

intern void update_scroll(Pane *pane);

intern U64 cursor_back(GapBuffer *buf, U64 crs);
intern U64 cursor_next(GapBuffer *buf, U64 crs);

// normal mode back and next. They don't change lines
intern U64 cursor_back_normal(GapBuffer *buf, U64 crs);
intern U64 cursor_next_normal(GapBuffer *buf, U64 crs);

intern U64 cursor_line_begin(GapBuffer *buf, U64 crs);
intern U64 cursor_line_end(GapBuffer *buf, U64 crs);
intern U64 cursor_next_line_begin(GapBuffer *buf, U64 crs);
intern U64 cursor_prev_line_begin(GapBuffer *buf, U64 crs);
intern U64 cursor_next_line_end(GapBuffer *buf, U64 crs);
intern U64 cursor_prev_line_end(GapBuffer *buf, U64 crs);
intern U64 cursor_column(GapBuffer *buf, U64 crs);

intern U64 cursor_prev_word(GapBuffer *buf, U64 crs);
intern U64 cursor_end_of_word(GapBuffer *buf, U64 crs);
intern U64 cursor_next_word(GapBuffer *buf, U64 crs);

intern U64 cursor_paragraph_up(GapBuffer *buf, U64 crs);
intern U64 cursor_paragraph_down(GapBuffer *buf, U64 crs);

intern U32 line_indent(GapBuffer *buf, U64 crs);

intern U32 brace_matching_indentation(GapBuffer *buf, U64 crs);