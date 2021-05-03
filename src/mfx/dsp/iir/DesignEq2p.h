/*****************************************************************************

        DesignEq2p.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_DesignEq2p_HEADER_INCLUDED)
#define mfx_dsp_iir_DesignEq2p_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <complex>



namespace mfx
{
namespace dsp
{
namespace iir
{



class DesignEq2p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:


	template <typename T>
	static void		make_dc_killer (T bs [3], T as [3]) noexcept;

	template <typename T>
	static void		make_3db_shelf_lo (T bs [3], T as [3], T q, T lvl) noexcept;
	template <typename T>
	static void		make_3db_shelf_hi (T bs [3], T as [3], T q, T lvl) noexcept;
	template <typename T>
	static void		make_3db_peak (T bs [3], T a [3], T q, T lvl) noexcept;

	template <typename T>
	static void		make_mid_shelf_lo (T bs [3], T as [3], T q, T lvl) noexcept;
	template <typename T>
	static void		make_mid_shelf_hi (T bs [3], T as [3], T q, T lvl) noexcept;
	template <typename T>
	static void		make_mid_peak (T bs [3], T as [3], T q, T lvl) noexcept;

	template <typename T>
	static void		make_low_pass (T bs [3], T as [3], T q) noexcept;
	template <typename T>
	static void		make_hi_pass (T bs [3], T as [3], T q) noexcept;
	template <typename T>
	static void		make_band_pass (T bs [3], T as [3], T q) noexcept;

	static void		make_nyq_peak (float bz [3], float az [3], double q, double lvl, double f0, double fs) noexcept;
	static void		make_nyq_peak (float bz [3], float az [3], double g0, double g, double gb, double w0, double dw) noexcept;

	static std::complex <double>  compute_butter_pole (int order, int biq) noexcept;
	static double  compute_butter_coef_a1 (int order, int biq) noexcept;

	static double  compute_thiele_coef_a1 (int order, int biq, double k) noexcept;

	template <typename T>
	static double  compute_group_delay (const T bz [3], const T az [3], double sample_freq, double f0) noexcept;
	template <typename T>
	static double  compute_group_delay_1p (const T bz [2], const T az [2], double sample_freq, double f0) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DesignEq2p ()                               = delete;
	               DesignEq2p (const DesignEq2p &other)        = delete;
	               DesignEq2p (DesignEq2p &&other)             = delete;
	               ~DesignEq2p ()                              = delete;
	DesignEq2p &   operator = (const DesignEq2p &other)        = delete;
	DesignEq2p &   operator = (DesignEq2p &&other)             = delete;
	bool           operator == (const DesignEq2p &other) const = delete;
	bool           operator != (const DesignEq2p &other) const = delete;

}; // class DesignEq2p



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/DesignEq2p.hpp"



#endif   // mfx_dsp_iir_DesignEq2p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
