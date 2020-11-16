/*****************************************************************************

        BigMuffPi.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_BigMuffPi_HEADER_INCLUDED)
#define mfx_dsp_va_BigMuffPi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/dkm/Simulator.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{



class BigMuffPi
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Pot
	{
		Pot_SUS = 0, // Gain
		Pot_TONE,
		Pot_VOL,

		Pot_NBR_ELT
	};

	void           set_sample_freq (double sample_freq);
	void           clear_buffers ();
	void           set_pot (Pot pot, float val);
	float          process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	void           reset_stats ();
	void           get_stats (dkm::Simulator::Stats &st_in, dkm::Simulator::Stats &st_dfirst, dkm::Simulator::Stats &st_dlast, dkm::Simulator::Stats &st_out) const;
#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           setup_circuit ();

	static const int  _nbr_dist_stages = 2;
	static const int  _max_it          = 20;

	class DistStage
	{
	public:
		dkm::Simulator _dist;
		int            _idx_i = -1;
		int            _idx_o = -1;
	};
	typedef std::array <DistStage, _nbr_dist_stages> DistStageArray;

	dkm::Simulator _buf_in;
	DistStageArray _dist_arr;
	dkm::Simulator _buf_out;
	bool           _constructed_flag = false;
	int            _idx_buf_in_i  = -1;
	int            _idx_buf_in_o  = -1;
	int            _idx_buf_out_i = -1;
	int            _idx_buf_out_o = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if 0
	               BigMuffPi ()                        = default;
	               BigMuffPi (const BigMuffPi &other)  = default;
	               BigMuffPi (BigMuffPi &&other)       = default;
	               ~BigMuffPi ()                       = default;
	BigMuffPi &    operator = (const BigMuffPi &other) = default;
	BigMuffPi &    operator = (BigMuffPi &&other)      = default;
#endif
	bool           operator == (const BigMuffPi &other) const = delete;
	bool           operator != (const BigMuffPi &other) const = delete;

}; // class BigMuffPi



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/BigMuffPi.hpp"



#endif   // mfx_dsp_va_BigMuffPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
