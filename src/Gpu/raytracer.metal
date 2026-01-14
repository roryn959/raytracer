#include <metal_stdlib>
using namespace metal;

#define WINDOW_W 700
#define WINDOW_H 700

kernel void TraceRay(
    device uint* buffer [[ buffer(0) ]],
    uint i [[ thread_position_in_grid ]]
) {
	buffer[i] = 0xFF000000;
}
