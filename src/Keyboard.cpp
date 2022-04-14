#include "Keyboard.hpp"

Keyboard::Keyboard()
	: keyboard_state { SDL_GetKeyboardState(nullptr) }
{
}

void Keyboard::poll()
{
	SDL_PumpEvents();

	for (std::uint8_t idx {}; idx < NumKeys; ++idx)
	{
		if ( keyboard_state[ Keymap[idx] ] )
		{
			pressed = true;
			pressed_idx = idx;
			break;
		}
	}
}
