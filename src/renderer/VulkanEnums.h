#pragma once

#include "Defs.h"

#define ADD_OR_OPERATOR(Enum, Type) constexpr Enum operator|(Enum x, Enum y) {                                  \
                                        return static_cast<Enum>(static_cast<Type>(x) | static_cast<Type>(y));  \
                                    }                                                                           \


namespace rk {
    enum class VecFormats : i32 {
        RG32_F32 = 103,
        RGB32_F32 = 106,
        RGBA32_F32 = 109,

        RG32_I32 = 102,
        RGB32_I32 = 105,
        RGBA32_I32 = 108,

        RG32_U32 = 101,
        RGB32_U32 = 104,
        RGBA32_U32 = 107,
    };

    enum class ShaderStage : i32 {
        VERTEX = 0x00000001,
        FRAGMENT = 0x00000010,
        ALL = 0x7FFFFFFF,
    };
    ADD_OR_OPERATOR(ShaderStage, i32)

    enum class VertexInputRate : i32 {
        VERTEX = 0,
        INSTANCE = 1,
    };

    enum class DynamicStates : i32 {
        VIEWPORT = 0,
        SCISSOR = 1,
    };

    enum class MemoryType : u32 {
        DEVICE_LOCAL = 1,
        HOST_VISIBLE = 2,
        HOST_COHERENT = 4,
    };
    ADD_OR_OPERATOR(MemoryType, u32)

    enum class BufferUsage : u32 {
        TRANSFER_SRC = 1,
        TRANSFER_DST = 2,
        UNIFORM_BUFFER = 16,
        VERTEX_BUFFER = 128,
        INDEX_BUFFER = 64,
    };
    ADD_OR_OPERATOR(BufferUsage, u32)
}