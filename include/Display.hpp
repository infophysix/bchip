#pragma once

#include <SDL.h>

#include <array>
#include <string>

constexpr std::string_view WINDOW_TITLE { "bchip" };
constexpr std::size_t WINDOW_WIDTH { 64 };
constexpr std::size_t WINDOW_HEIGHT { 32 };
constexpr std::size_t WINDOW_SIZE { WINDOW_WIDTH * WINDOW_HEIGHT };
constexpr std::size_t WINDOW_SCALE { 8 };

constexpr std::uint32_t PIXEL_ON { 0xffffffff };
constexpr std::uint32_t PIXEL_OFF { 0xff000000 };

class Display
{
public:

	Display();
	~Display();

	void clear();
	void renderFrame();

	std::array<std::uint32_t, WINDOW_SIZE> buffer { PIXEL_OFF };

private:

	SDL_Renderer *renderer { nullptr };
	SDL_Texture *texture { nullptr };
	SDL_Window *window { nullptr };
};
