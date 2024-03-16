// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <cinc>
#include <alice.h>
#include <aldbg.h>
#include <ustring.h>
#include <cinc>
#include "contask.h"
#include <stdio.h>
#include <iostream>//{TODO} -> conio

extern "C" stduint _MALCOUNT = 0;
extern "C" stduint _MALLIMIT = 0x1000;

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

	switch (option)
	{
	case COTLAB_OPTION_SHELL: while (true) {
		if (!innbuf) innbuf = (char*)malc(0x1000);
		std::cout << "COTLAB> ";
		i = 0;
		while (i < sizeof(innbuf)*0x1000 && std::cin.get(ch) && ch != '\n')
			innbuf[i++] = ch;
		innbuf[i] = 0;

		if (!StrCompare(innbuf, "exit")) break;

		Contask ctask(innbuf, CONTASK_BUF);//{TODO} task-pool and peculiar function
		ctask.Prep();
		// ctask.PrintDebug();
		ctask.Parse();
		ctask.PrintDebug();

	} break;
	case COTLAB_OPTION_FILE:
		break;
	case COTLAB_OPTION_COMMAND:
		break;
	default:
		printf("Unknown Option: %d\n", option);//# inside error
		break;
	}
	mfree(innbuf);//{TODO} COT-FREE-POOL by NodeChain, for reference(int a) and address(0xaaaacccc)
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
