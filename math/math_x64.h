#pragma once

#include "math.h"

#include <immintrin.h>

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

   __m128 m;
};

union Vec4i
{
   struct {
      int x;
      int y;
      int z;
      int w;
   };

   __m128i m;
};

intern inline Vec4
vec4(float v)
{
   return Vec4{.m = _mm_set1_ps(v)};
}

intern inline Vec4
vec4(Vec3 v, float w)
{
   return Vec4{.m = _mm_set_ps(v.x, v.y, v.z, w)};
}

intern inline Vec4
vec4(float x, float y, float z, float w)
{
   return Vec4{.m = _mm_set_ps(x, y, z, w)};
}

intern inline Vec4
operator+(Vec4 a, Vec4 b)
{
   return Vec4{.m = _mm_add_ps(a.m, b.m)};
}

intern inline Vec4
operator-(Vec4 a, Vec4 b)
{
   return Vec4{.m = _mm_sub_ps(a.m, b.m)};
}

intern inline Vec4
operator*(Vec4 a, float s)
{
   // TODO: FIX
   return Vec4{.m = _mm_mul_ps(a.m, _mm_set1_ps(s))};
}

intern inline Vec4
operator*(Vec4 a, Vec4 b)
{
   return Vec4{.m = _mm_mul_ps(a.m, b.m)};
}

intern inline Vec4
operator/(Vec4 a, float s)
{
   return Vec4{.m = _mm_div_ps(a.m, _mm_set1_ps(s))};
}

intern inline float
dot(Vec4 a, Vec4 b)
{
   __m128 prod = _mm_mul_ps(a.m, b.m);
   float *prod_ptr = (float *)&prod;
   return prod_ptr[0] + prod_ptr[1] + prod_ptr[2] + prod_ptr[3];
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
   return Vec4{.m = _mm_div_ps(v.m, _mm_set1_ps(len))};
}

intern inline Vec4
vmin(Vec4 a, Vec4 b)
{
   return Vec4{.m = _mm_min_ps(a.m, b.m)};
}

intern inline Vec4
vmax(Vec4 a, Vec4 b)
{
   return Vec4{.m = _mm_max_ps(a.m, b.m)};
}

intern inline Vec4i
vec4i(int v)
{
   return Vec4i{.m = _mm_set1_epi32(v)};
}

intern inline Vec4i
vec4i(Vec4i v, int w)
{
   return Vec4i{.m = _mm_set_epi32(v.x, v.y, v.z, w)};
}

intern inline Vec4i
vec4i(int x, int y, int z, int w)
{
   return Vec4i{.m = _mm_set_epi32(x, y, z, w)};
}

intern inline Vec4i
operator+(Vec4i a, Vec4i b)
{
   return Vec4i{.m = _mm_add_epi32(a.m, b.m)};
}

intern inline Vec4i
operator-(Vec4i a, Vec4i b)
{
   return Vec4i{.m = _mm_sub_epi32(a.m, b.m)};
}

intern inline Vec4i
operator*(Vec4i a, int s)
{
   return Vec4i{.m = _mm_mullo_epi32(a.m, _mm_set1_epi32(s))};
}

intern inline Vec4i
operator*(Vec4i a, Vec4i b)
{
   return Vec4i{.m = _mm_mullo_epi32(a.m, b.m)};
}

intern inline Vec4i
vmin(Vec4i a, Vec4i b)
{
   return Vec4i{.m = _mm_min_epi32(a.m, b.m)};
}

intern inline Vec4i
vmax(Vec4i a, Vec4i b)
{
   return Vec4i{.m = _mm_max_epi32(a.m, b.m)};
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
   U32 r = U32(rgba.r * 255.0f);
   U32 g = U32(rgba.g * 255.0f);
   U32 b = U32(rgba.b * 255.0f);
   U32 a = U32(rgba.a * 255.0f);

   return (a << 24) | (r << 16) | (g << 8) | b;
}
