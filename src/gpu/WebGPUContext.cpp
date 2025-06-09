#include "WebGPUContext.h"
#include <iostream>

bool WebGPUContext::initialize(const std::string& deviceLabel) {
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    instance_ = wgpuCreateInstance(&desc);

    if (!instance_) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return false;
    }

    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapter_ = std::make_unique<WebGPUAdapter>(instance_, &adapterOpts);

    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = deviceLabel.c_str();
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "Default Queue";
    deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void*) {
        std::cout << "Device lost: reason " << reason;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
        };

    device_ = std::make_unique<WebGPUDevice>(*adapter_, &deviceDesc);
    return true;
}
