/*****************************************************************************

        Testadrv.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (Testadrv_HEADER_INCLUDED)
#define Testadrv_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/adrv/CbInterface.h"

#include <atomic>



class Testadrv
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _nbr_chn = 2;

	class AdrvCallback
	:	public mfx::adrv::CbInterface
	{
	public:
		bool           is_exit_requested () const;
		bool           check_dropout ();
	protected:
		void           do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl) override;
		void           do_notify_dropout () override;
		void           do_request_exit () override;
	private:
		std::atomic <bool>
		               _request_exit_flag { false };
		std::atomic <bool>
		               _dropout_flag { false };
	};


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Testadrv ()                               = delete;
	               Testadrv (const Testadrv &other)          = delete;
	               Testadrv (Testadrv &&other)               = delete;
	Testadrv &     operator = (const Testadrv &other)        = delete;
	Testadrv &     operator = (Testadrv &&other)             = delete;
	bool           operator == (const Testadrv &other) const = delete;
	bool           operator != (const Testadrv &other) const = delete;

}; // class Testadrv



//#include "test/Testadrv.hpp"



#endif   // Testadrv_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
