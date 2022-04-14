#include "Display.hpp"

Display::Display()
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);

	window = SDL_CreateWindow(WINDOW_TITLE.data(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH * WINDOW_SCALE,
		WINDOW_HEIGHT * WINDOW_SCALE,
		SDL_WINDOW_SHOWN);

	if (window == nullptr)
		throw std::runtime_error("Error creating SDL_Window!\n");

	renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == nullptr)
		throw std::runtime_error("Error creating SDL_Renderer!\n");

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH,
		WINDOW_HEIGHT);

	if (texture == nullptr)
		throw std::runtime_error("Error creating SDL_Texture!\n");	

}

Display::~Display()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void Display::clear()
{
	buffer.fill(PIXEL_OFF);
}

void Display::renderFrame()
{
	SDL_UpdateTexture(texture,
		nullptr, 
		buffer.data(),
		WINDOW_WIDTH * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}
