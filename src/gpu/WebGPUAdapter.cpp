#include "WebGPUAdapter.h"
#include <cassert>
#include <iostream>

WebGPUAdapter::WebGPUAdapter(WGPUInstance instance, const WGPURequestAdapterOptions* options)
    : adapter_(nullptr) {
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
        char const* message, void* pUserData) {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
            if (status == WGPURequestAdapterStatus_Success) {
                userData.adapter = adapter;
            }
            else {
                std::cout << "Could not get WebGPU adapter: " << message << std::endl;
            }
            userData.requestEnded = true;
        };

    wgpuInstanceRequestAdapter(instance, options, onAdapterRequestEnded, (void*)&userData);
    assert(userData.requestEnded);
    adapter_ = userData.adapter;
}

WebGPUAdapter::~WebGPUAdapter() {
    if (adapter_) {
        wgpuAdapterRelease(adapter_);
    }
}

WebGPUAdapter::WebGPUAdapter(WebGPUAdapter&&) noexcept
{
}

void WebGPUAdapter::printLimits() const {
    WGPUSupportedLimits supportedLimits = {};
    supportedLimits.nextInChain = nullptr;

    bool success = wgpuAdapterGetLimits(adapter_, &supportedLimits);
    if (success) {
        std::cout << "Adapter limits:" << std::endl
            << " - maxTextureDimension1D: " << supportedLimits.limits.maxTextureDimension1D << std::endl
            << " - maxTextureDimension2D: " << supportedLimits.limits.maxTextureDimension2D << std::endl
            << " - maxTextureDimension3D: " << supportedLimits.limits.maxTextureDimension3D << std::endl
            << " - maxTextureArrayLayers: " << supportedLimits.limits.maxTextureArrayLayers << std::endl;
    }
}

void WebGPUAdapter::printProperties() const
{
	WGPUAdapterProperties properties = {};
	properties.nextInChain = nullptr;
	wgpuAdapterGetProperties(adapter_, &properties);
	std::cout << "Adapter properties:" << std::endl
		<< " - vendorID: " << properties.vendorID << std::endl;
	if (properties.vendorName) {
		std::cout << " - vendorName: " << properties.vendorName << std::endl;
	}
	if (properties.architecture) {
		std::cout << " - architecture: " << properties.architecture << std::endl;
	}
}
