/*****************************************************************************

        EnvFollowerRmsSimple.cpp
        Author: Laurent de Soras, 2016

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

#include "fstb/fnc.h"
#include "mfx/dsp/dyn/EnvFollowerRmsSimple.h"
#include "mfx/dsp/dyn/EnvHelper.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EnvFollowerRmsSimple::EnvFollowerRmsSimple ()
:	_sample_freq (44100)
,	_time (0)
,	_state (0)
,	_coef (1)
{
	set_time (0.001f);
	clear_buffers ();
}



void	EnvFollowerRmsSimple::set_sample_freq (double fs)
{
	assert (fs > 0);

	_sample_freq = float (fs);
	update_parameters ();
}



void	EnvFollowerRmsSimple::set_time (float t)
{
	assert (t >= 0);

	_time = t;
	update_parameters ();
}



void	EnvFollowerRmsSimple::process_block (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	float				state = _state;

	long				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		process_sample_internal (state, x);
		out_ptr [pos] = float (sqrt (state));

		++ pos;
	}
	while (pos < nbr_spl);

	_state = state;
}



void	EnvFollowerRmsSimple::process_block_no_sqrt (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	float				state = _state;

	long				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		process_sample_internal (state, x);
		out_ptr [pos] = state;

		++ pos;
	}
	while (pos < nbr_spl);

	_state = state;
}



// Input is not squared
// Output is not square-rooted
void	EnvFollowerRmsSimple::process_block_raw (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	float				state = _state;

	long				pos = 0;
	do
	{
		const float		x2 = in_ptr [pos];
		assert (x2 >= 0);
		process_sample_internal_no_sq (state, x2);
		out_ptr [pos] = state;

		++ pos;
	}
	while (pos < nbr_spl);

	_state = state;
}



float	EnvFollowerRmsSimple::analyse_block (const float data_ptr [], long nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	float				state = _state;

	long				pos = 0;
	do
	{
		const float		x = data_ptr [pos];
		process_sample_internal (state, x);

		++ pos;
	}
	while (pos < nbr_spl);

	_state = state;

	return float (sqrt (state));
}



float	EnvFollowerRmsSimple::analyse_block_no_sqrt (const float data_ptr [], long nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	float				state = _state;

	long				pos = 0;
	do
	{
		const float		x = data_ptr [pos];
		process_sample_internal (state, x);

		++ pos;
	}
	while (pos < nbr_spl);

	_state = state;

	return state;
}



// Input is not squared
// Output is not square-rooted
float	EnvFollowerRmsSimple::analyse_block_raw (const float data_ptr [], long nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	float				state = _state;

	long				pos = 0;
	do
	{
		const float		x2 = data_ptr [pos];
		assert (x2 >= 0);
		process_sample_internal_no_sq (state, x2);

		++ pos;
	}
	while (pos < nbr_spl);

	_state = state;

	return state;
}



// Input is not squared
// Output is not square-rooted
float	EnvFollowerRmsSimple::analyse_block_raw_cst (float x2, long nbr_spl)
{
	assert (x2 >= 0);
	assert (nbr_spl > 0);

	const float		delta = x2 - _state;

	float          coef_block;
	if (nbr_spl < 100000)
	{
		coef_block = 1 - fstb::ipowp (1 - _coef, nbr_spl);
	}
	else
	{
		coef_block = float (1 - pow (1 - _coef, nbr_spl));
	}

	_state += delta * coef_block;

	return _state;
}



void	EnvFollowerRmsSimple::clear_buffers ()
{
	_state = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EnvFollowerRmsSimple::update_parameters ()
{
	_coef = float (EnvHelper::compute_env_coef_simple (_time, _sample_freq));
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
