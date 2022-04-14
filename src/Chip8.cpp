#include "Chip8.hpp"

#define X ((opcode & 0x0f00) >> 8)
#define Y ((opcode & 0x00f0) >> 4)
#define N (opcode & 0x000f)
#define KK (opcode & 0x00ff)
#define NNN (opcode & 0x0fff)

#define VX v[X]
#define VY v[Y]
#define VF v[0xf]

std::mt19937 seed { static_cast<std::mt19937::result_type>(std::time(nullptr)) };
std::uniform_int_distribution rng { 1, 256 };

Chip8::Chip8()
	: execute_instructions
	{
		{0x00e0, [this]() { draw_flag = true; display.clear(); }},		// CLS
		{0x00ee, [this]() { pc = stack[sp--]; }},						// RET
		{0x1000, [this]() { pc = NNN; }},								// JP addr
		{0x2000, [this]() { stack[++sp] = pc; pc = NNN; }},				// CALL addr
		{0x3000, [this]() { if (VX == KK) pc += 2; }},					// SE Vx, byte
		{0x4000, [this]() { if (VX != KK) pc += 2; }},					// SNE Vx, byte
		{0x5000, [this]() { if (VX == VY) pc += 2; }},					// SE Vx, Vy
		{0x6000, [this]() { VX = KK; }},								// LD Vx, byte
		{0x7000, [this]() { VX += KK; }},								// ADD Vx, byte
		{0x8000, [this]() { VX = VY; }},								// LD Vx, Vy
		{0x8001, [this]() { VX |= VY; }},								// OR Vx, Vy
		{0x8002, [this]() { VX &= VY; }},								// AND Vx, Vy
		{0x8003, [this]() { VX ^= VY; }},								// XOR Vx, Vy
		{0x8004, [this]() { VF = VX + VY > 0xff; VX += VY; }},			// ADD Vx, Vy
		{0x8005, [this]() { VF = VX > VY; VX -= VY; }},					// SUB Vx, Vy
		{0x8006, [this]() { VF = VX & 1; VX /= 2; }},					// SHR Vx, {, Vy}
		{0x8007, [this]() { VF = VY > VX; VX = VY - VX; }},				// SUBN Vx, Vy
		{0x800e, [this]() { VF = VX >> 7; VX *= 2; }},					// SHL Vx, {, Vy}
		{0x9000, [this]() { if (VX != VY) pc += 2; }},					// SNE Vx, Vy
		{0xa000, [this]() { i = NNN; }},								// LD I, addr
		{0xb000, [this]() { pc = NNN + v[0]; }},						// JP V0, addr
		{0xc000, [this]() { VX = rng(seed) & KK; }},					// RND Vx, byte
		{0xd000, [this]()												// DRW Vx, Vy, nibble
		{ 
            VF = 0, draw_flag = true;
            for (std::size_t y {}; y < N; ++y)
                for(std::size_t x {}; x < 8; ++x)
                    if( memory[i+y] & (0x80 >> x) )
					{
						// TODO: clean up
						if (display.buffer[(VX+x+(VY+y)*64)%2048] == PIXEL_ON)
							VF = 1;
						display.buffer[(VX+x+(VY+y)*64)%2048] ^= PIXEL_ON;
					}
		}},
		{0xe09e, [this]() // SKP Vx
		{
			if (keyboard.pressed)
				if (VX == keyboard.pressed_idx)
					pc += 2;
		}},
		{0xe0a1, [this]() // SKNP Vx
		{ 
			if (keyboard.pressed)
				if (VX != keyboard.pressed_idx)
					pc += 2;
		}},
		{0xf007, [this]() { VX = delay_timer; }},								// LD Vx, DT
		{0xf00a, [this]()												// LD Vx, K
		{
			if (keyboard.pressed)
				VX = keyboard.pressed_idx;
			else
				pc -= 2;
		}},
		{0xf015, [this]() { delay_timer = VX; }},								// LD DT, Vx
		{0xf018, [this]() { sound_timer = VX; }},								// LD ST, Vx
		{0xf01e, [this]() { i += VX; }},								// ADD I, Vx
		{0xf029, [this]()
		{ i = VX * (FontSpriteLength + FontOffset); }},			// LD F, Vx
		{0xf033, [this]()	// LD B, Vx
		{
			memory[i]   = VX / 100;
			memory[i+1] = VX / 10 % 10;
			memory[i+2] = VX % 10;
		}},										
		{0xf055, [this]()	// LD [I], Vx
		{
			std::copy_n(v.begin(), X+1, memory.begin()+i);
		}},
		{0xf065, [this]()	// LD Vx, [I]
		{
			std::copy_n(memory.begin()+i, X+1, v.begin());
		}}
	}
{
	std::copy_n(Fontset.begin(), sizeof(Fontset), memory.begin() + FontOffset);
}

void Chip8::loadRom(const std::string_view& rom)
{
	std::ifstream ifs { rom, std::ios::binary };
	std::uintmax_t rom_size {};

	if ( ifs.is_open() )
	{
		rom_size = std::filesystem::file_size(rom);
		if (rom_size > memory.size() - RomOffset)
			throw std::runtime_error("ROM size exceeds memory limit!\n");

		ifs.read(reinterpret_cast<char*>(&memory[RomOffset]), rom_size);
	}
	else
		throw std::runtime_error("The file could not be opened!\n");

	pc = RomOffset;
}

void Chip8::cycle()
{
	opcode = memory[pc] << 8 | memory[pc+1];

	pc += 2;

	for (const auto mask : {0xf0ff, 0xf00f, 0xf000})
	{
		const auto function { execute_instructions[opcode & mask] };

		if (function != nullptr)
			return std::invoke(function);
	}
}

void Chip8::updateTimers()
{
	if (delay_timer > 0)
		delay_timer--;

	if (sound_timer > 0)
	{
		sound_timer--;

		if (sound_timer == 0)
			sound.beep(600, 50);
	}

}

void Chip8::step()
{
	const auto time_now { std::chrono::high_resolution_clock::now() };

	if (time_now - last_cycle >= CycleRate)
	{
		cycle();
		last_cycle = time_now;
	}

	if (time_now - last_timer_tick >= TimerRate)
	{
		updateTimers();
		last_timer_tick = time_now;
	}
}
