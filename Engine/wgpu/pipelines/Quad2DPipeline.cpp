#include "Quad2DPipeline.h"

WGPU::Pipeline::Quad2DPipeline::Quad2DPipeline(
	WGPUBuffer uniformBuffer,
	size_t bufferSize,
	WGPUTextureView textureView,
	WGPUSampler sampler
)
{
	pipelineDesc_.nextInChain = nullptr;

	createBindingLayoutDefaults(bufferSize); // Setup default binding layout
	createShaderModule(); // Load and create the shader module
	createVertexPipeline(); // Configure the vertex pipeline
	createFragmentPipeline(); // Configure the fragment pipeline
	createBindGroupLayout(); // Create the bind group layout
	createBindGroup(uniformBuffer, bufferSize, textureView, sampler); // Create the bind group
	createPipelineLayout(); // Create the pipeline layout
	createRenderPipeline(); // Create the render pipeline

	wgpuShaderModuleRelease(shaderModule_); // Release the shader module after pipeline creation
}

WGPU::Pipeline::Quad2DPipeline::~Quad2DPipeline()
{
	std::cout << "Releasing Quad2DPipeline..." << std::endl;
	if (pipeline_) {
		wgpuRenderPipelineRelease(pipeline_);
		pipeline_ = nullptr;
	}
	if (layout_) {
		wgpuPipelineLayoutRelease(layout_);
		layout_ = nullptr;
	}
	if (bindGroupLayout_) {
		wgpuBindGroupLayoutRelease(bindGroupLayout_);
		bindGroupLayout_ = nullptr;
	}
	if (bindGroup_) {
		wgpuBindGroupRelease(bindGroup_);
		bindGroup_ = nullptr;
	}
	if (shaderModule_) {
		wgpuShaderModuleRelease(shaderModule_);
		shaderModule_ = nullptr;
	}
}

/**
 * Sets up the default binding layout for the pipeline.
 * This defines the bindings for a uniform buffer, texture, and sampler.
 *
 * @param bufferSize The size of the uniform buffer to bind.
 */
void WGPU::Pipeline::Quad2DPipeline::createBindingLayoutDefaults(size_t bufferSize)
{
	// Uniform Buffer
	bindingLayout_[0].buffer.nextInChain = nullptr;
	bindingLayout_[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayout_[0].buffer.minBindingSize = bufferSize;
	bindingLayout_[0].buffer.hasDynamicOffset = false;
	bindingLayout_[0].binding = 0; // Matches `@binding(0)` in the shader
	bindingLayout_[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;

	// Texture
	bindingLayout_[1].texture.nextInChain = nullptr;
	bindingLayout_[1].texture.multisampled = false;
	bindingLayout_[1].texture.sampleType = WGPUTextureSampleType_Float; // Float type for texture_2d<f32>
	bindingLayout_[1].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayout_[1].binding = 1; // Matches `@binding(1)` in the shader
	bindingLayout_[1].visibility = WGPUShaderStage_Fragment;

	// Sampler
	bindingLayout_[2].sampler.nextInChain = nullptr;
	bindingLayout_[2].sampler.type = WGPUSamplerBindingType_Filtering; // Filtering for smooth sampling
	bindingLayout_[2].binding = 2; // Matches `@binding(2)` in the shader
	bindingLayout_[2].visibility = WGPUShaderStage_Fragment;

	// Update the total number of entries in the binding layout descriptor
	bindGroupLayoutDesc_.entryCount = 3;
	bindGroupLayoutDesc_.entries = bindingLayout_;
}

/**
 * Loads and creates the shader module used for the pipeline.
 */
void WGPU::Pipeline::Quad2DPipeline::createShaderModule()
{
	WGPUShaderModuleDescriptor shaderDesc{};

	// Use the extension mechanism to specify the WGSL shader source
	WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
	shaderCodeDesc.chain.next = nullptr;
	shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
	shaderDesc.nextInChain = &shaderCodeDesc.chain;
	shaderCodeDesc.code = shaderSource_;

	shaderModule_ = wgpuDeviceCreateShaderModule(Core::Device(), &shaderDesc);
}

/**
 * Configures the vertex pipeline.
 * Defines the vertex attributes and layouts for the pipeline.
 */
void WGPU::Pipeline::Quad2DPipeline::createVertexPipeline()
{
	// Position attribute
	attributes_[0].shaderLocation = 0;
	attributes_[0].format = WGPUVertexFormat_Float32x2;
	attributes_[0].offset = 0;

	// UV attribute
	attributes_[1].shaderLocation = 1;
	attributes_[1].format = WGPUVertexFormat_Float32x2;
	attributes_[1].offset = sizeof(float) * 2;

	// Vertex buffer layout
	vertexBufferLayout_.attributeCount = 2;
	vertexBufferLayout_.attributes = attributes_;
	vertexBufferLayout_.arrayStride = sizeof(float) * 4; // 2 floats for position + 2 floats for UV
	vertexBufferLayout_.stepMode = WGPUVertexStepMode_Vertex;

	// Set up the pipeline descriptor
	pipelineDesc_.vertex.bufferCount = 1;
	pipelineDesc_.vertex.buffers = &vertexBufferLayout_;
	pipelineDesc_.vertex.module = shaderModule_;
	pipelineDesc_.vertex.entryPoint = "vs_main";
	pipelineDesc_.vertex.constantCount = 0;
	pipelineDesc_.vertex.constants = nullptr;

	// Primitive state
	pipelineDesc_.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	pipelineDesc_.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
	pipelineDesc_.primitive.frontFace = WGPUFrontFace_CCW;
	pipelineDesc_.primitive.cullMode = WGPUCullMode_None;
}

/**
 * Configures the fragment pipeline.
 * Defines blending states and color output targets.
 */
void WGPU::Pipeline::Quad2DPipeline::createFragmentPipeline()
{
	fragmentState_.module = shaderModule_;
	fragmentState_.entryPoint = "fs_main";
	fragmentState_.constantCount = 0;
	fragmentState_.constants = nullptr;

	blendState_.color.srcFactor = WGPUBlendFactor_SrcAlpha;
	blendState_.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
	blendState_.color.operation = WGPUBlendOperation_Add;
	blendState_.alpha.srcFactor = WGPUBlendFactor_Zero;
	blendState_.alpha.dstFactor = WGPUBlendFactor_One;
	blendState_.alpha.operation = WGPUBlendOperation_Add;

	colorTarget_.format = Surface::Format();
	colorTarget_.blend = &blendState_;
	colorTarget_.writeMask = WGPUColorWriteMask_All;

	fragmentState_.targetCount = 1;
	fragmentState_.targets = &colorTarget_;
	pipelineDesc_.fragment = &fragmentState_;
}

/**
 * Creates the bind group layout for the pipeline.
 * This layout describes the resources available to shaders.
 */
void WGPU::Pipeline::Quad2DPipeline::createBindGroupLayout()
{
	bindGroupLayoutDesc_.nextInChain = nullptr;
	bindGroupLayoutDesc_.entryCount = 3; // Uniform buffer, texture, sampler
	bindGroupLayoutDesc_.entries = bindingLayout_;
	bindGroupLayout_ = wgpuDeviceCreateBindGroupLayout(Core::Device(), &bindGroupLayoutDesc_);
}

/**
 * Creates the bind group for the pipeline.
 * This binds resources like buffers, textures, and samplers.
 *
 * @param uniformBuffer The uniform buffer to bind.
 * @param bufferSize The size of the uniform buffer.
 * @param textureView The texture view to bind.
 * @param sampler The sampler to bind.
 */
void WGPU::Pipeline::Quad2DPipeline::createBindGroup(WGPUBuffer uniformBuffer, size_t bufferSize, WGPUTextureView textureView, WGPUSampler sampler)
{
	// Uniform buffer
	bindings_[0].nextInChain = nullptr;
	bindings_[0].binding = 0;
	bindings_[0].buffer = uniformBuffer;
	bindings_[0].offset = 0;
	bindings_[0].size = bufferSize;

	// Texture
	bindings_[1].nextInChain = nullptr;
	bindings_[1].binding = 1;
	bindings_[1].textureView = textureView;

	// Sampler
	bindings_[2].nextInChain = nullptr;
	bindings_[2].binding = 2;
	bindings_[2].sampler = sampler;

	// Bind group descriptor
	bindGroupDesc_.nextInChain = nullptr;
	bindGroupDesc_.layout = bindGroupLayout_;
	bindGroupDesc_.entryCount = 3;
	bindGroupDesc_.entries = bindings_;
	bindGroup_ = wgpuDeviceCreateBindGroup(Core::Device(), &bindGroupDesc_);
}

/**
 * Creates the pipeline layout for the pipeline.
 * The layout includes the bind group layout.
 */
void WGPU::Pipeline::Quad2DPipeline::createPipelineLayout()
{
	layoutDesc_.nextInChain = nullptr;
	layoutDesc_.bindGroupLayoutCount = 1;
	layoutDesc_.bindGroupLayouts = &bindGroupLayout_;
	layout_ = wgpuDeviceCreatePipelineLayout(Core::Device(), &layoutDesc_);
}

/**
 * Creates the render pipeline.
 * This finalizes all the configurations into a usable pipeline.
 */
void WGPU::Pipeline::Quad2DPipeline::createRenderPipeline()
{
	pipelineDesc_.depthStencil = nullptr; // No depth/stencil testing
	pipelineDesc_.multisample.count = 1;
	pipelineDesc_.multisample.mask = ~0u;
	pipelineDesc_.multisample.alphaToCoverageEnabled = false;
	pipelineDesc_.layout = layout_;
	pipeline_ = wgpuDeviceCreateRenderPipeline(Core::Device(), &pipelineDesc_);
}
