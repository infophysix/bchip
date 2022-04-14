#pragma once

#include "Display.hpp"
#include "Keyboard.hpp"
#include "Sound.hpp"

#include <array>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>
#include <random>
#include <string_view>
#include <unordered_map>

constexpr std::size_t MemorySize { 4096 };
constexpr std::uint16_t RomOffset { 0x200 };

constexpr std::uint16_t FontOffset { 0x0 };
constexpr std::size_t FontSpriteLength { 0x5 };
constexpr std::array<std::uint8_t, 80> Fontset
{
	0xF0, 0x90, 0x90, 0x90, 0xF0,	// 0
	0x20, 0x60, 0x20, 0x20, 0x70,	// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, 	// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, 	// 3
	0x90, 0x90, 0xF0, 0x10, 0x10, 	// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, 	// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, 	// 6
	0xF0, 0x10, 0x20, 0x40, 0x40, 	// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, 	// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, 	// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, 	// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, 	// B
	0xF0, 0x80, 0x80, 0x80, 0xF0, 	// C
	0xE0, 0x90, 0x90, 0x90, 0xE0, 	// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, 	// E
	0xF0, 0x80, 0xF0, 0x80, 0x80  	// F
};

constexpr std::size_t CyclesHz { 600 };
constexpr std::size_t TimersHz { 60 };
constexpr std::chrono::milliseconds CycleRate
{ 
	static_cast<std::size_t>( (1.0 / CyclesHz) * (1000.0 + 0.5) )
};
constexpr std::chrono::milliseconds TimerRate
{ 
	static_cast<std::size_t>( (1.0 / TimersHz) * (1000.0 + 0.5) )
};

class Chip8
{
public:

	Chip8();

	void loadRom(const std::string_view& rom);

	void cycle();
	void updateTimers();
	void step();

	Display display {};
	bool draw_flag {};

	Keyboard keyboard {};

	// Registers
	std::array<std::uint8_t, 16> v {};
	std::uint16_t pc {};
	std::uint16_t i {};

	// Stack
	std::array<std::uint16_t, 16> stack {};
	std::uint8_t sp {};

	// Timing
	std::uint8_t delay_timer {};
	std::uint8_t sound_timer {};

private:

	// Memory
	std::array<std::uint8_t, MemorySize> memory {};
	
	// Instructions
	std::uint16_t opcode {};

	std::unordered_map<std::uint16_t, std::function<void()>> execute_instructions;

	std::chrono::high_resolution_clock::time_point last_cycle {};
	std::chrono::high_resolution_clock::time_point last_timer_tick {};

	// Sound
	Sound sound {};
};
