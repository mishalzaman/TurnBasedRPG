#include "SurfaceHandler.h"

WGPU::System::SurfaceHandler::SurfaceHandler(
	int screenWidth,
	int screenHeight,
	GLFWwindow* window,
	WGPUInstance instance,
	WGPUAdapter adapter,
	WGPUDevice device
) : screenWidth_(screenWidth), screenHeight_(screenHeight)
{
	surface_ = glfwGetWGPUSurface(instance, window);
	configure(screenWidth, screenHeight, adapter, device);
}

WGPU::System::SurfaceHandler::~SurfaceHandler()
{
    if (surfaceTexture_.texture) {
        wgpuTextureRelease(surfaceTexture_.texture);
    }
    if (surfaceTextureView_) {
        wgpuTextureViewRelease(surfaceTextureView_);
    }
}

WGPUSurfaceTexture WGPU::System::SurfaceHandler::GetSurfaceTexture()
{
    return surfaceTexture_;
}

WGPUTextureView WGPU::System::SurfaceHandler::CreateTextureAndGetSurfaceTextureView() {
    if (surfaceTextureView_) {
        wgpuTextureViewRelease(surfaceTextureView_);
        surfaceTextureView_ = nullptr;
    }
    if (surfaceTexture_.texture) {
        wgpuTextureRelease(surfaceTexture_.texture);
        surfaceTexture_.texture = nullptr;
    }

    // Get the surface texture
    wgpuSurfaceGetCurrentTexture(surface_, &surfaceTexture_);
    if (surfaceTexture_.status != WGPUSurfaceGetCurrentTextureStatus_Success || !surfaceTexture_.texture) {
        return nullptr;
    }

    // Create a view for this surface texture
    WGPUTextureViewDescriptor viewDescriptor = {};
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = "Surface texture view";
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture_.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = WGPUTextureAspect_All;

    surfaceTextureView_ = wgpuTextureCreateView(surfaceTexture_.texture, &viewDescriptor);
    return surfaceTextureView_;
}


void WGPU::System::SurfaceHandler::ReleaseTextureAndView() {
    if (surfaceTextureView_) {
        wgpuTextureViewRelease(surfaceTextureView_);
        surfaceTextureView_ = nullptr;
    }
    if (surfaceTexture_.texture) {
        wgpuTextureRelease(surfaceTexture_.texture);
        surfaceTexture_.texture = nullptr;
    }
}

int WGPU::System::SurfaceHandler::GetWidth()
{
    return screenWidth_;
}

int WGPU::System::SurfaceHandler::GetHeight()
{
    return screenHeight_;
}

void WGPU::System::SurfaceHandler::configure(int screenWidth, int screenHeight, WGPUAdapter adapter, WGPUDevice device)
{
    // Retrieve surface capabilities
    WGPUSurfaceCapabilities capabilities;
    wgpuSurfaceGetCapabilities(surface_, adapter, &capabilities);

    // Use the first format as the preferred format
    surfaceFormat_ = capabilities.formats[0];

    WGPUSurfaceConfiguration config = {};
    config.nextInChain = nullptr;
    config.width = screenWidth;
    config.height = screenHeight;
    config.usage = WGPUTextureUsage_RenderAttachment;
    if (capabilities.formatCount == 0) {
        throw std::runtime_error("No supported surface formats found.");
    }
    surfaceFormat_ = capabilities.formats[0];
    config.format = surfaceFormat_;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.device = device;
    config.presentMode = WGPUPresentMode_Fifo;
    config.alphaMode = WGPUCompositeAlphaMode_Auto;

    wgpuSurfaceConfigure(surface_, &config);
}
