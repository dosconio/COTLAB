// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
#include "ulibex.h"
#include <stdio.h>
#include <cdear.h>
#include <inttypes.h>
#include "parser.h"
#include "executor.h"

void CotPrint(tnode* inp)
{
	for (tnode* crt = inp->right; crt; crt = crt->next)
	{
		if (crt->type == tok_number)
		{
			char* p;
			p = CoeToLocale((void*)crt->addr, 1);
			printf("Line %" PRIiPTR ": %s\n", 0LL, p);//{TODO} use tnode as dnode
			memf(p);
		}
		else
		{
			if (crt->addr)
				printf("Line %" PRIiPTR ": %s\n", 0LL, crt->addr);//{TODO} use tnode as dnode
		}
	}
}

nnode* CotExecuate(nnode* inp)
{
	// Nest ---> Linear of (func, imm-value)
	// (1) ---> 1;  pi() ---> 3.1416; ...
	CoeInit();
	for (nnode* crt = inp; crt; crt = crt->right)
		if (crt->subf || crt->bind)
		{
			if (!crt->subf)// pi()
			{
				Dnode* res = ((fstruc_t)crt->bind)(0);
				if (!res)
				{
					// delete the node, for its mainly focusing on side-effect
					if (crt->left)crt->left->right = crt->right;
					if (crt->right)crt->right->left = crt->left;
					memf(crt->addr);
					nnode* tmpdn = crt->left ? crt->left : crt->right;
					memf(crt);
					crt = tmpdn;
					if (!crt) 0;// nothing in the chain
				}
				else// if(fs.return_num==1)
				{
					// {TEMP} just for 1 return.
					crt->class = res->type;
					if (crt->addr) memf(crt->addr);
					crt->addr = res->addr;
					memf(res);// {TEMP}
				}
			}
			else// {TODO} merged with the above
			{
				// f(h(1,2))
				CotExecuate(crt->subf);
				Dnode* f_in = NnodeToDnode(crt->subf);// one of alias of crt->subf
				Dnode* res = 0;
				if (crt->bind)
				{
					res = ((fstruc_t)crt->bind)(f_in);
					DnodesReleaseCotlab(f_in);
				}
				else res = f_in;
				
				// {TEMP}
				if (res)
				{
					crt->class = res->type;
					if (crt->addr) memf(crt->addr);
					crt->addr = res->addr;
					DnodeRelease(res, 0);// {TOD} now just for single return function
				}
				else
				{
					// Remove the node
					if (!crt->right) return inp;
					// {confirm} there should always be a element on the left
					crt->left->right = crt->right;
					nnode* tmp = crt->right->left = crt->left;
					memf(crt->addr);
					memf(crt);
					crt = tmp;
				}
				crt->subf = 0;
			}
		}
	
}
