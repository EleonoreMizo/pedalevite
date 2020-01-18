/*****************************************************************************

        DelayLineSimple.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_fv_DelayLineSimple_HEADER_INCLUDED)
#define mfx_pi_cdsp_fv_DelayLineSimple_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace pi
{
namespace cdsp
{
namespace fv
{



class DelayLineSimple
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_delay (int len);

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

	int            _delay      = 0;
	int            _write_pos  = 0;
	int            _line_size  = 0;
	int            _line_mask  = 0;
	std::vector <float>
	               _line_data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayLineSimple &other) const = delete;
	bool           operator != (const DelayLineSimple &other) const = delete;

}; // class DelayLineSimple



}  // namespace fv
}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cdsp/fv/DelayLineSimple.hpp"



#endif   // mfx_pi_cdsp_fv_DelayLineSimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
