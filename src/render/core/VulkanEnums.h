#pragma once

#include "Defs.h"

#define ADD_OR_OPERATOR(Enum, Type) constexpr Enum operator|(Enum x, Enum y) {                                  \
                                        return static_cast<Enum>(static_cast<Type>(x) | static_cast<Type>(y));  \
                                    }                                                                           \


namespace rk {
    enum class Formats : i32 {
        Rgba8Srgb = 43,
        Bgra8Srgb = 50,

        RF32 = 100,
        RgF32 = 103,
        RgbF32 = 106,
        RgbaF32 = 109,

        RgI32 = 102,
        RgbI32 = 105,
        RgbaI32 = 108,

        RgU32 = 101,
        RgbU32 = 104,
        RgbaU32 = 107,

        RU8 = 13,
        RgU8 = 20,
        RgbU8 = 27,
        RgbaU8 = 41,

        DepthF32 = 126,
    };

    enum class ShaderStage : i32 {
        Vertex = 0x00000001,
        Fragment = 0x00000010,
        All = 0x7FFFFFFF,
    };

    enum class VertexInputRate : i32 {
        Vertex = 0,
        Instance = 1,
    };

    enum class DynamicState : i32 {
        Viewport = 0,
        Scissor = 1
    };

    enum class MemoryType : u32 {
        DeviceLocal = 1,
        HostVisible = 2,
        HostCoherent = 4
    };

    enum class BufferUsage : u32 {
        TransderSrc = 1,
        TransferDst = 2,
        UniformBuffer = 16,
        VertexBuffer = 128,
        IndexBuffer = 64,
    };

    enum class ImageUsage : i32 {
        TransferDst = 2,
        Sampled = 4,
        DepthStencilAttachment = 32,
    };

    enum class ImageLayout : i32 {
        Undefined = 0,
        ShaderReadOnlyOptimal = 5,
        TransferDstOptimal = 7,
    };

    enum class SamplerFilter : i32 {
        Nearest = 0,
        Linear = 1
    };

    enum class SamplerMode : i32 {
        Repeat = 0,
        ClampToEdge = 2,
        ClampToBorder= 3
    };

    enum class CullMode : u32 {
        Disable = 0,
        Front = 1,
        Back = 2,
        Both = 3
    };

    ADD_OR_OPERATOR(ShaderStage, i32)
    ADD_OR_OPERATOR(MemoryType, u32)
    ADD_OR_OPERATOR(BufferUsage, u32)
    ADD_OR_OPERATOR(ImageUsage, i32)
}