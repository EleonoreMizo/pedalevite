/*****************************************************************************

        FilterK.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/r128/FilterK.h"
#include "mfx/dsp/iir/TransSZBilin.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr float	FilterK::_ofs_997;



// Mandatory call before processing anything
void	FilterK::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;

	// Computes the coefficients for the given sampling rate. Coefficients
	// in the BS.1770 are specified in the z plane for a 48 kHz sampling rate.
	// So we convert back these coefs to the s plane and compute new z coefs
	// with the provided sampling rate.

	constexpr double  fs_ref = 48000;
	std::array <double, 3> tmp_bs;
	std::array <double, 3> tmp_as;

	// Shelf
	constexpr std::array <double, 3> shf_48_bz {
		1.53512485958697, -2.69169618940638, 1.19839281085285
	};
	constexpr std::array <double, 3> shf_48_az {
		1, -1.69065929318241, 0.73248077421585
	};
	constexpr double  shf_f0 = 1681.97445; // Hz

	iir::TransSZBilin::map_z_to_s (
		tmp_bs.data (), tmp_as.data (),
		shf_48_bz.data (), shf_48_az.data (),
		shf_f0, fs_ref
	);

	std::array <float, 3> shf_bz;
	std::array <float, 3> shf_az;
	iir::TransSZBilin::map_s_to_z (
		shf_bz.data (), shf_az.data (),
		tmp_bs.data (), tmp_as.data (),
		shf_f0, _sample_freq
	);
	_shelf.set_z_eq (shf_bz.data (), shf_az.data ());

	// High-pass filter
	constexpr std::array <double, 3> hpf_48_bz { 1, -2, 1 };
	constexpr std::array <double, 3> hpf_48_az {
		1, -1.99004745483398, 0.99007225036621
	};
	constexpr double  hpf_f0 = 38.13547; // Hz

	iir::TransSZBilin::map_z_to_s (
		tmp_bs.data (), tmp_as.data (),
		hpf_48_bz.data (), hpf_48_az.data (),
		hpf_f0, fs_ref
	);

	std::array <float, 3> hpf_bz;
	std::array <float, 3> hpf_az;
	iir::TransSZBilin::map_s_to_z (
		hpf_bz.data (), hpf_az.data (),
		tmp_bs.data (), tmp_as.data (),
		hpf_f0, _sample_freq
	);
	_hpf.set_z_eq (hpf_bz.data (), hpf_az.data ());

	clear_buffers ();
}



float	FilterK::process_sample (float x) noexcept
{
	assert (_sample_freq > 0);

	x = _shelf.process_sample (x);
	x = _hpf.process_sample (x);

	return x;
}



void	FilterK::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	_shelf.process_block (dst_ptr, src_ptr, nbr_spl);
	_hpf.process_block (dst_ptr, dst_ptr, nbr_spl);
}



void	FilterK::clear_buffers () noexcept
{
	_shelf.clear_buffers ();
	_hpf.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
