#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>

#include "Model/World.h"
#include "View/Canvas.h"

#define GPU_BUILD 1

#if GPU_BUILD
#include "Model/GpuExecutor.h"
typedef GpuExecutor Executor;
#else
#include "Model/CpuExecutor.h"
typedef CpuExecutor Executor;
#endif

#define FRAME_RATE_FREQUENCY 30

void handle_keydown(World& world, SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_a:
            world.MoveRight();
            break;
        
        case SDLK_d:
            world.MoveLeft();
            break;

        case SDLK_w:
            world.MoveBackward();
            break;

        case SDLK_s:
            world.MoveForward();
            break;
        
        default:
            break;
    }
}

void handle_keyup(World& world, SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_a:
            world.UnMoveRight();
            break;
        
        case SDLK_d:
            world.UnMoveLeft();
            break;

        case SDLK_w:
            world.UnMoveBackward();
            break;

        case SDLK_s:
            world.UnMoveForward();
            break;
        
        default:
            break;
    }
}

void RenderScene(Canvas& canvas, Executor& executor, const World& world, uint32_t* buffer) {
	executor.TraceRays(buffer);
	canvas.ApplyPixels(buffer);
}

void Mainloop(Canvas& canvas, World& world, Executor& executor) {
	uint32_t buffer[NUM_PIXELS];

	executor.RefreshAccumulator();
	RenderScene(canvas, executor, world, buffer);

	float lastTime = SDL_GetTicks() / 1000.0f;
    float lastFpsTime = lastTime;
    int frame_tick = FRAME_RATE_FREQUENCY;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

				case SDL_KEYDOWN:
                    handle_keydown(world, event);
                    break;
                
                case SDL_KEYUP:
                    handle_keyup(world, event);
                    break;

                default:
                    break;
            }
        }

		// Time-based processing
        float currentTime = SDL_GetTicks() / 1000.0f;
        float dt = currentTime - lastTime;

		// Calculate fps
        --frame_tick;
        if (0 == frame_tick) {
            float fps = FRAME_RATE_FREQUENCY / (currentTime - lastFpsTime);
            std::cout << "fps: " << fps << "\n";
            frame_tick = FRAME_RATE_FREQUENCY;
            lastFpsTime = currentTime;
		}

		world.ProcessTimeTick(dt, executor);
		RenderScene(canvas, executor, world, buffer);

		lastTime = currentTime;
    }
}

int main() {
    Canvas canvas;
	World world;
	Executor executor(world);

    Mainloop(canvas, world, executor);

    return 0;
}
