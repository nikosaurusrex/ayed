#include "buffer.h"

enum
{
   MAX_GAP_SIZE = 16
};

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

U64 insert_char(GapBuffer *buf, U8 c, U64 pos)
{
   ASSERT(pos <= buf->len && buf->len < buf->cap - MAX_GAP_SIZE);

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
   ASSERT(pos <= buf->len && buf->len + s.len < buf->cap - MAX_GAP_SIZE);

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
   // TODO:
   return pos;
}

U64
delete_char(GapBuffer *buf, U64 pos)
{
   ASSERT(pos < buf->len);
   
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
   ASSERT(pos + n < buf->len);

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
   s.ptr = push_array(a, U8, buf->len);
   s.len = buf->len;

   for (U64 i = 0; i < MIN(buf->start, buf->len); ++i) {
      s[i] = buf->ptr[i];
   }

   U64 gap_size = buf->end - buf->start;
   for (U64 i = buf->end; i < buf->len + gap_size; ++i) {
      s[i - gap_size] = buf->ptr[i];
   }

   return s;
}

U64
line_length(GapBuffer *buf, U64 crs)
{
   return cursor_line_end(buf, crs) - cursor_line_begin(buf, crs);
}

void
pane_cursor_back(Pane *p)
{
   p->cursor = cursor_back(&p->buffer, p->cursor);
   pane_reset_col_store(p);
}

void
pane_cursor_next(Pane *p)
{
   p->cursor = cursor_next(&p->buffer, p->cursor);
   pane_reset_col_store(p);
}

void
pane_set_cursor(Pane *p, U64 crs)
{
   p->cursor = crs;
   pane_reset_col_store(p);
}

void
pane_reset_col_store(Pane *p)
{
   p->cursor_store = -1;
}

/*
void
pane_update_scrolling(Pane *p, U64Offsets coff, float font_height)
{
    uint visible_lines = (uint)ceilf(p->h / font_height) - 1;

    if (coff.line < p->sy) {
        p->sy -= p->sy - coff.line;

        CLAMP_BOT(p->sy, 0);
    } else if (coff.ry >= visible_lines) {
        p->sy += (coff.ry - visible_lines + 1);
    }

    p->rsy = lerp(p->rsy, p->sy, 0.3);
}

intern U64Offsets
cursor_offsets(Pane *p, U64 crs)
{
    u32 line = 0;
    u32 col  = 0;
    u32 ridx = 0;

    for (u64 i = 0; i < crs; ++i) {
        if (p->buffer.data[i] == '\n') {
            line++;
            col = 0;
        } else {
            col++;
        }
    }

    u32 ry = 0;
    if (line > p->sy) {
        ry = line - p->sy;
    }
    return (U64Offsets){.rx = col, .ry = ry, .line = line};
}*/

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

U64
line_indent(GapBuffer *buf, U64 crs)
{
   U32 result = 0;

   U64 start = cursor_line_begin(buf, crs);
   for (U64 i = start; i < crs; ++i) {
      if (is_whitespace((*buf)[i])) {
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