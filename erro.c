// ASCII
#include <setjmp.h>
#include <ustring.h>

char* SGAFirstWarn = 0;
char* SGAErroMsg = 0;
size_t SGANumofWarn = 0;
jmp_buf errjb = { 0 };
Dnode* FreeListLast;

void warn(char* str)
{
	// in constant area
	SGANumofWarn++;
	if (!SGAFirstWarn) SGAFirstWarn = str;
}
void erro(char* str)
{
	SGAErroMsg = str;
	if (errjb)
		longjmp(errjb, 1);
}