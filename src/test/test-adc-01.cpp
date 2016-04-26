
// gpio load spi
// ls -l /dev/spi*
// -> /dev/spidev0.0 and /dev/spidev0.1

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <errno.h>
#include <unistd.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <ctime>




static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



// Returns -1 on error
// Valid results are in range 0-1023
static int MAIN_read_adc (int port, int chn)
{
	assert (port >= 0);
	assert (port < 2);
	assert (chn >= 0);
	assert (chn < 8);

	// MCP doc, p. 21
	// We could shift everything from 7 bits to improve the latency.
	const int      msg_len = 3;
	uint8_t        buffer [msg_len] =
	{
		0x01,
		uint8_t (chn << 4),
		0
	};

	int            ret_val = ::wiringPiSPIDataRW (port, &buffer [0], msg_len);
	if (ret_val == -1)
	{
		printf ("Error reading the ADC. errno = %d\n", errno);
	}
	else
	{
		ret_val = ((buffer [1] & 3) << 8) + buffer [2];
	}

	return (ret_val);
}



int main (int argc, char *argv [])
{
	int            ret_val = 0;

	int            port = 0;
	int            chn  = 0;

	::wiringPiSetupPhys ();
	int            file = ::wiringPiSPISetup  (port, 1000000);   // SPI clock: 1 MHz
	if (file == -1)
	{
		ret_val = errno;
		printf ("Error initializing SPI. errno = %d\n", ret_val);
	}

	while (ret_val == 0)
	{
		const int      val = MAIN_read_adc (port, chn);
		if (val < 0)
		{
			ret_val = -1;
		}
		else
		{
			printf ("\rPort %d, channel %d: 0x%03X", port, chn, val);
			fflush (stdout);
			::delay (250); // ms
		}
	}

	if (file != -1)
	{
		close (file);
	}

	return ret_val;
}
