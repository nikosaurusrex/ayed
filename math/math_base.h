#pragma once

#include "math.h"

#include "base/base.h"

union Vec2
{
   struct {
      float x;
      float y;
   };
   struct {
      float u;
      float v;
   };
};

union Vec3
{
   struct {
      float x;
      float y;
      float z;
   };
   struct {
      float r;
      float g;
      float b;
   };
   struct {
      float _unused0;
      Vec2 yz;
   };
   struct {
      Vec2 xy;
      float _unused1;
   };
};

union Vec2i
{
   struct {
      int x;
      int y;
   };
};

union Vec3i
{
   struct {
      int x;
      int y;
      int z;
   };
   struct {
      int _unused0;
      Vec2 yz;
   };
   struct {
      Vec2 xy;
      int _unused1;
   };
};

intern inline Vec2
vec2(float v)
{
   return Vec2{v, v};
}

intern inline Vec2
vec2(float x, float y)
{
   return Vec2{x, y};
}

intern inline Vec2
operator+(Vec2 a, Vec2 b)
{
   return Vec2{a.x + b.x, a.y + b.y};
}

intern inline Vec2
operator-(Vec2 a, Vec2 b)
{
   return Vec2{a.x - b.x, a.y - b.y};
}

intern inline Vec2
operator*(Vec2 a, float s)
{
   return Vec2{a.x * s, a.y * s};
}

intern inline Vec2
operator*(Vec2 a, Vec2 b)
{
   return Vec2{a.x * b.x, a.y * b.y};
}

intern inline Vec2
operator/(Vec2 a, float s)
{
   return Vec2{a.x / s, a.y / s};
}

intern inline float
dot(Vec2 a, Vec2 b)
{
   return a.x * b.x + a.y * b.y;
}

intern inline float
length_squared(Vec2 v)
{
   return dot(v, v);
}

intern inline float
length(Vec2 v)
{
   return sqrtf(length_squared(v));
}

intern inline Vec2
normalize(Vec2 v)
{
   return v / length(v);
}

intern inline Vec3
vec3(float v)
{
   return Vec3{v, v, v};
}

intern inline Vec3
vec3(float x, float y, float z)
{
   return Vec3{x, y, z};
}

intern inline Vec3
operator+(Vec3 a, Vec3 b)
{
   return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

intern inline Vec3
operator-(Vec3 a, Vec3 b)
{
   return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

intern inline Vec3
operator*(Vec3 a, float s)
{
   return Vec3{a.x * s, a.y * s, a.z * s};
}

intern inline Vec3
operator*(Vec3 a, Vec3 b)
{
   return Vec3{a.x * b.x, a.y * b.y, a.z * b.z};
}

intern inline Vec3
operator/(Vec3 a, float s)
{
   return Vec3{a.x / s, a.y / s, a.z / s};
}

intern inline float
dot(Vec3 a, Vec3 b)
{
   return a.x * b.x + a.y * b.y + a.z * b.z;
}

intern inline Vec3
cross(Vec3 a, Vec3 b)
{
   return Vec3{
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x
   };
}

intern inline float
length_squared(Vec3 v)
{
   return dot(v, v);
}

intern inline float
length(Vec3 v)
{
   return sqrtf(length_squared(v));
}

intern inline Vec3
normalize(Vec3 v)
{
   return v / length(v);
}

intern inline Vec2i
vec2i(int v)
{
   return Vec2i{v, v};
}

intern inline Vec2i
vec2i(int x, int y)
{
   return Vec2i{x, y};
}

intern inline Vec2i
operator+(Vec2i a, Vec2i b)
{
   return Vec2i{a.x + b.x, a.y + b.y};
}

intern inline Vec2i
operator-(Vec2i a, Vec2i b)
{
   return Vec2i{a.x - b.x, a.y - b.y};
}

intern inline Vec2i
operator*(Vec2i a, int s)
{
   return Vec2i{a.x * s, a.y * s};
}

intern inline Vec2i
operator*(Vec2i a, Vec2i b)
{
   return Vec2i{a.x * b.x, a.y * b.y};
}

intern inline int
dot(Vec2i a, Vec2i b)
{
   return a.x * b.x + a.y * b.y;
}

intern inline int
length_squared(Vec2i v)
{
   return dot(v, v);
}

intern inline Vec3i
vec3i(int v)
{
   return Vec3i{v, v, v};
}

intern inline Vec3i
vec3i(int x, int y, int z)
{
   return Vec3i{x, y, z};
}

intern inline Vec3i
operator+(Vec3i a, Vec3i b)
{
   return Vec3i{a.x + b.x, a.y + b.y, a.z + b.z};
}

intern inline Vec3i
operator-(Vec3i a, Vec3i b)
{
   return Vec3i{a.x - b.x, a.y - b.y, a.z - b.z};
}

intern inline Vec3i
operator*(Vec3i a, int s)
{
   return Vec3i{a.x * s, a.y * s, a.z * s};
}

intern inline Vec3i
operator*(Vec3i a, Vec3i b)
{
   return Vec3i{a.x * b.x, a.y * b.y, a.z * b.z};
}

intern inline int
dot(Vec3i a, Vec3i b)
{
   return a.x * b.x + a.y * b.y + a.z * b.z;
}

intern inline Vec3i
cross(Vec3i a, Vec3i b)
{
   return Vec3i{
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x
   };
}

intern inline int
length_squared(Vec3i v)
{
   return dot(v, v);
}

intern inline U32
hex_from_rgb(Vec3 rgb)
{
   U32 r = U32(rgb.r * 255.0f);
   U32 g = U32(rgb.g * 255.0f);
   U32 b = U32(rgb.b * 255.0f);

   return (r << 16) | (g << 8) | b;
}

intern inline Vec3
rgb_from_hex(U32 hex)
{
   Vec3 rgb = {};

   rgb.r = ((hex >> 16) & 0xFF) / 255.0f;
   rgb.g = ((hex >> 8) & 0xFF) / 255.0f;
   rgb.b = (hex & 0xFF) / 255.0f;

   return rgb;
}
