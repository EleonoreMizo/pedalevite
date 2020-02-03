/*****************************************************************************

        Psu.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist3_Psu_HEADER_INCLUDED)
#define mfx_pi_dist3_Psu_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>

#include <array>


namespace mfx
{
namespace pi
{
namespace dist3
{



class Psu
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_half_cycle_relative_lvl (float lvl);
	void           set_ac_freq (float freq);
	void           set_ac_lvl (float lvl);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Smoother
	{
	public:
		void           set_sample_freq (double sample_freq);
		inline float   process_sample (float x);
		inline float   get_val () const;
		void           clear_buffers ();
	private:
		static const int  _order = 2;
		std::array <float, _order + 1>
		               _state_arr {};
		float          _coef = 1;
	};

	void           update_constants ();
	void           process_polarity (Smoother &voltage, float &charge, float &x, float ac);
	static float   clip (float x, float vmax);

	float          _sample_freq = 0;    // 0 = not initialized
	float          _ratio       = 0;    // 0 = not initialized
	float          _ac_freq     = 0;    // 0 = not initialized
	float          _ac_lvl      = 0;

	float          _capa        = 0;
	float          _capa_inv    = 0;    // 1 / _capa
	Smoother       _voltage_p;
	Smoother       _voltage_n;
	float          _charge_p    = 0;
	float          _charge_n    = 0;

	int32_t        _phase       = 0;    // [-0.5; 0.5[
	int32_t        _step        = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Psu &other) const = delete;
	bool           operator != (const Psu &other) const = delete;

}; // class Psu



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist3/Psu.hpp"



#endif   // mfx_pi_dist3_Psu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
