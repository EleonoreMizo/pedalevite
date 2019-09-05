/*****************************************************************************

        TestOscWavetableSyncHard.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestOscWavetableSyncHard_HEADER_INCLUDED)
#define TestOscWavetableSyncHard_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestOscWavetableSyncHard
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <typename O>
	static int		test_valid ();

	template <typename O>
	static void		test_speed ();

	template <typename O, typename V>
	static void    test_sweep (O &osc, V &result_m, int len, int block_len, int32_t pitch_beg, int32_t pitch_end, int32_t pitch_slave_beg, int32_t pitch_slave_end, uint32_t sync_pos);

	template <typename O>
	static void		configure_osc (O &osc, typename O::WavetableDataType &wt, typename O::AntialiasedStep &st);

	template <typename O>
	static float   get_data_scale ();

	template <typename O>
	static void    add_result (std::vector <float> &result_m, const std::vector <typename O::DataType> &data, size_t len = 0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestOscWavetableSyncHard ()                               = delete;
	               TestOscWavetableSyncHard (const TestOscWavetableSyncHard &other) = delete;
	virtual        ~TestOscWavetableSyncHard ()                              = delete;
	TestOscWavetableSyncHard &
	               operator = (const TestOscWavetableSyncHard &other)        = delete;
	bool           operator == (const TestOscWavetableSyncHard &other) const = delete;
	bool           operator != (const TestOscWavetableSyncHard &other) const = delete;

}; // class TestOscWavetableSyncHard



//#include "test/TestOscWavetableSyncHard.hpp"



#endif   // TestOscWavetableSyncHard_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
