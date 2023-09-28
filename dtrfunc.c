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
	unsigned char weekday, mondays;
	time_t timep;
	struct tm* tmp;
	time(&timep);
	tmp = gmtime(&timep);
	pasts = GetMoexDayIdentity(1900+tmp->tm_year, 1+tmp->tm_mon, &weekday, &mondays);
	puts("");
	printf("    %s %d\n", ((char* []){"   January ", "  February ", "     March ",
		"     April ", "       May ", "      June ",
		"      July ", "    August ", " September ",
		"   October ", "  November ", "  December "})[tmp->tm_mon], tmp->tm_year + 1900);
	puts("Sun.Mon.Tue.Wed.Thr.Fri.Sat.");
	ConStyleAbnormal();
	ConCursorMoveRight(weekday << 2);
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
		weekday++; pasts++;
		if (weekday >= 7)
		{
			ConStyleNormal();
			puts("");
			ConStyleAbnormal();
			weekday = 0;
		}
	}
	ConStyleNormal();
	puts("");
	return 0;
}