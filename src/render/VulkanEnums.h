#pragma once

#include "Defs.h"

#define ADD_OR_OPERATOR(Enum, Type) constexpr Enum operator|(Enum x, Enum y) {                                  \
                                        return static_cast<Enum>(static_cast<Type>(x) | static_cast<Type>(y));  \
                                    }                                                                           \


namespace rk {
    enum class Formats : i32 {
        RGBA8_SRGB = 43,
        BGRA8_SRGB = 50,

        R_F32 = 100,
        RG_F32 = 103,
        RGB_F32 = 106,
        RGBA_F32 = 109,

        RG_I32 = 102,
        RGB_I32 = 105,
        RGBA_I32 = 108,

        RG_U32 = 101,
        RGB_U32 = 104,
        RGBA_U32 = 107,

        R_U8 = 13,
        RG_U8 = 20,
        RGB_U8 = 27,
        RGBA_U8 = 41,

        DEPTH_F32 = 126
    };

    enum class ShaderStage : i32 {
        VERTEX = 0x00000001,
        FRAGMENT = 0x00000010,
        ALL = 0x7FFFFFFF,
    };

    enum class VertexInputRate : i32 {
        VERTEX = 0,
        INSTANCE = 1,
    };

    enum class DynamicState : i32 {
        VIEWPORT = 0,
        SCISSOR = 1,
    };

    enum class MemoryType : u32 {
        DEVICE_LOCAL = 1,
        HOST_VISIBLE = 2,
        HOST_COHERENT = 4,
    };

    enum class BufferUsage : u32 {
        TRANSFER_SRC = 1,
        TRANSFER_DST = 2,
        UNIFORM_BUFFER = 16,
        VERTEX_BUFFER = 128,
        INDEX_BUFFER = 64,
    };

    enum class ImageUsage : i32 {
        TRANSFER_DST = 2,
        SAMPLED = 4,
        DEPTH_STENCIL_ATTACHMENT = 32
    };

    enum class ImageLayout : i32 {
        UNDEFINED = 0,
        SHADER_READ_ONLY_OPTIMAL = 5,
        TRANSFER_DST_OPTIMAL = 7,
    };

    enum class SamplerFilter : i32 {
        NEAREST = 0,
        LINEAR = 1
    };

    enum class SamplerMode : i32 {
        REPEAT = 0,
        CLAMP_TO_EDGE = 2,
        CLAMP_TO_BORDER = 3,
    };

    enum class CullMode : u32 {
        DISABLE = 0,
        FRONT = 1,
        BACK = 2,
        BOTH = 3
    };

    ADD_OR_OPERATOR(ShaderStage, i32)
    ADD_OR_OPERATOR(MemoryType, u32)
    ADD_OR_OPERATOR(BufferUsage, u32)
    ADD_OR_OPERATOR(ImageUsage, i32)
}