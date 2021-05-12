/*****************************************************************************

        SweepingSine.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_SweepingSine_HEADER_INCLUDED)
#define hiir_test_SweepingSine_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{
namespace test
{



class SweepingSine
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SweepingSine (float sample_freq, float freq_beg, float freq_end, long nbr_spl);
	               SweepingSine ()                          = delete;

	               SweepingSine (const SweepingSine &other) = default;
	               SweepingSine (SweepingSine &&other)      = default;
	               ~SweepingSine ()                         = default;

	SweepingSine & operator = (const SweepingSine &other)   = default;
	SweepingSine & operator = (SweepingSine &&other)        = default;

	template <typename T>
	void           generate (T dest_ptr []) const;

	long           get_len () const;
	float          get_sample_freq () const;
	float          get_freq_at_sample (long pos) const;
	long           get_sample_pos_for (float freq) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _sample_freq;
	float          _freq_beg;
	float          _freq_end;
	long           _nbr_spl;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SweepingSine &other) = delete;
	bool           operator != (const SweepingSine &other) = delete;

}; // class SweepingSine



}  // namespace test
}  // namespace hiir



#include "hiir/test/SweepingSine.hpp"



#endif   // hiir_test_SweepingSine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
