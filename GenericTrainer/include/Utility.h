#pragma once

#include <Windows.h>
#include <vector>

class Utility
{
public:

	static DWORD GetProcId(const wchar_t* procName);
	static uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
	static uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	static void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProc);
	static void NopEx(BYTE* dst, unsigned int size, HANDLE hProc);
};