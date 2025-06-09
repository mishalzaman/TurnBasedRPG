#include "Core.h"

/*============================================================
* INITIALIZE CORE
=============================================================*/

bool Core::Activate()
{
	try {
		WGPUInstanceDescriptor desc = {};
		instance_ = wgpuCreateInstance(&desc);

		if (!instance_) {
			std::cerr << "Could not initialize WebGPU!" << std::endl;
			return false;
		}

		WGPURequestAdapterOptions adapterOpts = {};
		adapter_ = WGPU::System::Adapter::Register(instance_, &adapterOpts);
		if (!adapter_) {
			throw std::runtime_error("Failed to create WGPU adapter");
		}

		WGPUDeviceDescriptor deviceDesc = {};
		device_ = WGPU::System::Device::Register(adapter_, &deviceDesc);
		if (!device_) {
			throw std::runtime_error("Failed to create WGPU device");
		}

		queue_ = WGPU::System::Queue::Register(device_);
		if (!queue_) {
			throw std::runtime_error("Failed to create WGPU queue");
		}

		std::cout << "Core activated successfully!" << std::endl;

		initialized_ = true;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Activation failed: " << e.what() << std::endl;
		Cleanup();
		return false;
	}
}


/*============================================================
* CLEANUP
=============================================================*/

void Core::Cleanup()
{
	// Adapter
	if (adapter_) {
		std::cout << "Core::~Core - Releasing adapter..." << std::endl;
		wgpuAdapterRelease(adapter_);
	}

	// Device
	if (device_) {
		std::cout << "Core::~Core - Releasing device..." << std::endl;
		wgpuDeviceRelease(device_);
	}


	// Queue
	if (queue_) {
		std::cout << "Core::~Core - Releasing queue..." << std::endl;
		wgpuQueueRelease(queue_);
	}

	// Instance
	if (instance_) {
		std::cout << "Core::~Core - Releasing instance..." << std::endl;
		wgpuInstanceRelease(instance_);
		instance_ = nullptr;
	}
}

Core::~Core()
{
	std::cout << "Starting Core cleanup..." << std::endl;
	Cleanup();
}
