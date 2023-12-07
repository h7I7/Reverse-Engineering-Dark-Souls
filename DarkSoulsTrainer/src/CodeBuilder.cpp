#include "CodeBuilder.h"

asmjit::Environment CodeBuilder::Env;
bool CodeBuilder::Initialised = false;

CodeBuilder::CodeBuilder(size_t baseAddress)
	: BaseAddress(baseAddress)
{
	if (!Initialised)
	{
		Env.setArch(asmjit::Arch::kHost);
		Initialised = true;
	}

	Code.init(Env, baseAddress);
	Asm = new asmjit::x86::Assembler(&Code);
}

CodeBuilder::~CodeBuilder()
{
	if (Asm)
	{
		delete Asm;
	}
}

std::string CodeBuilder::Assemble()
{
	std::string byteCode;

	if (!Initialised)
	{
		return byteCode;
	}

	asmjit::CodeBuffer& buffer = Code.textSection()->buffer();
	for (size_t i = 0; i < buffer.size(); i++)
	{
		byteCode += buffer.data()[i];
	}
	return byteCode;
}

void CodeBuilder::SetArch(asmjit::Arch arch)
{
	Env.setArch(arch);
	Initialised = true;
}