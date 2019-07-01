/*****************************************************************************

        DelaySimple.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelaySimple_HEADER_INCLUDED)
#define mfx_dsp_dly_DelaySimple_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace dsp
{
namespace dly
{



class DelaySimple
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DelaySimple ()  = default;
	virtual        ~DelaySimple () = default;

	// Mandatory call
	void           setup (int max_dly, int max_block_len);

	void           set_delay (int d);
	inline float   process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float> Buffer;

	void           update_buf ();
	inline int     delay (int pos) const;

	Buffer         _buf           = Buffer (64, 0);
	int            _len           = 64; // Buffer length in samples, power of 2, > 0
	int            _mask          = _len - 1;
	int            _pos_w         = 0;  // Writing position, [0 ; _len[
	int            _dly           = 0;  // Samples, >= 0
	int            _max_block_len = 64; // > 0, for block processing
	int            _max_dly       = 0;  // >= 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelaySimple (const DelaySimple &other)       = delete;
	DelaySimple &  operator = (const DelaySimple &other)        = delete;
	bool           operator == (const DelaySimple &other) const = delete;
	bool           operator != (const DelaySimple &other) const = delete;

}; // class DelaySimple



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/DelaySimple.hpp"



#endif   // mfx_dsp_dly_DelaySimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
