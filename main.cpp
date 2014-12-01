#include <stdio.h>
#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <SDL/SDL.h>
#include <emscripten.h>
#else
#include <SDL.h> // Custom path
#endif

typedef struct {
	SDL_Surface *Screen;
	uint16_t ScreenWidth;
	uint16_t ScreenHeight;
	uint32_t FrameCounter;
} RendererOptions;

void GameLoop(void* arg);

int main(int argc, char** argv) {
	const uint16_t ScreenWidth = 800;
	const uint16_t ScreenHeight = 600;

	//std::cout << "SDL to Canvas" << std::endl;
	// NOTE: Dont use iostream, if is not necessary. Because output JS will be much larger due iostream complexity. 
	printf("SDL to Canvas\n");

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(ScreenWidth, ScreenHeight, 32, SDL_SWSURFACE);

	RendererOptions Options;
	Options.Screen = screen;
	Options.ScreenHeight = ScreenHeight;
	Options.ScreenWidth = ScreenWidth;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(GameLoop, &Options, 0, true);
#else
	bool IsRunning = true;

	while (IsRunning)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				IsRunning = false;
				break;
			default:
				break;
			}
		}

		GameLoop(&Options);
	}
#endif

	SDL_Quit();

	return 0;
}

void GameLoop(void *arg)
{
	RendererOptions *Options = (RendererOptions*)arg;

	Options->FrameCounter = (Options->FrameCounter == UINT32_MAX) ? 0 : ++Options->FrameCounter;

	if (SDL_MUSTLOCK(Options->Screen))
	{
		SDL_LockSurface(Options->Screen);
	}

	for (int i = 0; i < Options->ScreenHeight; i++) {
		for (int j = 0; j < Options->ScreenWidth; j++) {
#ifdef TEST_SDL_LOCK_OPTS
			uint8_t Alpha = 255;
#else
			uint8_t Alpha = (i + j) % 255;
#endif
			uint8_t Red = (i + Options->FrameCounter) % 255;
			uint8_t Green = (j + Options->FrameCounter) % 255;
			uint8_t Blue = ((255 - i) + Options->FrameCounter) % 255;

			*((Uint32*)Options->Screen->pixels + i * Options->ScreenWidth + j) = SDL_MapRGBA(Options->Screen->format, Red, Green, Blue, Alpha);
		}
	}

	if (SDL_MUSTLOCK(Options->Screen))
	{
		SDL_UnlockSurface(Options->Screen);
	}

	SDL_Flip(Options->Screen);
}