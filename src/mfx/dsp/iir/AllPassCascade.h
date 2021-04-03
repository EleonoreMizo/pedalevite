/*****************************************************************************

        AllPassCascade.h
        Author: Laurent de Soras, 2021

Serial cascade of 2-pole and 1-pole all-pass filter sections.

Template parameters:

- T: processed data type (floating point)

- N2P: number of 2-pole sections, >= 0

- N1P: number of 1-pole sections, >= 0

Note: N2P + N1P should be > 0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_AllPassCascade_HEADER_INCLUDED)
#define mfx_dsp_iir_AllPassCascade_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <typename T, int N2P, int N1P>
class AllPassCascade
{
	static_assert (N2P >= 0, "Number of 2nd-order APF should be > 0.");
	static_assert (N1P >= 0, "Number of 1st-order APF should be > 0.");
	static_assert (N2P + N1P > 0, "There should be at least 1 APF.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;
	typedef AllPassCascade <T, N2P, N1P> ThisType;

	static constexpr int  _order = N2P * 2 + N1P;

	inline void    set_coefs (const T b_arr [_order]) noexcept;
	inline void    get_coefs (T b_arr [_order]) const noexcept;
	inline void    copy_z_eq (const ThisType &other) noexcept;

	fstb_FORCEINLINE float
	               process_sample (T x) noexcept;
	fstb_FORCEINLINE float
	               process_sample (T x, const T inc_b [_order]) noexcept;

	void           process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept;
	void           process_block (T dst_ptr [], const T src_ptr [], int nbr_spl, const T inc_b [_order]) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Coefficients or memory
	class Data2p
	{
	public:
		T              _r0 = 0;
		T              _r1 = 0;
	};
	class Data1p
	{
	public:
		T              _r0 = 0;
	};

	typedef std::array <Data2p, N2P    > F2pCoefArray;
	typedef std::array <Data2p, N2P + 1> F2pMemArray;

	typedef std::array <Data1p, N1P    > F1pCoefArray;
	typedef std::array <Data1p, N1P + 1> F1pMemArray;

	template <typename F>
	fstb_FORCEINLINE float
	               proc_spl_ap2 (float x, int pos, F fnc_inc) noexcept;
	template <typename F>
	fstb_FORCEINLINE float
	               proc_spl_ap1 (float x, int pos, F fnc_inc) noexcept;

	F2pCoefArray   _f2p_coef_arr;
	F2pMemArray    _f2p_mem_arr;

	F1pCoefArray   _f1p_coef_arr;
	F1pMemArray    _f1p_mem_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const AllPassCascade &other) const = delete;
	bool           operator != (const AllPassCascade &other) const = delete;

}; // class AllPassCascade



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/AllPassCascade.hpp"



#endif   // mfx_dsp_iir_AllPassCascade_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
