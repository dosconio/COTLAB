// ASCII
#include <stdio.h>
#include <ustring.h>
#include "parser.h"
#include "executor.h"

static void CotExecuateEvaluate(Tode* t)
{
	if (t->type == tok_else)
	{
		fstruc* fs = (void*)t->addr;
		for (unsigned int i = 0; i < fs->counts; i++)
		{
			if (fs->paras[i].ptype == dt_func)
			{
				CotExecuateEvaluate(fs->paras[i].content);
				fs->paras[i].content = ((Tode*)fs->paras[i].content)->addr;// {} cost me half day to check the mistake, the pointer acting as the value is bad, but this will be changed in the future.
									
			}
		}
		expr* re = fs->flink(fs);
		if (re->ptype == dt_uint)
			t->type = tok_number,
			t->addr = (void*)(size_t)re->content;
		// {} free it
	}
}

// Do not free execs.
void CotExecuate(Tode* execs, expr* returns, unsigned int* retnums)
{
	// may rise errors
	Tode* crt = execs->next;
	unsigned v_retnum = 0;
	while (crt)
	{
		v_retnum++;
		crt = crt->next;
	}
	//  *retnums = v_retnum;
	crt = execs->next;
	while (crt)
	{
		// temporarily just print them
		if (crt->type == tok_else)
		{
			CotExecuateEvaluate(crt);
			printf("Line%4llu: %lld\n", crt->row, (long long int)crt->addr);//{ } assert type: uint
		}
		else if (crt->type == tok_number)
		{
			printf("Line%4llu: %lld\n", crt->row, atoins(crt->addr));
		}
		else
		{
			printf("element: [%x] %s\n", crt->type, crt->addr);
		}
		crt = crt->next;
	}
	// Test stage
	// - If and only if result is a unsigned integer, output it.
	// - Just realize + operator.
	// - Do not FREE temporarily!!!
	
}
