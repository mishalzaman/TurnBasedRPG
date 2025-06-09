#pragma once

#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <iostream>
#include <cassert>

namespace WGPU::System {
    class Surface
    {
    public:
        static WGPUSurface Register(
            WGPUInstance instance,
            GLFWwindow* window
        ) {
            return glfwGetWGPUSurface(instance, window);
        };

        static void Configure(
            int screenWidth,
            int screenHeight,
            WGPUAdapter adapter,
            WGPUDevice device,
            WGPUSurface surface
        ) {
            // Retrieve surface capabilities
            WGPUSurfaceCapabilities capabilities;
            wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);

            // Use the first format as the preferred format
            WGPUTextureFormat surfaceFormat = capabilities.formats[0];

            WGPUSurfaceConfiguration config = {};
            config.nextInChain = nullptr;
            config.width = screenWidth;
            config.height = screenHeight;
            config.usage = WGPUTextureUsage_RenderAttachment;
            config.format = surfaceFormat;
            config.viewFormatCount = 0;
            config.viewFormats = nullptr;
            config.device = device;
            config.presentMode = WGPUPresentMode_Fifo;
            config.alphaMode = WGPUCompositeAlphaMode_Auto;

            wgpuSurfaceConfigure(surface, &config);
        };

        static WGPUTextureFormat GetSurfaceTextureFormat(WGPUSurface surface, WGPUAdapter adapter) {
            WGPUSurfaceCapabilities capabilities;
            wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);

			return capabilities.formats[0];
        }

		static WGPUSurfaceTexture RefreshTexture(WGPUSurface surface, WGPUSurfaceTexture Surfacetexture) {
            // Release the previous surface texture if it exists
            if (Surfacetexture.texture) {
                wgpuTextureRelease(Surfacetexture.texture);
            }

            // Get the surface texture
            wgpuSurfaceGetCurrentTexture(surface, &Surfacetexture);
            if (Surfacetexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
                std::cerr << "RefreshTexture: Could not get the current surface texture\n" << std::endl;
            }

            return Surfacetexture;
		};

        static WGPUTextureView GetView(WGPUSurfaceTexture Surfacetexture) {
            if (!Surfacetexture.texture) {
                std::cerr << "GetView: Texture does not exist\n" << std::endl;
                return nullptr;
            }

            // Create a view for this surface texture
            WGPUTextureViewDescriptor viewDescriptor = {};
            viewDescriptor.nextInChain = nullptr;
            viewDescriptor.label = "Surface texture view";
            viewDescriptor.format = wgpuTextureGetFormat(Surfacetexture.texture);
            viewDescriptor.dimension = WGPUTextureViewDimension_2D;
            viewDescriptor.baseMipLevel = 0;
            viewDescriptor.mipLevelCount = 1;
            viewDescriptor.baseArrayLayer = 0;
            viewDescriptor.arrayLayerCount = 1;
            viewDescriptor.aspect = WGPUTextureAspect_All;

            return wgpuTextureCreateView(Surfacetexture.texture, &viewDescriptor);
        };
    };
}
