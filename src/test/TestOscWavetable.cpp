/*****************************************************************************

        TestOscWavetable.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/lang/type_name.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/osc/OscWavetable.h"
#include "mfx/dsp/osc/WavetableMipMapper.h"
#include "mfx/dsp/rspl/InterpFtor.h"
#include "mfx/FileOpWav.h"
#include "test/TestOscWavetable.h"
#include "test/TimerAccurate.h"

#include <limits>
#include <typeinfo>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscWavetable::perform_test ()
{
	typedef mfx::dsp::rspl::InterpFtor::CubicHermite Interpolator;

	typedef mfx::dsp::osc::OscWavetable <
		Interpolator,
		12, 6, 3,
		float,
		Interpolator::DATA_PRE, Interpolator::DATA_POST
	> OscTypeFloat;

	typedef mfx::dsp::osc::OscWavetable <
		Interpolator,
		12, 6, 3,
		int16_t,
		Interpolator::DATA_PRE, Interpolator::DATA_POST
	> OscTypeInt16;

	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = test_valid <OscTypeFloat> ();
	}

	if (ret_val == 0)
	{
		test_speed <OscTypeFloat> ();
	}

	if (ret_val == 0)
	{
		ret_val = test_valid <OscTypeInt16> ();
	}

	if (ret_val == 0)
	{
		test_speed <OscTypeInt16> ();
	}

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename O>
int	TestOscWavetable::test_valid ()
{
	typedef O OscType;
	typedef typename OscType::DataType DataType;
	typedef typename OscType::WavetableDataType WtType;

	printf (
		"Testing %s...\n",
		fstb::lang::type_name <OscType> ().to_str ().c_str ()
	);

	std::vector <float>  result_m;

	OscType        osc;
	WtType         wt;
	configure_osc (osc, wt);

	int            table_len_spl = 0;
	for (int t_idx = 0; t_idx < wt.get_nbr_tables (); ++t_idx)
	{
		table_len_spl += wt.get_table_len (t_idx);
		table_len_spl += WtType::UNROLL_PRE;
		table_len_spl += WtType::UNROLL_POST;
	}
	const int      table_len_bytes = table_len_spl * sizeof (DataType);
	printf ("Total wavetable size: %d bytes.\n", table_len_bytes);

	const int      base_pitch = osc.get_base_pitch ();

	const int      sample_freq = 44100;
	const int      block_len   = 64;
	const int      len         = (sample_freq * 16) & ~(block_len - 1);
	assert (len % block_len == 0);
	std::vector <DataType>  data (len);

	const int      nbr_oct = 10;
	const float    mult    = float (nbr_oct << OscType::PITCH_FRAC_BITS) / len;
	const int32_t  offset  = base_pitch - (nbr_oct << OscType::PITCH_FRAC_BITS);
	for (int pos = 0; pos < len; pos += block_len)
	{
		const int32_t  pitch = offset + fstb::floor_int (float (pos) * mult);
		osc.set_pitch (pitch);
		osc.process_block (&data [pos], block_len);
	}

	add_result <O> (result_m, data);

	for (int pos = 0; pos < len; ++ pos)
	{
		const int32_t  pitch = offset + fstb::floor_int (float (pos) * mult);
		osc.set_pitch (pitch);
		data [pos] = osc.process_sample ();
	}

	add_result <O> (result_m, data);

	for (int pos = 0; pos < len; pos += block_len)
	{
		const double	freq  =
			1000 * exp (cos (pos * 2 * fstb::PI * 3 / sample_freq) * 1);
		const int32_t  pitch =
			osc.conv_freq_to_pitch (float (freq), float (sample_freq));
		osc.set_pitch (pitch);
		osc.process_block (&data [pos], block_len);
	}

	add_result <O> (result_m, data);

	std::string filename = "results/oscwavetable";
	filename += typeid (DataType).name ();
	filename += "0.wav";
	mfx::FileOpWav::save (filename.c_str (), result_m, 44100, 0.5f);

	printf ("Done.\n");

	return 0;
}



template <typename O>
void	TestOscWavetable::test_speed ()
{
	typedef O OscType;
	typedef typename OscType::DataType DataType;

	const int      block_len  = 256;
	const int      nbr_blocks = 65536;

	OscType        osc;
	typename OscType::WavetableDataType wt;
	configure_osc (osc, wt);
	osc.set_pitch (osc.conv_freq_to_pitch (1000, 44100));

	std::vector <DataType>   dest (block_len);
	DataType *     dest_ptr = &dest [0];

	printf (
		"Speed test for %s...\n",
		fstb::lang::type_name <OscType> ().to_str ().c_str ()
	);

	TimerAccurate  tim;
	double         acc_dummy = 0;

	tim.reset ();
	tim.start ();
	for (int block_cnt = 0; block_cnt < nbr_blocks; ++block_cnt)
	{
		osc.process_block (dest_ptr, block_len);

		// Prevents the optimizer to remove all the loops
		acc_dummy += double (dest_ptr [block_len - 1]);
	}
	tim.stop ();

	double	      spl_per_s = tim.get_best_rate (block_len * nbr_blocks);
	spl_per_s += fstb::limit (acc_dummy, -1e-300, 1e-300); // Anti-optimizer trick
	const double   mega_sps  = spl_per_s / 1'000'000.0;
	printf ("Speed: %12.3f Mspl/s.\n\n", mega_sps);
}



template <typename O>
void	TestOscWavetable::configure_osc (O &osc, typename O::WavetableDataType &wt)
{
	typedef O OscType;
	typedef typename OscType::DataType DataType;

	const float    scale = get_data_scale <OscType> ();

	// Wavetable generation
	const int      last_table = wt.get_nbr_tables () - 1;
	const int      table_len  = wt.get_table_len (last_table);
	for (long pos = 0; pos < table_len; ++pos)
	{

#if 1
		// Saw
		wt.set_sample (
			last_table,
			(pos - table_len / 4) & (table_len - 1),
			DataType (((float (pos * 2 + 1) / float (table_len)) - 1) * scale)
		);
#else
		// Sine
		wt.set_sample (
			last_table,
			pos,
			DataType (cos (pos * (2 * basic::PI) / double (table_len) * scale)
		);
#endif
	}

	// Mipmapping
	typedef typename OscType::WavetableDataType  WavetableDataType;
	mfx::dsp::osc::WavetableMipMapper <WavetableDataType> mipmapper;
	mipmapper.build_mipmaps (wt);

	osc.set_wavetable (wt);
	osc.set_base_pitch (16 << OscType::PITCH_FRAC_BITS);
}



template <typename T, bool INT_FLAG>
class TestOscWavetable_GetNumScale
{
public:
	static float   get () { return 1.f; }
};

template <typename T>
class TestOscWavetable_GetNumScale <T, true>
{
public:
	static float   get ()
	{
		return float (T (1) << (std::numeric_limits <T>::digits - 1));
	}
};

template <typename O>
float	TestOscWavetable::get_data_scale ()
{
	typedef O OscType;
	typedef typename OscType::DataType DataType;

	return TestOscWavetable_GetNumScale <
		DataType,
		std::numeric_limits <DataType>::is_integer
	>::get ();
}



template <typename O>
void	TestOscWavetable::add_result (std::vector <float> &result_m, const std::vector <typename O::DataType> &data)
{
	const float    scale = get_data_scale <O> ();
	const float    scale_inv = 1.0f / scale;
	std::for_each (
		data.begin (),
		data.end (),
		[&] (const typename O::DataType val)
		{
			result_m.push_back (float (val) * scale_inv);
		}
	);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
