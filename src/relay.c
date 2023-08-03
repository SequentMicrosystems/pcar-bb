#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pcar.h"
#include "comm.h"
#include "thread.h"

int relayChSet(int dev, u8 channel, OutStateEnumType state)
{
	int resp = 0;
	u8 buff[2];

	if ( (channel < CHANNEL_NR_MIN) || (channel > RELAY_CH_NO))
	{
		printf("Invalid relay nr!\n");
		return ERROR;
	}

   buff[0] = channel;
	switch (state)
	{
	case OFF:
		resp = i2cMem8Write(dev, I2C_MEM_OD_CLR, buff, 1);
		break;
	case ON:
		resp = i2cMem8Write(dev, I2C_MEM_OD_SET, buff, 1);
		break;
	default:
		printf("Invalid relay state!\n");
		return ERROR;
		break;
	}
	return resp;
}

int relayChGet(int dev, u8 channel, OutStateEnumType* state)
{
	u8 buff[2];
	u16 relays = 0;

	if (NULL == state)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > RELAY_CH_NO))
	{
		printf("Invalid relay nr!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, I2C_MEM_OD_VAL, buff, 2))
	{
		return ERROR;
	}
	memcpy(&relays, buff, 2);

	if (relays & (1 << (channel - 1)))
	{
		*state = ON;
	}
	else
	{
		*state = OFF;
	}
	return OK;
}

int relaySet(int dev, int val)
{
	u8 buff[2];
 int ret = 0;

	memcpy(buff, &val, 2);

	ret =  i2cMem8Write(dev, I2C_MEM_OD_VAL, buff, 1);
 if (ret != 0)
 {
 return ret;
 }
 ret = i2cMem8Write(dev, I2C_MEM_OD_VAL+1, &buff[1], 1);
 return ret;
 
}

int relayGet(int dev, int* val)
{
	u8 buff[2];
 u16 temp = 0;

	if (NULL == val)
	{
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, I2C_MEM_OD_VAL, buff, 2))
	{
		return ERROR;
	}
	memcpy(&temp, buff, 2);
 *val = temp;
	return OK;
}

static int doRelayWrite(int argc, char *argv[]);
const CliCmdType CMD_RELAY_WRITE =
{
	"outwr",
	1,
	&doRelayWrite,
	"\toutwr:		Set outpus (switch) On/Off\n",
	"\tUsage:		pcar outwr <channel[1..4]> <0/1>\n",
	"\tUsage:		pcar outwr <value[0..15]>\n",
	"\tExample:		pcar outwr 2 1; Turn output #2 On\n"};

static int doRelayWrite(int argc, char *argv[])
{
	int pin = 0;
	OutStateEnumType state = STATE_COUNT;
	int val = 0;
	int dev = 0;
	OutStateEnumType stateR = STATE_COUNT;
	int valR = 0;
	int retry = 0;

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
		if ( (pin < CHANNEL_NR_MIN) || (pin > RELAY_CH_NO))
		{
			printf("Output channel number value out of range\n");
			return ARG_RANGE_ERROR;
		}

		/**/if ( (strcasecmp(argv[3], "up") == 0)
			|| (strcasecmp(argv[3], "on") == 0))
			state = ON;
		else if ( (strcasecmp(argv[3], "down") == 0)
			|| (strcasecmp(argv[3], "off") == 0))
			state = OFF;
		else
		{
			if ( (atoi(argv[3]) >= STATE_COUNT) || (atoi(argv[3]) < 0))
			{
				printf("Invalid output state!\n");
				return ARG_RANGE_ERROR;
			}
			state = (OutStateEnumType)atoi(argv[3]);
		}

		retry = RETRY_TIMES;

		while ( (retry > 0) && (stateR != state))
		{
     
			if (OK != relayChSet(dev, pin, state))
			{
				printf("Fail to write output\n");
				return IO_ERROR;
			}
			if (OK != relayChGet(dev, pin, &stateR))
			{
				printf("Fail to read output\n");
				return IO_ERROR;
			}
			retry--;
		}
#ifdef DEBUG_I
		if(retry < RETRY_TIMES)
		{
			printf("retry %d times\n", 3-retry);
		}
#endif
		if (retry == 0)
		{
			printf("Fail to write output\n");
			return IO_ERROR;
		}
		return OK;
	}
	else
	{
		val = atoi(argv[2]);
		if (val < 0 || val > 0xffff)
		{
			printf("Invalid outputs value\n");
			return ARG_RANGE_ERROR;
		}

		retry = RETRY_TIMES;
		valR = -1;
		while ( (retry > 0) && (valR != val))
		{
      if(retry != RETRY_TIMES)
      {
         printf("write %d, read %d\n", val, valR);
      }
			if (OK != relaySet(dev, val))
			{
				printf("Fail to write outputs!\n");
				return IO_ERROR;
			}
      busyWait(20);
			if (OK != relayGet(dev, &valR))
			{
				printf("Fail to read outputs!\n");
				return IO_ERROR;
			}
      
      busyWait(20);
			retry --;
		}
		if (retry == 0)
		{
			printf("Fail to write outputs retry expires!\n");
			return IO_ERROR;
		}
		return OK;
	}
	return ARG_CNT_ERR;
}

static int doRelayRead(int argc, char *argv[]);
const CliCmdType CMD_RELAY_READ =
{
	"outrd",
	1,
	&doRelayRead,
	"\toutrd:		Read relays  status\n",
	"\tUsage:		pcar outrd <channel[1..4]>\n",
	"\tUsage:		pcar outrd\n",
	"\tExample:		pcar outrd 2; Read Status of Relay #2 \n"};

static int doRelayRead(int argc, char *argv[])
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
		if ( (pin < CHANNEL_NR_MIN) || (pin > RELAY_CH_NO))
		{
			printf("Output channel number value out of range!\n");
			return ERROR;
		}

		if (OK != relayChGet(dev, pin, &state))
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
		if (OK != relayGet(dev, &val))
		{
			printf("Fail to read!\n");
			return IO_ERROR;
		}
		printf("%d\n", val);
		return OK;
	}
	return ARG_CNT_ERR;
}


