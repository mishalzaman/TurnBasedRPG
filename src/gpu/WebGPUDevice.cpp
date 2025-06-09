#include "WebGPUDevice.h"
#include <iostream>
#include <cassert>
#include <vector>

WebGPUDevice::WebGPUDevice(const WebGPUAdapter& adapter, const WGPUDeviceDescriptor* descriptor)
    : device_(nullptr) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device,
        char const* message, void* pUserData) {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
            if (status == WGPURequestDeviceStatus_Success) {
                userData.device = device;
            }
            else {
                std::cout << "Could not get WebGPU device: " << message << std::endl;
            }
            userData.requestEnded = true;
        };

    wgpuAdapterRequestDevice(adapter.get(), descriptor, onDeviceRequestEnded, (void*)&userData);
    assert(userData.requestEnded);
    device_ = userData.device;
}

WebGPUDevice::~WebGPUDevice() {
    if (device_) {
        wgpuDeviceRelease(device_);
    }
}

void WebGPUDevice::printFeatures() const {
    std::vector<WGPUFeatureName> features;
    size_t featureCount = wgpuDeviceEnumerateFeatures(device_, nullptr);
    features.resize(featureCount);
    wgpuDeviceEnumerateFeatures(device_, features.data());

    std::cout << "Device features:" << std::endl;
    for (auto f : features) {
        std::cout << " - 0x" << std::hex << f << std::dec << std::endl;
    }
}

void WebGPUDevice::printLimits() const
{
    
}
