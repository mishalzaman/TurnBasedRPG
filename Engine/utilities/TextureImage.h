#pragma once

#include <webgpu/webgpu.h>
#include <core/Core.h>
#include <iostream>
#include <vector>

namespace Utilities {
	class TextureImage {
	public:
		TextureImage(const char* path);
		~TextureImage();

		TextureImage(TextureImage&& other) noexcept;
		TextureImage& operator=(TextureImage&& other) noexcept;

		WGPUTexture GetTexture() const { return texture_; }
		WGPUTextureView GetView() const { return view_; }
		WGPUSampler GetSampler() const { return sampler_; }
	private:
		WGPUTexture texture_;
		WGPUTextureView view_;
		WGPUSampler sampler_;

		// Descriptors
		WGPUTextureDescriptor textureDesc_{};
		WGPUTextureViewDescriptor textureViewDesc_{};
		WGPUSamplerDescriptor samplerDesc_{};

		WGPUTexture loadTexture(const char* path);
		bool setView();
		bool setSampler();
	};
}