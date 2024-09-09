#pragma once

#include "math.h"

#include <arm_neon.h>

union Vec4
{
   struct {
      float x;
      float y;
      float z;
      float w;
   };

   struct {
      float r;
      float g;
      float b;
      float a;
   };

   struct {
      Vec3 xyz;
      float _unused0;
   };

   struct {
      float _unused1;
      Vec3 yzw;
   };

   float32x4_t m;
};

union Vec4i
{
   struct {
      int x;
      int y;
      int z;
      int w;
   };

   int32x4_t m;
};

intern inline Vec4
vec4(float v)
{
   return Vec4{.m = {v, v, v, v}};
}

intern inline Vec4
vec4(Vec3 v, float w)
{
   return Vec4{.m = {v.x, v.y, v.z, w}};
}

intern inline Vec4
vec4(float x, float y, float z, float w)
{
   return Vec4{.m = {x, y, z, w}};
}

intern inline Vec4
operator+(Vec4 a, Vec4 b)
{
   return Vec4{.m = vaddq_f32(a.m, b.m)};
}

intern inline Vec4
operator-(Vec4 a, Vec4 b)
{
   return Vec4{.m = vsubq_f32(a.m, b.m)};
}

intern inline Vec4
operator*(Vec4 a, float s)
{
   return Vec4{.m = vmulq_n_f32(a.m, s)};
}

intern inline Vec4
operator*(Vec4 a, Vec4 b)
{
   return Vec4{.m = vmulq_f32(a.m, b.m)};
}

intern inline Vec4
operator/(Vec4 a, float s)
{
   return Vec4{.m = vmulq_n_f32(a.m, 1.0f / s)};
}

intern inline float
dot(Vec4 a, Vec4 b)
{
   float32x4_t prod = vmulq_f32(a.m, b.m);
   return vaddvq_f32(prod);
}

intern inline float
length_squared(Vec4 v)
{
   return dot(v, v);
}

intern inline float
length(Vec4 v)
{
   return sqrtf(length_squared(v));
}

intern inline Vec4
normalize(Vec4 v)
{
   float len = length(v);
   return Vec4{.m = vmulq_n_f32(v.m, 1.0f / len)};
}

intern inline Vec4
vmin(Vec4 a, Vec4 b)
{
   return Vec4{.m = vminq_f32(a.m, b.m)};
}

intern inline Vec4
vmax(Vec4 a, Vec4 b)
{
   return Vec4{.m = vmaxq_f32(a.m, b.m)};
}

intern inline Vec4i
vec4i(int v)
{
   return Vec4i{.m = {v, v, v, v}};
}

intern inline Vec4i
vec4i(Vec4i v, int w)
{
   return Vec4i{.m = {v.x, v.y, v.z, w}};
}

intern inline Vec4i
vec4i(int x, int y, int z, int w)
{
   return Vec4i{.m = {x, y, z, w}};
}

intern inline Vec4i
operator+(Vec4i a, Vec4i b)
{
   return Vec4i{.m = vaddq_s32(a.m, b.m)};
}

intern inline Vec4i
operator-(Vec4i a, Vec4i b)
{
   return Vec4i{.m = vsubq_s32(a.m, b.m)};
}

intern inline Vec4i
operator*(Vec4i a, int s)
{
   return Vec4i{.m = vmulq_n_s32(a.m, s)};
}

intern inline Vec4i
operator*(Vec4i a, Vec4i b)
{
   return Vec4i{.m = vmulq_s32(a.m, b.m)};
}

intern inline Vec4i
vmin(Vec4i a, Vec4i b)
{
   return Vec4i{.m = vminq_s32(a.m, b.m)};
}

intern inline Vec4i
vmax(Vec4i a, Vec4i b)
{
   return Vec4i{.m = vmaxq_s32(a.m, b.m)};
}

intern inline Vec4
rgba_from_hex(U32 hex)
{
   Vec4 rgba = {};

   rgba.a = ((hex >> 24) & 0xFF) / 255.0f;
   rgba.r = ((hex >> 16) & 0xFF) / 255.0f;
   rgba.g = ((hex >> 8) & 0xFF) / 255.0f;
   rgba.b = (hex & 0xFF) / 255.0f;

   return rgba;
}

intern inline U32
hex_from_rgba(Vec4 rgba)
{
   U32 r = rgba.r * 255.0f;
   U32 g = rgba.g * 255.0f;
   U32 b = rgba.b * 255.0f;
   U32 a = rgba.a * 255.0f;

   return (a << 24) | (r << 16) | (g << 8) | b;
}
