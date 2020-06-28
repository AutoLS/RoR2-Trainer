#ifndef PROC_H
#define PROC_H

#include <tlhelp32.h>
#include <stdint.h>
#include <AE/Math.h>

enum attach_status
{
	ATTACH_FAILED,
	ATTACH_SUCCEED,
};

struct attach_result
{
	DWORD ProcID;
	HANDLE Process;
	
	attach_status Status;
};

DWORD GetProcID(const char* ProcName);

uintptr_t GetModuleBaseAddress(DWORD ProcID, const char* ModuleName);

uintptr_t FindDAMAddr(HANDLE Process, uintptr_t BaseAddr, 
					uint32* Offset, int OffsetCount);

#endif