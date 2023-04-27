/*****************************************************************************

        TestUserInputPi3.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/GpioPin.h"
#include "mfx/hw/UserInputPi3.h"
#include "mfx/ui/TimeShareThread.h"
#include "test/TestUserInputPi3.h"

#include <wiringPi.h>

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestUserInputPi3::perform_test ()
{
	int            ret_val = 0;

	const int      pin_rst = mfx::hw::GpioPin::_reset;

	::wiringPiSetupPhys ();
	::pinMode (pin_rst, OUTPUT);
	::digitalWrite (pin_rst, LOW);
	::delay (100);
	::digitalWrite (pin_rst, HIGH);
	::delay (1);

	typedef mfx::ui::UserInputInterface::MsgQueue MQueue;
	mfx::ui::UserInputInterface::MsgQueue  queue_input;
	mfx::ui::TimeShareThread   thread_spi (std::chrono::milliseconds (10));
	mfx::hw::UserInputPi3   user_input (thread_spi);
	user_input.assign_queues_to_input_dev (queue_input, queue_input, queue_input);

	do
	{
		MQueue::CellType * cell_ptr = queue_input.dequeue ();
		if (cell_ptr != nullptr)
		{
			const mfx::ui::UserInputMsg & msg  = cell_ptr->_val;
			const mfx::ui::UserInputType  type = msg.get_type ();
			const int                     idx  = msg.get_index ();
			const float                   val  = msg.get_val ();

			const char *   type_0 = "***unknown***";
			switch (type)
			{
			case mfx::ui::UserInputType_POT:    type_0 = "Pot   "; break;
			case mfx::ui::UserInputType_ROTENC: type_0 = "RotEnc"; break;
			case mfx::ui::UserInputType_SW:     type_0 = "Switch"; break;
			default:
				assert (false);
				break;
			}
			printf ("Type: %s, index: %2d, val: %f\n", type_0, idx, val);

			user_input.return_cell (*cell_ptr);
		}
	}
	while (true);

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
