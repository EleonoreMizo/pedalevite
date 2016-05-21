
// g++ --std=c++11 -l wiringPi test-7920.cpp

#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <cstdint>
#include <cstdio>
#include <ctime>



static const int MAIN_pin_dc  = 12;
static const int MAIN_pin_cs  = 16;
static const int MAIN_pin_rst = 18;

static const int MAIN_width   = 128;
static const int MAIN_height  = 64;

static const int MAIN_spi_rate = 1000 * 1000;
static const int MAIN_spi_port = 1;
static const int MAIN_delay_std = 101;



// p. 16
enum Cmd : uint8_t
{
	// Basic commands (RE=0)
	Cmd_CLEAR         = 0x01,
	Cmd_HOME          = 0x02,
	Cmd_ENTRY         = 0x04,
	Cmd_DISPLAY       = 0x08,
	Cmd_CURS_CTRL     = 0x10,
	Cmd_FNC_SET       = 0x20,
	Cmd_CGRAM_ADR     = 0x40,
	Cmd_DDRAM_ADR     = 0x80,

	// Extended commands (RE=1)
	Cmd_STANDBY       = 0x01,
	Cmd_RAM_SEL       = 0x02,
	Cmd_SCROLL        = 0x03,
	Cmd_REVERSE       = 0x04,
	Cmd_FNC_SET_E     = 0x24, // With RE flag set
	Cmd_IRAM_ADR      = 0x40,
	Cmd_SCROLL_ADR    = 0x40,
	Cmd_GDRAM_ADR     = 0x80,

	// Cmd_ENTRY
	Cmd_ENTRY_S       = 0x01,
	Cmd_ENTRY_ID      = 0x02,

	// Cmd_DISPLAY
	Cmd_DISPLAY_BLINK = 0x01,
	Cmd_DISPLAY_CURS  = 0x02,
	Cmd_DISPLAY_DISP  = 0x04,

	// Cmd_CURS_CTRL
	Cmd_CURS_CTRL_RL  = 0x04,
	Cmd_CURS_CTRL_SC  = 0x08,

	// Cmd_FNC_SET / Cmd_FNC_SET_E
	Cmd_FNC_SET_RE    = 0x04,
	Cmd_FNC_SET_DL    = 0x10,

	// Cmd_FNC_SET_E
	Cmd_FNC_SET_E_G   = 0x02,
};

// p. 26
enum Serial : uint8_t
{
	Serial_HEADER     = 0xF8,
	Serial_RW         = 0x04,
	Serial_RS         = 0x02
};



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}




#if 0

static void	MAIN_send_byte_raw (uint8_t a)
{
	uint8_t        buffer [2] =
	{
		uint8_t (a & 0xF0),
		uint8_t (a << 4)
	};
	::wiringPiSPIDataRW (MAIN_spi_port, &buffer [0], sizeof (buffer));
}



static void	MAIN_send_bytes_header (uint8_t rwrs, uint8_t a)
{
	uint8_t        buffer [3] =
	{
		uint8_t (Serial_HEADER | rwrs),
		uint8_t (a & 0xF0),
		uint8_t (a << 4)
	};
	::wiringPiSPIDataRW (MAIN_spi_port, &buffer [0], sizeof (buffer));
}



static void	MAIN_send_cmd (uint8_t x)
{
	::digitalWrite (MAIN_pin_cs, HIGH);
	MAIN_send_bytes_header (0, x);
	::digitalWrite (MAIN_pin_cs, LOW);

	::delayMicroseconds (MAIN_delay_std);
}



static void	MAIN_send_data (uint8_t x)
{
	::digitalWrite (MAIN_pin_cs, HIGH);
	MAIN_send_bytes_header (Serial_RS, x);
	::digitalWrite (MAIN_pin_cs, LOW);

	::delayMicroseconds (MAIN_delay_std);
}

#else

static void	MAIN_send_byte (uint8_t x)
{
	::digitalWrite (MAIN_pin_cs, HIGH);
	::delayMicroseconds (MAIN_delay_std);
	uint8_t        buffer [1] = { x };
	::wiringPiSPIDataRW (MAIN_spi_port, &buffer [0], sizeof (buffer));
	::digitalWrite (MAIN_pin_cs, LOW);
}



static void	MAIN_send_3bytes (uint8_t rwrs, uint8_t a)
{
		MAIN_send_byte (Serial_HEADER | rwrs);
		MAIN_send_byte (a & 0xF0);
		MAIN_send_byte (a << 4);
}



static void	MAIN_send_cmd (uint8_t x)
{
	MAIN_send_3bytes (0, x);
}



static void	MAIN_send_data (uint8_t x)
{
	MAIN_send_3bytes (Serial_RS, x);
}

#endif



int main (int argc, char *argv [])
{
	::wiringPiSetupPhys ();

	int             handle = ::wiringPiSPISetupMode (MAIN_spi_port, MAIN_spi_rate, 3);

	::pinMode  (MAIN_pin_dc , OUTPUT);
	::pinMode  (MAIN_pin_cs , OUTPUT);
	::pinMode  (MAIN_pin_rst, OUTPUT);

	::digitalWrite (MAIN_pin_rst, LOW);
	::delay (100);
	::digitalWrite (MAIN_pin_rst, HIGH);
	::delay (100);

	MAIN_send_cmd (0x30);
	MAIN_send_cmd (0x0C);
	MAIN_send_cmd (0x01);
	MAIN_send_cmd (0x06);

	MAIN_send_cmd (0x80);

	uint8_t         val = 33;
	while (true)
	{
		for (int y = 0; y < (MAIN_height >> 3); ++y)
		{
			for (int x = 0; x < MAIN_width; ++x)
			{
				MAIN_send_data (val);
				++ val;
			}
		}

		sleep (1);
	}
	
	close (handle);

	return 0;
}

