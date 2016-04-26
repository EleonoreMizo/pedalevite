
#include <unistd.h>

#include <wiringPi.h>

#include <cstdio>



static const int  MAIN_pin = 7;



static void interrupt_cb ()
{
	const int      val = ::digitalRead (MAIN_pin);
	printf ("Edge on pin %d, reading %d.\n", MAIN_pin, val);
}



int main (int argc, char *argv [])
{
	::wiringPiSetupPhys ();

	::pinMode  (MAIN_pin, INPUT);
	::wiringPiISR (MAIN_pin, INT_EDGE_BOTH, &interrupt_cb) ;

	while (true)
	{
		sleep (1);
	}

	return 0;
}
