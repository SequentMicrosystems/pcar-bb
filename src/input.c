#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pcar.h"
#include "comm.h"
#include "thread.h"

typedef enum
{
	COUNT_EDGES = 0,
	COUNT_ENCODER,
} COUNT_TYPE;

int inChGet(int dev, u8 channel, OutStateEnumType *state)
{
	u8 buff[2];

	if (NULL == state)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > IN_CH_NO))
	{
		printf("Invalid input nr!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, I2C_MEM_OPTO_IN_VAL, buff, 1))
	{
		return ERROR;
	}

	if (buff[0] & (1 << (channel - 1)))
	{
		*state = ON;
	}
	else
	{
		*state = OFF;
	}
	return OK;
}

int inGet(int dev, int *val)
{
	u8 buff[2];

	if (NULL == val)
	{
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, I2C_MEM_OPTO_IN_VAL, buff, 1))
	{
		return ERROR;
	}
	*val = buff[0];
	return OK;
}



static int doInRead(int argc, char *argv[]);
const CliCmdType CMD_IN_READ =
{
	"inrd",
	1,
	&doInRead,
	"\tinrd:		Read inputs  status\n",
	"\tUsage:		pcar inrd <channel[1..8]>\n",
	"\tUsage:		pcar inrd\n",
	"\tExample:		pcar inrd 2; Read Status of Input channel #2 \n"};

static int doInRead(int argc, char *argv[])
{
	int pin = 0;
	int val = 0;
	int dev = 0;
	OutStateEnumType state = STATE_COUNT;

	dev = doBoardInit();
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 3)
	{
		pin = atoi(argv[2]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > 8))
		{
			printf("Input channel number value out of range!\n");
			return ERROR;
		}

		if (OK != inChGet(dev, pin, &state))
		{
			printf("Fail to read!\n");
			return IO_ERROR;
		}
		if (state != 0)
		{
			printf("1\n");
		}
		else
		{
			printf("0\n");
		}
		return OK;
	}
	else if (argc == 2)
	{
		if (OK != inGet(dev, &val))
		{
			printf("Fail to read!\n");
			return IO_ERROR;
		}
		printf("%d\n", val);
		return OK;
	}
	return ARG_CNT_ERR;
}


//==================== COUNTERS ==========================

int cfgCntChGet(int dev, u8 channel, OutStateEnumType *state)
{
	u8 buff[2];
	int chMax = IN_CH_NO;
	int add = I2C_MEM_OPTO_RISING_ENABLE;

	if (NULL == state)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > chMax))
	{
		printf("Invalid input nr!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, add, buff, 1))
	{
		return ERROR;
	}

	if (buff[0] & (1 << (channel - 1)))
	{
		*state = ON;
	}
	else
	{
		*state = OFF;
	}
	return OK;
}

int cfgCntGet(int dev, int *val)
{
	u8 buff[2];
	int add = I2C_MEM_OPTO_RISING_ENABLE;

	if (NULL == val)
	{
		return ERROR;
	}

	if (OK != i2cMem8Read(dev, add, buff, 1))
	{
		return ERROR;
	}
	*val = buff[0];
	return OK;
}

int cfgCntChSet(int dev, u8 channel, OutStateEnumType state)
{
	u8 buff[2];
	int chMax = IN_CH_NO;
	int add = I2C_MEM_OPTO_RISING_ENABLE;


	if ( (channel < CHANNEL_NR_MIN) || (channel > chMax))
	{
		printf("Invalid input nr!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, add, buff, 1))
	{
		return ERROR;
	}
	if (state)
	{
		buff[0] |= 1 << (channel - 1);
	}
	else
	{
		buff[0] &= ~ (u8)(1 << (channel - 1));
	}

	return i2cMem8Write(dev, add, buff, 1);
}

int cfgCntSet(int dev, int val)
{
	u8 buff[2];
	int add = I2C_MEM_OPTO_RISING_ENABLE;

	buff[0] = (u8)val;
	return i2cMem8Write(dev, add, buff, 1);

}

int cntGet(int dev, u8 channel, int *val)
{
	u8 buff[COUNT_SIZE];
	uint32_t uContor = 0;
	int chMax = IN_CH_NO;
	int add = I2C_MEM_OPTO_COUNT1;

	if (NULL == val)
	{
		return ERROR;
	}
	if ( (channel < CHANNEL_NR_MIN) || (channel > chMax))
	{
		printf("Invalid input nr!\n");
		return ERROR;
	}
	if (OK
		!= i2cMem8Read(dev, add + (channel - 1) * COUNT_SIZE, buff, COUNT_SIZE))
	{
		return ERROR;
	}
	memcpy(&uContor, buff, COUNT_SIZE);
	*val = uContor;
	return OK;
}

int cntReset(int dev, u8 channel)
{
	u8 buff[2];
	int chMax = IN_CH_NO;
	int add = I2C_MEM_OPTO_CH_CONT_RESET;

	if ( (channel < CHANNEL_NR_MIN) || (channel > chMax))
	{
		printf("Invalid input nr!\n");
		return ERROR;
	}

	buff[0] = channel;
	return i2cMem8Write(dev, add, buff, 1);
}

static int doCfgCountRead(int argc, char *argv[]);
const CliCmdType CMD_CFG_COUNT_READ =
	{
		"cfgcntrd",
		1,
		&doCfgCountRead,
		"\tcfgcntrd:		Read the configuration of the count function\n",
		"\tUsage:		pcar  cfgcntrd <channel[1..8]>\n",
		"\tUsage:		pcar  cfgcntrd\n",
		"\tExample:		pcar cfgcntrd 2; Read configuration for counting on Input channel #2 coud be 0/1 -> disabled/enabled \n"};

static int doCfgCountRead(int argc, char *argv[])
{
	int pin = 0;
	int val = 0;
	int dev = 0;
	OutStateEnumType state = STATE_COUNT;

	dev = doBoardInit();
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 3)
	{
		pin = atoi(argv[2]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > IN_CH_NO))
		{
			printf("Input channel number value out of range!\n");
			return ERROR;
		}

		if (OK != cfgCntChGet(dev, pin, &state))
		{
			printf("Fail to read!\n");
			return IO_ERROR;
		}
		if (state != 0)
		{
			printf("1\n");
		}
		else
		{
			printf("0\n");
		}
		return OK;
	}
	else if (argc == 2)
	{
		if (OK != cfgCntGet(dev, &val))
		{
			printf("Fail to read!\n");
			return IO_ERROR;
		}
		printf("%d\n", val);
		return OK;
	}
	return ARG_CNT_ERR;
}

static int doCfgCountWrite(int argc, char *argv[]);
const CliCmdType CMD_CFG_COUNT_WRITE =
	{
		"cfgcntwr",
		1,
		&doCfgCountWrite,
		"\tcfgcntwr:		Set counting function On/Off\n",
		"\tUsage:		pcar  cfgcntwr <channel[1..8]> <0/1>\n",
		"\tUsage:		pcar  cfgcntwr <value[0..255]>\n",
		"\tExample:		pcar 0 cfgcntwr 2 1; Enable counting function on input channel #2\n"};

static int doCfgCountWrite(int argc, char *argv[])
{
	int pin = 0;
	OutStateEnumType state = STATE_COUNT;
	int val = 0;
	int dev = 0;

	if ( (argc != 4) && (argc != 3))
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return ERROR;
	}
	if (argc == 4)
	{
		pin = atoi(argv[2]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > IN_CH_NO))
		{
			printf("Input channel number out of range\n");
			return ARG_RANGE_ERROR;
		}

		if ( (atoi(argv[3]) >= STATE_COUNT) || (atoi(argv[3]) < 0))
		{
			printf("Invalid state for counting enable (0/1)!\n");
			return ARG_RANGE_ERROR;
		}
		state = (OutStateEnumType)atoi(argv[3]);

		if (OK != cfgCntChSet(dev, pin, state))
		{
			printf("Fail to write counting config! \n");
			return IO_ERROR;
		}

		return OK;
	}
	else
	{
		val = atoi(argv[2]);
		if (val < 0 || val > 0xff)
		{
			printf("Invalid config value [0-255]!\n");
			return ARG_RANGE_ERROR;
		}

		if (OK != cfgCntSet(dev, val))
		{
			printf("Fail to write configuration register!\n");
			return IO_ERROR;
		}

		return OK;
	}
	return ARG_CNT_ERR;
}

static int doCountRead(int argc, char *argv[]);
const CliCmdType CMD_COUNT_READ =
{
	"cntrd",
	1,
	&doCountRead,
	"\tcntrd:		Read the counter of one input channel\n",
	"\tUsage:		pcar  cntrd <channel[1..8]>\n",
	"",
	"\tExample:		pcar cntrd 2; Read conter for Input channel #2  \n"};

static int doCountRead(int argc, char *argv[])
{
	int pin = 0;
	int val = 0;
	int dev = 0;

	dev = doBoardInit();
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 3)
	{
		pin = atoi(argv[2]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > IN_CH_NO))
		{
			printf("Input channel number value out of range!\n");
			return ERROR;
		}

		if (OK != cntGet(dev, pin, &val))
		{
			printf("Fail to read!\n");
			return IO_ERROR;
		}

		printf("%d\n", val);

		return OK;
	}

	return ARG_CNT_ERR;
}

static int doCountReset(int argc, char *argv[]);
const CliCmdType CMD_COUNT_RESET =
{
	"cntrst",
	1,
	&doCountReset,
	"\tcntrst:		Reset the counter of one input channel\n",
	"\tUsage:		pcar  cntrst <channel[1..8]>\n",
	"",
	"\tExample:		pcar cntrst 2; Reset the conter for Input channel #2\n"};

static int doCountReset(int argc, char *argv[])
{
	int pin = 0;
	int dev = 0;

	dev = doBoardInit();
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 3)
	{
		pin = atoi(argv[2]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > IN_CH_NO))
		{
			printf("Input channel number value out of range!\n");
			return ERROR;
		}

		if (OK != cntReset(dev, pin))
		{
			printf("Fail to write!\n");
			return IO_ERROR;
		}

		return OK;
	}

	return ARG_CNT_ERR;
}


