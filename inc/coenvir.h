
#ifndef _INC_COENVIR
#define _INC_COENVIR

#include <cpp/unisym>
#ifdef _WinNT

#define _CRT_SECURE_NO_WARNINGS 1

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>


#else

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#ifndef _Linux
#include <c/ISO_IEC_STD/signal.h>
#else
#include <signal.h>
#endif

#endif

#include <stdio.h>

#endif
