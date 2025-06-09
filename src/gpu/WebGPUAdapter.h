#pragma once
#include <webgpu/webgpu.h>
#include <memory>
#include <string>

class WebGPUAdapter {
public:
    WebGPUAdapter(WGPUInstance instance, const WGPURequestAdapterOptions* options);
    ~WebGPUAdapter();

    // Prevent copying
    WebGPUAdapter(const WebGPUAdapter&) = delete;
    WebGPUAdapter& operator=(const WebGPUAdapter&) = delete;

    // Allow moving
    WebGPUAdapter(WebGPUAdapter&&) noexcept;
    WebGPUAdapter& operator=(WebGPUAdapter&&) noexcept;

    void printLimits() const;
    void printProperties() const;
    WGPUAdapter get() const { return adapter_; }

private:
    WGPUAdapter adapter_;
};