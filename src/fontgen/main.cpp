/*****************************************************************************

        main.cpp
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

#include	"FontRenderer.h"

#include	<cassert>
#include	<cstdio>



/*\\\ CLASS DEFINITIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	main (int /*argc*/, char * /*argv*/ [])
{
	int            ret_val = 0;

	try
	{
		// C:\\Windows\\Fonts\\DejaVuSansMono.ttf
		// 23.0 -> 24
		// 31.0 -> 32
		// C:\\Windows\\Fonts\\DejaVuSansMono-Bold.ttf
		// 15.0 -> 16
		// 46.0 -> 48
		const std::string	font_filename ("C:\\Windows\\Fonts\\DejaVuSansMono.ttf");
		const std::string	expt_filename ("font-24-w125.data");
		double			size_pix = 23.0;
		double         scale_h  = 1.25;
		
		FontRenderer	rend (font_filename.c_str (), size_pix, scale_h);

		ret_val = rend.export_to_raw (expt_filename.c_str ());
		if (ret_val != 0)
		{
			printf ("Something failed (return code %d).\n", ret_val);
			assert (false);
		}
	}

	catch (std::exception &e)
	{
		printf ("*** main(): exception: %s\n", e.what ());
		ret_val = -1;
	}

	catch (...)
	{
		printf ("*** main(): exception (undefined)\n");
		ret_val = -1;
	}

	return (ret_val);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
