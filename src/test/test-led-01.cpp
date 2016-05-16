
#include <unistd.h>

#include <wiringPi.h>

#include <cstdint>
#include <cstdio>
#include <ctime>



static const int  MAIN_nbr_led = 3;
static const int  MAIN_led_pin_arr [MAIN_nbr_led] = { 11, 13, 15};



static int64_t MAIN_get_time ()
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



int main (int argc, char *argv [])
{
	::wiringPiSetupPhys ();

	for (int i = 0; i < MAIN_nbr_led; ++i)
	{
		::pinMode  (MAIN_led_pin_arr [i], OUTPUT);
		::digitalWrite (MAIN_led_pin_arr [i], LOW);
	}

	int            active = 0;
	double         t      = 1000;
	double         r      = 0.99;
	while (true)
	{
		::digitalWrite (MAIN_led_pin_arr [active], HIGH);
		::delay (int (t + 0.5));
		::digitalWrite (MAIN_led_pin_arr [active], LOW);

		active = (active + 1) % MAIN_nbr_led;
		t *= r;
		if (t <= 1 || t >= 1000)
		{
			r = 1 / r;
		}
	}

	return 0;
}
