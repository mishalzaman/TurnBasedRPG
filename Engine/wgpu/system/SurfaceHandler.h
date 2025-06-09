#pragma once

#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <iostream>

namespace WGPU::System {
	class SurfaceHandler {
	public:
		SurfaceHandler(
			int screenWidth,
			int screenHeight,
			GLFWwindow* window,
			WGPUInstance instance,
			WGPUAdapter adapter,
			WGPUDevice device
		);
		~SurfaceHandler();

		WGPUSurface GetSurface() const noexcept { return surface_; };
		WGPUTextureFormat GetSurfaceTextureFormat() const noexcept { return surfaceFormat_; };
		WGPUSurfaceTexture GetSurfaceTexture();
		WGPUTextureView CreateTextureAndGetSurfaceTextureView();
		void ReleaseTextureAndView();
		int GetWidth();
		int GetHeight();
	private:
		WGPUSurface surface_;
		WGPUTextureFormat surfaceFormat_;
		WGPUSurfaceTexture surfaceTexture_ = {};
		WGPUTextureView surfaceTextureView_ = {};
		int screenWidth_;
		int screenHeight_;

		void configure(
			int screenWidth,
			int screenHeight,
			WGPUAdapter adapter,
			WGPUDevice device
		);
	};
}