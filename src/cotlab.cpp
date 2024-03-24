// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <stdio.h>
#include <iostream>//{TODO} -> conio
#include "../inc/cothead.h"

extern "C" {
stduint _MALCOUNT = 0;
stduint _MALLIMIT = 0x1000;
stduint call_state = 0x1000;
}


#define CotTitle "COTLAB Console Build " __DATE__ "\n"

const char* ConMode[] {
	"Shell", "File", "Command"
};

char ulibver[16 + 1] = { "Not detected." };// from unisymlib



int cotmain(int argc, char** argv) {
	enum option_t { COTLAB_OPTION_SHELL, COTLAB_OPTION_FILE, COTLAB_OPTION_COMMAND } option{ COTLAB_OPTION_COMMAND };
	enum prompt_t { COTLAB_PROMPT_FULL, COTLAB_PROMPT_TRIARROW } prompt{ COTLAB_PROMPT_FULL };

	FILE* fp = 0;
	stduint i = 0;
	char ch;
	char* innbuf{ 0 };

	uni::InodeChain* ic_mac = zalcof(uni::InodeChain);
	uni::InodeChain* ic_sen = zalcof(uni::InodeChain);
	uni::InodeChain* ic_ise = zalcof(uni::InodeChain);
	new (ic_mac) uni::InodeChain(true); ic_mac->Onfree((_tofree_ft)ReleaseTofreeCotlabInode);
	new (ic_sen) uni::InodeChain(true); ic_sen->Onfree((_tofree_ft)ReleaseTofreeCotlabInode);
	new (ic_ise) uni::InodeChain(true); ic_ise->Onfree((_tofree_ft)ReleaseTofreeCotlabInode);
	uni::InodeChain* ic_list[]{ ic_mac, ic_sen, ic_ise };

	uni::Coe* local_pi = zalcof(uni::Coe); new (local_pi) uni::Coe(uni::Coe::pi());
	uni::Coe* local_e = zalcof(uni::Coe); new (local_e) uni::Coe(uni::Coe::e());

	ic_sen->Update("last", 0, 0);
	ic_sen->Update("pi", local_pi, tok_number, true);
	ic_sen->Update("e", local_e, tok_number, true);

	if (fp = fopen("../unisym/lib/Script/datura/version", "r")) {
		for (i = 0; i < sizeof(ulibver) - 1 && (((int)(ulibver[i] = fgetc(fp)) != EOF) || (ulibver[i] = 0)); i++);
		fclose(fp);
	}

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') switch (argv[i][1]) {
		case 's':// omit the rest of this argv
			option = COTLAB_OPTION_SHELL;
			break;
		case 'f':// follow by a file name
			option = COTLAB_OPTION_FILE;
		default:
			printf("Unknown Option: %s\n", argv[i]);
			return -1;
		}
	}

	printf("Library: %s\n", ulibver);
	printf("Mode   : %s\n", ConMode[option]);

	// ConStyleNormal();// <- use this for IO

	switch (option) {
	case COTLAB_OPTION_SHELL: while (true) {
		if (!innbuf) innbuf = (char*)malc(0x1000);
		(void)ConGetCurrentDirectory(innbuf, 0x1000);
		for0(i, StrLength(innbuf)) if (innbuf[i] == '\\') innbuf[i] = '/';
		if(StrCharLast(innbuf) != '/') StrAppend(innbuf, "/");
		std::cout << innbuf << "> ";
		i = 0;

		ConScanLine(innbuf, 0x1000);
		StrDeprefixSpaces(innbuf);

		if (!StrCompare(innbuf, "exit")) break;
		else if (!StrCompare(innbuf, "help")) {
			puts("\nCOTLAB: Her SGA-4 (doscon.io), GNU GPL-3 (cotlab.org)\n"
				"   ||"
				"\n   |+ Doshou Haruno [Dscn.Org.Chief] (dosconyo@gmail.com)"
				"\n   |+ Ren    Phina  [None.Org.Advis] (   phina@ tuta.io )"
			);// A: Author
			printf("   + UNISYM: %s\n", ulibver);
			printf("Language: COTOBA\n");// COTOBA used name Datura
		}
		else if (!StrCompare(innbuf, "list")) {
			InodePrint(ic_mac);
			InodePrint(ic_sen);
			InodePrint(ic_ise);
		}
		else if (!StrCompare(innbuf, "cls"))
			ConClearScreen();
		else if (!StrCompareN(innbuf, "cd ", 3)) {
			if (ConSetCurrentDirectory(innbuf + 3))
				fprintf(stderr, "Invaild Working Path.\n");
		}
		// [dir] ...
		// else : A single identifier which is not a valuable for,
		//        cotlib void(void) functions will take over
		else {
			Contask ctask(innbuf, CONTASK_BUF, ic_list);//{TODO} task-pool and peculiar function
			ctask.Prep(); // ctask.PrintDebug();
			ctask.Parse(); // ctask.PrintDebug();
			if (ctask.Execute()) {
				uni::Nnode* last = ctask.npu->GetNetwork()->Root();
				void* content = stepval(last)->addr;
				ic_sen->Update("last", CotCopy(content, stepval(last)->type), stepval(last)->type);
			}
			ctask.PrintDebug();
		}
	} break;
	case COTLAB_OPTION_FILE:
		//{TODO}
		//if (!(fp = fopen(point, "r")))
		//{
		//	printf("File %s is misty.", (char*)point);
		//	goto endo;
		//}
		break;
	case COTLAB_OPTION_COMMAND:
		//{CANCELLED}
		break;
	default:
		printf("Unknown Option: %d\n", option);//# inside error
		break;
	}
	mfree(innbuf);//{TODO} COT-FREE-POOL by NodeChain, for reference(int a) and address(0xaaaacccc)
	ic_mac->~InodeChain(); mfree(ic_mac);
	ic_sen->~InodeChain(); mfree(ic_sen);
	ic_ise->~InodeChain(); mfree(ic_ise);
	return 0;
}
#include <inttypes.h>
int main(int argc, char** argv) {
	int retno = cotmain(argc, argv);
	if (_MALCOUNT) 
		printf("Memory Leak: 0x%" PRIxSTD " !\n", _MALCOUNT);
	return retno;
}

void entry() {
	//TEMP for Mecocoa and other free-standing environments
	cotmain(0, 0);
}
