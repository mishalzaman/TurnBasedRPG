#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

#include "CONFIG.h"
#include <core/Core.h>
#include <wgpu/buffer/UniformBuffers.h>
#include <utilities/Quad.h>
#include <wgpu/buffer/VertexBuffer.h>
#include <wgpu/buffer/IndexBuffer.h>
#include <wgpu/pipelines/Quad2DPipeline.h>
#include <wgpu/system/Surface.h>
#include <wgpu/renderers/Quad2DRenderPass.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <core/Surface.h>
#include <core/Window.h>
#include <utilities/TextureImage.h>

int main(int, char**) {
	if (!Core::retrieveInstance().Activate()) {
		std::cerr << "Failed to initialize Core." << std::endl;
		return -1;
	}
	if (!Window::retrieveInstance().Activate(CONFIG::TITLE, CONFIG::WINDOW_SCREEN_WIDTH, CONFIG::WINDOW_SCREEN_HEIGHT)) {
		std::cerr << "Failed to initialize Window." << std::endl;
		return -1;
	}
	if (!Surface::retrieveInstance().Activate(
		CONFIG::NATIVE_SCREEN_WIDTH,
		CONFIG::NATIVE_SCREEN_HEIGHT,
		Window::Get(),
		Core::Instance(),
		Core::Adapter(),
		Core::Device()
	)) {
		std::cerr << "Failed to initialize Surface." << std::endl;
		return -1;
	}

	// texture
	auto texture = std::make_unique<Utilities::TextureImage>("../assets/test.png");
	if (!texture) {
		std::cerr << "Failed to load texture." << std::endl;
		return -1;
	}

	// uniform buffer
	auto ub = std::make_unique<WGPU::Buffer::UniformBuffer>();
	ub->Add("time", 1.0f);
	ub->Add("position", glm::vec2(150.0, 150.0));
	ub->Add("size", glm::vec2(200.0, 200.0));
	ub->Add("Projection", glm::ortho(
		0.0f,                                              // left
		static_cast<float>(CONFIG::NATIVE_SCREEN_WIDTH),   // right
		static_cast<float>(CONFIG::NATIVE_SCREEN_HEIGHT),  // bottom (now height)
		0.0f,                                              // top (now 0)
		-1.0f,                                             // near
		1.0f                                               // far
	));

	ub->Write();

	// vertex buffer
	Utilities::QuadStruct quad = Utilities::Quad().CreateCentered();
	auto vb = std::make_unique<WGPU::Buffer::VertexBuffer>(quad.Vertices, quad.VertexCount, Core::Device(), Core::Queue());
	auto ib = std::make_unique<WGPU::Buffer::IndexBuffer>(quad.Indices, quad.IndexCount, Core::Device(), Core::Queue());

	// pipeline
	auto pipeline = std::make_unique<WGPU::Pipeline::Quad2DPipeline>(
		ub->Get(),
		ub->GetCurrentBufferSize(),
		texture->GetView(),
		texture->GetSampler()
	);

	while (!glfwWindowShouldClose(Window::Get())) {
		glfwPollEvents();

		float t = static_cast<float>(glfwGetTime());
		ub->Update("time", t);
		ub->Write();

		// surface
		WGPUTextureView view = Surface::View();

		if (!view) {
			std::cerr << "Could not get the surface texture view" << std::endl;
		}

		WGPU::Renderer::Quad2DRenderPass::Present(
			view,
			Surface::Get(),
			Core::Device(),
			Core::Queue(),
			pipeline->GetPipeline(),
			vb->GetBuffer(),
			ib->GetBuffer(),
			ib->GetIndexCount(),
			pipeline->GetBindGroup()
		);
	}

	return 0;
}
