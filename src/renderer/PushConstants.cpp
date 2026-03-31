#include "PushConstants.h"

void rk::PushConstants::create(u32 size, ShaderStage stage) {
    m_pushConstantInfo.offset = 0;
    m_pushConstantInfo.size = size;
    m_pushConstantInfo.stageFlags = (i32)stage;
}
