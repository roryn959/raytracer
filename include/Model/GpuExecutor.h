#pragma once
#include <cstddef>
#include <iostream>

#include "View/Canvas.h"
#include "Core/Colour.h"
#include "World.h"


class GpuExecutor {
public:
    GpuExecutor(const World& world);
    ~GpuExecutor();

    void TraceRays(uint32_t* buffer);

private:
	void InitialiseDevice();
	void InitialiseCommandQueue();
	void CheckCommandBuffer();
	void InitialisePipeline();

	const World& m_world;

    struct Impl;     // opaque
    Impl* impl;      // pimpl to hide Objective-C++
};
