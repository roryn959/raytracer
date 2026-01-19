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

#define LOOK_SENSITIVITY 300


struct MousePosition {
	int m_x;
	int m_y;
};

void handle_keydown(World& world, SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_a:
            world.MoveLeft();
            break;
        
        case SDLK_d:
            world.MoveRight();
            break;

        case SDLK_w:
            world.MoveForward();
            break;

        case SDLK_s:
            world.MoveBackward();
            break;
		
		case SDLK_q:
			world.MoveUp();
			break;
		
		case SDLK_e:
			world.MoveDown();
			break;
        
        default:
            break;
    }
}

void handle_keyup(World& world, SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_a:
            world.UnMoveLeft();
            break;
        
        case SDLK_d:
            world.UnMoveRight();
            break;

        case SDLK_w:
            world.UnMoveForward();
            break;

        case SDLK_s:
            world.UnMoveBackward();
            break;

		case SDLK_q:
			world.UnMoveUp();
			break;
		
		case SDLK_e:
			world.UnMoveDown();
			break;
        
        default:
            break;
    }
}

void handle_mouse_motion(World& world, SDL_Event& event, MousePosition& mp) {
	int x;
	int y;

	SDL_GetMouseState(&x, &y);

	int dx = x - mp.m_x;
	int dy = mp.m_y - y;

	world.ShiftView(dx * (M_PI / LOOK_SENSITIVITY), dy * (M_PI / LOOK_SENSITIVITY));

	mp.m_x = x;
	mp.m_y = y;
}

void RenderScene(Canvas& canvas, Executor& executor, World& world, uint32_t* buffer) {
	executor.TraceRays(buffer);
	canvas.ApplyPixels(buffer);
	world.SetViewChanged(false);
}

void Mainloop(Canvas& canvas, World& world, Executor& executor) {
	uint32_t buffer[NUM_PIXELS];

	RenderScene(canvas, executor, world, buffer);

	float lastTime = SDL_GetTicks() / 1000.0f;
    float lastFpsTime = lastTime;
    int frame_tick = FRAME_RATE_FREQUENCY;

	MousePosition mousePosition;
	SDL_GetMouseState(&mousePosition.m_x, &mousePosition.m_y);

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

				case SDL_MOUSEMOTION:
					handle_mouse_motion(world, event, mousePosition);
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
