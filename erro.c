// ASCII
#define _LIB_STRING_HEAP
#include <setjmp.h>
#include <ustring.h>
#include <cdear.h>
#include <inttypes.h>
#include <stdio.h>

Dnode* SGAWarnChain;//{TODO} warnings chain
size_t SGANumofWarn = 0;

char* SGAErroMsg = 0;
jmp_buf errjb = { 0 };

Dnode* ToFreeList;//{TODO}
char* COT_CRTFILE;

void warn(char* str)
{
	SGANumofWarn++;
	printf("WARN: %s\n", str);
	///if (!SGAFirstWarn) SGAFirstWarn = str;
}
void erro(char* str)
{
	SGAErroMsg = str;
	if (errjb)
		longjmp(errjb, 1);
}

void cabort(char* str, size_t row, size_t col)
{
	col++;
	if (!COT_CRTFILE)
		fprintf(stderr, "ERROR R%" PRIuPTR " C%" PRIuPTR " %s\n", row, col, str);
	else fprintf(stderr, "ERROR F\"%s\" R%" PRIuPTR " C%" PRIuPTR " %s\n", COT_CRTFILE, row, col, str);
	if (errjb) longjmp(errjb, 1);
}

void NnodeReleaseTofreeCotlab(void* n)
{
	nnode* nod = n;
	if (nod->class == tok_number)
		CoeDel((void*)nod->addr);
	else memf(nod->addr);
	memf(n);
}

void InodeReleaseTofreeElementCotlab(void* n)
{
	if (!n) return;
	inode* nod = n;
	if (nod->type == tok_number)
	{
		CoeDel((void*)nod->data);
	}
	else memf(nod->data);

	nod->data = 0;
}

void InodeReleaseTofreeCotlab(void* n)
{
	if (!n) return;
	inode* nod = n;
	memf(nod->addr);
	if (nod->type == tok_number)
	{
		CoeDel(nod->data);
	}
	else if (nod->data) memf(nod->data);
	memf(nod);

	//{TODO}
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

