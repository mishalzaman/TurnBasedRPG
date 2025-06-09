#pragma once

#include <webgpu/webgpu.h>

namespace WGPU::Renderer {
	class Quad2DRenderPass {
	public:
		static void Present(
			WGPUTextureView targetView,
			WGPUSurface surface,
			WGPUDevice device,
			WGPUQueue queue,
			WGPURenderPipeline pipeline,
			WGPUBuffer vertexBuffer,
			WGPUBuffer indexBuffer,
			uint32_t indexCount,
			WGPUBindGroup bindGroup
		) {

			// Create a command encoder for the draw call
			WGPUCommandEncoderDescriptor encoderDesc = {};
			encoderDesc.nextInChain = nullptr;
			encoderDesc.label = "My command encoder";
			WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

			// Create the render pass that clears the screen with our color
			WGPURenderPassDescriptor renderPassDesc = {};
			renderPassDesc.nextInChain = nullptr;

			// The attachment part of the render pass descriptor describes the target texture of the pass
			WGPURenderPassColorAttachment renderPassColorAttachment = {};
			renderPassColorAttachment.view = targetView;
			renderPassColorAttachment.resolveTarget = nullptr;
			renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
			renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
			renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };
			renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

			renderPassDesc.colorAttachmentCount = 1;
			renderPassDesc.colorAttachments = &renderPassColorAttachment;
			renderPassDesc.depthStencilAttachment = nullptr;
			renderPassDesc.timestampWrites = nullptr;

			// Create the render pass and end it immediately (we only clear the screen but do not draw anything)
			WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
			// Select which render pipeline to use
			wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);

			// Set vertex buffer while encoding the render pass
			wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, wgpuBufferGetSize(vertexBuffer));

			// The second argument must correspond to the choice of uint16_t or uint32_t
			// we've done when creating the index buffer.
			wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(indexBuffer));

			wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);

			wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);

			// Replace `draw()` with `drawIndexed()` and `vertexCount` with `indexCount`
			// The extra argument is an offset within the index buffer.
			wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);

			// Draw 1 instance of a 3-vertices shape
			//wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);
			wgpuRenderPassEncoderEnd(renderPass);
			wgpuRenderPassEncoderRelease(renderPass);

			// Finally encode and submit the render pass
			WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
			cmdBufferDescriptor.nextInChain = nullptr;
			cmdBufferDescriptor.label = "Command buffer";
			WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
			wgpuCommandEncoderRelease(encoder);

			wgpuQueueSubmit(queue, 1, &command);
			wgpuCommandBufferRelease(command);

			wgpuSurfacePresent(surface);

			wgpuDeviceTick(device);
		};
	private:
	};
}