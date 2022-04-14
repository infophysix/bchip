#pragma once

#include <SDL.h>
#include <array>
#include <cstdlib>

constexpr std::size_t NumKeys { 16 };
constexpr std::array<SDL_Scancode, NumKeys> Keymap
{
	SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
	SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
	SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V,
};

class Keyboard
{
public:

	Keyboard();

	void poll();

	bool pressed { false };
	std::uint8_t pressed_idx {};

private:

	const std::uint8_t* keyboard_state;
};
