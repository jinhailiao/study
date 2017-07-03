//
//
//
#include <stdio.h>
#include "string.h"
#include "hfileapi.h"

#define S_WORD unsigned short
#define S_BYTE unsigned char

void  hai_InitMemHeap(void *pMem, unsigned long size);


int listdir(void)
{
	int err;
	long handle;
	struct finddata fd;

	handle = hai_findfirst("", &fd);
	if (!handle)
	{
		printf("not find!\n");
		return -1;
	}

	do {
		if (fd.attrib&0x10)
			printf("d     ");
		else
			printf("f     ");
		printf(fd.name); printf("\n");
		err = hai_findnext(handle, &fd);
	}while (!err);
	
	hai_findclose(handle);
	
	return 0;
}

char *cmdset[] =
{
	"exit",
	"format",
	"chdir",
	"mkdir",
	"rmdir",
	"flush",
	"create",
	"open",
	"read",
	"write",
	"close",
	"dir",
	"rm",
	""
};

S_BYTE heap[1024*20];

int main(int argc, char *argv[])
{
	int loop = 1;
	int i, err;
	long size;
	char cwd[260];
	char cmd[260], arg[260];
	HFILE hfile = 0;
	HFILE hfile2 = 0;

	hai_InitMemHeap(heap, 1024*20);
	hai_InitFileSys();

	while (loop)
	{
		hai_getcwd(cwd, 260);
		printf(cwd);printf("\\>:");
		scanf("%s", cmd);
		for (i = 0; i < sizeof(cmdset)/sizeof(cmdset[0]); i++)
		{
			if (!strcmp(cmd, cmdset[i]))
				break;
		}

		switch (i)
		{
		case 0:
			loop = 0;
			break;
		case 1:
			scanf("%s", arg);
			err = hai_format(arg, 0);
			break;
		case 2:
			scanf("%s", arg);
			err = hai_chdir(arg);
			break;
		case 3:
			scanf("%s", arg);
			err = hai_mkdir(arg);
			break;
		case 4:
			scanf("%s", arg);
			err = hai_rmdir(arg);
			break;
		case 5:
			err = hai_flushall();
			break;
		case 6:
			scanf("%s", arg);
			hfile2 = hai_fopen(arg, "w");
			if (hfile2)
				err = hai_fclose(hfile2);
			else
				err = -1;
			break;
		case 7:
			scanf("%s", arg);
			if (hfile)
				hai_fclose(hfile);
			hfile = hai_fopen(arg, "r+");
			err = hfile?0:-1;
			break;
		case 8:
			err = hai_fseek(hfile, 0, SEEK_END);
			if (!err)
			{
				size = hai_ftell(hfile);
				memset(arg, 0x00, sizeof(arg));
				hai_fseek(hfile, 0, SEEK_SET);
				if (size > 250)
					size = 250;
				if (hai_fread(arg, size, 1, hfile) == 1)
				{
					err = 0;
					printf(arg);
					printf("\n");
				}
				else
					err = -1;
			}
			break;
		case 9:
			scanf("%s", arg);
			if (hai_fwrite(arg, strlen(arg), 1, hfile) == 1)
				err = 0;
			else
				err = -1;
			break;
		case 10:
			if (hfile)
			{
				err = hai_fclose(hfile);
				if (!err)
					hfile = 0;
			}
			else
				err = -1;
			break;
		case 11:
			listdir();
			err = 0;
			break;
		case 12:
			scanf("%s", arg);
			err = hai_remove(arg);
			break;
		default:
			err = -1;
			break;
		}
		if (err)
			printf("bad command or argument!\n");
		else
			printf("OK!\n");
	}
	hai_flushall();
	return 0;
}

