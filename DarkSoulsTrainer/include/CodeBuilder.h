#pragma once

#include <asmjit/x86.h>
#include <string>

class CodeBuilder
{
public:
	CodeBuilder(size_t baseAddress = 0x0);
	~CodeBuilder();

	std::string Assemble();

	static void SetArch(asmjit::Arch arch);

	asmjit::x86::Assembler* Asm;

private:
	size_t BaseAddress;

	asmjit::CodeHolder Code;

	static asmjit::Environment Env;
	static bool Initialised;
};