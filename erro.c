// ASCII
#pragma warning(disable:4005)// redefine of macro
#pragma warning(disable:6011)
#define _LIB_STRING_HEAP
#include <setjmp.h>
#include <inttypes.h>
#include <stdio.h>
#include <ustring.h>
#include <cdear.h>
#include <consio.h>


Dnode* SGAWarnChain;//{TODO} warnings chain
size_t SGANumofWarn = 0;

char* SGAErroMsg = 0;
jmp_buf errjb = { 0 };

Dnode* ToFreeList;//{TODO}
char* COT_CRTFILE;

void warn(char* str)
{
	SGANumofWarn++;
	printf("Warn %s\n", str);
	///if (!SGAFirstWarn) SGAFirstWarn = str;
}
void erro(char* str)
{
	SGAErroMsg = str;
	printf("Erro: %s \n", str);//{TEMP OCCUPY}
	return;//{TEMP OCCUPY}
	if (errjb)
		longjmp(errjb, 1);
}

void cabort(const char* str, size_t row, size_t col, char* txt)
{
	row++;
	ConStyleAbnormal();
	if (!COT_CRTFILE)
		fprintf(stderr, "Error R%" PRIuPTR " C%" PRIuPTR , row, col);
	else fprintf(stderr, "Error F\"%s\" R%" PRIuPTR " C%" PRIuPTR, COT_CRTFILE, row, col);
	ConStyleNormal();
	fprintf(stderr, " %s", str);
	if (txt) 
	{
		fprintf(stderr, " (%s)", txt);
		memf(txt);
	}
	fprintf(stderr, "\n");
	if (errjb) longjmp(errjb, 1);
}

void NnodeReleaseTofreeCotlab(void* n)
{
	nnode* nod = n;
	if (nod->addr)
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

