#include "Modifications/InfiniteHealth.h"

#include "Utility.h"

#include <iostream>

InfiniteHealth::InfiniteHealth(HANDLE process, size_t moduleBaseAddress)
	: Modification(), Process(process), JumpBaseAddress(moduleBaseAddress + 0x322955), CaveBaseAddress(0x13FFE0000)
{
	Setup();
}

InfiniteHealth::~InfiniteHealth()
{
	VirtualFreeEx(Process, (BYTE*)CaveBaseAddress, 0, MEM_RELEASE);
}

bool InfiniteHealth::Update()
{
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		Toggle();
		return true;
	}

	return false;
}

void InfiniteHealth::Display()
{
	std::cout << " || F1 || TOGGLE INFINITE HEALTH                                                                    ||       " << (int)Activated << "       || " << std::endl;
}

void InfiniteHealth::Setup()
{
	using namespace asmjit;
	using namespace asmjit::x86;

	// Original Jump
	CodeBuilder original_cb(JumpBaseAddress);
	original_cb.Asm->mov(ptr(rbx, 0x3E8), eax);	// Set current health to new health
	OriginalJumpBytes = original_cb.Assemble();

	// Mod Jump
	CodeBuilder jmp_cb(JumpBaseAddress);
	jmp_cb.Asm->jmp(CaveBaseAddress);			// Far jump to 0x13FFE0000
	jmp_cb.Asm->nop();							// Nop to pad memory
	ModJumpBytes = jmp_cb.Assemble();

	// Mod Cave
	CodeBuilder cave_cb(CaveBaseAddress);
	Label IsPlayer = cave_cb.Asm->newLabel();
	Label Exit = cave_cb.Asm->newLabel();

	cave_cb.Asm->cmp(r11, 0x0014EC90);			// Check if enemy
	cave_cb.Asm->jne(IsPlayer);					// If false jump to IsPlayer
	cave_cb.Asm->mov(ptr(rbx, 0x3E8), eax);		// Set current health to new health
	cave_cb.Asm->jmp(Exit);						// jump to exit

	cave_cb.Asm->bind(IsPlayer);				// IsPlayer label
	cave_cb.Asm->cmp(ptr(rbx, 0x3E8), eax);		// Check current health against new health
	cave_cb.Asm->jg(Exit);						// If current health is greater than new health jump to Exit
	cave_cb.Asm->mov(ptr(rbx, 0x3E8), eax);		// Set current health to new health

	cave_cb.Asm->bind(Exit);					// Exit label
	cave_cb.Asm->jmp(ptr(rip));					// Return to jump
	cave_cb.Asm->embedUInt64(0x14032295B);		// Encoding for far jump

	ModCaveBytes = cave_cb.Assemble();

	// Patch in code cave
	VirtualAllocEx(Process, (BYTE*)CaveBaseAddress, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	Utility::PatchEx((BYTE*)CaveBaseAddress, (BYTE*)ModCaveBytes.c_str(), ModCaveBytes.size(), Process);
}

void InfiniteHealth::Toggle()
{
	Activated = !Activated;

	if (Activated)
	{
		// Patch jump
		Utility::PatchEx((BYTE*)JumpBaseAddress, (BYTE*)ModJumpBytes.c_str(), ModJumpBytes.size(), Process);
	}
	else
	{
		// Revert jump
		Utility::PatchEx((BYTE*)JumpBaseAddress, (BYTE*)OriginalJumpBytes.c_str(), OriginalJumpBytes.size(), Process);
	}
}