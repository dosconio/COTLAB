// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

#include <alice.h>
#include <aldbg.h>
#include <consio.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int FFSet(const char* dest, const char* sors, size_t sector)// wo unisym/Kasha/kit/vdkcpy.cpp
{
	FILE* fp, * destfp;
	size_t bytes = 0;
	if ((fp = fopen(sors, "rb")) == nil)
	{
		fprintf(stderr, "Error: file not found");
		return -1;
	}
	if ((destfp = fopen(dest, "rb+")) == nil)
	{
		fprintf(stderr, "Error: file not found");
		return -1;
	}
	fseek(destfp, sector * 512, SEEK_SET);// 512 is the size of a sector
	int ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		fputc(ch, destfp);
		bytes++;
	}
	printf("%" PRIdPTR " bytes yo source.", bytes);
	fclose(fp);
	fclose(destfp);
	return 0;
}

int FileDump(const char* argv)// wo unisym/c/demo/filedump.c
{
	size_t crtline = 0;
	ConStyleNormal();
	FILE* fp = fopen(argv, "rb");
	if (fp == nil)
	{
		printline("Error: file not found");
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (size > 0xFFFFFFFF)
	{
		printline("Sorry: this program can only dump files smaller than 4GB");
		fclose(fp);
		return -1;
	}
	byte* buf = malc(size);
	fread(buf, 1, size, fp);
	fclose(fp);

	printf("         ");
	ConStyleAbnormal();
	printf("+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F 0123456789ABCDEF");
	ConStyleNormal();

	printline("");

	ConStyleAbnormal();
	printf("%08" PRIXPTR, crtline);
	crtline += 0x10;
	ConStyleNormal();
	putchar(' ');

	size_t i;
	for (i = 0; i < size; i++)
	{
		printf("%02X", buf[i]);
		if ((i & 0xF) == 0xF)
		{
			putchar(' ');
			i &= ~(size_t)0xF;
			for (size_t j = 0; j < 0x10; j++)
			{
				if (buf[i + j] >= 0x20 && buf[i + j] <= 0x7E)
					putchar(buf[i + j]);
				else putchar('.');
			}
			i |= (size_t)0xF;
			ConStyleAbnormal();
			printf("\n%08" PRIXPTR, crtline);
			crtline += 0x10;
			ConStyleNormal();
			putchar(' ');
		}
		else if ((i & 0x7) == 0x7)
			putchar('-');
		else putchar(' ');
	}
	if (i & 0xF)
	{
		unsigned char crtline_rest = i & 0xF;
		for (size_t j = 0; j < 0x10 - crtline_rest; j++)
			printf("   ");
		i &= ~(size_t)0xF;
		for (size_t j = 0; j < crtline_rest; j++)
		{
			if (buf[i + j] >= 0x20 && buf[i + j] <= 0x7E)
				putchar(buf[i + j]);
			else putchar('.');
		}
	}

	printline("");

	memf(buf);
	return 0;
}




