#include "GenericTrainer.h"

#include "Utility.h"
#include "ExternalValue.h"

#include "Modifications/InfiniteHealth.h"

#include <string>
#include <iostream>

#define HEADER "S.       .S  S.       .S S.     sSSs        sdSS_SSSSSSbs   .S_sSSs     .S_SSSs     .S   .S_sSSs      sSSs   .S_sSSs    \nSS.     .SS  SS.     .SS SS.   d%%SP        YSSS~S%SSSSSP  .SS~YS%%b   .SS~SSSSS   .SS  .SS~YS%%b    d%%SP  .SS~YS%%b   \nS%S     S%S  S%S     S%S S%S  d%S'               S%S       S%S   `S%b  S%S   SSSS  S%S  S%S   `S%b  d%S'    S%S   `S%b  \nS%S     S%S  S%S     S%S S%S  S%|                S%S       S%S    S%S  S%S    S%S  S%S  S%S    S%S  S%S     S%S    S%S  \nS&S     S&S  S&S     S%S S%S  S&S                S&S       S%S    d*S  S%S SSSS%S  S&S  S%S    S&S  S&S     S%S    d*S  \nS&S     S&S  S&S      SS SS   Y&Ss               S&S       S&S   .S*S  S&S  SSS%S  S&S  S&S    S&S  S&S_Ss  S&S   .S*S  \nS&S     S&S  S&S       S S    `S&&S              S&S       S&S_sdSSS   S&S    S&S  S&S  S&S    S&S  S&S~SP  S&S_sdSSS   \nS&S     S&S  S&S       SSS      `S*S             S&S       S&S~YSY%b   S&S    S&S  S&S  S&S    S&S  S&S     S&S~YSY%b   \nS*b     S*S  S*b       S*S       l*S             S*S       S*S   `S%b  S*S    S&S  S*S  S*S    S*S  S*b     S*S   `S%b  \nS*S.    S*S  S*S.      S*S      .S*P             S*S       S*S    S%S  S*S    S*S  S*S  S*S    S*S  S*S.    S*S    S%S  \n SSSbs  S*S   SSSbs    S*S    sSS*S              S*S       S*S    S&S  S*S    S*S  S*S  S*S    S*S   SSSbs  S*S    S&S  \n  YSSP  S*S    YSSP    S*S    YSS'               S*S       S*S    SSS  SSS    S*S  S*S  S*S    SSS    YSSP  S*S    SSS  \n        SP             SP                        SP        SP                 SP   SP   SP                  SP          \n        Y              Y                         Y         Y                  Y    Y    Y                   Y           \n"
#define BREAK "========================================================================================================================\n"
#define EXITMESSAGE "====================================================TRAINER STOPPED=====================================================\n"

GenericTrainer::GenericTrainer(const wchar_t* procName)
	: Running(true)
{
	DWORD procId = Utility::GetProcId(procName);
	ModuleBaseAddress = Utility::GetModuleBaseAddress(procId, procName);
	Process = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	InfiniteHealth* infiniteHealth = new InfiniteHealth(Process, ModuleBaseAddress);
	Mods.push_back(infiniteHealth);

	UpdateDisplay();
	Run();
}

GenericTrainer::~GenericTrainer()
{
	for (unsigned int i = 0; i < Mods.size(); ++i)
	{
		delete Mods[i];
	}

	CloseHandle(Process);
}

void GenericTrainer::UpdateDisplay()
{
	system("cls");

	std::cout << HEADER << BREAK;
	for (unsigned int i = 0; i < Mods.size(); ++i)
	{
		Mods[i]->Display();
	}
}

void GenericTrainer::Run()
{
	ExternalValue<int32_t> Health(Process, ModuleBaseAddress + 0x1A31768, { 0x0, 0x3E8 });		// Actual health value
	ExternalValue<int32_t> Souls(Process, ModuleBaseAddress + 0x1A31768, {0x0, 0x578, 0x94});	// Actual souls value
	Souls.AddAddress(ModuleBaseAddress + 0x01B69128, { 0xC0, 0x268, 0x50, 0x10, 0x20, 0x220 });	// Visual souls value

	while (IsRunning())
	{
		bool toggled = false;
		for (unsigned int i = 0; i < Mods.size(); ++i)
		{
			if (Mods[i]->Update())
			{
				toggled = true;
			}
		}

		if (toggled)
		{
			UpdateDisplay();
		}

		//Souls = Souls + 1;
	}

	std::cout << EXITMESSAGE;
}

bool GenericTrainer::IsRunning()
{
	DWORD dwExit = 0;
	return Running && GetExitCodeProcess(Process, &dwExit) && dwExit == STILL_ACTIVE;
}