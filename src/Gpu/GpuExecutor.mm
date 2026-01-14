#import <Metal/Metal.h>
#import <Foundation/Foundation.h>

#include "Model/GpuExecutor.h"

struct GpuExecutor::Impl {
    id<MTLDevice> device;
    id<MTLCommandQueue> queue;
    id<MTLComputePipelineState> pipeline;
};

GpuExecutor::GpuExecutor(const World& world) :
	m_world{world}
{
	@autoreleasepool {
		impl = new Impl{};

		InitialiseDevice();
		if (!impl->device) std::exit(1);

		InitialiseCommandQueue();
		if (!impl->queue) std::exit(1);

		CheckCommandBuffer();

		InitialisePipeline();
	}
}

GpuExecutor::~GpuExecutor() {
    delete impl;
}

void GpuExecutor::InitialiseDevice() {
	impl->device = MTLCreateSystemDefaultDevice();
	if (impl->device) return;

	NSArray<id<MTLDevice>> *devices = nil;
	if (!&MTLCopyAllDevices) {
		std::cerr << "Copy all devices not available.\n";
		std::exit(1);
	}

	devices = MTLCopyAllDevices();
	if (!devices || devices.count == 0) {
		std::cerr << "Copy returned no devices.\n";
		std::exit(1);
	}

	impl->device = devices[0];
}

void GpuExecutor::InitialiseCommandQueue() {
	impl->queue = [impl->device newCommandQueue];
	if (!impl->queue) {
		std::cerr << "Failed to create command queue\n";
		impl->device = nil;
		std::exit(1);
	}
}

void GpuExecutor::CheckCommandBuffer() {
	id<MTLCommandBuffer> cb = [impl->queue commandBuffer];
	[cb commit];
	[cb waitUntilCompleted];

	if (cb.status != MTLCommandBufferStatusCompleted) {
		std::cerr << "Command buffer failure.\n";
		std::exit(1);
	}
}

void GpuExecutor::InitialisePipeline()
{
    NSURL* url = [NSURL fileURLWithPath:@"raytracer.metallib"];
	NSError* error = nil;
	id<MTLLibrary> library = [impl->device newLibraryWithURL:url error:&error];
    if (!library) {
        std::cerr << "Failed to load metallib: " << [[error localizedDescription] UTF8String] << '\n';
        return;
    }

    id<MTLFunction> kernel = [library newFunctionWithName:@"TraceRay"];
    if (!kernel) {
        std::cerr << "Kernel 'TraceRay' not found.\n";
        return;
    }

    impl->pipeline = [impl->device newComputePipelineStateWithFunction:kernel error:&error];
    if (!impl->pipeline) {
        std::cerr << "Failed to create pipeline: " << [[error localizedDescription] UTF8String] << '\n';
        return;
    }
}

void GpuExecutor::TraceRays(uint32_t* pixels) {
	size_t count = NUM_PIXELS;
	size_t byteSize = count * sizeof(int);

	id<MTLBuffer> pixelBuffer = [
		impl->device newBufferWithBytesNoCopy:pixels
        length:byteSize
        options:MTLResourceStorageModeShared
        deallocator:nil
	];

    if (!pixelBuffer) {
        std::cerr << "Failed to create buffer.\n";
        return;
    }

	id<MTLCommandBuffer> cmd = [impl->queue commandBuffer];

    if (!cmd) {
        std::cerr << "Failed to create command buffer\n";
        return;
    }

    id<MTLComputeCommandEncoder> enc = [cmd computeCommandEncoder];

	[enc setComputePipelineState:impl->pipeline];
    [enc setBuffer:pixelBuffer offset:0 atIndex:0];

    MTLSize gridSize = MTLSizeMake(count, 1, 1);

    NSUInteger tgSize = impl->pipeline.maxTotalThreadsPerThreadgroup;
    if (tgSize > count) tgSize = count;

    MTLSize threadgroupSize = MTLSizeMake(tgSize, 1, 1);

    [enc dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];

	[enc endEncoding];
    [cmd commit];
    [cmd waitUntilCompleted];

    if (cmd.status != MTLCommandBufferStatusCompleted) {
        std::cerr << "Compute dispatch failed.\n";
        return;
    }
}