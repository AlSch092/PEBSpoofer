//By Alsch092 @ github, March 2024
// Proof of concept of how we can modify the PEB's address at runtime

#ifdef _WIN64
#define IS_64_BIT 1
#else
#define IS_64_BIT 0
#endif

#include <windows.h>
#include <stdio.h>
#include <winternl.h>

#if IS_64_BIT
#define SIZE_PEB 0x2000 
#else
#define SIZE_PEB 0x1000 
#endif

#if IS_64_BIT

typedef struct _TEB
{
	PVOID Reserved1[12];
	PVOID ProcessEnvironmentBlock;
} TEB, *PTEB;

UINT64 GetPEBPointerAddress()
{
	PTEB teb = (PTEB)__readgsqword(0x30);
	
	if (teb == NULL)
		return NULL;

	return (UINT64)&(teb->ProcessEnvironmentBlock);
}

PVOID GetPEBAddress() 
{
	PTEB teb = (PTEB)__readgsqword(0x30);
	if (teb == NULL) 
		return NULL;
	
	return teb->ProcessEnvironmentBlock;
}

void SetPEBAddress(UINT64 address)
{
	__try
	{
		UINT64 PEBPtrInTEB = GetPEBPointerAddress();
		*(UINT64*)PEBPtrInTEB = address;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("Failed at SetPEBAddress: memory exception writing PEB ptr\n");
		return;
	}
}
#else
PVOID GetPEBAddress()
{
	PVOID pebAddress = 0;

    __asm
    {
        mov eax, fs:[0x30]  //Offset of PEB in TEB, change in x64
        mov pebAddress, eax
    }

    return pebAddress;
}

void SetPEBAddress(DWORD address)
{
    __asm
    {
        mov eax, address
        mov fs : [0x30], eax
    }
}
#endif

BYTE* CopyPEBBytes(unsigned int pebSize) 
{
	LPVOID pebAddress = GetPEBAddress();

	BYTE* peb_bytes = new BYTE[pebSize];

	BOOL success = ReadProcessMemory(GetCurrentProcess(), pebAddress, peb_bytes, pebSize, NULL);
	if (!success) 
	{
		printf("Failed to copy PEB bytes. Error: %d\n", GetLastError());
		delete[] peb_bytes;
		return NULL;
	}

	return peb_bytes;
}

BYTE* SetNewPEB() //in this example, we are copying the original PEB to a byte array and then setting the pointer to the PEB to our byte array.
{
	BYTE* newPeb = CopyPEBBytes(SIZE_PEB);

	if (newPeb != NULL)
	{
		printf("Setting new PEB at: %llx\n", (UINT64)newPeb);
		SetPEBAddress((UINT64)newPeb); //set address of PEB to our buffer
	}

	return newPeb;
}

int main() 
{
	UINT64 pebAddr_Original = (UINT64)GetPEBAddress();
	printf("Original PEB: %llx\n", pebAddr_Original);

	BYTE* newPEBBytes = SetNewPEB(); //copy original PEB to byte*, change pointer of PEB to the byte*
	
	UINT64 newPebAddr = (UINT64)GetPEBAddress(); //verify that original PEB pointer was written
	printf("New PEB address: %llx\n", newPebAddr);

	SetPEBAddress(pebAddr_Original); //set the PEB pointer back to our original
	
	if ((UINT64)GetPEBAddress() == pebAddr_Original)
		delete[] newPEBBytes; //now we can delete our new PEB byte array before program shutdown, otherwise delete[] will throw an exception since the byte array is being used as the PEB
	
	pebAddr_Original = (UINT64)GetPEBAddress(); //verify that original PEB pointer was written
	printf("PEB (reset to original): %llx\n", pebAddr_Original);
	
	system("pause");
	return 0;
}