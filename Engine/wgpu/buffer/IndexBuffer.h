#pragma once

#include <webgpu/webgpu.h>
#include <vector>
#include <iostream>

namespace WGPU::Buffer {
	class IndexBuffer {
	public:
		IndexBuffer(
			std::vector<uint16_t> indices,
			unsigned int indexCount,
			WGPUDevice device,
			WGPUQueue queue
		);
		~IndexBuffer();

		WGPUBuffer GetBuffer() const { return indexBuffer_; }
		int GetIndexCount() const { return indexCount_; }
	private:
		WGPUBuffer indexBuffer_;
		int indexCount_;
	};
}