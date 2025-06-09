#include "SurfaceRecycler.h"

void WGPU::Renderer::SurfaceRecycler::RefreshTexture(WGPUSurface surface)
{
    // Release the previous surface texture if it exists
    if (surfaceTexture_.texture) {
        wgpuTextureRelease(surfaceTexture_.texture);
    }

    // Get the surface texture
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture_);
    if (surfaceTexture_.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
        std::cerr << "RefreshTexture: Could not get the current surface texture\n" << std::endl;
    }
}
