#pragma once
#include <webgpu/webgpu.h>
#include <memory>
#include <string>

#include "WebGPUAdapter.h"

class WebGPUDevice {
public:
    WebGPUDevice(const WebGPUAdapter& adapter, const WGPUDeviceDescriptor* descriptor);
    ~WebGPUDevice();

    // Prevent copying
    WebGPUDevice(const WebGPUDevice&) = delete;
    WebGPUDevice& operator=(const WebGPUDevice&) = delete;

    // Allow moving
    WebGPUDevice(WebGPUDevice&&) noexcept;
    WebGPUDevice& operator=(WebGPUDevice&&) noexcept;

    void printFeatures() const;
    void printLimits() const;
    WGPUDevice get() const { return device_; }

private:
    WGPUDevice device_;
};