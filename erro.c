// ASCII
#define _LIB_STRING_HEAP
#include <setjmp.h>
#include <ustring.h>

Dnode* SGAWarnChain;//{TODO} warnings chain
size_t SGANumofWarn = 0;

char* SGAErroMsg = 0;
jmp_buf errjb = { 0 };

Dnode* ToFreeList;//{TODO}

void warn(char* str)
{
	SGANumofWarn++;
	///if (!SGAFirstWarn) SGAFirstWarn = str;
}
void erro(char* str)
{
	SGAErroMsg = str;
	if (errjb)
		longjmp(errjb, 1);
}



