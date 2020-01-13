/*****************************************************************************

        PedalActionSingleInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_PedalActionSingleInterface_HEADER_INCLUDED)
#define mfx_doc_PedalActionSingleInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/ActionType.h"

#include <memory>



namespace mfx
{
namespace doc
{



class SerWInterface;

class PedalActionSingleInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalActionSingleInterface ()                        = default;
	               PedalActionSingleInterface (const PedalActionSingleInterface &other) = default;
	               PedalActionSingleInterface (PedalActionSingleInterface &&other)      = default;
	virtual        ~PedalActionSingleInterface ()                       = default;
	virtual PedalActionSingleInterface &
	               operator = (const PedalActionSingleInterface &other) = default;
	virtual PedalActionSingleInterface &
	               operator = (PedalActionSingleInterface &&other)      = default;

	ActionType     get_type () const;
	std::shared_ptr <PedalActionSingleInterface>
	               duplicate () const;

	virtual void   ser_write (SerWInterface &ser) const = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual ActionType
	               do_get_type () const = 0;
	virtual std::shared_ptr <PedalActionSingleInterface>
	               do_duplicate () const = 0;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PedalActionSingleInterface &other) const = delete;
	bool           operator != (const PedalActionSingleInterface &other) const = delete;

}; // class PedalActionSingleInterface



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/PedalActionSingleInterface.hpp"



#endif   // mfx_doc_PedalActionSingleInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
