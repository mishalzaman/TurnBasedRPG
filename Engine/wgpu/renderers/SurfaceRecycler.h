#pragma once

#include <webgpu/webgpu.h>
#include <iostream>

namespace WGPU::Renderer {
	class SurfaceRecycler {
	public:
		SurfaceRecycler(WGPUSurfaceTexture surfaceTexture) : surfaceTexture_(surfaceTexture) {};
		~SurfaceRecycler() {
			if (surfaceTexture_.texture) {
				wgpuTextureRelease(surfaceTexture_.texture);
			}
		};

		WGPUSurfaceTexture GetSurfaceTexture() { return surfaceTexture_; };

		void RefreshTexture(WGPUSurface surface);

	private:
		WGPUSurfaceTexture surfaceTexture_;
	};
}