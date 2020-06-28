#include "proc.h"

attach_result AttachProcess(const char* ProcName)
{
	attach_result Result = {};
	Result.ProcID = GetProcID(ProcName);
	if(Result.ProcID)
	{
		Result.Process = OpenProcess(PROCESS_ALL_ACCESS, 0, Result.ProcID);
		if(Result.Process)
		{
			Result.Status = ATTACH_SUCCEED;
		}
	}
	return Result;
}

DWORD GetProcID(const char* ProcName)
{
	DWORD ProcID = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 ProcEntry = {};
		ProcEntry.dwSize = sizeof(PROCESSENTRY32);
		
		if(Process32First(hSnap, &ProcEntry))
		{
			do
			{
				if(_stricmp(ProcEntry.szExeFile, ProcName) == 0)
				{
					ProcID = ProcEntry.th32ProcessID;
					break;
				}
			} while(Process32Next(hSnap, &ProcEntry));
		}
	}
	CloseHandle(hSnap);
	return ProcID;
}

uintptr_t GetModuleBaseAddress(DWORD ProcID, const char* ModuleName)
{
	uintptr_t ModuleBaseAddress = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);
	if(hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry = {};
		ModuleEntry.dwSize = sizeof(MODULEENTRY32);
		
		if(Module32First(hSnap, &ModuleEntry))
		{
			do
			{
				if(_stricmp(ModuleEntry.szModule, ModuleName) == 0)
				{
					ModuleBaseAddress = (uintptr_t)ModuleEntry.modBaseAddr;
					break;
				}
			} while(Module32Next(hSnap, &ModuleEntry));
		}
	}
	
	CloseHandle(hSnap);
	
	return ModuleBaseAddress;
}

uintptr_t FindDAMAddr(HANDLE Process, uintptr_t BaseAddr, 
					uint32* Offset, int OffsetCount)
{
	uintptr_t Result = BaseAddr;
	for(int i = 0; i < OffsetCount; ++i)
	{
		ReadProcessMemory(Process, (void*)Result, &Result, sizeof(Result), 0);
		Result += Offset[i];
	}
	return Result;
}

uintptr_t GetDAMAddr(attach_result* Attach, char* ModuleName, uint32 ModuleOffset, 
					 uint32* AddrOffet, int AddrOffetCount)
{
	uintptr_t ModuleBaseAddr = 
	GetModuleBaseAddress(Attach->ProcID, ModuleName);
	uintptr_t DynPtrBaseAddr = ModuleBaseAddr + ModuleOffset;
	
	uintptr_t Result = FindDAMAddr(Attach->Process, DynPtrBaseAddr, AddrOffet, AddrOffetCount);
	return Result;
}
