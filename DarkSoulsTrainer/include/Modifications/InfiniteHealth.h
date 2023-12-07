#pragma once

#include "Modifications/Modification.h"
#include "CodeBuilder.h"

#include <string>
#include <Windows.h>

class InfiniteHealth : public Modification
{
public:
	InfiniteHealth(HANDLE process, size_t moduleBaseAddress);
	virtual ~InfiniteHealth();

	virtual bool Update() override;
	virtual void Display() override;

protected:
	virtual void Setup() override;
	virtual void Toggle() override;

private:
	size_t JumpBaseAddress;
	size_t CaveBaseAddress;

	std::string ModJumpBytes;
	std::string ModCaveBytes;

	std::string OriginalJumpBytes;

	HANDLE Process;
};