#pragma once

#include "wgpu/system/Adapter.h"
#include "wgpu/system/Device.h"
#include "wgpu/system/Queue.h"

#include <iostream>
#include <webgpu/webgpu.h>

class Core {
public:
    static Core& retrieveInstance() {
        static Core instance;
        return instance;
    }

	/*============================================================
	* INITIALIZE CORE
	=============================================================*/

    bool Activate();

    /*============================================================
     *STATIC ACCESSORS
    =============================================================*/

	static WGPUInstance Instance() noexcept { return retrieveInstance().instance_; }
    static WGPUAdapter Adapter() noexcept { return retrieveInstance().adapter_; }
    static WGPUDevice Device() noexcept { return retrieveInstance().device_; }
    static WGPUQueue Queue() noexcept { return retrieveInstance().queue_; }

    /*============================================================
    * CLEANUP
    =============================================================*/

	void Cleanup();

    // Rule of 5
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;
    Core(Core&&) = delete;
    Core& operator=(Core&&) = delete;
private:
    Core() = default;
    ~Core();

	bool initialized_ = false;

    WGPUInstance instance_ = nullptr;
    WGPUAdapter adapter_ = nullptr;
	WGPUDevice device_ = nullptr;
	WGPUQueue queue_ = nullptr;

	bool hasInitialized() const noexcept { return initialized_; }
};