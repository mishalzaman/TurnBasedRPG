#pragma once

#include <webgpu/webgpu.h>
#include <iostream>
#include <cassert>

namespace WGPU::System {
    class Queue
    {
    public:
        static WGPUQueue Register(WGPUDevice device) {
            return wgpuDeviceGetQueue(device);
        };
    };
}
