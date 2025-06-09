#pragma once
#include <webgpu/webgpu.h>
#include <memory>
#include <string>

#include "WebGPUAdapter.h"
#include "WebGPUDevice.h"

class WebGPUContext {
public:
    static WebGPUContext& getInstance();

    // Initialize the WebGPU context
    bool initialize(const std::string& deviceLabel = "Default Device");

    // Accessors
    const WebGPUAdapter& getAdapter() const { return *adapter_; }
    const WebGPUDevice& getDevice() const { return *device_; }

    // Prevent copying and assignment
    WebGPUContext(const WebGPUContext&) = delete;
    WebGPUContext& operator=(const WebGPUContext&) = delete;

private:
    WebGPUContext() = default;  // Private constructor for singleton
    ~WebGPUContext() = default;

    WGPUInstance instance_ = nullptr;
    std::unique_ptr<WebGPUAdapter> adapter_;
    std::unique_ptr<WebGPUDevice> device_;
};