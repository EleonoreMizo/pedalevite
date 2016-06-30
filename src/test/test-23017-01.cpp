
#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <array>

#include <cstdint>
#include <cstdio>
#include <ctime>



static const int     MAIN_pin_reset       = 18;
static const int     MAIN_pin_interrupt   = 8;
static const int     MAIN_nbr_dev         = 2;
static const std::array <int, MAIN_nbr_dev> MAIN_i2c_dev_23017_arr = {{ 0x20 + 0, 0x20 + 1 }};   // Slave address, p. 8
static const int64_t MAIN_antibounce_time = 30 * 1000 * 1000; // Nanoseconds
static const bool    MAIN_incremental_coder_flag = true;

// IOCON.BANK = 0
enum Cmd : uint8_t
{
	Cmd_IODIRA = 0x00,
	Cmd_IODIRB,
	Cmd_IPOLA,
	Cmd_IPOLB,
	Cmd_GPINTENA,
	Cmd_GPINTENB,
	Cmd_DEFVALA,
	Cmd_DEFVALB,
	Cmd_INTCONA,
	Cmd_INTCONB,
	Cmd_IOCONA,
	Cmd_IOCONB,
	Cmd_GPPUA,
	Cmd_GPPUB,
	Cmd_INTFA,
	Cmd_INTFB,
	Cmd_INTCAPA,
	Cmd_INTCAPB,
	Cmd_GPIOA,
	Cmd_GPIOB,
	Cmd_OLATA,
	Cmd_OLATB
};

enum IOCon : uint8_t
{
	IOCon_BANK   = 0x80,
	IOCon_MIRROR = 0x40,
	IOCon_SEQOP  = 0x20,
	IOCon_DISSLW = 0x10,
	IOCon_HAEN   = 0x08,
	IOCon_ODR    = 0x04,
	IOCon_INTPOL = 0x02
};



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



static void interrupt_cb ()
{
	printf ("Interrupt!\n");
}



class State
{
public:
	bool           _flag      = false;
	int64_t        _time_last = 0;
};

int main (int argc, char *argv [])
{
	int            ret_val = 0;

	::wiringPiSetupPhys ();

	::pinMode  (MAIN_pin_interrupt, INPUT );
	::pinMode  (MAIN_pin_reset    , OUTPUT);

	::digitalWrite (MAIN_pin_reset, LOW);
	::delay (100);
	::digitalWrite (MAIN_pin_reset, HIGH);
	::delay (100);

	std::array <int, MAIN_nbr_dev> handle_arr;
	for (int d = 0; d < MAIN_nbr_dev && ret_val == 0; ++d)
	{
		handle_arr [d] = ::wiringPiI2CSetup (MAIN_i2c_dev_23017_arr [d]);
		if (handle_arr [d] == -1)
		{
			ret_val = errno;
			printf ("Error initializing I2C. errno = %d\n", ret_val);
		}

		if (ret_val == 0)
		{
			::wiringPiI2CWriteReg8 (handle_arr [d], Cmd_IOCONA, IOCon_MIRROR);

			// All the pins are set in read mode.
			::wiringPiI2CWriteReg16 (handle_arr [d], Cmd_IODIRA, 0xFFFF);

			if (! MAIN_incremental_coder_flag)
			{
				// All the pins will cause an interrupt on input change
				::wiringPiI2CWriteReg16 (handle_arr [d], Cmd_INTCONA , 0x0000);
				::wiringPiI2CWriteReg16 (handle_arr [d], Cmd_GPINTENA, 0xFFFF);

				::wiringPiISR (MAIN_pin_interrupt, INT_EDGE_BOTH, &interrupt_cb);
			}
		}
	}

	std::array <std::array <State, 16>, MAIN_nbr_dev>  state_arr;
	while (ret_val == 0)
	{
		// Update with anti-bounce system
		for (int d = 0; d < MAIN_nbr_dev; ++d)
		{
			uint16_t       state_all = ::wiringPiI2CReadReg16 (handle_arr [d], Cmd_GPIOA);
			const int64_t  cur_time  = MAIN_get_time ();

			for (int i = 0; i < 16; ++i)
			{
				State &        state_prev = state_arr [d] [i];
				const bool     cur_flag   = (((state_all >> i) & 1) != 0);
				if (cur_flag != state_prev._flag)
				{
					const int64_t  dist = cur_time - state_prev._time_last;
					if (MAIN_incremental_coder_flag || dist >= MAIN_antibounce_time)
					{
						printf ("Dev %02x, pin %2d, value %d\n", MAIN_i2c_dev_23017_arr [d], i, cur_flag ? 1 : 0);
						state_prev._flag      = cur_flag;
						state_prev._time_last = cur_time;
					}
				}
			}
		}

		if (! MAIN_incremental_coder_flag)
		{
			::delay (20);
		}
	}

	for (int d = 0; d < MAIN_nbr_dev; ++d)
	{
		if (handle_arr [d] != -1)
		{
			close (handle_arr [d]);
		}
	}

	return ret_val;
}
