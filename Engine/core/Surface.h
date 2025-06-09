#pragma once

#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <string>
#include <memory>

#include "wgpu/system/SurfaceHandler.h"

class Surface {
public:
    static Surface& retrieveInstance() {
        static Surface instance;
        return instance;
    }

    /*============================================================
    * INITIALIZE SURFACE
    =============================================================*/

    bool Activate(
        int baseResolutionWidth,
        int baseResolutionHeight,
        GLFWwindow* window,
        WGPUInstance instance,
        WGPUAdapter adapter,
        WGPUDevice device
    ) {
        surfaceHandler_ = std::make_unique<WGPU::System::SurfaceHandler>(
            baseResolutionWidth,
            baseResolutionHeight,
            window,
            instance,
            adapter,
            device
        );
        return surfaceHandler_ != nullptr;
    }

	/*============================================================
	* STATIC ACCESSORS
    =============================================================*/

    static WGPUSurface Get() {
        auto& instance = retrieveInstance();
        if (!instance.surfaceHandler_) {
            throw std::runtime_error("SurfaceHandler is not initialized.");
        }
        return instance.surfaceHandler_->GetSurface();
    }
    static WGPUTextureFormat Format() noexcept { return retrieveInstance().surfaceHandler_->GetSurfaceTextureFormat(); }
    static WGPUTextureView View() { return retrieveInstance().surfaceHandler_->CreateTextureAndGetSurfaceTextureView(); }
	static int Width() { return retrieveInstance().surfaceHandler_->GetWidth(); }
	static int Height() { return retrieveInstance().surfaceHandler_->GetHeight(); }

	// Rule of 5
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(Surface&&) = delete;
    
private:
    Surface() = default;
    ~Surface() {
        if (surfaceHandler_) {
            if (surfaceHandler_->GetSurface()) {
                std::cout << "Surface::~Surface - Releasing surface..." << std::endl;
                wgpuSurfaceUnconfigure(surfaceHandler_->GetSurface());
                wgpuSurfaceRelease(surfaceHandler_->GetSurface());
            }
            surfaceHandler_.reset();
        }
    }

    std::unique_ptr<WGPU::System::SurfaceHandler> surfaceHandler_;

};