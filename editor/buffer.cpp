#include "buffer.h"

#include "editor.h"

#include "tree_sitter/api.h"

enum
{
   MAX_GAP_SIZE = 16
};

extern "C" const TSLanguage *tree_sitter_cpp(void);

intern NKINLINE int
char_type(U8 c)
{
   if (isalnum(c) || c == '_') {
      return 1;
   }

   if (c == ' ' || c == 0) {
      return 0;
   }

   return c;
}

NKINLINE B32
is_whitespace(U8 c)
{
   int s = c == ' ';
   int n = c == '\n';
   int r = c == '\t';

   return s | n | r;
}

intern U32
utf8_len(U8 byte)
{
   if_likely ((byte & 0x80) == 0) return 1;
   if ((byte & 0xE0) == 0xC0) return 2;
   if ((byte & 0xF0) == 0xE0) return 3;
   if ((byte & 0xF8) == 0xF0) return 4;

   return 1;
}

intern void
move_gap(GapBuffer *buf, U64 pos)
{
   if (pos == buf->start) {
      return;
   }

   if (buf->start == buf->end) {
      buf->start = pos;
      buf->end = pos;
      return;
   }

   if (pos < buf->start) {
      U64 move = buf->start - pos;
      MEM_MOVE(buf->ptr + buf->end - move, buf->ptr + pos, move);
      buf->start -= move;
      buf->end -= move;
   } else {
      U64 move = pos - buf->start;
      MEM_MOVE(buf->ptr + buf->start, buf->ptr + buf->end, move);
      buf->start += move;
      buf->end += move;
   }
}

GapBuffer
gap_buffer_from_arena(Arena a)
{
   GapBuffer buf = {};

   buf.ptr = a.ptr;
   buf.cap = a.size;
   buf.start = 0;
   buf.end = MAX_GAP_SIZE;
   buf.len = 0;

   return buf;
}

void
load_source_file(GapBuffer *buf, String8 path, Arena *a)
{
   TempArena temp = begin_temp_arena(a);

   String8 content = os_read_file(path, temp.arena);
   if (!content.ptr) {
      log_error("File '%.*s' does not exist\n", (int)path.len, path.ptr);
      end_temp_arena(temp);
      return;
   }

   // remove carriage returns
   U8 *src = content.ptr;
   U8 *src_end = content.ptr + content.len;
   U8 *dst = buf->ptr;
   while (src < src_end) {
      if (*src != '\r') {
         *dst = *src;
         dst++;
         buf->start++;
         buf->len++;
      }
      src++;
   }

   buf->ptr[buf->len] = 0;

   buf->end = buf->start + MAX_GAP_SIZE;

   end_temp_arena(temp);
}

U64 insert_char(GapBuffer *buf, U8 c, U64 pos)
{
   ASSERT(buf->len < buf->cap - MAX_GAP_SIZE);

   if (pos > buf->len) {
      return pos;
   }

   move_gap(buf, pos);

   if (buf->start == buf->end) {
      U64 shift = MAX_GAP_SIZE;
      MEM_MOVE(buf->ptr + buf->end + shift, buf->ptr + buf->end, buf->len - buf->end);
      buf->end += shift;
   }
   buf->ptr[buf->start++] = c;
   buf->len++;

   return pos + 1;
}

U64
insert_string(GapBuffer *buf, String8 s, U64 pos)
{
   ASSERT(buf->len + s.len < buf->cap - MAX_GAP_SIZE);

   if (pos > buf->len) {
      return pos;
   }

   move_gap(buf, pos);

   while (s.len > 0) {
      U64 gap_size = buf->end - buf->start;
      if (gap_size == 0) {
         U64 shift = MAX_GAP_SIZE;
         MEM_MOVE(buf->ptr + buf->end + shift, buf->ptr + buf->end, buf->len - buf->end);
         buf->end += shift;
         gap_size = shift;
      }

      U64 copy_len = MIN(s.len, gap_size);
      MEM_COPY(buf->ptr + buf->start, s.ptr, copy_len);

      s.ptr += copy_len;
      s.len -= copy_len;

      buf->start += copy_len;
      buf->len += copy_len;
   }

   return pos + s.len;
}

U64
insert_line(GapBuffer *buf, U64 pos, B32 auto_indent)
{
   if (pos > buf->len) {
      return pos;
   }

   if (auto_indent) {
      U64 indent = line_indent(buf, pos);

      U64 lcc = cursor_back(buf, pos);
      while (lcc > 0 && is_whitespace((*buf)[lcc])) {
         lcc = cursor_back(buf, lcc);
      }

      U8 last_char = (*buf)[lcc];

      if (last_char == '{') {
         indent += TAB_SIZE;
      }

      pos = insert_char(buf, '\n', pos);

   /*
      for (U64 i = 0; i < indent / TAB_SIZE; ++i) {
         pos = insert_char(buf, '\t', pos);
      }

      for (U64 i = 0; i < indent % TAB_SIZE; ++i) {
         pos = insert_char(buf, ' ', pos);
      }*/
   } else {
      pos = insert_char(buf, '\n', pos);
   }

   return pos;
}

U64
delete_char(GapBuffer *buf, U64 pos)
{
   if (pos >= buf->len) {
      return pos;
   }
   
   move_gap(buf, pos);

   U32 cl = utf8_len(buf->ptr[buf->end]);
   buf->end += cl;
   buf->len -= cl;

   return pos;
}

U64
delete_char_back(GapBuffer *buf, U64 pos)
{
   // TODO:
   return pos;
}

U64
delete_chars(GapBuffer *buf, U64 pos, U64 n)
{
   if (pos + n >= buf->len) {
      return pos;
   }

   move_gap(buf, pos);

   while (n > 0 && buf->end < buf->cap) {
      U32 cl = utf8_len(buf->ptr[buf->end]);
      buf->end += cl;
      buf->len -= cl;
      n--;
   }

   return pos;
}

String8
str8_from_gap_buffer(GapBuffer *buf, Arena *a)
{
   String8 s = {};
   s.ptr = push_array(a, U8, buf->len + 1);
   s.len = buf->len;

   if (buf->start > 0) {
      MEM_COPY(s.ptr, buf->ptr, buf->start);
   }

   U64 spl = buf->len - buf->start;
   if (spl > 0) {
      MEM_COPY(s.ptr + buf->start, buf->ptr + buf->end, spl);
   }
   s.ptr[s.len] = 0;

   return s;
}

U64
line_length(GapBuffer *buf, U64 crs)
{
   return cursor_line_end(buf, crs) - cursor_line_begin(buf, crs);
}

Pane
create_pane(U64 cap, U32 cols, U32 rows)
{
   Pane p = {};
   p.rows = rows;
   p.cols = cols;
   
   init_arena(&p.arena, cap);

   p.buffer = gap_buffer_from_arena(p.arena);
   p.highlighter = create_syntax_highlighter();

   return p;
}

void
destroy_pane(Pane p)
{
   destroy_syntax_highlighter(p.highlighter);
   free_arena(&p.arena, p.arena.size);
}

SyntaxHighlighter
create_syntax_highlighter()
{
   SyntaxHighlighter hl = {};

   hl.parser = ts_parser_new();

   const TSLanguage *lang = tree_sitter_cpp();
   ts_parser_set_language(hl.parser, lang);

   const char *query_string =
      "(comment) @comment "
      "(identifier) @variable";
    
   U32 error_offset;
   TSQueryError error_type;
   hl.query = ts_query_new(lang, query_string, U32(strlen(query_string)), &error_offset, &error_type);
   
   if (!hl.query) {
      log_fatal("failed to create highlighting query");
   }

   return hl;
}

void
destroy_syntax_highlighter(SyntaxHighlighter hl)
{
   ts_query_delete(hl.query);
   ts_tree_delete(hl.tree);
   ts_parser_delete(hl.parser);
}

void
update_syntax_highlighting(Pane *p, Arena *a)
{
   SyntaxHighlighter *hl = &p->highlighter;

   String8 src = str8_from_gap_buffer(&p->buffer, a);

   if (hl->tree) {
      TSTree *new_tree = ts_parser_parse_string(hl->parser, hl->tree, (const char *) src.ptr, (U32) src.len);

      ts_tree_delete(hl->tree);
      hl->tree = new_tree;
   } else {
      hl->tree = ts_parser_parse_string(hl->parser, 0, (const char *) src.ptr, (U32) src.len);
   }
}

void
pane_cursor_back(Pane *p)
{
   p->cursor = cursor_back_normal(&p->buffer, p->cursor);
   pane_reset_col_store(p);
}

void
pane_cursor_next(Pane *p)
{
   p->cursor = cursor_next_normal(&p->buffer, p->cursor);
   pane_reset_col_store(p);
}

void
pane_set_cursor(Pane *p, U64 crs)
{
   p->cursor = crs;
   pane_reset_col_store(p);

   update_scroll(p);
}

void
pane_reset_col_store(Pane *p)
{
   p->cursor_store = -1;
}

void
update_scroll(Pane *pane)
{
   U32 cursor_line = 0;
   U64 cursor_col = 0;

   for (U64 i = 0; i < pane->cursor; ++i) {
      if (pane->buffer[i] == '\n') {
         cursor_line++;
         cursor_col = 0;
      } else {
         cursor_col++;
      }
   }

   if (cursor_line < pane->scroll_offset) {
      pane->scroll_offset = cursor_line;
   } else if (cursor_line >= pane->scroll_offset + pane->rows) {
      pane->scroll_offset = cursor_line - pane->rows + 1;
   }

   U32 total_lines = 1;
   for (U64 i = 0; i < pane->buffer.len; ++i) {
      if (pane->buffer[i] == '\n') {
         total_lines++;
      }
   }

   U32 max_scroll = (total_lines > pane->rows) ? (total_lines - pane->rows) : 0;

   pane->scroll_offset = MIN(pane->scroll_offset, max_scroll);
}

U64
cursor_back(GapBuffer *buf, U64 crs)
{
   if (crs > 0) {
      crs--;
   }

   return crs;
}

U64
cursor_next(GapBuffer *buf, U64 crs)
{
   if (crs < buf->len) {
      crs++;
   }

   return crs;
}

U64
cursor_back_normal(GapBuffer *buf, U64 crs)
{
   if (crs > 0) {
      if ((*buf)[crs - 1] != '\n') {
         crs--;
      }
   }

   return crs;
}

U64
cursor_next_normal(GapBuffer *buf, U64 crs)
{
   if (crs < buf->len) {
      if ((*buf)[crs + 1] != '\n') {
         crs++;
      }
   }

   return crs;
}

U64
cursor_line_begin(GapBuffer *buf, U64 crs)
{
   crs = cursor_back(buf, crs);

   while (crs > 0) {
      U8 ch = (*buf)[crs];
      if (ch == '\n') {
         return cursor_next(buf, crs);
      }

      crs = cursor_back(buf, crs);
   }

   return 0;
}

U64
cursor_line_end(GapBuffer *buf, U64 crs)
{
   while (crs < buf->len) {
      U8 ch = (*buf)[crs];
      if (ch == '\n') {
         return crs;
      }

      crs = cursor_next(buf, crs);
   }

   return buf->len;
}

U64
cursor_next_line_begin(GapBuffer *buf, U64 crs)
{
   return cursor_next(buf, cursor_line_end(buf, crs));
}

U64
cursor_prev_line_begin(GapBuffer *buf, U64 crs)
{
   return cursor_line_begin(buf, cursor_back(buf, cursor_line_begin(buf, crs)));
}

U64
cursor_next_line_end(GapBuffer *buf, U64 crs)
{
   return cursor_line_end(buf, cursor_next_line_begin(buf, crs));
}

U64
cursor_prev_line_end(GapBuffer *buf, U64 crs)
{
   return cursor_back(buf, cursor_line_begin(buf, crs));
}

U64
cursor_column(GapBuffer *buf, U64 crs)
{
   return crs - cursor_line_begin(buf, crs);
}

intern U64
cursor_skip_whitespace(U64 crs, GapBuffer *buf)
{
   U8 c = (*buf)[crs];

   crs = cursor_next(buf, crs);

   c = (*buf)[crs];

   while (is_whitespace(c) && crs < buf->len) {
      crs = cursor_next(buf, crs);

      c = (*buf)[crs];
   }

   return crs;
}

intern U64
cursor_skip_whitespace_reverse(U64 crs, GapBuffer *buf)
{
   U8 c = (*buf)[crs];

   crs = cursor_back(buf, crs);

   c = (*buf)[crs];

   while (is_whitespace(c) && crs > 0) {
      crs = cursor_back(buf, crs);

      c = (*buf)[crs];
   }

   return crs;
}

U64
cursor_prev_word(GapBuffer *buf, U64 crs)
{
   U8 c = (*buf)[crs];
   if (is_whitespace(c)) {
      crs = cursor_skip_whitespace_reverse(crs, buf);
   } else {
      int start_type = char_type(c);

      crs = cursor_back(buf, crs);
      c   = (*buf)[crs];

      if (is_whitespace(c)) {
         crs        = cursor_skip_whitespace_reverse(crs, buf);
         c          = (*buf)[crs];
         start_type = char_type(c);
      }

      if (char_type(c) == start_type) {
         while (char_type(c) == start_type && crs > 0) {
            crs = cursor_back(buf, crs);

            c = (*buf)[crs];
         }

         if (crs != 0) {
            crs = cursor_next(buf, crs);
         }
      }
   }

   return crs;
}

U64
cursor_end_of_word(GapBuffer *buf, U64 crs)
{
   U8 c = (*buf)[crs];

   if (is_whitespace(c)) {
      crs = cursor_skip_whitespace(crs, buf);
   } else {
      crs = cursor_next(buf, crs);

      int start_type = char_type(c);

      c = (*buf)[crs];

      if (is_whitespace(c)) {
         crs = cursor_skip_whitespace(crs, buf);

         c          = (*buf)[crs];
         start_type = char_type(c);
      }

      if (char_type(c) == start_type) {
         while (char_type(c) == start_type && crs < buf->len) {
            crs = cursor_next(buf, crs);

            c = (*buf)[crs];
         }

         crs = cursor_back(buf, crs);
      }
   }

   return crs;
}

U64
cursor_next_word(GapBuffer *buf, U64 crs)
{
   U8 c = (*buf)[crs];

   if (is_whitespace(c)) {
      crs = cursor_skip_whitespace(crs, buf);
   } else {
      int start_type = char_type(c);

      crs = cursor_next(buf, crs);

      c = (*buf)[crs];

      if (start_type != char_type(c)) {
         return crs;
      }

      if (is_whitespace(c)) {
         crs = cursor_skip_whitespace(crs, buf);
      } else {
         crs = cursor_back(buf, crs);
         crs = cursor_end_of_word(buf, crs);
         crs = cursor_next(buf, crs);

         c = (*buf)[crs];

         if (is_whitespace(c)) {
            crs = cursor_skip_whitespace(crs, buf);
         }
      }
   }

   return crs;
}

U64
cursor_paragraph_up(GapBuffer *buf, U64 crs)
{
   crs = cursor_back(buf, crs);

   while (crs > 0) {
      if (crs > 1) {
         if ((*buf)[crs] == '\n' && (*buf)[crs - 1] == '\n') {
            return crs;
         }
      }

      crs = cursor_back(buf, crs);
   }

   return crs;
}

U64
cursor_paragraph_down(GapBuffer *buf, U64 crs)
{
   crs = cursor_next(buf, crs);

   while (crs < buf->len) {
      if (crs < buf->len - 1) {
         if ((*buf)[crs] == '\n' && (*buf)[crs + 1] == '\n') {
               return crs + 1;
         }
      }

      crs = cursor_next(buf, crs);
   }

   return crs;
}

U32
line_indent(GapBuffer *buf, U64 crs)
{
   U32 result = 0;

   U64 start = cursor_line_begin(buf, crs);
   for (U64 i = start; i < crs; ++i) {
      U8 c = (*buf)[i];
      if (c == '\t') {
         result += TAB_SIZE;
      } else if (c == ' ') {
         result++;
      } else {
         break;
      }
   }

   return result;
}

U32
brace_matching_indentation(GapBuffer *buf, U64 crs)
{
   crs = cursor_back(buf, crs);
   crs = cursor_back(buf, crs);

   U32 score = 1;

   while (crs > 0) {
      U8 c = (*buf)[crs];

      if (c == '}') {
         score++;
      }

      if (c == '{') {
         score--;

         if (score == 0) {
            return line_indent(buf, crs);
         }
      }

      crs = cursor_back(buf, crs);
   }

   return 0;
}