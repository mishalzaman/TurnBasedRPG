#pragma once

#include <webgpu/webgpu.h>
#include <iostream>
#include <cassert>

namespace WGPU::System {
    class Device
    {
    public:
        static WGPUDevice Register(const WGPUAdapter& adapter, const WGPUDeviceDescriptor* descriptor) {
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

            wgpuAdapterRequestDevice(adapter, descriptor, onDeviceRequestEnded, (void*)&userData);
            assert(userData.requestEnded);

            // Set uncaptured error callback
            wgpuDeviceSetUncapturedErrorCallback(userData.device, [](WGPUErrorType type, const char* message, void* /*userData*/) {
                const char* errorType = "";
                switch (type) {
                case WGPUErrorType_Validation: errorType = "Validation"; break;
                case WGPUErrorType_OutOfMemory: errorType = "OutOfMemory"; break;
                case WGPUErrorType_Unknown: errorType = "Unknown"; break;
                case WGPUErrorType_DeviceLost: errorType = "DeviceLost"; break;
                default: errorType = "UnknownErrorType"; break;
                }
                std::cerr << "WebGPU Error [" << errorType << "]: " << message << std::endl;
                }, nullptr);

            return userData.device;
        };
    };
}



