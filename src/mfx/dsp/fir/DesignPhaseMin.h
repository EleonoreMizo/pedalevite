/*****************************************************************************

        DesignPhaseMin.h
        Author: Laurent de Soras, 2021

Turns an order 2N-1-order linear-phase FIR into an N-order minimum-phase FIR.

The linear-phase FIR should have specific characteristics.
First call compute_linear_spec() to design the linear-phase FIR depending on
the desired minimum-phase specs.
Then call minimize_phase() to obtain the minimum-phase FIR.

Reference:
Niranjan Damera-Venkata, Brian L. Evans, Shawn R. McCaslin,
Design of Optimal Minimum Phase Digital FIR Filters Using Discrete Hilbert
Transforms,
IEEE Transactions on Signal Processing, Volume 48, Issue 5, 2000-05
https://doi.org/10.1109/78.840000

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_DesignPhaseMin_HEADER_INCLUDED)
#define mfx_dsp_fir_DesignPhaseMin_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTReal.h"

#include <array>
#include <vector>
#include <memory>



namespace mfx
{
namespace dsp
{
namespace fir
{



class DesignPhaseMin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Ripple
	{
	public:
		double         _passband; // dB, > 0
		double         _stopband; // dB, > _passband
	};

	               DesignPhaseMin ()                       = default;
	               DesignPhaseMin (DesignPhaseMin &&other) = default;
	DesignPhaseMin &
	               operator = (DesignPhaseMin &&other)     = default;

   void           set_fft_len (int fft_len);
   void           release_buffers ();

	int            compute_optimal_fft_length (double f_stop, int n2, double epsilon) noexcept;
   void           minimize_phase (float min_ptr [], const float lin_ptr [], int len) noexcept;

	static Ripple  compute_linear_spec (const Ripple &min_phase_spec)  noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _nbr_buf    = 3;
	static constexpr int _buf_b      = 0;
	static constexpr int _buf_h      = 1;
	static constexpr int _buf_h123r  = 0;
	static constexpr int _buf_maglog = 1;
	static constexpr int _buf_in     = 2;
	static constexpr int _buf_ph     = 1;
	static constexpr int _buf_rec    = 2;
	static constexpr int _buf_recu   = 1;
	static constexpr int _buf_y      = 0;

	typedef double DataType;
	typedef std::vector <DataType> Buffer;
	typedef std::array <Buffer, _nbr_buf> BufferList;
	typedef ffft::FFTReal <DataType> FftType;
	typedef std::unique_ptr <FftType> FftUPtr;

	inline int     get_fft_len () const noexcept;
	void           compute_dht (DataType recu_ptr [], const DataType mag_ptr [], int trunc_len) noexcept;

	BufferList     _buf_list;
   FftUPtr        _fft_uptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DesignPhaseMin (const DesignPhaseMin &other)    = delete;
	DesignPhaseMin &
	               operator = (const DesignPhaseMin &other)        = delete;
	bool           operator == (const DesignPhaseMin &other) const = delete;
	bool           operator != (const DesignPhaseMin &other) const = delete;

}; // class DesignPhaseMin



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/fir/DesignPhaseMin.hpp"



#endif   // mfx_dsp_fir_DesignPhaseMin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
