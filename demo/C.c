#include <windows.h>
#include <stdio.h>
typedef char* (__stdcall* SGA)(const char*);

int main()//x86
{
	HMODULE HDLL = LoadLibrary(TEXT("sgax.dll"));// WinNT32
	if (HDLL == NULL)
	{
		printf("error!");
		return 0;
	}
	SGA Calc = (SGA)GetProcAddress(HDLL, "NumberArithmetic");
	printf("1+2=%s", Calc("1+2"));
	FreeLibrary(HDLL);
}

// gcc c.c sgax.dll -o c.exe
// It is well in GCC but MSVC ?WHY?
