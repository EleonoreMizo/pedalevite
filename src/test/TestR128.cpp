/*****************************************************************************

        TestR128.cpp
        Author: Laurent de Soras, 2022

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

#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/AllMeters.h"
#include "mfx/dsp/mix/Fpu.h"
#include "mfx/FileOpWav.h"
#include "test/TestR128.h"

#include <algorithm>
#include <iterator>

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestR128::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::ana::r128...\n");

	double         sample_freq = 48000;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// EBU Tech 3341-2016, table 1, p. 10

	// 1
	perform_single_test (ret_val, "3341_01", sample_freq,
		SpecGen { { SpecSineSeg { { dB (-23), dB (-23) } } } },
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::M, -23 }, Result { Win::S, -23 }, Result { Win::I, -23 }
		} } } }
	);

	// 2
	perform_single_test (ret_val, "3341_02", sample_freq,
		SpecGen { { SpecSineSeg { { dB (-33), dB (-33) } } } },
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::M, -33 }, Result { Win::S, -33 }, Result { Win::I, -33 }
		} } } }
	);

	// 3
	perform_single_test (ret_val, "3341_03", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-36), dB (-36) }, 10 },
			SpecSineSeg { { dB (-23), dB (-23) }, 60 },
			SpecSineSeg { { dB (-36), dB (-36) }, 10 },
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::I, -23 } } } } }
	);

	// 4
	perform_single_test (ret_val, "3341_04", sample_freq,
		SpecGen { {
				SpecSineSeg { { dB (-72), dB (-72) }, 10 },
				SpecSineSeg { { dB (-36), dB (-36) }, 10 },
				SpecSineSeg { { dB (-23), dB (-23) }, 60 },
				SpecSineSeg { { dB (-36), dB (-36) }, 10 },
				SpecSineSeg { { dB (-72), dB (-72) }, 10 }
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::I, -23 } } } } }
	);

	// 5
	perform_single_test (ret_val, "3341_05", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-26), dB (-26) }, 20.0 },
			SpecSineSeg { { dB (-20), dB (-20) }, 20.1 },
			SpecSineSeg { { dB (-26), dB (-26) }, 20.0 },
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::I, -23 } } } } }
	);

	// 6
	perform_single_test (ret_val, "3341_06", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-28), dB (-28), dB (-24), dB (-30), dB (-30) } },
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::I, -23 } } } } }
	);

	// 7 and 8 require real audio exerpts

	// 9
	{
		auto           sg_base = std::vector <SpecSineSeg> {
			SpecSineSeg { { dB (-20), dB (-20) }, 1.34 },
			SpecSineSeg { { dB (-30), dB (-30) }, 1.66 },
		};
		SpecGen        sg;
		for (int i = 0; i < 5; ++i)
		{
			sg._seg_arr.insert (sg._seg_arr.end (), sg_base.begin (), sg_base.end ());
		}
		perform_single_test (ret_val, "3341_09", sample_freq,
			sg,
			SpecAllResults { { SpecResult { 0, 3.0, { Result { Win::S, -23 } } } } }
		);
	}

	// 10
	{
		SpecGen        sg;
		SpecAllResults sar;
		for (int i = 0; i < 20; ++i)
		{
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 0.15 * i });
			sg._seg_arr.emplace_back (SpecSineSeg { { dB (-23), dB (-23) }, 3.0 });
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 1.0 });
			sar._spec_arr.emplace_back (SpecResult {
				0.15 * i + 3.0 + 1.0, 0,
				{ Result { Win::S, -23, 0.1, 0.1, true } }
			});
		}
		perform_single_test (ret_val, "3341_10", sample_freq, sg, sar);
	}

	// 11
	{
		SpecGen        sg;
		SpecAllResults sar;
		for (int i = 0; i < 20; ++i)
		{
			const auto     vdb = -38 + i;
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 0.15 * i });
			sg._seg_arr.emplace_back (SpecSineSeg { { dB (vdb), dB (vdb) }, 3.0 });
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 1.0 });
			sar._spec_arr.emplace_back (SpecResult {
				0.15 * i + 3.0 + 1.0, 0,
				{ Result { Win::S, double (vdb), 0.1, 0.1, true } }
			});
		}
		perform_single_test (ret_val, "3341_11", sample_freq, sg, sar);
	}

	// 12
	{
		auto           sg_base = std::vector <SpecSineSeg> {
			SpecSineSeg { { dB (-20), dB (-20) }, 0.18 },
			SpecSineSeg { { dB (-30), dB (-30) }, 0.22 },
		};
		SpecGen        sg;
		for (int i = 0; i < 25; ++i)
		{
			sg._seg_arr.insert (sg._seg_arr.end (), sg_base.begin (), sg_base.end ());
		}
		perform_single_test (ret_val, "3341_12", sample_freq,
			sg,
			SpecAllResults { { SpecResult { 0, 1.0, { Result { Win::M, -23 } } } } }
		);
	}

	// 13
	{
		SpecGen        sg;
		SpecAllResults sar;
		for (int i = 0; i < 20; ++i)
		{
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 0.20 * i });
			sg._seg_arr.emplace_back (SpecSineSeg { { dB (-23), dB (-23) }, 0.4 });
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 1.0 });
			sar._spec_arr.emplace_back (SpecResult {
				0.20 * i + 0.4 + 1.0, 0,
				{ Result { Win::M, -23, 0.1, 0.1, true } }
			});
		}
		perform_single_test (ret_val, "3341_13", sample_freq, sg, sar);
	}

	// 14
	{
		SpecGen        sg;
		SpecAllResults sar;
		for (int i = 0; i < 20; ++i)
		{
			const auto     vdb = -38 + i;
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 0.20 * i });
			sg._seg_arr.emplace_back (SpecSineSeg { { dB (vdb), dB (vdb) }, 0.4 });
			sg._seg_arr.emplace_back (SpecSineSeg { { 0, 0 }, 1.0 });
			sar._spec_arr.emplace_back (SpecResult {
				0.20 * i + 0.4 + 1.0, 0,
				{ Result { Win::M, double (vdb), 0.1, 0.1, true } }
			});
		}
		perform_single_test (ret_val, "3341_14", sample_freq, sg, sar);
	}

	// 15
	perform_single_test (ret_val, "3341_15", sample_freq,
		SpecGen {
			{ SpecSineSeg { { 0.5, 0.5 }, 5, sample_freq / 4 } },
			true
		},
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::TP, -6, 0.4, 0.2, true }
		} } } }
	);

	// 16
	perform_single_test (ret_val, "3341_16", sample_freq,
		SpecGen {
			{ SpecSineSeg { { 0.5, 0.5 }, 5, sample_freq / 4 } },
			true, deg (45)
		},
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::TP, -6, 0.4, 0.2, true }
		} } } }
	);

	// 17
	perform_single_test (ret_val, "3341_17", sample_freq,
		SpecGen {
			{ SpecSineSeg { { 0.5, 0.5 }, 5, sample_freq / 6 } },
			true, deg (60)
		},
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::TP, -6, 0.4, 0.2, true }
		} } } }
	);

	// 18
	perform_single_test (ret_val, "3341_18", sample_freq,
		SpecGen {
			{ SpecSineSeg { { 0.5, 0.5 }, 5, sample_freq / 8 } },
			true, deg (67.5)
		},
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::TP, -6, 0.4, 0.2, true }
		} } } }
	);

	// 19
	perform_single_test (ret_val, "3341_19", sample_freq,
		SpecGen {
			{ SpecSineSeg { { 1.41, 1.41 }, 5, sample_freq / 4 } },
			true, deg (45)
		},
		SpecAllResults { { SpecResult { 0, 0, {
			Result { Win::TP, +3, 0.4, 0.2, true }
		} } } }
	);

	// 20-23
	/***
	To do: these tests are not exactly as specified. We should synthetise
	everything at 4x and downsample instead of shifting the initial phase.
	***/
	for (int shift = 0; shift < 4 && ret_val == 0; ++shift)
	{
		char           txt_0 [63+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "3341_%d", 20 + shift);
		perform_single_test (ret_val, txt_0, sample_freq,
			SpecGen {
				{
					SpecSineSeg { { 0.5, 0.5 }, 1, sample_freq / 6 },
					SpecSineSeg { { 1.0, 1.0 }, 4 / sample_freq, sample_freq / 4 },
					SpecSineSeg { { 0.5, 0.5 }, 1, sample_freq / 6 }
				},
				true, 0, shift * 0.25
			},
			SpecAllResults { { SpecResult { 0, 0, {
				Result { Win::TP, 0, 0.4, 0.2, true }
			} } } }
		);
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// EBU Tech 3342-2016, table 1, p. 8

	// 1
	perform_single_test (ret_val, "3342_01", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-20), dB (-20) }, 20 },
			SpecSineSeg { { dB (-30), dB (-30) }, 20 }
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::LRA, 10, 1, 1 } } } } }
	);

	// 2
	perform_single_test (ret_val, "3342_02", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-20), dB (-20) }, 20 },
			SpecSineSeg { { dB (-15), dB (-15) }, 20 }
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::LRA, 5, 1, 1 } } } } }
	);

	// 3
	perform_single_test (ret_val, "3342_03", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-40), dB (-40) }, 20 },
			SpecSineSeg { { dB (-20), dB (-20) }, 20 }
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::LRA, 20, 1, 1 } } } } }
	);

	// 4
	perform_single_test (ret_val, "3342_04", sample_freq,
		SpecGen { {
			SpecSineSeg { { dB (-50), dB (-50) }, 20 },
			SpecSineSeg { { dB (-35), dB (-35) }, 20 },
			SpecSineSeg { { dB (-20), dB (-20) }, 20 },
			SpecSineSeg { { dB (-35), dB (-35) }, 20 },
			SpecSineSeg { { dB (-50), dB (-50) }, 20 }
		} },
		SpecAllResults { { SpecResult { 0, 0, { Result { Win::LRA, 15, 1, 1 } } } } }
	);

	// 5 and 6 require real audio exerpts

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	TestR128::dB (double db) noexcept
{
	return double (pow (10.0, db / 20.0));
}



double	TestR128::deg (double deg) noexcept
{
	return deg / 360.0;
}



void	TestR128::perform_single_test (int &ret_val, std::string name, double sample_freq, const SpecGen &gen, const SpecAllResults &res_all)
{
	if (ret_val == 0)
	{
		printf ("Checking %s... ", name.c_str ());
		fflush (stdout);

		BufMulti       buf_arr;
		generate_signal (buf_arr, sample_freq, gen);

		std::string    filename { "results/r128_" };
		filename += name + ".wav";
		mfx::FileOpWav::save (filename.c_str (), buf_arr, sample_freq, 1.f);

		ret_val = check_signal (buf_arr, sample_freq, res_all);
		if (ret_val == 0)
		{
			printf ("pass.\n");
		}
		else
		{
			printf ("*** failed ***\n");
		}
	}
}



void	TestR128::generate_signal (BufMulti &buf_arr, double sample_freq, const SpecGen &gen)
{
	assert (! gen._seg_arr.empty ());

	const auto     nbr_chn = int (gen._seg_arr [0]._lvl_arr.size ());
	assert (std::all_of (
		gen._seg_arr.begin (), gen._seg_arr.end (),
		[nbr_chn] (const SpecSineSeg &s) {
			return (int (s._lvl_arr.size ()) == nbr_chn);
		}
	));

	buf_arr.clear ();
	buf_arr.resize (nbr_chn);

	double         phase = gen._phase;
	phase += gen._shift * gen._seg_arr.front ()._freq / sample_freq;

	int            seg_pos = 0;
	for (auto &spec : gen._seg_arr)
	{
		const auto     seg_len = fstb::round_int (spec._duration * sample_freq);
		const auto     len_new = seg_pos + seg_len;

		for (auto &buf : buf_arr)
		{
			buf.resize (len_new);
		}

		const auto     phase_inc = spec._freq / sample_freq;
		for (int pos = 0; pos < seg_len; ++pos)
		{
			const auto     val = sinf (float (phase * (2 * fstb::PI)));
			phase += phase_inc;
			if (phase > 1)
			{
				phase -= 1;
			}

			for (int chn_idx = 0; chn_idx < nbr_chn; ++chn_idx)
			{
				auto &         buf = buf_arr [chn_idx];
				const auto     lvl = float (spec._lvl_arr [chn_idx]);
				buf [seg_pos + pos] = val * lvl;
			}
		}

		seg_pos += seg_len;
	}

	// Fade in/out
	if (gen._fade_flag)
	{
		constexpr double  fade_dur = 0.010; // s
		const auto     fade_len = fstb::round_int (fade_dur * sample_freq);
		const auto     buf_len  = int (buf_arr.front ().size ());
		assert (fade_len <= buf_len);

		for (auto &buf : buf_arr)
		{
			const auto      fi_ptr = buf.data ();
			const auto      fo_ptr = buf.data () + buf_len - fade_len;
			mfx::dsp::mix::Fpu::scale_1_vlr (fi_ptr, fade_len, 0, 1);
			mfx::dsp::mix::Fpu::scale_1_vlr (fo_ptr, fade_len, 1, 0);
		}
	}
}



int	TestR128::check_signal (const BufMulti &buf_arr, double sample_freq, const SpecAllResults &res_all)
{
	assert (! res_all._spec_arr.empty ());
	assert (   res_all._spec_arr.front ()._duration > 0
	        || res_all._spec_arr.size () == 1);
	assert (res_all._spec_arr.size () == 1 || std::all_of (
		std::next (res_all._spec_arr.begin ()), res_all._spec_arr.end (),
		[] (const SpecResult &s) { return (s._duration > 0); }
	));

	int            ret_val = 0;

	const auto     nbr_chn = int (buf_arr.size ());
	const auto     buf_len = int (buf_arr.front ().size ());

	mfx::dsp::ana::r128::AllMeters meters;
	meters.set_sample_freq (sample_freq);
	meters.set_nbr_chn (nbr_chn);
	std::vector <const float *> ptr_arr (nbr_chn);
	for (int chn_idx = 0; chn_idx < nbr_chn; ++chn_idx)
	{
		ptr_arr [chn_idx] = buf_arr [chn_idx].data ();
	}
	meters.set_chn_buffers (ptr_arr.data ());
	// Standard weights for stereo and 5.0 signals
	const std::array <float, 5> weight_arr { 1, 1, 1, 1.41f, 1.41f };
	meters.set_chn_weights (weight_arr.data ());
	meters.clear_buffers ();

	int            seg_pos = 0;
	for (const auto &spec : res_all._spec_arr)
	{
		auto           seg_len = fstb::round_int (spec._duration * sample_freq);
		if (spec._duration <= 0)
		{
			assert (seg_pos == 0);
			seg_len = buf_len;
		}

		auto           valid_pos_m = seg_pos + fstb::round_int (0.400 * sample_freq);
		auto           valid_pos_s = seg_pos + fstb::round_int (3.000 * sample_freq);

		if (spec._delay > 0)
		{
			const auto     dly_len = fstb::round_int (spec._delay * sample_freq);
			meters.analyse_block (seg_pos, seg_pos + dly_len);
			seg_pos += dly_len;
			seg_len -= dly_len;
		}

		// Resolution: 50 ms
		const auto     max_blk_size = fstb::round_int (0.050 * sample_freq);
		const auto     seg_end      = seg_pos + seg_len;
		float          max_m        = -999;
		float          max_s        = -999;
		do
		{
			const auto     work_len = std::min (seg_end - seg_pos, max_blk_size);
			meters.analyse_block (seg_pos, seg_pos + work_len);
			const auto     val_m = meters.get_loudness_m ();
			const auto     val_s = meters.get_loudness_s ();

			seg_pos += work_len;

			for (auto &res : spec._res_arr)
			{
				if (res._win == Win::M && seg_pos >= valid_pos_m)
				{
					max_m = std::max (max_m, val_m);
					ret_val = check_value (res, val_m, max_m, false);
				}
				else if (res._win == Win::S && seg_pos >= valid_pos_s)
				{
					max_s = std::max (max_s, val_s);
					ret_val = check_value (res, val_s, max_s, false);
				}

				if (ret_val != 0)
				{
					break;
				}
			}
		}
		while (seg_pos < seg_end && ret_val == 0);

		// Stops the test immediately on failure
		if (ret_val != 0)
		{
			break;
		}

		for (auto &res : spec._res_arr)
		{
			if (res._win == Win::M)
			{
				ret_val = check_value (res, max_m, max_m, true);
			}
			else if (res._win == Win::S)
			{
				ret_val = check_value (res, max_s, max_s, true);
			}
			else if (res._win == Win::I)
			{
				const auto     val_i = meters.compute_loudness_i ();
				ret_val = check_value (res, val_i, val_i, true);
			}
			else if (res._win == Win::LRA)
			{
				const auto     val_r = meters.compute_loudness_r ();
				ret_val = check_value (res, val_r, val_r, true);
			}
			else if (res._win == Win::TP)
			{
				float          max_tp_lin = 0;
				for (int chn_idx = 0; chn_idx < nbr_chn; ++chn_idx)
				{
					const auto     val_tp = meters.get_peak (chn_idx);
					max_tp_lin = std::max (max_tp_lin, val_tp);
				}
				const auto     max_tp = 20 * log10f (max_tp_lin);
				ret_val = check_value (res, max_tp, max_tp, true);
			}

			if (ret_val != 0)
			{
				break;
			}
		}
	}

	return ret_val;
}



int	TestR128::check_value (const Result &res, float val_cur, float val_max, bool end_flag)
{
	int            ret_val = 0;

	// Check max values only at the end
	if (end_flag || ! res._max_flag)
	{
		const auto     val = (res._max_flag) ? val_max : val_cur;
		if (val < res._val - res._tol_n)
		{
			ret_val = -1;
		}
		else if (val > res._val + res._tol_p)
		{
			ret_val = -1;
		}
	}

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
