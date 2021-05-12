/*****************************************************************************

        SpeedTesterBase.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_SpeedTesterBase_HEADER_INCLUDED)
#define	hiir_test_SpeedTesterBase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace hiir
{
namespace test
{



template <class TO>
class SpeedTesterBase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {         MAX_BLOCK_LEN  = 256 };
	enum {         MAX_BUF_LEN    = MAX_BLOCK_LEN * 2 };  // For oversampled data
	enum {         MAX_BUFFERS    = 2 };

	typedef std::array <typename TO::DataType, MAX_BUF_LEN>  Buffer;
	typedef std::array <Buffer, MAX_BUFFERS> BufferArr;

	               SpeedTesterBase () noexcept;
	               ~SpeedTesterBase () = default;

	BufferArr     _src_arr;
	BufferArr     _dest_arr;
	long          _block_len;  // ]0 ; MAX_BLOCK_LEN]
	int           _nbr_blocks; // > 0



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SpeedTesterBase (const SpeedTesterBase <TO> &other) = delete;
	               SpeedTesterBase (SpeedTesterBase <TO> &&other)      = delete;
	SpeedTesterBase <TO> &
	               operator = (const SpeedTesterBase <TO> &other)      = delete;
	SpeedTesterBase <TO> &
	               operator = (SpeedTesterBase <TO> &&other)           = delete;
	bool           operator == (const SpeedTesterBase <TO> &other)     = delete;
	bool           operator != (const SpeedTesterBase <TO> &other)     = delete;

}; // class SpeedTesterBase



}  // namespace test
}  // namespace hiir



#include "hiir/test/SpeedTesterBase.hpp"



#endif   // hiir_test_SpeedTesterBase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
