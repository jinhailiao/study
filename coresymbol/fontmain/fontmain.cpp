// fontmain.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

extern unsigned char *fontmat8x16[];
extern unsigned char *fontmat16x16[];


int _tmain(int argc, _TCHAR* argv[])
{
	char *buffer[32];
	memset(buffer, 0x00, sizeof(buffer));

	FILE *fp1616 = fopen("sym1616.bin", "wb");
	for (int i = 0; i <= '~'-' '; i++)
	{
		if (fontmat16x16[i] == NULL)
			fwrite(buffer, 32, 1, fp1616);
		else
			fwrite(fontmat16x16[i], 32, 1, fp1616);
	}
	fclose(fp1616);

	FILE *fp0816 = fopen("sym0816.bin", "wb");
	for (int i = 0; i <= '~'-' '; i++)
	{
		if (fontmat8x16[i] == NULL)
			fwrite(buffer, 16, 1, fp0816);
		else
			fwrite(fontmat8x16[i], 16, 1, fp0816);
	}
	fclose(fp0816);

	return 0;
}

