
#include <unistd.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <cstdint>
#include <cstdio>
#include <ctime>



static const int MAIN_spi_port =  1;
static const int MAIN_pin_dc   = 12;
static const int MAIN_pin_cs   = 16;
static const int MAIN_pin_rst  = 18;

static const int MAIN_width    = 84;
static const int MAIN_height   = 48;

// p. 14
enum Cmd : uint8_t
{
	Cmd_FUNC_SET  = 0x20,

	// Function Set
	Cmd_PD        = 0x04,
	Cmd_V         = 0x02,
	Cmd_H         = 0x01,

	// H = 0
	Cmd_SET_X     = 0x80,
	Cmd_SET_Y     = 0x40,
	Cmd_DISP_CTRL = 0x08,

	// Display Control
	Cmd_BLANK     = 0x00,
	Cmd_NORMAL    = 0x04,
	Cmd_ALL_ON    = 0x01,
	Cmd_INV_VID   = 0x05,

	// H = 1
	Cmd_SET_VOP   = 0x80,
	Cmd_BIAS_SYS  = 0x10,
	Cmd_TEMP_CTRL = 0x04,
};


static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



static void MAIN_send_spi (uint8_t a)
{
	uint8_t        buffer [1] = { a };
	::wiringPiSPIDataRW (MAIN_spi_port, &buffer [0], 1);
}



static void MAIN_send_cmd (uint8_t c)
{
	::digitalWrite (MAIN_pin_dc, LOW);
	::digitalWrite (MAIN_pin_cs, LOW);
	MAIN_send_spi (c);
	::digitalWrite (MAIN_pin_cs, HIGH);
}



static void MAIN_send_data (uint8_t a)
{
	::digitalWrite (MAIN_pin_dc, HIGH);
	::digitalWrite (MAIN_pin_cs, LOW);
	MAIN_send_spi (a);
	::digitalWrite (MAIN_pin_cs, HIGH);
}



int main (int argc, char *argv [])
{
	int             ret_val = 0;

	::wiringPiSetupPhys ();

	::pinMode  (MAIN_pin_dc , OUTPUT);
	::pinMode  (MAIN_pin_cs , OUTPUT);
	::pinMode  (MAIN_pin_rst, OUTPUT);
	int            handle = ::wiringPiSPISetup  (MAIN_spi_port, 1000000);   // SPI clock: 1 MHz
	if (handle == -1)
	{
		ret_val = errno;
		printf ("Error initializing SPI. errno = %d\n", ret_val);
	}

	if (ret_val == 0)
	{
		// Unforunately the PCD8544 controller datasheet doesn't give
		// the timing for the reset pulse.
		::digitalWrite (MAIN_pin_rst, LOW);
		::delay (100);
		::digitalWrite (MAIN_pin_rst, HIGH);
		::delay (100);

		MAIN_send_cmd (Cmd_FUNC_SET  | Cmd_H);
		MAIN_send_cmd (Cmd_TEMP_CTRL | 0x00);  // See 7.8
		MAIN_send_cmd (Cmd_BIAS_SYS  | 0x03);  // See 8.8
		MAIN_send_cmd (Cmd_SET_VOP   | 0x3A);  // See 8.9, and modified manually
		MAIN_send_cmd (Cmd_FUNC_SET);
		MAIN_send_cmd (Cmd_DISP_CTRL | Cmd_NORMAL);
	}

	uint8_t         val = 0;
	while (ret_val == 0)
	{
		MAIN_send_cmd (Cmd_SET_X     | 0);
		MAIN_send_cmd (Cmd_SET_Y     | 0);
		for (int y = 0; y < (MAIN_height >> 3); ++y)
		{
			for (int x = 0; x < MAIN_width; ++x)
			{
				MAIN_send_data (val);
			}
		}
		MAIN_send_cmd (Cmd_SET_Y);
		++ val;

		::delay (1000);
	}

	if (handle != -1)
	{
		close (file);
	}

	return ret_val;
}
