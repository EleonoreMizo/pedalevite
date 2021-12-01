/*****************************************************************************

        BufferFiller.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (BufferFiller_HEADER_INCLUDED)
#define BufferFiller_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class BufferFiller
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	static void    gen_rnd_non_zero (T buf_ptr [], int nbr_spl);
	template <typename T>
	static void    gen_rnd_positive (T buf_ptr [], int nbr_spl);
	template <typename T>
	static void    gen_rnd_scaled (T buf_ptr [], int nbr_spl, T min_val, T max_val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BufferFiller ()                               = delete;
	               BufferFiller (const BufferFiller &other)      = delete;
	virtual        ~BufferFiller ()                              = delete;
	BufferFiller & operator = (const BufferFiller &other)        = delete;
	bool           operator == (const BufferFiller &other) const = delete;
	bool           operator != (const BufferFiller &other) const = delete;

}; // class BufferFiller



#include "test/BufferFiller.hpp"



#endif   // BufferFiller_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
