#pragma once

#include <string>
using String = std::string;

#include <vector>
template<typename T>
using List = std::vector<T>;

#include <unordered_map>
template<typename K, typename V>
using Map = std::unordered_map<K, V>;

#include <array>
template<typename T, size_t Size>
using Array = std::array<T, Size>;

#pragma region Nummeral types
#include <cstdint>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;
#pragma endregion

#pragma region Vectors
template<typename T>
struct V2
{
    T x, y;

    T& operator[]( const u64& idx )
    {
        return ( ! idx % 2 ) ? x : y;
    }

    bool operator==( const V2<T>& other )
    {
        return ( other.x == x && other.y == y );
    }
};

using V2i = V2<s32>;
using V2u = V2<u32>;
using V2f = V2<f32>;

#define Exp_V2( v ) v.x, v.y

template<typename T>
struct Extent2
{
    T width, height;
};

using Extent2i = Extent2<s32>;
using Extent2u = Extent2<u32>;
using Extent2f = Extent2<f32>;

#define Exp_Extent2( e ) e.width, e.height

template<typename T>
struct Extent3
{
    T width, height, depth;
};

using Extent3i = Extent3<s32>;
using Extent3u = Extent3<u32>;
using Extent3f = Extent3<f32>;

#define Exp_Extent3( e ) e.width, e.height, e.depth
#pragma endregion

#pragma region Colors
struct Color
{
    f32 r, g, b, a;
};

#define Exp_Color( c ) c.r, c.g, c.b, c.a

#define Exp_Color_u32( c ) (u32)(c.r * 255u), (u32)(c.g * 255u), (u32)(c.b * 255u), (u32)(c.a * 255u)
#pragma endregion

#pragma region Structs
template<typename T>
struct Rect
{
    T left, right, up, down;
};
#pragma endregion