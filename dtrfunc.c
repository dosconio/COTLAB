// ASCII TAB4 C99 ArnAssume
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <consio.h>
#include <datime.h>
#include <time.h>
#include "cotlab.h"

dnode* DtrCalendar(dnode* const callinfo)
{
	//{TEMP} no input of some month
	unsigned pasts;
	unsigned char wday, mondays;
	time_t timep;
	struct tm* tmp;
	time(&timep);
	tmp = localtime(&timep);
	pasts = herspan(1900 + tmp->tm_year, 1 + tmp->tm_mon, 1);
	wday = weekday(1900+ tmp->tm_year, 1 + tmp->tm_mon, 1);
	mondays = moondays(1900 + tmp->tm_year, 1 + tmp->tm_mon);
	puts("");
	printf("    %s %d\n", ((char* []){"   January ", "  February ", "     March ",
		"     April ", "       May ", "      June ",
		"      July ", "    August ", " September ",
		"   October ", "  November ", "  December "})[tmp->tm_mon], tmp->tm_year + 1900);
	puts("Sun.Mon.Tue.Wed.Thr.Fri.Sat.");
	ConStyleAbnormal();
	ConCursorMoveRight(wday << 2);
	for (unsigned char i = 1; i <= mondays; i++)
	{
		if (i == tmp->tm_mday)
		{
			ConStyleNormal();
			printf("[%02d]", i);
			ConStyleAbnormal();
		}
		else if (!((pasts + 1) % 30))
			printf("|%02d]", i);
		else
			printf("[%02d]", i);
		wday++; pasts++;
		if (wday >= 7)
		{
			ConStyleNormal();
			puts("");
			ConStyleAbnormal();
			wday = 0;
		}
	}
	ConStyleNormal();
	puts("");
	return 0;
}