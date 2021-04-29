/*****************************************************************************

        FFTReal.h
        By Laurent de Soras

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (ffft_FFTReal_HEADER_INCLUDED)
#define	ffft_FFTReal_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"ffft/def.h"
#include	"ffft/OscSinCos.h"

#include <vector>



namespace ffft
{



template <class DT>
class FFTReal
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// So length can be represented as long int
	static constexpr int MAX_BIT_DEPTH = 30;

	typedef	DT	DataType;

	explicit			FFTReal (long length);
						FFTReal (const FFTReal &other)    = default;
						FFTReal (FFTReal &&other)         = default;

	virtual			~FFTReal ()                       = default;

	FFTReal &		operator = (const FFTReal &other) = default;
	FFTReal &		operator = (FFTReal &&other)      = default;

	long				get_length () const noexcept;
	void				do_fft (DataType f [], const DataType x []) const noexcept;
	void				do_ifft (const DataType f [], DataType x []) const noexcept;
	void				rescale (DataType x []) const noexcept;
	DataType *		use_buffer () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

   // Over this bit depth, we use direct calculation for sin/cos
   static constexpr int TRIGO_BD_LIMIT	= 12;

	typedef	OscSinCos <DataType>	OscType;

	void				init_br_lut ();
	void				init_trigo_lut ();
	void				init_trigo_osc ();

	ffft_FORCEINLINE const long *
						get_br_ptr () const noexcept;
	ffft_FORCEINLINE const DataType	*
						get_trigo_ptr (int level) const noexcept;
	ffft_FORCEINLINE long
						get_trigo_level_index (int level) const noexcept;

	inline void		compute_fft_general (DataType f [], const DataType x []) const noexcept;
	inline void		compute_direct_pass_1_2 (DataType df [], const DataType x []) const noexcept;
	inline void		compute_direct_pass_3 (DataType df [], const DataType sf []) const noexcept;
	inline void		compute_direct_pass_n (DataType df [], const DataType sf [], int pass) const noexcept;
	inline void		compute_direct_pass_n_lut (DataType df [], const DataType sf [], int pass) const noexcept;
	inline void		compute_direct_pass_n_osc (DataType df [], const DataType sf [], int pass) const noexcept;

	inline void		compute_ifft_general (const DataType f [], DataType x []) const noexcept;
	inline void		compute_inverse_pass_n (DataType df [], const DataType sf [], int pass) const noexcept;
	inline void		compute_inverse_pass_n_osc (DataType df [], const DataType sf [], int pass) const noexcept;
	inline void		compute_inverse_pass_n_lut (DataType df [], const DataType sf [], int pass) const noexcept;
	inline void		compute_inverse_pass_3 (DataType df [], const DataType sf []) const noexcept;
	inline void		compute_inverse_pass_1_2 (DataType x [], const DataType sf []) const noexcept;

	const long		_length;
	const int		_nbr_bits;
	std::vector <long>
						_br_lut;
	std::vector <DataType>
						_trigo_lut;
	mutable std::vector <DataType>
						_buffer;
   mutable std::vector <OscType>
						_trigo_osc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						FFTReal ()                         = delete;
	bool				operator == (const FFTReal &other) = delete;
	bool				operator != (const FFTReal &other) = delete;

};	// class FFTReal



}	// namespace ffft



#include	"ffft/FFTReal.hpp"



#endif	// ffft_FFTReal_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
