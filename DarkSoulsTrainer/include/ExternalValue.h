#pragma once

#include "Utility.h"

#include <vector>

template <typename T>
class ExternalValue
{
public:
	ExternalValue<T>(HANDLE process, uintptr_t dynamicPtrBaseAddress, std::vector<unsigned int> offsets)
		: Process(process)
	{
		uintptr_t addr = Utility::FindDMAAddy(Process, dynamicPtrBaseAddress, offsets);
		Addresses.push_back(addr);
		CurrentValue = Read();
	}

	virtual ~ExternalValue() {}

	void AddAddress(uintptr_t dynamicPtrBaseAddress, std::vector<unsigned int> offsets)
	{
		uintptr_t addr = Utility::FindDMAAddy(Process, dynamicPtrBaseAddress, offsets);
		Addresses.push_back(addr);
	}

	operator T ()
	{
		return CurrentValue;
	}

	void operator = (T other)
	{
		Write(other);
	}

protected:
	T Read()
	{
		ReadProcessMemory(Process, (BYTE*)Addresses[0], &CurrentValue, sizeof(CurrentValue), nullptr);
		return CurrentValue;
	}

	void Write(T value)
	{
		CurrentValue = value;
		for (unsigned int i = 0; i < Addresses.size(); ++i)
		{
			WriteProcessMemory(Process, (BYTE*)Addresses[i], &CurrentValue, sizeof(CurrentValue), nullptr);
		}
	}

private:
	T CurrentValue;
	std::vector<uintptr_t> Addresses;
	HANDLE Process;
};