/*****************************************************************************

        AlignedObject.h
        Author: Laurent de Soras, 2005

Template parameters:
	- T: Object to construct. Should have T::T() and T::~T()

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_AlignedObject_HEADER_INCLUDED)
#define hiir_test_AlignedObject_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



// Since hiir 1.32, library objects are automatically aligned by the compiler,
// hence manual alignment is not necessary anymore.
// If for some reason the alignment fails, #undef this macro to force it as it
// was done with the previous library versions.
#define hiir_test_AlignedObject_BYPASS



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{
namespace test
{



#if defined (hiir_test_AlignedObject_BYPASS)

template <class T>
class AlignedObject
{
public:
	typedef T EmbeddedType;
	inline EmbeddedType &
	               use () { return _obj; }
	inline const EmbeddedType &
	               use () const { return _obj; }
private:
	T              _obj;
};

#else // hiir_test_AlignedObject_BYPASS

template <class T>
class AlignedObject
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T EmbeddedType;

	// Must be a power of 2. 64 should be enough for AVX-512.
	enum {         ALIGNMENT = 64 };

	               AlignedObject ();
	               ~AlignedObject ();

	inline EmbeddedType &
	               use ();
	inline const EmbeddedType &
	               use () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef char Zone [sizeof (EmbeddedType) + ALIGNMENT-1];

	Zone           _obj_zone;
	EmbeddedType * _obj_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               AlignedObject (const AlignedObject <T> &other) = delete;
	               AlignedObject (AlignedObject <T> &&other)      = delete;
	AlignedObject <T> &
	               operator = (const AlignedObject <T> &other)    = delete;
	AlignedObject <T> &
	               operator = (AlignedObject <T> &&other)         = delete;
	bool           operator == (const AlignedObject <T> &other)   = delete;
	bool           operator != (const AlignedObject <T> &other)   = delete;

}; // class AlignedObject

#endif // hiir_test_AlignedObject_BYPASS



}  // namespace test
}  // namespace hiir



#if ! defined (hiir_test_AlignedObject_BYPASS)
#include "hiir/test/AlignedObject.hpp"
#endif // hiir_test_AlignedObject_BYPASS



#endif   // hiir_test_AlignedObject_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
