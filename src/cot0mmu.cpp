// ASCII CPP-14 TAB4 CRLF
// ModuTitle: COTLAB NoMMU Process Support
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#ifdef _NOMMU

#include "aaaaa.h"
#include <sys/wait.h>

extern "C" char** environ;

int cot_nommu_spawn(const char* path, char** argv) {
	if (!path || !argv) return -1;
	return spawnve(path, argv, environ);
}

int cot_nommu_wait(int* status) {
	return wait(status);
}

#endif
