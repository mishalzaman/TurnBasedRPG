#pragma once

#include <webgpu/webgpu.h>
#include <iostream>

#include <core/Core.h>
#include <core/Surface.h>

namespace WGPU::Pipeline {
	class Quad2DPipeline {
	public:
		Quad2DPipeline(
            WGPUBuffer uniformBuffer,
            size_t bufferSize,
            WGPUTextureView textureView,
            WGPUSampler sampler
        );
		~Quad2DPipeline();

		WGPURenderPipeline GetPipeline() const { return pipeline_; }
		WGPUBindGroup GetBindGroup() const { return bindGroup_; }
	private:
        const char* shaderSource_ = R"(
            struct Uniforms {
                uTime: f32,                       // Offset: 0, Size: 4 bytes
                position: vec2<f32>,              // Offset: 80, Size: 8 bytes
                size: vec2<f32>,                  // Offset: 96, Size: 8 bytes
                Projection: mat4x4<f32>           // Offset: 112, Size: 64 bytes
            }

            @group(0) @binding(0) var<uniform> uniforms: Uniforms;
            @group(0) @binding(1) var myTexture: texture_2d<f32>;
            @group(0) @binding(2) var mySampler: sampler;

            struct VertexOutput {
                @builtin(position) position: vec4f,
                @location(0) uv: vec2f
            };

            @vertex
            fn vs_main(@location(0) in_vertex_position: vec2f, @location(1) in_uv: vec2f) -> VertexOutput {
                // Flip the Y-axis for top-left origin
                let position = vec2f(in_vertex_position.x, in_vertex_position.y);

                // Transform the quad position to match the top-left positioning and size
                let scaled_position = (position * uniforms.size) + uniforms.position;

                // Project the position using the uniform projection matrix
                let projected_position = uniforms.Projection * vec4f(scaled_position, 0.0, 1.0);

                var output: VertexOutput;
                output.position = projected_position;
                output.uv = in_uv;
                return output;
            }

            @fragment
            fn fs_main(@location(0) in_uv: vec2f) -> @location(0) vec4f {
                let tex_color = textureSample(myTexture, mySampler, in_uv);
                return tex_color;
            }
        )";

        // WebGPU resources
		WGPURenderPipeline pipeline_;
		WGPUPipelineLayout layout_;
		WGPUBindGroupLayout bindGroupLayout_;
		WGPUBindGroup bindGroup_;
		WGPUShaderModule shaderModule_;
        WGPUVertexBufferLayout vertexBufferLayout_;
        WGPUVertexAttribute positionAttrib_;
		WGPUVertexAttribute uvAttrib_;
        WGPUVertexAttribute attributes_[2];

        // WebGPU resources
        WGPUBindGroupLayoutEntry bindingLayout_[3]{};
        WGPURenderPipelineDescriptor pipelineDesc_{};
        WGPUFragmentState fragmentState_{};
        WGPUBlendState blendState_{};
        WGPUColorTargetState colorTarget_{};
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc_{};
        WGPUBindGroupEntry bindings_[3]{};
        WGPUBindGroupDescriptor bindGroupDesc_{};
        WGPUPipelineLayoutDescriptor layoutDesc_{};

		void createBindingLayoutDefaults(size_t bufferSize);
        void createShaderModule();
		void createVertexPipeline();
		void createFragmentPipeline();
		void createBindGroupLayout();
        void createBindGroup(WGPUBuffer uniformBuffer, size_t bufferSize, WGPUTextureView textureView, WGPUSampler sampler);
		void createPipelineLayout();
		void createRenderPipeline();
	};
}