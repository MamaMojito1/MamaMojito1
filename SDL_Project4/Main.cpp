#include <SDL.h>
#include <vector>
#include <algorithm>

const int TEXTURE_H = 500;
const int TEXTURE_W = 500;
const int SCREEN_H = 640;
const int SCREEN_W = 480;

int main(int argv, char* argc[]) {
	SDL_Rect source{ 0, 0, SCREEN_W / 16, SCREEN_H / 16 };
	SDL_Rect dest{ 10, 10, SCREEN_W - 20, SCREEN_H - 20 };
	SDL_Event e;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("SDL_Init Error: %s", SDL_GetError());
		return 1;
	}

	auto window = SDL_CreateWindow("Scrolling", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, 0);
	if (window == nullptr) {
		SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TEXTURE_W, TEXTURE_H);
	if (texture == nullptr) {
		SDL_Log("SDL_CreateTexture Error: %s", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	std::vector<SDL_Point> pv;
	for (int i = 0; i < 10000; i++) {
		pv.push_back(SDL_Point{ rand() % TEXTURE_W, rand() % TEXTURE_H });
	}

	auto running = true;
	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { running = false; }
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_UP:    source.y = std::max(0, source.y - 3); break;
				case SDLK_DOWN:  source.y = std::min(TEXTURE_H - source.h, source.y + 3); break;
				case SDLK_LEFT:  source.x = std::max(0, source.x - 3); break;
				case SDLK_RIGHT: source.x = std::min(TEXTURE_W - source.w, source.x + 3); break;
				case SDLK_1:
					if (source.w > 1 && source.h > 1) {
						source.w /= 2;
						source.h /= 2;
					}
					break;
				case SDLK_2:
					if (source.w < TEXTURE_W / 2 && source.h < TEXTURE_H / 2) {
						source.w *= 2;
						source.h *= 2;
					}
					break;
				}
			}
		}

		// Clear texture
		SDL_SetRenderTarget(renderer, texture);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// Move Position of dots around
		std::for_each(pv.begin(), pv.end(), [](auto& item) {
			item.x = (item.x + (rand() % 3 - 1) + TEXTURE_W) % TEXTURE_W;
			item.y = (item.y + (rand() % 3 - 1) + TEXTURE_H) % TEXTURE_H;
		});

		// Draw dots to the screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (const auto& point : pv) {
			SDL_RenderDrawPoint(renderer, point.x, point.y);
		}
		
		SDL_SetRenderTarget(renderer, nullptr);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, &source, &dest);
		SDL_RenderPresent(renderer);

		SDL_Delay(50);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
