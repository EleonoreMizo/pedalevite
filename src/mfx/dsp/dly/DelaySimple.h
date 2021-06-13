/*****************************************************************************

        DelaySimple.h
        Author: Laurent de Soras, 2019

Template parameters:

- T: Stored data type.

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

#include "fstb/def.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace dly
{



template <typename T>
class DelaySimple
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;

	// Mandatory call
	void           setup (int max_dly, int max_block_len);
	inline int     get_max_delay () const noexcept;

	void           set_delay (int d) noexcept;
	fstb_FORCEINLINE T
	               read_at (int d) const noexcept;
	fstb_FORCEINLINE T
	               process_sample (T x) noexcept;
	fstb_FORCEINLINE void
	               write_sample (T x) noexcept;
	void           read_block_at (T dst_ptr [], int d, int nbr_spl) const noexcept;
	void           process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept;
	fstb_FORCEINLINE void
	               write_block (const T src_ptr [], int nbr_spl) noexcept;
	fstb_FORCEINLINE void
	               push_block (const T src_ptr [], int nbr_spl) noexcept;
	fstb_FORCEINLINE void
	               step (int nbr_spl);
	void           clear_buffers () noexcept;
	void           clear_buffers_quick () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <T> Buffer;

	void           update_buf ();
	fstb_FORCEINLINE int
	               delay (int pos) const noexcept;
	int            write_block_internal (const T src_ptr [], int nbr_spl) noexcept;

	Buffer         _buf           = Buffer (64, T (0));
	int            _len           = int (_buf.size ()); // Buffer length in samples, power of 2, > 0
	int            _mask          = _len - 1;
	int            _pos_w         = 0;  // Writing position, [0 ; _len[
	int            _dly           = 0;  // Samples, >= 0
	int            _max_block_len = 64; // > 0, for block processing
	int            _max_dly       = 0;  // >= 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelaySimple &other) const = delete;
	bool           operator != (const DelaySimple &other) const = delete;

}; // class DelaySimple



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/DelaySimple.hpp"



#endif   // mfx_dsp_dly_DelaySimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
