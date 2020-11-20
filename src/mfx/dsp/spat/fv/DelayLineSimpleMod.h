/*****************************************************************************

        DelayLineSimpleMod.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_fv_DelayLineSimpleMod_HEADER_INCLUDED)
#define mfx_dsp_spat_fv_DelayLineSimpleMod_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace fv
{



class DelayLineSimpleMod
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DelayLineSimpleMod ()  = default;
	virtual        ~DelayLineSimpleMod () = default;

	void           set_delay (int len, int mod_per, int mod_depth);

	void           clear_buffers ();

	int            get_max_rw_len () const;
	const float *  use_read_data () const;
	float *        use_write_data ();
	void           step (int len);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline int     compute_read_pos () const;
	inline int     compute_read_pos (int dly_cur) const;
	inline int     compute_delay () const;

	int            _delay      = 0;
	int            _mod_per    = 0;
	int            _mod_depth  = 0;
	int            _mod_pos_t  = 0;
	int            _mod_pos_d  = 0;
	int            _mod_dir    = 0;
	int            _write_pos  = 0;
	int            _line_size  = 0;
	int            _line_mask  = 0;
	std::vector <float>
	               _line_data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelayLineSimpleMod (const DelayLineSimpleMod &other) = delete;
	DelayLineSimpleMod &
	               operator = (const DelayLineSimpleMod &other)        = delete;
	bool           operator == (const DelayLineSimpleMod &other) const = delete;
	bool           operator != (const DelayLineSimpleMod &other) const = delete;

}; // class DelayLineSimpleMod



}  // namespace fv
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/spat/fv/DelayLineSimpleMod.hpp"



#endif   // mfx_dsp_spat_fv_DelayLineSimpleMod_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
