#pragma once

#include <webgpu/webgpu.h>
#include <iostream>
#include <cassert>

namespace WGPU::System {
	class Adapter
	{
    public:
		static WGPUAdapter Register(WGPUInstance instance, const WGPURequestAdapterOptions* options) {
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

			return userData.adapter;
		};
	};
}



