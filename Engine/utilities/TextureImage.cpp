#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

#include "TextureImage.h"

Utilities::TextureImage::TextureImage(const char* path)
{
	// Load the texture
	texture_ = loadTexture(path);
    setView();
	setSampler();
}

Utilities::TextureImage::~TextureImage()
{
    if (texture_) {
        std::cout << "Utilities::TextureImage::~TextureImage() - Releasing texture." << std::endl;
        wgpuTextureRelease(texture_);
    }
    if (view_) {
        wgpuTextureViewRelease(view_);
        view_ = nullptr;
    }
    if (sampler_) {
        wgpuSamplerRelease(sampler_);
        sampler_ = nullptr;
    }
}

WGPUTexture Utilities::TextureImage::loadTexture(const char* path)
{
    int width, height, channels;
    unsigned char* pixelData = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (nullptr == pixelData) {
        std::cerr << "Failed to load texture from path: " << path << std::endl;
        return nullptr;
    }

    textureDesc_.nextInChain = nullptr;
    textureDesc_.dimension = WGPUTextureDimension_2D;
    textureDesc_.format = WGPUTextureFormat_RGBA8Unorm;
    textureDesc_.mipLevelCount = 1; // No mipmaps for simplicity
    textureDesc_.sampleCount = 1;
    textureDesc_.size = { (unsigned int)width, (unsigned int)height, 1 };
    textureDesc_.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;

    WGPUTexture texture = wgpuDeviceCreateTexture(Core::Device(), &textureDesc_);
    if (!texture) {
        std::cerr << "Failed to create texture." << std::endl;
        stbi_image_free(pixelData);
        return nullptr;
    }

    WGPUImageCopyTexture destination = {};
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTextureDataLayout source = {};
    source.offset = 0;
    source.bytesPerRow = 4 * textureDesc_.size.width;
    source.rowsPerImage = height;

    std::vector<unsigned char> alignedData(source.bytesPerRow * height, 0);
    for (int row = 0; row < height; ++row) {
        std::memcpy(&alignedData[row * source.bytesPerRow], &pixelData[row * width * 4], width * 4);
    }

    WGPUQueue queue = wgpuDeviceGetQueue(Core::Device());
    wgpuQueueWriteTexture(queue, &destination, alignedData.data(), alignedData.size(), &source, &textureDesc_.size);

    stbi_image_free(pixelData);

    return texture;
}

bool Utilities::TextureImage::setView()
{
    textureViewDesc_.nextInChain = nullptr;
    textureViewDesc_.aspect = WGPUTextureAspect_All;
    textureViewDesc_.baseArrayLayer = 0;
    textureViewDesc_.arrayLayerCount = 1;
    textureViewDesc_.baseMipLevel = 0;
    textureViewDesc_.mipLevelCount = textureDesc_.mipLevelCount;
    textureViewDesc_.dimension = WGPUTextureViewDimension_2D;
    textureViewDesc_.format = textureDesc_.format;

    view_ = wgpuTextureCreateView(texture_, &textureViewDesc_);

    if (!view_) {
		throw std::runtime_error("Failed to create texture view.");
	}
    else {
        return true;
    }
}

bool Utilities::TextureImage::setSampler()
{
    samplerDesc_.nextInChain = nullptr;
    samplerDesc_.addressModeU = WGPUAddressMode_ClampToEdge;
    samplerDesc_.addressModeV = WGPUAddressMode_ClampToEdge;
    samplerDesc_.addressModeW = WGPUAddressMode_ClampToEdge;
    samplerDesc_.magFilter = WGPUFilterMode_Nearest;
    samplerDesc_.minFilter = WGPUFilterMode_Nearest;
    samplerDesc_.mipmapFilter = WGPUMipmapFilterMode_Linear;
    samplerDesc_.lodMinClamp = 0.0f;
    samplerDesc_.lodMaxClamp = FLT_MAX; // Ensure the sampler can handle all LOD levels.
    samplerDesc_.maxAnisotropy = 1;     // Set max anisotropy to at least 1.

    sampler_ = wgpuDeviceCreateSampler(Core::Device(), &samplerDesc_);

	if (!sampler_) {
		throw std::runtime_error("Failed to create sampler.");
	}
	else {
		return true;
	}
}
