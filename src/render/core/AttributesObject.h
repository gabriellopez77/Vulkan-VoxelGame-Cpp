#pragma once

#include <vector>

#include "Utils.h"


namespace rk {
	enum class UpdateType {
		OneTime,
		Sometimes,
		Often
	};

	class AttributesObject {
		struct BufferDataInfo {
			// if ONE_TIMES or OFTEN then it is nullptr
			// if SOMETIMES then it is staging buffer used to copy data to vram buffer
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingMemory;

			// if ONE_TIME or SOMETIMES then it is vram buffer
			// if OFTEN then it is host local buffer
			VkBuffer buffer;
			VkDeviceMemory memory;

			// if ONE_TIMES then it is nullptr
			// if SOMETIMES then it is staging buffer mapped memory
			// if OFTEN then it is buffer mapped memory
			void* mappedMemory;
		};

		struct BufferInfo {
			u32 size;
			UpdateType updateType;
			u32 binding;
			BufferDataInfo buffers[utl::FRAMES_COUNT];
		};

	public:
		struct AttributeDescription {
			u32 location;
			u32 binding;
			Formats format;
			u32 offset;
		};

		struct BindingDescription {
			u32 binding;
			u32 stride;
			VertexInputRate inputRate;
		};


		i32 createVertexBuffer(u32 size, u64 offset, const void* data, u32 stride, VertexInputRate inputRate, UpdateType updateType) {
			m_currentBinding = m_buffersInfo.size();

			BindingDescription description{};
			description.stride = stride;
			description.binding = m_currentBinding;
			description.inputRate = inputRate;

			m_bindingsDescription.push_back(description);

			BufferInfo bufferInfo{};
			bufferInfo.size = size;
			bufferInfo.updateType = updateType;
			bufferInfo.binding = m_currentBinding;

			for (int i = 0; i < utl::FRAMES_COUNT; i++) {
				createOrSendBuffer(data, i, bufferInfo, BufferUsage::VertexBuffer);

				// if buffer is one time then do not make buffer copies
				if (updateType == UpdateType::OneTime)
					break;
			}

			m_buffersInfo.push_back(bufferInfo);
			m_buffersOffsets.push_back(offset);

			return m_currentBinding;
		}

		void createIndexBuffer(u32 size, const void* data, UpdateType updateType) {
			m_indexBufferInfo.size = size;
			m_indexBufferInfo.updateType = updateType;

			for (int i = 0; i < utl::FRAMES_COUNT; i++) {
				createOrSendBuffer(data, i, m_indexBufferInfo, BufferUsage::IndexBuffer);

				// if buffer is one time then do not make buffer copies
				if (updateType == UpdateType::OneTime)
					break;
			}
		}

		void setAttributes(u32 location, Formats format, u32 offset) {
			m_attributes.emplace_back(location, m_currentBinding, format, offset);
		}

		void updateVertexBuffer(i32 bufferIndex, u32 size, const void* data) {
			updateBufferData(m_buffersInfo[bufferIndex], size, data);
		}

		void updateIndexBuffer(u32 size, const u32* data) {
			updateBufferData(m_indexBufferInfo, size, data);
		}

		void bind(VkCommandBuffer command);

		const std::vector<AttributeDescription>& getAttributesInfo() const { return m_attributes; }
		const std::vector<BindingDescription>& getBindingDescription() const { return m_bindingsDescription; }

	private:
		void updateBufferData(BufferInfo& bufferInfo, u32 size, const void* data);
		void createOrSendBuffer(const void* data, i32 index, BufferInfo& vertexBufferInfo, BufferUsage usage);

		// index buffer
		BufferInfo m_indexBufferInfo{};

		// stores all vertex buffers info that we create
		std::vector<BufferInfo> m_buffersInfo;

		std::vector<AttributeDescription> m_attributes;
		std::vector<BindingDescription> m_bindingsDescription;

		// buffers that we are bind with one vulkan bind funtion
		std::vector<VkBuffer> m_bindBuffers;
		std::vector<u64> m_buffersOffsets;

		u32 m_currentBinding = 0;
	};
}
