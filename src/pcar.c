#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pcar.h"
#include "comm.h"
#include "thread.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)1

char *warranty =
	"	       Copyright (c) 2016-2023 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";

void usage(void)
{
	int i = 0;
	while (gCmdArray[i] != NULL)
	{
		if (gCmdArray[i]->name != NULL)
		{
			if (strlen(gCmdArray[i]->usage1) > 2)
			{
				printf("%s", gCmdArray[i]->usage1);
			}
			if (strlen(gCmdArray[i]->usage2) > 2)
			{
				printf("%s", gCmdArray[i]->usage2);
			}
		}
		i++;
	}
	printf("Type pcar -h <command> for more help\n");
}

int doBoardInit(void)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	add = SLAVE_OWN_ADDRESS_BASE;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, I2C_MEM_REVISION_MAJOR, buff, 1))
	{
		printf("Perimeter Patrol Controller card did not detected!\n");
		return ERROR;
	}
	return dev;
}

int printInfo(int dev)
{
	uint8_t buff[8];
	uint16_t aux16;
	float vBigle = 0;
	float vIn = 0;

	if (ERROR == i2cMem8Read(dev, I2C_MEM_DIAG_TEMPERATURE, buff, 5))
	{
		printf("Err read\n");
		return ERROR;
	}
	memcpy(&aux16, &buff[1], 2);
	vIn = (float)aux16 / 1000;

	memcpy(&aux16, &buff[3], 2);
	vBigle = (float)aux16 / 1000;

	printf("Vin %.2fV, Beagle %.2fV Temperature %.1fF\n", vIn, vBigle,
		buff[0] * 1.8 + 32);

	return OK;
}

int setFan(int dev, int enable)
{
	u8 buff[2] ;

	if (enable)
	{
		buff[0] = 1;
	}
	else
	{
		buff[0] = 0;
	}
	return i2cMem8Write(dev, I2C_MEM_FAN, buff, 1);
}

int boardCheck(void)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	add = SLAVE_OWN_ADDRESS_BASE;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, I2C_MEM_REVISION_MAJOR, buff, 2))
	{
		return ERROR;
	}
	printf("Perimeter Patrol Controller found FW_VER %d.%d\n", buff[0], buff[1]);
	printInfo(dev);
	return OK;
}

static int doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP = {"-h", 1, &doHelp,
	"\t-h		Display the list of command options or one command option details\n",
	"\tUsage:		pcar -h    Display command options list\n",
	"\tUsage:		pcar -h <param>   Display help for <param> command option\n",
	"\tExample:		pcar -h rread    Display help for \"rread\" command option\n"};

static int doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				if (strcasecmp(argv[2], gCmdArray[i]->name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i]->help, gCmdArray[i]->usage1,
						gCmdArray[i]->usage2, gCmdArray[i]->example);
					break;
				}
			}
			i++;
		}
		if (NULL == gCmdArray[i])
		{
			printf("Option \"%s\" not found\n", argv[2]);
			i = 0;
			while (NULL != gCmdArray[i])
			{
				if (gCmdArray[i]->name != NULL)
				{
					printf("%s", gCmdArray[i]->help);
					break;
				}
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				printf("%s", gCmdArray[i]->help);
			}
			i++;
		}
	}
	return OK;
}

static int doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION = {"-v", 1, &doVersion,
	"\t-v		Display the pcar command version number\n", "\tUsage:		pcar -v\n", "",
	"\tExample:		pcar -v  Display the version number\n"};

static int doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("pcar v%d.%d.%d Copyright (c) 2016 - 2023 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: pcar -warranty\n");
	return OK;
}

static int doWarranty(int argc, char *argv[]);
const CliCmdType CMD_WAR = {"-warranty", 1, &doWarranty,
	"\t-warranty	Display the warranty\n", "\tUsage:		pcar -warranty\n", "",
	"\tExample:		pcar -warranty  Display the warranty text\n"};

static int doWarranty(int argc UNU, char* argv[] UNU)
{
	printf("%s\n", warranty);
	return OK;
}

static int doList(int argc, char *argv[]);
const CliCmdType CMD_LIST = {"-list", 1, &doList,
	"\t-list:		Show if a Perimeter Patrol Controller  is detected \n",
	"\tUsage:		pcar -list\n", "", "\tExample:		pcar -list display: 1,0 \n"};

int doList(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	if (boardCheck() == OK)
	{

	}
	else
	{
		printf("Perimeter Patrol Controller did not detected!\n");
	}

	return OK;
}

static int doFanWrite(int argc, char *argv[]);
const CliCmdType CMD_FAN_WRITE = {"fanwr", 1, &doFanWrite,
	"\tfanwr:		Set Fan On/Off\n", "\tUsage:		pcar fanwr <0/1>\n", "",
	"\tExample:		pcar fanwr 1; Turn On the fan\n"};

static int doFanWrite(int argc, char *argv[])
{
	int val = 0;
	int dev = 0;

	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		return ERROR;
	}
	val = atoi(argv[2]);

	return setFan(dev, val);
}

const CliCmdType *gCmdArray[] = {&CMD_VERSION, &CMD_HELP, &CMD_WAR, &CMD_LIST,
	&CMD_RELAY_READ, &CMD_RELAY_WRITE, &CMD_IN_READ, &CMD_CFG_COUNT_READ,
	&CMD_CFG_COUNT_WRITE, &CMD_COUNT_READ, &CMD_COUNT_RESET, &CMD_FAN_WRITE,
	NULL}; //null terminated array of cli structure pointers

int main(int argc, char *argv[])
{
	int i = 0;
	int ret = OK;

	if (argc == 1)
	{
		usage();
		return -1;
	}
	while (NULL != gCmdArray[i])
	{
		if ( (gCmdArray[i]->name != NULL) && (gCmdArray[i]->namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i]->namePos], gCmdArray[i]->name) == 0)
			{
				ret = gCmdArray[i]->pFunc(argc, argv);
				if (ret == ARG_CNT_ERR)
				{
					printf("Invalid parameters number!\n");
					printf("%s", gCmdArray[i]->usage1);
					if (strlen(gCmdArray[i]->usage2) > 2)
					{
						printf("%s", gCmdArray[i]->usage2);
					}
				}
				return ret;
			}
		}
		i++;
	}
	printf("Invalid command option\n");
	usage();

	return -1;
}

