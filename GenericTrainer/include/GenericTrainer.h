// https://defuse.ca/online-x86-assembler.htm#disassembly
// https://github.com/asmjit/asmjit
// https://youtu.be/wiX5LmdD5yk

#pragma once

#include <Windows.h>
#include <vector>

class Modification;

class GenericTrainer
{
public:

	GenericTrainer(const wchar_t* procName);
	virtual ~GenericTrainer();

protected:

	void UpdateDisplay();
	void Run();

	bool IsRunning();

private:

	bool Running;

	HANDLE Process;
	uintptr_t ModuleBaseAddress;

	std::vector<Modification*> Mods;
};