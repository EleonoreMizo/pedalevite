/*****************************************************************************

        RingBufVectorizer.h
        Author: Laurent de Soras, 2017

Typical use:

RingBufVectorizer rbv (size);
for (rbv.start (arg); rbv.end (); rbv.next ())
{
	const int len = rbv.get_seg_len ();
	const int pos = rbv.get_curs_pos (0);
}

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_RingBufVectorizer_HEADER_INCLUDED)
#define mfx_dsp_dly_RingBufVectorizer_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace mfx
{
namespace dsp
{
namespace dly
{



class RingBufVectorizer
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_curs = 16;  // Maximum number of simultaneous cursors

	inline explicit
	               RingBufVectorizer (int size);
	               RingBufVectorizer (const RingBufVectorizer &other) = default;
	virtual        ~RingBufVectorizer ()                              = default;

	RingBufVectorizer &
	               operator = (const RingBufVectorizer &other)        = default;

	void				set_size (int size);
	inline int     get_size () const;
	void           set_curs (int nbr_curs, const int pos_arr []);
	void           start (int len, int pos);
	void           start (int len, int pos_0, int pos_1);
	void           start (int len, int nbr_curs, const int pos_arr []);
	void           restart (int len);
	inline bool    end () const;
	void           next ();
	inline int     get_seg_len () const;
	inline int     get_curs_pos (int curs) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	void           compute_seg ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            _size;            // Buffer size, > 0
	std::array <int, _max_nbr_curs>  // Current positions for each cursor
	               _pos;
	int            _len;             // Remaining length
	int            _seg_len;			// Length of the current segment
	int            _nbr_curs;			// Number of active cursors. > 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               RingBufVectorizer ()                               = delete;
	bool           operator == (const RingBufVectorizer &other) const = delete;
	bool           operator != (const RingBufVectorizer &other) const = delete;

}; // class RingBufVectorizer



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/RingBufVectorizer.hpp"



#endif   // mfx_dsp_dly_RingBufVectorizer_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
