#pragma once
#include <cstddef>
#include <iostream>

#include "View/Canvas.h"
#include "Core/Colour.h"
#include "Core/Viewpoint.h"
#include "World.h"


class GpuExecutor {
public:
    GpuExecutor(const World& world);
    ~GpuExecutor();

	void RefreshAccumulator();

    void TraceRays(uint32_t* buffer);

private:
	void InitialiseDevice();
	void InitialiseCommandQueue();
	void CheckCommandBuffer();
	void InitialisePipeline();

	const World& 	m_world;
	Colour* 		m_accumulator;
	size_t			m_accumulationCount;
	size_t			m_seed;

    struct Impl;     // opaque
    Impl* impl;      // pimpl to hide Objective-C++
};
