#pragma once

#include <vector>

#include "vertexBuffer.h"


namespace rk {
	class AttributesObject {
	public:
		struct AttributesInfo {
			u32 location;
			u32 binding;
			Formats format;
			u32 offset;
		};
		struct BufferInfo {
			VertexBuffer buffer;
			u32 binding;
			u32 stride;
			VertexInputRate inputRate;
		};

		VertexBuffer& addVertexBuffer(u32 stride, VertexInputRate inputRate, i32* bufferIndex) {
			m_currentBinding = m_buffers.size();

			if (bufferIndex)
				*bufferIndex = m_currentBinding;

			BufferInfo bufferInfo{};
			bufferInfo.stride = stride;
			bufferInfo.binding = m_currentBinding;
			bufferInfo.inputRate = inputRate;

			m_buffers.push_back(bufferInfo);
			
			return m_buffers[m_currentBinding].buffer;
		}

		void setAttributes(u32 location, Formats format, u32 offset) {
			m_attributes.emplace_back(location, m_currentBinding, format, offset);
		}

		void bind(VkCommandBuffer command) {
			for (const auto& buffer : m_buffers)
				buffer.buffer.bind(command, buffer.binding);
		}

		void update(i32 bufferIndex, u32 size, const void* data) {
			m_buffers[bufferIndex].buffer.update(size, data);
		}

		const std::vector<AttributesInfo>& getAttributesInfo() const { return m_attributes; }
		const std::vector<BufferInfo>& getBuffersInfo() const { return m_buffers; }

	private:
		std::vector<AttributesInfo> m_attributes;
		std::vector<BufferInfo> m_buffers;

		u32 m_currentBinding = 0;
	};
}
