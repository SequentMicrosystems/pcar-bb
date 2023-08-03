#ifndef REL4IN4_H_
#define REL4IN4_H_

#include <stdint.h>

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define CHANNEL_NR_MIN 1
#define RELAY_CH_NO 16
#define OD_SIZE_B 2

#define IN_CH_NO 8
#define COUNT_SIZE 4
#define ENC_COUNT_SIZE 4
#define ENC_NO 2
#define SCAN_FREQ_SIZE 2
#define PWM_IN_FILL_SIZE 2
#define PWM_IN_FILL_SCALE ((float)100)
#define IN_FREQENCY_SIZE 2
#define RETRY_TIMES	10


enum
{
	I2C_MEM_OD_VAL = 0,
	I2C_MEM_OD_SET = I2C_MEM_OD_VAL + OD_SIZE_B,
	I2C_MEM_OD_CLR,
	I2C_MEM_OPTO_IN_VAL,

	I2C_MEM_WDT_RESET_ADD,
	I2C_MEM_WDT_INTERVAL_SET_ADD,
	I2C_MEM_WDT_INTERVAL_GET_ADD = I2C_MEM_WDT_INTERVAL_SET_ADD + 2,
	I2C_MEM_WDT_INIT_INTERVAL_SET_ADD = I2C_MEM_WDT_INTERVAL_GET_ADD + 2,
	I2C_MEM_WDT_INIT_INTERVAL_GET_ADD = I2C_MEM_WDT_INIT_INTERVAL_SET_ADD + 2,
	I2C_MEM_WDT_RESET_COUNT_ADD = I2C_MEM_WDT_INIT_INTERVAL_GET_ADD + 2,
	I2C_MEM_WDT_CLEAR_RESET_COUNT_ADD = I2C_MEM_WDT_RESET_COUNT_ADD + 2,
	I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD,
	I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD = I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD
		+ 4,
	I2C_MEM_OPTO_RISING_ENABLE = I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD + 4,
	I2C_MEM_OPTO_FALLING_ENABLE,
	I2C_MEM_OPTO_CH_CONT_RESET,
	I2C_MEM_OPTO_COUNT1,
	I2C_MEM_OPTO_COUNT2 = I2C_MEM_OPTO_COUNT1 + COUNT_SIZE,
	I2C_MEM_OPTO_COUNT3 = I2C_MEM_OPTO_COUNT2 + COUNT_SIZE,
	I2C_MEM_OPTO_COUNT4 = I2C_MEM_OPTO_COUNT3 + COUNT_SIZE,
	I2C_MEM_OPTO_COUNT5 = I2C_MEM_OPTO_COUNT4 + COUNT_SIZE,
	I2C_MEM_OPTO_COUNT6 = I2C_MEM_OPTO_COUNT5 + COUNT_SIZE,
	I2C_MEM_OPTO_COUNT7 = I2C_MEM_OPTO_COUNT6 + COUNT_SIZE,
	I2C_MEM_OPTO_COUNT8 = I2C_MEM_OPTO_COUNT7 + COUNT_SIZE,
	I2C_MEM_DIAG_TEMPERATURE = 0x72,
	I2C_MEM_DIAG_12V,
	I2C_MEM_DIAG_12V_1,
	I2C_MEM_DIAG_5V,
	I2C_MEM_DIAG_5V_1,
	I2C_MEM_FAN,


	I2C_MEM_REVISION_MAJOR = 0x78,
	I2C_MEM_REVISION_MINOR,
	I2C_MEM_DIAG_3V,
	I2C_MEM_DIAG_3V_1,

	SLAVE_BUFF_SIZE = 255
};

#define ERROR	-1
#define OK		0
#define FAIL	-1
#define ARG_CNT_ERR -2
#define ARG_RANGE_ERROR -3
#define IO_ERROR -4

#define SLAVE_OWN_ADDRESS_BASE 0x55

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef enum
{
	OFF = 0,
	ON,
	STATE_COUNT
} OutStateEnumType;

typedef struct
{
	const char *name;
	const int namePos;
	int (*pFunc)(int, char**);
	const char *help;
	const char *usage1;
	const char *usage2;
	const char *example;
} CliCmdType;


int doBoardInit(void);


extern const CliCmdType CMD_RELAY_TEST;
extern const CliCmdType CMD_RELAY_READ;
extern const CliCmdType CMD_RELAY_WRITE;

extern const CliCmdType CMD_IN_READ;
extern const CliCmdType CMD_AC_IN_READ;
extern const CliCmdType CMD_CFG_COUNT_READ;
extern const CliCmdType CMD_CFG_COUNT_WRITE;
extern const CliCmdType CMD_COUNT_READ;
extern const CliCmdType CMD_COUNT_RESET;
extern const CliCmdType CMD_COUNT_PPS_READ;

extern const CliCmdType *gCmdArray[];

#endif
