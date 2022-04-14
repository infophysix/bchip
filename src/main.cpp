#include "Chip8.hpp"

#include <SDL.h>

#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: chip8 <ROM file>\n";
		return EXIT_FAILURE;
	}

	Chip8 game {};
	try
	{
		game.loadRom( argv[1] );
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what();
	}

	bool running { true };
	bool wait { false };

	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				running = false;

			else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_SPACE)
				wait ^= 1;

			else if (e.type == SDL_KEYUP)
				game.keyboard.pressed = false;
		}

		if (wait == false) {
			game.keyboard.poll();
			game.step();
		}
		
		if (game.draw_flag == true)
		{
			game.draw_flag = false;
			game.display.renderFrame();
		}
	}

	return 0;
}