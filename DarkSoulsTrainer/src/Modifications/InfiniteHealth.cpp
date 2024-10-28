#include "Modifications/InfiniteHealth.h"

#include "Utility.h"

#include <iostream>

InfiniteHealth::InfiniteHealth(HANDLE process, size_t moduleBaseAddress)
	: Modification()
	, ModuleBaseAddress(moduleBaseAddress)
	, Process(process)
	, JumpBaseAddress(moduleBaseAddress + 0x322955)
	, CaveBaseAddress(0x13FFE0000)
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

#define OFFSET_ISPLAYER 0xB0
#define VALUE_ISPLAYER 0

#define OFFSET_ENTITYHEALTH 0x3E8
#define VALUE_ENTITYHEALTH ptr(rbx, OFFSET_ENTITYHEALTH)

#define FLAG_ISENEMY 0x0014EC90
#define FLAG_ISPLAYER ptr(rbx, OFFSET_ISPLAYER)

#define REGISTER_15 r15
#define REGISTER_11 r11
#define REGISTER_8 r8

// Must be called one after the other, will be stored in register 8
#define VALUE_PLAYEROBJECT1 ptr(ModuleBaseAddress + 0x1A31768, 0x0)
#define VALUE_PLAYEROBJECT2 ptr(REGISTER_8, 0x0)

void InfiniteHealth::Setup()
{
	using namespace asmjit;
	using namespace asmjit::x86;

	// Original Jump
	CodeBuilder Original_CB(JumpBaseAddress);
	Original_CB.Asm->mov(ptr(rbx, 0x3E8), eax);		// Set current health to new health
	OriginalJumpBytes = Original_CB.Assemble();

	// Mod Jump
	CodeBuilder Jump_CB(JumpBaseAddress);
	Jump_CB.Asm->jmp(CaveBaseAddress);			// Far jump to 0x13FFE0000
	Jump_CB.Asm->nop();					// Nop to pad memory
	ModJumpBytes = Jump_CB.Assemble();

	// Mod Cave
	CodeBuilder Cave_CB(CaveBaseAddress);
	Label IsHuman = Cave_CB.Asm->newLabel();
	Label IsPlayer = Cave_CB.Asm->newLabel();
	Label HandleHealthChangeHeader = Cave_CB.Asm->newLabel();
	Label HandleHealthChange = Cave_CB.Asm->newLabel();
	Label Exit = Cave_CB.Asm->newLabel();

	Cave_CB.Asm->cmp(REGISTER_11, FLAG_ISENEMY);		// Check if enemy
	Cave_CB.Asm->jne(IsHuman);				// If false jump to IsHuman section
	Cave_CB.Asm->jmp(HandleHealthChangeHeader);		// Else jump to TakeDamageHeader section
	
	Cave_CB.Asm->bind(IsHuman);				// IsHuman section
	Cave_CB.Asm->mov(REGISTER_8, FLAG_ISPLAYER);		// Get ISPLAYER flag and store in register 8
	Cave_CB.Asm->cmp(REGISTER_8, VALUE_ISPLAYER);		// Compare against the expected value for the player object
	Cave_CB.Asm->je(IsPlayer);				// If equal, jump to the IsPlayer section
	Cave_CB.Asm->jmp(HandleHealthChangeHeader);		// Else jump to the TakeDamageHeader section

	Cave_CB.Asm->bind(IsPlayer);				// IsPlayer section
	Cave_CB.Asm->cmp(VALUE_ENTITYHEALTH, eax);		// Check current health against new health
	Cave_CB.Asm->jg(Exit);					// If current health is greater than new health jump to Exit
	Cave_CB.Asm->mov(VALUE_ENTITYHEALTH, eax);		// Else set current health to new health
	Cave_CB.Asm->jmp(Exit);					// Jump to Exit section

	Cave_CB.Asm->bind(HandleHealthChangeHeader);		// HandleHealthChangeHeader section
	Cave_CB.Asm->mov(REGISTER_8, VALUE_PLAYEROBJECT1);	// Get a pointer to the player object container
	Cave_CB.Asm->mov(REGISTER_8, VALUE_PLAYEROBJECT2);	// Deference the pointer and get the address of the player object
	Cave_CB.Asm->cmp(REGISTER_8, REGISTER_15);		// Compare the attacker to the player object
	Cave_CB.Asm->jne(HandleHealthChange);			// If not equal jump to regular HandleHealthChange section
	Cave_CB.Asm->sub(eax, eax);				// Else set new health to 0 (one-shot entity)

	Cave_CB.Asm->bind(HandleHealthChange);			// HandleHealthChange section
	Cave_CB.Asm->mov(VALUE_ENTITYHEALTH, eax);		// Set current health to new health
	Cave_CB.Asm->jmp(Exit);					// Jump to Exit section

	Cave_CB.Asm->bind(Exit);				// Exit section
	Cave_CB.Asm->mov(REGISTER_8, 0);			// Clear register 8
	Cave_CB.Asm->jmp(ptr(rip));				// Return to original code address
	// Jump requires encoding as the 'jmp' command takes a relative address
	// the return address is too far away to pass the number as a 32-bit value
	// Address Limit:			0xFFFFFFFF	(4294967295)
	// Original Return Address:	0x13FFE0000	(5371996507)
	size_t OriginalCodeReturnAddress = JumpBaseAddress + Jump_CB.Asm->_code->codeSize();	// Calculate the return address
	Cave_CB.Asm->embedUInt64(OriginalCodeReturnAddress);	// Encode for far jump
	ModCaveBytes = Cave_CB.Assemble();

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
