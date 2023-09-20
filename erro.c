// ASCII
#define _LIB_STRING_HEAP
#include <setjmp.h>
#include <ustring.h>
#include <cdear.h>

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

void cabort(_Need_free char* str)
{
	
}

void NnodeReleaseTofreeCotlab(void* n)
{
	nnode* nod = n;
	if (nod->class == tok_number)
		CoeDel((void*)nod->addr);
	else memf(nod->addr);
	memf(n);
}

void DnodesReleaseTofreeCotlab(dnode* inp)
{
	dnode* first = inp;
	if (!inp) return;
	if (inp->next) DnodesReleaseTofreeCotlab(inp->next);
	if (first->type == tok_number)
		CoeDel((void*)first->addr);
	else if (first->addr) memfree(first->addr);
	memfree(first);
}

void TnodesReleaseTofreeCotlab(void* inp)
{
	tnode* first = inp;
	if (first->type == tok_number)
		CoeDel((void*)first->addr);
	else if(first->addr) memfree(first->addr);
	memfree(first);
}

