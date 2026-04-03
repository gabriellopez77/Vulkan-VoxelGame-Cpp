#pragma once

#include "Defs.h"


namespace math {
    static f32 radians(f32 degrees) { return degrees * 0.01745329251994329576923690768489; }

    static f32 min(f32 a, f32 b) { return a < b ? a : b; }
    static i32 min(i32 a, i32 b) { return a < b ? a : b; }
    static u32 min(u32 a, u32 b) { return a < b ? a : b; }

    static f32 max(f32 a, f32 b) { return a > b ? a : b; }
    static i32 max(i32 a, i32 b) { return a > b ? a : b; }
    static u32 max(u32 a, u32 b) { return a > b ? a : b; }

    static f32 clamp(f32 value, f32 minV, f32 maxV) { return min(max(value, minV), maxV); }
    static i32 clamp(i32 value, i32 minV, i32 maxV) { return min(max(value, minV), maxV); }
    static u32 clamp(u32 value, u32 minV, u32 maxV) { return min(max(value, minV), maxV); }


    static f32 lerp(f32 a, f32 b, f32 t) { return a + (b - a) * t; }
}
