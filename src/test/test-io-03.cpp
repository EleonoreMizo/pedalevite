
#include <unistd.h>

#include <wiringPi.h>

#include <cstdint>
#include <cstdio>
#include <ctime>



static const int64_t MAIN_antibounce_time = 30 * 1000 * 1000; // Nanoseconds



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}




template <int P>
static void interrupt_cb ()
{
	static int     reg_val    = 0;
	static int64_t reg_time   = 0;
	static int     bounce_cnt = 0;

	const int      val      = ::digitalRead (P);
	const int64_t  cur_time = MAIN_get_time ();
	const int64_t  dist     = cur_time - reg_time;
	if (dist < MAIN_antibounce_time)
	{
		++ bounce_cnt;
	}
	else
	{
		const int       bounce_cnt2 = bounce_cnt;
		if (val == reg_val)
		{
			printf ("Edge on pin %d, probably missed %d.\n", P, 1 - val);
		}
		reg_val  = val;
		reg_time = cur_time;
		bounce_cnt = 0;
		printf ("Edge on pin %d, reading %d, bounced %d times.\n", P, val, bounce_cnt2);
	}
}



template <int P>
static void config_pin ()
{
	::pinMode  (P, INPUT);
	::wiringPiISR (P, INT_EDGE_BOTH, &interrupt_cb <P>);
}

int main (int argc, char *argv [])
{
	::wiringPiSetupPhys ();

	config_pin <7> ();
	config_pin <22> ();

	while (true)
	{
		sleep (1);
	}

	return 0;
}
