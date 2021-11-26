/*****************************************************************************

        Simd.hpp
        Author: Laurent de Soras, 2001

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_mix_Simd_CODEHEADER_INCLUDED)
#define mfx_dsp_mix_Simd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"
#include "mfx/dsp/mix/Fpu.h"
#include "mfx/dsp/StereoLevel.h"

#include <array>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace mix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*****************************************************************************
*
*       SCALE
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::scale_1_v (float data_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (data_ptr, nbr_spl);
	}

	else if (vol != 1.0f)
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);

		while (pos < nbr_loop)
		{
			auto			vec_data = V128Dst::load_f32 (data_ptr);
			vec_data *= vec_vol;
			V128Dst::store_f32 (data_ptr, vec_data);
			data_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::scale_1_v (data_ptr, nbr_spl, vol);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::scale_1_vlr (float data_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Dst::load_f32 (data_ptr);
		vec_data *= vec_vol;
		V128Dst::store_f32 (data_ptr, vec_data);
		vec_vol += vec_step;
		data_ptr += 4;
		++pos;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::scale_1_vlr (
			data_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::scale_1_vlrauto (float data_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		scale_1_v (data_ptr, nbr_spl, s_vol);
	}
	else
	{
		scale_1_vlr (data_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::scale_2_v (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (data_1_ptr));
	assert (V128Dst::check_ptr (data_2_ptr));

	if (vol == 0.0f)
	{
		clear (data_1_ptr, nbr_spl);
		clear (data_2_ptr, nbr_spl);
	}

	else if (vol != 1.0f)
	{
		auto			vec_vol = fstb::Vf32 (vol);
		
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		while (pos < nbr_loop)
		{
			auto			vec_data_1 = V128Dst::load_f32 (data_1_ptr);
			auto			vec_data_2 = V128Dst::load_f32 (data_2_ptr);
			vec_data_1 *= vec_vol;
			vec_data_2 *= vec_vol;
			V128Dst::store_f32 (data_1_ptr, vec_data_1); 
			V128Dst::store_f32 (data_2_ptr, vec_data_2); 		
			data_1_ptr += 4;
			data_2_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::scale_2_v (
				data_1_ptr,
				data_2_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::scale_2_vlr (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (data_1_ptr));
	assert (V128Dst::check_ptr (data_2_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		auto			vec_data_1 = V128Dst::load_f32 (data_1_ptr);
		auto			vec_data_2 = V128Dst::load_f32 (data_2_ptr);
		vec_data_1 *= vec_vol;
		vec_data_2 *= vec_vol;
		V128Dst::store_f32 (data_1_ptr, vec_data_1); 
		V128Dst::store_f32 (data_2_ptr, vec_data_2); 		
		vec_vol += vec_step;
		data_1_ptr += 4;
		data_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::scale_2_vlr (
			data_1_ptr,
			data_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::scale_2_vlrauto (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		scale_2_v (data_1_ptr, data_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		scale_2_vlr (data_1_ptr, data_2_ptr, nbr_spl, s_vol, e_vol);
	}
}




/*****************************************************************************
*
*       COPY
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

#if 1

	memcpy (out_ptr, in_ptr, nbr_spl * sizeof (out_ptr [0]));

#else

	const int			nbr_loop = nbr_spl >> 2;
	int					pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	vec_data = V128Src::load_f32 (in_ptr);
		V128Dst::store_f32 (out_ptr, vec_data);
		in_ptr += 4;
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_1 (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
#endif
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);
	
	if (vol == 0.0f)
	{
		clear (out_ptr, nbr_spl);
	}

	else if (vol == 1.0f)
	{
		copy_1_1 (out_ptr, in_ptr, nbr_spl);
	}

	else
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto     vec_vol = fstb::Vf32 (vol);

		while (pos < nbr_loop)
		{
			auto			vec_data = V128Src::load_f32 (in_ptr);
			vec_data *= vec_vol;
			V128Dst::store_f32 (out_ptr, vec_data);
			in_ptr += 4;
			out_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::copy_1_1_v (
				out_ptr,
				in_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= vec_vol;
		V128Dst::store_f32 (out_ptr, vec_data);
		vec_vol += vec_step;
		in_ptr += 4;
		out_ptr += 4;
		++pos;
	}
			
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_1_vlr (
			out_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_1_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_1_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);
	
#if 1

	memcpy (out_1_ptr, in_ptr, nbr_spl * sizeof (out_1_ptr [0]));
	memcpy (out_2_ptr, in_ptr, nbr_spl * sizeof (out_2_ptr [0]));

#else

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	vec_data = V128Src::load_f32 (in_ptr);
		V128Dst::store_f32 (out_1_ptr, vec_data);
		V128Dst::store_f32 (out_2_ptr, vec_data);
		in_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_2 (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl
		);
	}

#endif
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_1_ptr, nbr_spl);
		clear (out_2_ptr, nbr_spl);
	}

	else if (vol == 1.0f)
	{
		copy_1_2 (out_1_ptr, out_2_ptr, in_ptr, nbr_spl);
	}

	else
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);

		while (pos < nbr_loop)
		{
			auto			vec_data = V128Src::load_f32 (in_ptr);
			vec_data *= vec_vol;
			V128Dst::store_f32 (out_1_ptr, vec_data);
			V128Dst::store_f32 (out_2_ptr, vec_data);
			in_ptr += 4;
			out_1_ptr += 4;
			out_2_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::copy_1_2_v (
				out_1_ptr,
				out_2_ptr,
				in_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= vec_vol;
		V128Dst::store_f32 (out_1_ptr, vec_data);
		V128Dst::store_f32 (out_2_ptr, vec_data);
		vec_vol += vec_step;
		in_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	vec_data_1 = V128Src::load_f32 (in_1_ptr);
		const auto	vec_data_2 = V128Src::load_f32 (in_2_ptr);
		const auto	vec_data = vec_data_1 + vec_data_2;
		V128Dst::store_f32 (out_ptr, vec_data);
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_1 (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_ptr, nbr_spl);
	}

	else if (vol == 1.0f)
	{
		copy_2_1 (out_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}

	else
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);

		while (pos < nbr_loop)
		{
			const auto	vec_data_1 = V128Src::load_f32 (in_1_ptr);
			const auto	vec_data_2 = V128Src::load_f32 (in_2_ptr);
			auto			vec_data = vec_data_1 + vec_data_2;
			vec_data *= vec_vol;
			V128Dst::store_f32 (out_ptr, vec_data);
			in_1_ptr += 4;
			in_2_ptr += 4;
			out_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::copy_2_1_v (
				out_ptr,
				in_1_ptr,
				in_2_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	vec_data_1 = V128Src::load_f32 (in_1_ptr);
		const auto	vec_data_2 = V128Src::load_f32 (in_2_ptr);
		auto			vec_data = vec_data_1 + vec_data_2;
		vec_data *= vec_vol;
		V128Dst::store_f32 (out_ptr, vec_data);
		vec_vol += vec_step;
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_ptr += 4;
		++pos;
	}
		
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_1_vlr (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_2_1_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2_1_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

#if 1

	memcpy (out_1_ptr, in_1_ptr, nbr_spl * sizeof (out_1_ptr [0]));
	memcpy (out_2_ptr, in_2_ptr, nbr_spl * sizeof (out_2_ptr [0]));

#else

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	vec_data_1 = V128Src::load_f32 (in_1_ptr);
		const auto	vec_data_2 = V128Src::load_f32 (in_2_ptr);
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_2 (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}

#endif
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_1_ptr, nbr_spl);
		clear (out_2_ptr, nbr_spl);
	}

	else if (vol == 1.0f)
	{
		copy_2_2 (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}

	else
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);

		while (pos < nbr_loop)
		{
			auto			vec_data_1 = V128Src::load_f32 (in_1_ptr);
			auto			vec_data_2 = V128Src::load_f32 (in_2_ptr);
			vec_data_1 *= vec_vol;
			vec_data_2 *= vec_vol;
			V128Dst::store_f32 (out_1_ptr, vec_data_1);
			V128Dst::store_f32 (out_2_ptr, vec_data_2);
			in_1_ptr += 4;
			in_2_ptr += 4;
			out_1_ptr += 4;
			out_2_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::copy_2_2_v (
				out_1_ptr,
				out_2_ptr,
				in_1_ptr,
				in_2_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		auto			vec_data_1 = V128Src::load_f32 (in_1_ptr);
		auto			vec_data_2 = V128Src::load_f32 (in_2_ptr);
		vec_data_1 *= vec_vol;
		vec_data_2 *= vec_vol;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		vec_vol += vec_step;
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       SPREAD COPYING
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::copy_spread_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	   vol_l_vec = fstb::Vf32 (vol_l);
	const auto	   vol_r_vec = fstb::Vf32 (vol_r);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		const auto	out_1_vec_data = in_vec_data * vol_l_vec;
		const auto	out_2_vec_data = in_vec_data * vol_r_vec;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_1_ptr += 4;
		out_2_ptr += 4;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_spread_1_2_v (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			vol_l,
			vol_r
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_spread_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol_l;
	fstb::Vf32     vec_step_l;
	fstb::ToolsSimd::start_lerp (vec_vol_l, vec_step_l, s_vol_l, e_vol_l, nbr_spl);

	fstb::Vf32     vec_vol_r;
	fstb::Vf32     vec_step_r;
	fstb::ToolsSimd::start_lerp (vec_vol_r, vec_step_r, s_vol_r, e_vol_r, nbr_spl);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		const auto	out_1_vec_data = in_vec_data * vec_vol_l;
		const auto	out_2_vec_data = in_vec_data * vec_vol_r;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		vec_vol_l += vec_step_l;
		vec_vol_r += vec_step_r;
		in_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_spread_1_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_l),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_r),
			e_vol_l,
			e_vol_r
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_spread_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) noexcept
{
	if (e_vol_l == s_vol_l && e_vol_r == s_vol_r)
	{
		copy_spread_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r);
	}
	else
	{
		copy_spread_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r, e_vol_l, e_vol_r);
	}
}



/*****************************************************************************
*
*       CROSS-FADE COPYING
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::copy_xfade_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float xf) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	xf_vec = fstb::Vf32 (xf);
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		const auto	diff_vec_data = in_2_vec_data - in_1_vec_data;
		const auto	mul_vec_data = diff_vec_data * xf_vec;
		const auto	out_vec_data = in_1_vec_data + mul_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		out_ptr += 4;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_xfade_2_1_v (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			xf
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_xfade_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_xf;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_xf, vec_step, s_xf, e_xf, nbr_spl);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		const auto	diff_vec_data = in_2_vec_data - in_1_vec_data;
		const auto	mul_vec_data = diff_vec_data * vec_xf;
		const auto	out_vec_data = in_1_vec_data + mul_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		vec_xf += vec_step;
		out_ptr += 4;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_xfade_2_1_vlr (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_xf),
			e_xf
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_xfade_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf) noexcept
{
	if (e_xf == s_xf)
	{
		copy_xfade_2_1_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_xf);
	}
	else
	{
		copy_xfade_2_1_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_xf, e_xf);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_xfade_3_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (V128Src::check_ptr (in_3_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		const auto	xf_vec_data   = V128Src::load_f32 (in_3_ptr);
		const auto	diff_vec_data = in_2_vec_data - in_1_vec_data;
		const auto	mul_vec_data = diff_vec_data * xf_vec_data;
		const auto	out_vec_data = in_1_vec_data + mul_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		out_ptr  += 4;
		in_1_ptr += 4;
		in_2_ptr += 4;
		in_3_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_xfade_3_1 (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			in_3_ptr,
			nbr_spl
		);
	}
}



/*****************************************************************************
*
*       MATRIX COPYING
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::copy_mat_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);
	
	const int nbr_loop = nbr_spl >> 2;
	int pos = 0;
	
	const float &	vol_l2l = vol.get_l2l ();
	const float &	vol_r2l = vol.get_r2l ();
	const float &	vol_l2r = vol.get_l2r ();
	const float &	vol_r2r = vol.get_r2r ();
	
	const auto	vec_vol_l2l = fstb::Vf32 (vol_l2l);
	const auto	vec_vol_r2l = fstb::Vf32 (vol_r2l);
	const auto	vec_vol_l2r = fstb::Vf32 (vol_l2r);
	const auto	vec_vol_r2r = fstb::Vf32 (vol_r2r);

	while (pos < nbr_loop)
	{
		const auto	in_data_1 = V128Src::load_f32 (in_1_ptr);
		auto			in_data_2 = V128Src::load_f32 (in_2_ptr);
		const auto	vol_data_l2l = in_data_1 * vec_vol_l2l;
		const auto	vol_data_r2l = in_data_2 * vec_vol_r2l;
		const auto	vol_data_l2r = in_data_1 * vec_vol_l2r;
		const auto	vol_data_r2r = in_data_2 * vec_vol_r2r;
		const auto	vec_data_1 = vol_data_l2l + vol_data_r2l;
		const auto	vec_data_2 = vol_data_l2r + vol_data_r2r;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_mat_2_2_v (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_mat_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	StereoLevel		step = e_vol;
	step -= s_vol;
	step *= 4.f / float (nbr_spl);
	
	const auto  c0123 = fstb::Vf32 (0, 0.25f, 0.5f, 0.75f);

	auto        vec_step_l2l = fstb::Vf32 (step.get_l2l ());
	auto        vec_vol_l2l = fstb::Vf32 (s_vol.get_l2l ());
	vec_vol_l2l.mac (vec_step_l2l, c0123);

	auto        vec_step_r2l = fstb::Vf32 (step.get_r2l ());
	auto        vec_vol_r2l = fstb::Vf32 (s_vol.get_r2l ());
	vec_vol_r2l.mac (vec_step_r2l, c0123);

	auto        vec_step_l2r = fstb::Vf32 (step.get_l2r ());
	auto        vec_vol_l2r = fstb::Vf32 (s_vol.get_l2r ());
	vec_vol_l2r.mac (vec_step_l2r, c0123);
	
	auto        vec_step_r2r = fstb::Vf32 (step.get_r2r ());
	auto        vec_vol_r2r = fstb::Vf32 (s_vol.get_r2r ());
	vec_vol_r2r.mac (vec_step_r2r, c0123);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	in_data_1 = V128Src::load_f32 (in_1_ptr);
		const auto	in_data_2 = V128Src::load_f32 (in_2_ptr);
		const auto	vol_data_l2l = in_data_1 * vec_vol_l2l;
		const auto	vol_data_r2l = in_data_2 * vec_vol_r2l;
		const auto	vol_data_l2r = in_data_1 * vec_vol_l2r;
		const auto	vol_data_r2r = in_data_2 * vec_vol_r2r;
		const auto	vec_data_1 = vol_data_l2l + vol_data_r2l;
		const auto	vec_data_2 = vol_data_l2r + vol_data_r2r;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		vec_vol_l2l += vec_step_l2l;
		vec_vol_l2r += vec_step_l2r;
		vec_vol_r2l += vec_step_r2l;
		vec_vol_r2r += vec_step_r2r;
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		StereoLevel new_s_vol (
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_l2l),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_l2r),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_r2l),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_r2r)
		);
		
		Fpu::copy_mat_2_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			new_s_vol,
			e_vol
		);
	}
}




template <class VD, class VS>
void	Simd <VD, VS>::copy_mat_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_mat_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_mat_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       COPY AND INTERLEAVE
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2i (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	vec_data = V128Src::load_f32 (in_ptr);
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::interleave_f32 (vec_data_1, vec_data_2, vec_data, vec_data);
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_ptr += 4;
		V128Dst::store_f32 (out_ptr, vec_data_2);
		out_ptr += 4;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_2i (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2i_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::interleave_f32 (vec_data_1, vec_data_2, vec_data, vec_data);
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_ptr += 4;
		V128Dst::store_f32 (out_ptr, vec_data_2);
		out_ptr += 4;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_2i_v (
			out_ptr,
			in_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2i_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= vec_vol;
		fstb::Vf32 vec_data_1;
		fstb::Vf32 vec_data_2;
		fstb::ToolsSimd::interleave_f32 (vec_data_1, vec_data_2, vec_data, vec_data);
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_ptr += 4;
		V128Dst::store_f32 (out_ptr, vec_data_2);
		out_ptr += 4;
		vec_vol += vec_step;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_1_2i_vlr (
			out_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_1_2i_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_1_2i_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_1_2i_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		fstb::Vf32 vec_data_1;
		fstb::Vf32 vec_data_2;
		fstb::ToolsSimd::interleave_f32 (vec_data_1, vec_data_2, in_1_vec_data, in_2_vec_data);
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_ptr += 4;
		V128Dst::store_f32 (out_ptr, vec_data_2);
		out_ptr += 4;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_2i (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2i_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_vec_data *= vec_vol;
		auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::interleave_f32 (vec_data_1, vec_data_2, in_1_vec_data, in_2_vec_data);
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_ptr += 4;
		V128Dst::store_f32 (out_ptr, vec_data_2);
		out_ptr += 4;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_2i_v (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2i_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	while (pos < nbr_loop)
	{
		auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_vec_data *= vec_vol;
		auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::interleave_f32 (vec_data_1, vec_data_2, in_1_vec_data, in_2_vec_data);
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_ptr += 4;
		V128Dst::store_f32 (out_ptr, vec_data_2);
		out_ptr += 4;
		vec_vol += vec_step;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_2i_vlr (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_2i_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_2_2i_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2_2i_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_4_4i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], const float in_4_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (V128Src::check_ptr (in_3_ptr));
	assert (V128Src::check_ptr (in_4_ptr));
	assert (nbr_spl > 0);

	for (int nbr_loop = nbr_spl >> 2; nbr_loop > 0; --nbr_loop)
	{
		// We start with:
		auto           a1 = V128Src::load_f32 (in_1_ptr);	// A1 B1 C1 D1
		auto           a2 = V128Src::load_f32 (in_2_ptr);	// A2 B2 C2 D2
		auto           a3 = V128Src::load_f32 (in_3_ptr);	// A3 B3 C3 D3
		auto           a4 = V128Src::load_f32 (in_4_ptr);	// A4 B4 C4 D4

		fstb::ToolsSimd::transpose_f32 (a1, a2, a3, a4);

		V128Dst::store_f32 (out_ptr     , a1);
		V128Dst::store_f32 (out_ptr +  4, a2);
		V128Dst::store_f32 (out_ptr +  8, a3);
		V128Dst::store_f32 (out_ptr + 12, a4);

		in_1_ptr += 4;
		in_2_ptr += 4;
		in_3_ptr += 4;
		in_4_ptr += 4;
		out_ptr += 16;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_4_4i (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			in_3_ptr,
			in_4_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2_4i2 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	for (int nbr_loop = nbr_spl >> 2; nbr_loop > 0; --nbr_loop)
	{
		// We start with:
		const auto	i1 = V128Src::load_f32 (in_1_ptr);	// A1 B1 C1 D1
		const auto	i2 = V128Src::load_f32 (in_2_ptr);	// A2 B2 C2 D2

		// First unpacking
		fstb::Vf32     o1;	// A1 A2 B1 B2
		fstb::Vf32     o3;	// C1 C2 D1 D2
		fstb::ToolsSimd::interleave_f32 (o1, o3, i1, i2);

		// Upper part to lower pos.
		const auto	o2 = fstb::ToolsSimd::Shift <2>::rotate (o1);	// B1 B2 xx xx
		const auto	o4 = fstb::ToolsSimd::Shift <2>::rotate (o3);	// D1 D2 xx xx

		V128Dst::store_f32 (out_ptr     , o1);
		V128Dst::store_f32 (out_ptr +  4, o2);
		V128Dst::store_f32 (out_ptr +  8, o3);
		V128Dst::store_f32 (out_ptr + 12, o4);

		in_1_ptr += 4;
		in_2_ptr += 4;
		out_ptr += 8;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2_4i2 (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



/*****************************************************************************
*
*       COPY AND DEINTERLEAVE
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_1 (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr);
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		const auto	vec_data = vec_data_1 + vec_data_2;
		V128Dst::store_f32 (out_ptr, vec_data);
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2i_1 (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		auto			vec_data = vec_data_1 + vec_data_2;
		vec_data *= vec_vol;
		V128Dst::store_f32 (out_ptr, vec_data);
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2i_1_v (
			out_ptr,
			in_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		auto			vec_data = vec_data_1 + vec_data_2;
		vec_data *= vec_vol;
		V128Dst::store_f32 (out_ptr, vec_data);
		vec_vol += vec_step;
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2i_1_vlr (
			out_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_2i_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2i_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2i_2 (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		vec_data_1 *= vec_vol;
		vec_data_2 *= vec_vol;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2i_2_v (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		vec_data_1 *= vec_vol;
		vec_data_2 *= vec_vol;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		vec_vol += vec_step;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_2i_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_2i_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		copy_2i_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2i_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_4i_1 (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	for (int nbr_loop = nbr_spl >> 2; nbr_loop > 0; --nbr_loop)
	{
		// We start with:
		auto       a1 = V128Src::load_f32 (in_ptr     );
		auto       a2 = V128Src::load_f32 (in_ptr +  4);
		auto       a3 = V128Src::load_f32 (in_ptr +  8);
		auto       a4 = V128Src::load_f32 (in_ptr + 12);

		fstb::ToolsSimd::transpose_f32 (a1, a2, a3, a4);

		// Sum
		const auto	s12 =  a1 +  a2;
		const auto	s34 =  a3 +  a4;
		const auto	s   = s12 + s34;

		V128Dst::store_f32 (out_ptr, s);

		in_ptr += 16;
		out_ptr += 4;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_4i_1 (out_ptr, in_ptr, nbr_spl);
	}
}




template <class VD, class VS>
void	Simd <VD, VS>::copy_4i_4 (float out_1_ptr [], float out_2_ptr [], float out_3_ptr [], float out_4_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Dst::check_ptr (out_3_ptr));
	assert (V128Dst::check_ptr (out_4_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	for (int nbr_loop = nbr_spl >> 2; nbr_loop > 0; --nbr_loop)
	{
		// We start with:
		auto           a1 = V128Src::load_f32 (in_ptr     );	// A1 A2 A3 A4
		auto           a2 = V128Src::load_f32 (in_ptr +  4);	// B1 B2 B3 B4
		auto           a3 = V128Src::load_f32 (in_ptr +  8);	// C1 C2 C3 C4
		auto           a4 = V128Src::load_f32 (in_ptr + 12);	// D1 D2 D3 D4

		fstb::ToolsSimd::transpose_f32 (a1, a2, a3, a4);

		V128Dst::store_f32 (out_1_ptr, a1);
		V128Dst::store_f32 (out_2_ptr, a2);
		V128Dst::store_f32 (out_3_ptr, a3);
		V128Dst::store_f32 (out_4_ptr, a4);

		in_ptr += 16;
		out_1_ptr += 4;
		out_2_ptr += 4;
		out_3_ptr += 4;
		out_4_ptr += 4;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_4i_4 (
			out_1_ptr,
			out_2_ptr,
			out_3_ptr,
			out_4_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::copy_4i2_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);


	for (int nbr_loop = nbr_spl >> 2; nbr_loop > 0; --nbr_loop)
	{
		// We start with:
		auto           a1 = V128Src::load_f32 (in_ptr     );	// A1 A2 xx xx
		auto           a2 = V128Src::load_f32 (in_ptr +  4);	// B1 B2 xx xx
		auto           a3 = V128Src::load_f32 (in_ptr +  8);	// C1 C2 xx xx
		auto           a4 = V128Src::load_f32 (in_ptr + 12);	// D1 D2 xx xx

		fstb::Vf32     k1;   // A1 B1 A2 B2
		fstb::Vf32     k2;   // C1 D1 C2 D2
		fstb::Vf32     dummy;
		fstb::ToolsSimd::interleave_f32 (k1, dummy, a1, a2);
		fstb::ToolsSimd::interleave_f32 (k2, dummy, a3, a4);
		a1 = fstb::ToolsSimd::interleave_2f32_lo (k1, k2);
		a2 = fstb::ToolsSimd::interleave_2f32_hi (k1, k2);
		V128Dst::store_f32 (out_1_ptr, a1);
		V128Dst::store_f32 (out_2_ptr, a2);

		in_ptr += 16;
		out_1_ptr += 4;
		out_2_ptr += 4;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::copy_4i2_2 (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



/*****************************************************************************
*
*       COPY AND CONVERT INTERLEAVING
*
*****************************************************************************/



/*****************************************************************************
*
*       MIX
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		in_ptr += 4;
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		out_vec_data += in_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_1 (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_1_1 (out_ptr, in_ptr, nbr_spl);
	}

	else if (vol != 0.0f)
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);
		
		while (pos < nbr_loop)
		{
			auto			in_vec_data = V128Src::load_f32 (in_ptr);
			in_ptr += 4;
			in_vec_data *= vec_vol;
			auto			out_vec_data = V128Dst::load_f32 (out_ptr);
			out_vec_data += in_vec_data;
			V128Dst::store_f32 (out_ptr, out_vec_data);
			out_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::mix_1_1_v (
				out_ptr,
				in_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	while (pos < nbr_loop)
	{
		auto			in_vec_data = V128Src::load_f32 (in_ptr);
		in_ptr += 4;
		in_vec_data *= vec_vol;
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		out_vec_data += in_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		out_ptr += 4;
		vec_vol += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_1_vlr (
			out_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_1_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_1_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		in_ptr += 4;
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_1_vec_data += in_vec_data;
		out_2_vec_data += in_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_2 (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_1_2 (out_1_ptr, out_2_ptr, in_ptr, nbr_spl);
	}

	else if (vol != 0.0f)
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);
		
		while (pos < nbr_loop)
		{
			auto			in_vec_data = V128Src::load_f32 (in_ptr);
			in_vec_data *= vec_vol;
			in_ptr += 4;
			auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
			auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
			out_1_vec_data += in_vec_data;
			out_2_vec_data += in_vec_data;
			V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
			out_1_ptr += 4;
			V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
			out_2_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::mix_1_2_v (
				out_1_ptr,
				out_2_ptr,
				in_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	while (pos < nbr_loop)
	{
		auto			in_vec_data = V128Src::load_f32 (in_ptr);
		in_vec_data *= vec_vol;
		in_ptr += 4;
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_1_vec_data += in_vec_data;
		out_2_vec_data += in_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_2_ptr += 4;
		vec_vol += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_ptr += 4;
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_ptr += 4;
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		out_vec_data += in_1_vec_data;
		out_vec_data += in_2_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_1 (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_2_1 (out_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}

	else if (vol != 0.0f)
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);
		
		while (pos < nbr_loop)
		{
			auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
			in_1_vec_data *= vec_vol;
			in_1_ptr += 4;
			auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
			in_2_vec_data *= vec_vol;
			in_2_ptr += 4;
			auto			out_vec_data = V128Dst::load_f32 (out_ptr);
			out_vec_data += in_1_vec_data;
			out_vec_data += in_2_vec_data;
			V128Dst::store_f32 (out_ptr, out_vec_data);
			out_ptr += 4;
			++pos;
		}
		
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::mix_2_1_v (
				out_ptr,
				in_1_ptr,
				in_2_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
	
	while (pos < nbr_loop)
	{
		auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_vec_data *= vec_vol;
		in_1_ptr += 4;
		auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_vec_data *= vec_vol;
		in_2_ptr += 4;
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		out_vec_data += in_1_vec_data;
		out_vec_data += in_2_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		out_ptr += 4;
		vec_vol += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_1_vlr (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_2_1_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2_1_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_ptr += 4;
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_ptr += 4;
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		out_1_vec_data += in_1_vec_data;
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_2_vec_data += in_2_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_2 (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_2_2 (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}

	else if (vol != 0.0f)
	{
		const int		nbr_loop = nbr_spl >> 2;
		int				pos = 0;
		
		const auto	vec_vol = fstb::Vf32 (vol);
		
		while (pos < nbr_loop)
		{
			auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
			in_1_vec_data *= vec_vol;
			in_1_ptr += 4;
			auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
			in_2_vec_data *= vec_vol;
			in_2_ptr += 4;
			auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
			out_1_vec_data += in_1_vec_data;
			auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
			out_2_vec_data += in_2_vec_data;
			V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
			out_1_ptr += 4;
			V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
			out_2_ptr += 4;
			++pos;
		}
			
		nbr_spl &= 3;
		if (nbr_spl > 0)
		{
			Fpu::mix_2_2_v (
				out_1_ptr,
				out_2_ptr,
				in_1_ptr,
				in_2_ptr,
				nbr_spl,
				vol
			);
		}
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);
		
	while (pos < nbr_loop)
	{
		auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_vec_data *= vec_vol;
		in_1_ptr += 4;
		auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_vec_data *= vec_vol;
		in_2_ptr += 4;
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		out_1_vec_data += in_1_vec_data;
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_2_vec_data += in_2_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_2_ptr += 4;
		vec_vol += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       SPREAD MIXING
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mix_spread_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vol_l_vec = fstb::Vf32 (vol_l);
	const auto	vol_r_vec = fstb::Vf32 (vol_r);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		
		const auto	add_out_1_vec_data = in_vec_data * vol_l_vec;
		const auto	add_out_2_vec_data = in_vec_data * vol_r_vec;
		
		out_1_vec_data += add_out_1_vec_data;
		out_2_vec_data += add_out_2_vec_data;

		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_1_ptr += 4;
		out_2_ptr += 4;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_spread_1_2_v (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			vol_l,
			vol_r
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_spread_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	fstb::Vf32     vec_vol_l;
	fstb::Vf32     vec_step_l;
	fstb::ToolsSimd::start_lerp (vec_vol_l, vec_step_l, s_vol_l, e_vol_l, nbr_spl);

	fstb::Vf32     vec_vol_r;
	fstb::Vf32     vec_step_r;
	fstb::ToolsSimd::start_lerp (vec_vol_r, vec_step_r, s_vol_r, e_vol_r, nbr_spl);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);

		const auto	add_out_1_vec_data = in_vec_data * vec_vol_l;
		const auto	add_out_2_vec_data = in_vec_data * vec_vol_r;

		out_1_vec_data += add_out_1_vec_data;
		out_2_vec_data += add_out_2_vec_data;

		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		vec_vol_l += vec_step_l;
		vec_vol_r += vec_step_r;
		in_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}

	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_spread_1_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_l),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_r),
			e_vol_l,
			e_vol_r
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_spread_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r) noexcept
{
	if (e_vol_l == s_vol_l && e_vol_r == s_vol_r)
	{
		mix_spread_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r);
	}
	else
	{
		mix_spread_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r, e_vol_l, e_vol_r);
	}
}



/*****************************************************************************
*
*       MATRIX MIXING
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mix_mat_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const float &	vol_l2l = vol.get_l2l ();
	const float &	vol_r2l = vol.get_r2l ();
	const float &	vol_l2r = vol.get_l2r ();
	const float &	vol_r2r = vol.get_r2r ();
	
	const auto	vec_vol_l2l = fstb::Vf32 (vol_l2l);
	const auto	vec_vol_r2l = fstb::Vf32 (vol_r2l);
	const auto	vec_vol_l2r = fstb::Vf32 (vol_l2r);
	const auto	vec_vol_r2r = fstb::Vf32 (vol_r2r);

	while (pos < nbr_loop)
	{
		const auto	in_data_1 = V128Src::load_f32 (in_1_ptr);
		const auto	in_data_2 = V128Src::load_f32 (in_2_ptr);
		const auto	vol_data_l2l = in_data_1 * vec_vol_l2l;
		const auto	vol_data_r2l = in_data_2 * vec_vol_r2l;
		const auto	vol_data_l2r = in_data_1 * vec_vol_l2r;
		const auto	vol_data_r2r = in_data_2 * vec_vol_r2r;
		auto			vec_data_1 = vol_data_l2l + vol_data_r2l;
		auto			vec_data_2 = vol_data_l2r + vol_data_r2r;
		const auto	out_data_1 = V128Dst::load_f32 (out_1_ptr);
		const auto	out_data_2 = V128Dst::load_f32 (out_2_ptr);
		vec_data_1 += out_data_1;
		vec_data_2 += out_data_2;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_mat_2_2_v (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_mat_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	StereoLevel		step = e_vol;
	step -= s_vol;
	step *= 4.f / float (nbr_spl);
	
	const auto  c0123 = fstb::Vf32 (0, 0.25f, 0.5f, 0.75f);

	auto        vec_step_l2l = fstb::Vf32 (step.get_l2l ());
	auto        vec_vol_l2l = fstb::Vf32 (s_vol.get_l2l ());
	vec_vol_l2l.mac (vec_step_l2l, c0123);

	auto        vec_step_r2l = fstb::Vf32 (step.get_r2l ());
	auto        vec_vol_r2l = fstb::Vf32 (s_vol.get_r2l ());
	vec_vol_r2l.mac (vec_step_r2l, c0123);

	auto        vec_step_l2r = fstb::Vf32 (step.get_l2r ());
	auto        vec_vol_l2r = fstb::Vf32 (s_vol.get_l2r ());
	vec_vol_l2r.mac (vec_step_l2r, c0123);
	
	auto        vec_step_r2r = fstb::Vf32 (step.get_r2r ());
	auto        vec_vol_r2r = fstb::Vf32 (s_vol.get_r2r ());
	vec_vol_r2r.mac (vec_step_r2r, c0123);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	in_data_1 = V128Src::load_f32 (in_1_ptr);
		const auto	in_data_2 = V128Src::load_f32 (in_2_ptr);
		const auto	vol_data_l2l = in_data_1 * vec_vol_l2l;
		const auto	vol_data_r2l = in_data_2 * vec_vol_r2l;
		const auto	vol_data_l2r = in_data_1 * vec_vol_l2r;
		const auto	vol_data_r2r = in_data_2 * vec_vol_r2r;
		auto			vec_data_1 = vol_data_l2l + vol_data_r2l;
		auto			vec_data_2 = vol_data_l2r + vol_data_r2r;
		const auto	out_data_1 = V128Dst::load_f32 (out_1_ptr);
		const auto	out_data_2 = V128Dst::load_f32 (out_2_ptr);
		vec_data_1 += out_data_1;
		vec_data_2 += out_data_2;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		vec_vol_l2l += vec_step_l2l;
		vec_vol_l2r += vec_step_l2r;
		vec_vol_r2l += vec_step_r2l;
		vec_vol_r2r += vec_step_r2r;
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
		
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		StereoLevel new_s_vol (
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_l2l),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_l2r),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_r2l),
			fstb::ToolsSimd::Shift <0>::extract (vec_vol_r2r)
		);
		
		Fpu::mix_mat_2_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			new_s_vol,
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_mat_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_mat_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_mat_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       MIX AND INTERLEAVE
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2i (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	vec_data = V128Src::load_f32 (in_ptr);
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, vec_data, vec_data);
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data_1 += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_vec_data = V128Dst::load_f32 (out_ptr + 4);
		vec_data_2 += out_vec_data;
		V128Dst::store_f32 (out_ptr + 4, vec_data_2);
		out_ptr += 8;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_2i (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2i_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, vec_data, vec_data);
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data_1 += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_vec_data = V128Dst::load_f32 (out_ptr + 4);
		vec_data_2 += out_vec_data;
		V128Dst::store_f32 (out_ptr + 4, vec_data_2);
		out_ptr += 8;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_2i_v (
			out_ptr,
			in_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2i_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, vec_data, vec_data);
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data_1 += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_vec_data = V128Dst::load_f32 (out_ptr + 4);
		vec_data_2 += out_vec_data;
		V128Dst::store_f32 (out_ptr + 4, vec_data_2);
		out_ptr += 8;
		vec_vol += vec_step;
		in_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_1_2i_vlr (
			out_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_1_2i_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_1_2i_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_1_2i_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_1_vec_data, in_2_vec_data);
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data_1 += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_vec_data = V128Dst::load_f32 (out_ptr + 4);
		vec_data_2 += out_vec_data;
		V128Dst::store_f32 (out_ptr + 4, vec_data_2);
		out_ptr += 8;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_2i (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2i_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		auto			in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_vec_data *= vec_vol;
		auto			in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_1_vec_data, in_2_vec_data);
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data_1 += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_vec_data = V128Dst::load_f32 (out_ptr + 4);
		vec_data_2 += out_vec_data;
		V128Dst::store_f32 (out_ptr + 4, vec_data_2);
		out_ptr += 8;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_2i_v (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2i_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int      nbr_loop = nbr_spl >> 2;
	int            pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	while (pos < nbr_loop)
	{
		auto			   in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		in_1_vec_data *= vec_vol;
		auto			   in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		in_2_vec_data *= vec_vol;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_1_vec_data, in_2_vec_data);
		auto           out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data_1 += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data_1);
		out_vec_data = V128Dst::load_f32 (out_ptr + 4);
		vec_data_2 += out_vec_data;
		V128Dst::store_f32 (out_ptr + 4, vec_data_2);
		out_ptr += 8;
		vec_vol += vec_step;
		in_1_ptr += 4;
		in_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2_2i_vlr (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2_2i_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_2_2i_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2_2i_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       MIX AND DEINTERLEAVE
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_1 (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		auto			vec_data = vec_data_1 + vec_data_2;
		const auto	out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data);
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2i_1 (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		auto			vec_data = vec_data_1 + vec_data_2;
		vec_data *= vec_vol;
		const auto	out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data);
		out_ptr += 4;
		++pos;
	}
		
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2i_1_v (
			out_ptr,
			in_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		auto			vec_data = vec_data_1 + vec_data_2;
		vec_data *= vec_vol;
		const auto	out_vec_data = V128Dst::load_f32 (out_ptr);
		vec_data += out_vec_data;
		V128Dst::store_f32 (out_ptr, vec_data);
		out_ptr += 4;
		vec_vol += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2i_1_vlr (
			out_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_2i_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2i_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		const auto	out_vec_data_1 = V128Dst::load_f32 (out_1_ptr);
		vec_data_1 += out_vec_data_1;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		out_1_ptr += 4;
		const auto	out_vec_data_2 = V128Dst::load_f32 (out_2_ptr);
		vec_data_2 += out_vec_data_2;
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2i_2 (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	const auto	vec_vol = fstb::Vf32 (vol);
	
	while (pos < nbr_loop)
	{
		auto			in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		in_vec_data_1 *= vec_vol;
		auto			in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_vec_data_2 *= vec_vol;
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		const auto	out_vec_data_1 = V128Dst::load_f32 (out_1_ptr);
		vec_data_1 += out_vec_data_1;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		out_1_ptr += 4;
		const auto	out_vec_data_2 = V128Dst::load_f32 (out_2_ptr);
		vec_data_2 += out_vec_data_2;
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2i_2_v (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	fstb::Vf32     vec_vol;
	fstb::Vf32     vec_step;
	fstb::ToolsSimd::start_lerp (vec_vol, vec_step, s_vol, e_vol, nbr_spl);

	while (pos < nbr_loop)
	{
		const auto	in_vec_data_1 = V128Src::load_f32 (in_ptr    );
		const auto	in_vec_data_2 = V128Src::load_f32 (in_ptr + 4);
		in_ptr += 8;
		fstb::Vf32     vec_data_1;
		fstb::Vf32     vec_data_2;
		fstb::ToolsSimd::deinterleave_f32 (vec_data_1, vec_data_2, in_vec_data_1, in_vec_data_2);
		vec_data_1 *= vec_vol;
		vec_data_2 *= vec_vol;
		const auto	out_vec_data_1 = V128Dst::load_f32 (out_1_ptr);
		vec_data_1 += out_vec_data_1;
		V128Dst::store_f32 (out_1_ptr, vec_data_1);
		out_1_ptr += 4;
		const auto	out_vec_data_2 = V128Dst::load_f32 (out_2_ptr);
		vec_data_2 += out_vec_data_2;
		V128Dst::store_f32 (out_2_ptr, vec_data_2);
		out_2_ptr += 4;
		vec_vol += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mix_2i_2_vlr (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_vol),
			e_vol
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mix_2i_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol) noexcept
{
	if (e_vol == s_vol)
	{
		mix_2i_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2i_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       MULTIPLICATION
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mult_1_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	while (pos < nbr_loop)
	{
		const auto	vec_in_1 = V128Src::load_f32 (in_1_ptr);
		const auto	vec_in_2 = V128Src::load_f32 (in_2_ptr);
		const auto	vec_out = vec_in_1 * vec_in_2;
		V128Dst::store_f32 (out_ptr, vec_out);
		
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_ptr += 4;
		++ pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mult_1_1 (
			out_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}



/*****************************************************************************
*
*       MULTIPLY, IN PLACE
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::mult_ip_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		auto			out_vec_data = V128Dst::load_f32 (out_ptr);
		out_vec_data *= in_vec_data;
		V128Dst::store_f32 (out_ptr, out_vec_data);
		in_ptr += 4;
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mult_ip_1_1 (
			out_ptr,
			in_ptr,
			nbr_spl
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::mult_ip_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	in_vec_data = V128Src::load_f32 (in_ptr);
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		out_1_vec_data *= in_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_2_vec_data *= in_vec_data;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		in_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mult_ip_1_2 (
			out_1_ptr,
			out_2_ptr,
			in_ptr,
			nbr_spl
		);
	}
	
}



template <class VD, class VS>
void	Simd <VD, VS>::mult_ip_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (V128Src::check_ptr (in_1_ptr));
	assert (V128Src::check_ptr (in_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
		
	while (pos < nbr_loop)
	{
		const auto	in_1_vec_data = V128Src::load_f32 (in_1_ptr);
		const auto	in_2_vec_data = V128Src::load_f32 (in_2_ptr);
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		out_1_vec_data *= in_1_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_2_vec_data *= in_2_vec_data;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		in_1_ptr += 4;
		in_2_ptr += 4;
		out_1_ptr += 4;
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::mult_ip_2_2 (
			out_1_ptr,
			out_2_ptr,
			in_1_ptr,
			in_2_ptr,
			nbr_spl
		);
	}
}

/*****************************************************************************
*
*       MISC
*
*****************************************************************************/



template <class VD, class VS>
void	Simd <VD, VS>::clear (float out_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (nbr_spl > 0);
	
	fill (out_ptr, nbr_spl, 0);
}



template <class VD, class VS>
void	Simd <VD, VS>::fill (float out_ptr [], int nbr_spl, float val) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 4;
	int				pos = 0;

	const auto	vec_val = fstb::Vf32 (val);
	
	while (pos < nbr_loop)
	{
		V128Dst::store_f32 (out_ptr     , vec_val);
		V128Dst::store_f32 (out_ptr +  4, vec_val);
		V128Dst::store_f32 (out_ptr +  8, vec_val);
		V128Dst::store_f32 (out_ptr + 12, vec_val);
		out_ptr += 16;
		++pos;
	}
	
	nbr_spl &= 15;
	if (nbr_spl > 0)
	{
		Fpu::fill (
			out_ptr,
			nbr_spl,
			val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::fill_lr (float out_ptr [], int nbr_spl, float s_val, float e_val) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (nbr_spl > 0);
	
	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	const float		step = (e_val - s_val) / float (nbr_spl);
	
	auto			vec_step     = fstb::Vf32 (step);
	auto			vec_val      = fstb::Vf32 (s_val);
	const auto	vec_step_spl = vec_step * fstb::Vf32 (0, 1, 2, 3);
	
	vec_val += vec_step_spl;
	vec_step *= fstb::Vf32 (4);
	
	while (pos < nbr_loop)
	{
		V128Dst::store_f32 (out_ptr, vec_val);
		out_ptr += 4;
		vec_val += vec_step;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::fill_lr (
			out_ptr,
			nbr_spl,
			fstb::ToolsSimd::Shift <0>::extract (vec_val),
			e_val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::fill_lrauto (float out_ptr [], int nbr_spl, float s_val, float e_val) noexcept
{
	if (e_val == s_val)
	{
		fill (out_ptr, nbr_spl, s_val);
	}
	else
	{
		fill_lr (out_ptr, nbr_spl, s_val, e_val);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::add_cst_1_1 (float out_ptr [], int nbr_spl, float val) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_val = fstb::Vf32 (val);
	
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Dst::load_f32 (out_ptr);
		vec_data += vec_val;
		V128Dst::store_f32 (out_ptr, vec_data);
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::add_cst_1_1 (
			out_ptr,
			nbr_spl,
			val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::add_cst_1_2 (float out_1_ptr [], float out_2_ptr [], int nbr_spl, float val) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	vec_val = fstb::Vf32 (val);
	
	while (pos < nbr_loop)
	{
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		out_1_vec_data += vec_val;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		out_1_ptr += 4;
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		out_2_vec_data += vec_val;
		V128Dst::store_f32 (out_2_ptr, out_2_vec_data);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::add_cst_1_2 (
			out_1_ptr,
			out_2_ptr,
			nbr_spl,
			val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::linop_cst_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl, float mul_val, float add_val) noexcept
{
	assert (V128Dst::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	mul_val_vec = fstb::Vf32 (mul_val);
	const auto	add_val_vec = fstb::Vf32 (add_val);
	
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Src::load_f32 (in_ptr);
		vec_data *= mul_val_vec;
		vec_data += add_val_vec;
		V128Dst::store_f32 (out_ptr, vec_data);
		in_ptr += 4;
		out_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::linop_cst_1_1 (
			out_ptr,
			in_ptr,
			nbr_spl,
			mul_val,
			add_val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::linop_cst_ip_1 (float data_ptr [], int nbr_spl, float mul_val, float add_val) noexcept
{
	assert (V128Dst::check_ptr (data_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;
	
	const auto	mul_val_vec = fstb::Vf32 (mul_val);
	const auto	add_val_vec = fstb::Vf32 (add_val);
	
	while (pos < nbr_loop)
	{
		auto			vec_data = V128Dst::load_f32 (data_ptr);
		vec_data *= mul_val_vec;
		vec_data += add_val_vec;
		V128Dst::store_f32 (data_ptr, vec_data);
		data_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::linop_cst_ip_1 (
			data_ptr,
			nbr_spl,
			mul_val,
			add_val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::add_sub_ip_2_2 (float out_1_ptr [], float out_2_ptr [], int nbr_spl) noexcept
{
	assert (V128Dst::check_ptr (out_1_ptr));
	assert (V128Dst::check_ptr (out_2_ptr));
	assert (nbr_spl > 0);

	const int		nbr_loop = nbr_spl >> 2;
	int				pos = 0;

	while (pos < nbr_loop)
	{
		auto			out_1_vec_data = V128Dst::load_f32 (out_1_ptr);
		auto			out_2_vec_data = V128Dst::load_f32 (out_2_ptr);
		const auto	temp_vec = out_1_vec_data - out_2_vec_data;
		out_1_vec_data += out_2_vec_data;
		V128Dst::store_f32 (out_1_ptr, out_1_vec_data);
		out_1_ptr += 4;
		V128Dst::store_f32 (out_2_ptr, temp_vec);
		out_2_ptr += 4;
		++pos;
	}
	
	nbr_spl &= 3;
	if (nbr_spl > 0)
	{
		Fpu::add_sub_ip_2_2 (
		  out_1_ptr,
		  out_2_ptr,
		  nbr_spl
	  );
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::sum_square_n_1 (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val) noexcept
{
	static const int  max_nbr_chn = 64;
	assert (out_ptr != nullptr);
	assert (src_ptr_arr != nullptr);
	assert (src_ptr_arr [0] != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_chn > 0);
	assert (nbr_chn <= max_nbr_chn);

	const auto     init_val_v = fstb::Vf32 (init_val);
	const int     end        = nbr_spl & -8L;
	int           pos        = 0;
	if (nbr_chn == 1)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		while (pos < end)
		{
			const auto     a0 = V128Dst::load_f32 (c0_ptr + pos    );
			const auto     a1 = V128Dst::load_f32 (c0_ptr + pos + 4);
			auto           v0 = init_val_v;
			auto           v1 = init_val_v;
			v0.mac (a0, a0);
			v1.mac (a1, a1);
			V128Dst::store_f32 (out_ptr + pos    , v0);
			V128Dst::store_f32 (out_ptr + pos + 4, v1);
			pos += 8;
		}
	}
	else if (nbr_chn == 2)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		const float *  c1_ptr = src_ptr_arr [1];
		while (pos < end)
		{
			const auto     a0 = V128Dst::load_f32 (c0_ptr + pos    );
			const auto     a1 = V128Dst::load_f32 (c0_ptr + pos + 4);
			const auto     b0 = V128Dst::load_f32 (c1_ptr + pos    );
			const auto     b1 = V128Dst::load_f32 (c1_ptr + pos + 4);
			auto           v0 = init_val_v;
			auto           v1 = init_val_v;
			v0.mac (a0, a0);
			v1.mac (a1, a1);
			v0.mac (b0, b0);
			v1.mac (b1, b1);
			V128Dst::store_f32 (out_ptr + pos    , v0);
			V128Dst::store_f32 (out_ptr + pos + 4, v1);
			pos += 8;
		}
	}
	else
	{
		while (pos < end)
		{
			auto           v0  = init_val_v;
			auto           v1  = init_val_v;
			int            chn = 0;
			do
			{
				const auto     a0 = V128Dst::load_f32 (src_ptr_arr [chn] + pos    );
				const auto     a1 = V128Dst::load_f32 (src_ptr_arr [chn] + pos + 4);
				v0.mac (a0, a0);
				v1.mac (a1, a1);
				++ chn;
			}
			while (chn < nbr_chn);
			V128Dst::store_f32 (out_ptr + pos    , v0);
			V128Dst::store_f32 (out_ptr + pos + 4, v1);
			pos += 8;
		}
	}

	nbr_spl &= 7;
	if (nbr_spl > 0)
	{
		std::array <const float *, max_nbr_chn> src2_ptr_arr;
		for (int chn = 0; chn < nbr_chn; ++chn)
		{
			src2_ptr_arr [chn] = src_ptr_arr [chn] + pos;
		}
		Fpu::sum_square_n_1 (
			out_ptr + pos,
			&src2_ptr_arr [0],
			nbr_spl,
			nbr_chn,
			init_val
		);
	}
}



template <class VD, class VS>
void	Simd <VD, VS>::sum_square_n_1_v (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val, float vol) noexcept
{
	static const int  max_nbr_chn = 64;
	assert (out_ptr != nullptr);
	assert (src_ptr_arr != nullptr);
	assert (src_ptr_arr [0] != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_chn > 0);
	assert (nbr_chn <= max_nbr_chn);

	const auto     init_val_v = fstb::Vf32 (init_val);
	const auto     vol_v      = fstb::Vf32 (vol);
	const int     end        = nbr_spl & -8L;
	int           pos        = 0;
	if (nbr_chn == 1)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		while (pos < end)
		{
			const auto     a0 = V128Dst::load_f32 (c0_ptr + pos    );
			const auto     a1 = V128Dst::load_f32 (c0_ptr + pos + 4);
			auto           v0 = init_val_v;
			auto           v1 = init_val_v;
			v0.mac (a0, a0);
			v1.mac (a1, a1);
			v0 *= vol_v;
			v1 *= vol_v;
			V128Dst::store_f32 (out_ptr + pos    , v0);
			V128Dst::store_f32 (out_ptr + pos + 4, v1);
			pos += 8;
		}
	}
	else if (nbr_chn == 2)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		const float *  c1_ptr = src_ptr_arr [1];
		while (pos < end)
		{
			const auto     a0 = V128Dst::load_f32 (c0_ptr + pos    );
			const auto     a1 = V128Dst::load_f32 (c0_ptr + pos + 4);
			const auto     b0 = V128Dst::load_f32 (c1_ptr + pos    );
			const auto     b1 = V128Dst::load_f32 (c1_ptr + pos + 4);
			auto           v0 = init_val_v;
			auto           v1 = init_val_v;
			v0.mac (a0, a0);
			v1.mac (a1, a1);
			v0.mac (b0, b0);
			v1.mac (b1, b1);
			v0 *= vol_v;
			v1 *= vol_v;
			V128Dst::store_f32 (out_ptr + pos    , v0);
			V128Dst::store_f32 (out_ptr + pos + 4, v1);
			pos += 8;
		}
	}
	else
	{
		while (pos < end)
		{
			auto           v0  = init_val_v;
			auto           v1  = init_val_v;
			int            chn = 0;
			do
			{
				const auto     a0 = V128Dst::load_f32 (src_ptr_arr [chn] + pos    );
				const auto     a1 = V128Dst::load_f32 (src_ptr_arr [chn] + pos + 4);
				v0.mac (a0, a0);
				v1.mac (a1, a1);
				++ chn;
			}
			while (chn < nbr_chn);
			v0 *= vol_v;
			v1 *= vol_v;
			V128Dst::store_f32 (out_ptr + pos    , v0);
			V128Dst::store_f32 (out_ptr + pos + 4, v1);
			pos += 8;
		}
	}

	nbr_spl &= 7;
	if (nbr_spl > 0)
	{
		std::array <const float *, max_nbr_chn> src2_ptr_arr;
		for (int chn = 0; chn < nbr_chn; ++chn)
		{
			src2_ptr_arr [chn] = src_ptr_arr [chn] + pos;
		}
		Fpu::sum_square_n_1_v (
			out_ptr + pos,
			&src2_ptr_arr [0],
			nbr_spl,
			nbr_chn,
			init_val,
			vol
		);
	}
}



template <class VD, class VS>
template <typename OPS, typename OPV>
void	Simd <VD, VS>::vec_op_1_1 (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT src_ptr, int nbr_spl, OPS op_s, OPV op_v) noexcept
{
	assert (V128Dst::check_ptr (dst_ptr));
	assert (V128Src::check_ptr (src_ptr));
	assert (dst_ptr != src_ptr);
	assert (nbr_spl > 0);

	const int      n16 = nbr_spl & ~15;
	for (int k = 0; k < n16; k += 16)
	{
		const auto     xa = V128Src::load_f32 (src_ptr + k     );
		const auto     xb = V128Src::load_f32 (src_ptr + k +  4);
		const auto     xc = V128Src::load_f32 (src_ptr + k +  8);
		const auto     xd = V128Src::load_f32 (src_ptr + k + 12);
		const auto     ya  = op_v (xa);
		const auto     yb  = op_v (xb);
		const auto     yc  = op_v (xc);
		const auto     yd  = op_v (xd);
		V128Dst::store_f32 (dst_ptr + k     , ya);
		V128Dst::store_f32 (dst_ptr + k +  4, yb);
		V128Dst::store_f32 (dst_ptr + k +  8, yc);
		V128Dst::store_f32 (dst_ptr + k + 12, yd);
	}
	for (int k = n16; k < nbr_spl; ++k)
	{
		dst_ptr [k] = op_s (src_ptr [k]);
	}
}



template <class VD, class VS>
template <typename OPS, typename OPV>
void	Simd <VD, VS>::vec_op_2_1 (float * fstb_RESTRICT dst_ptr, const float * fstb_RESTRICT lhs_ptr, const float * fstb_RESTRICT rhs_ptr, int nbr_spl, OPS op_s, OPV op_v) noexcept
{
	assert (V128Dst::check_ptr (dst_ptr));
	assert (V128Src::check_ptr (lhs_ptr));
	assert (V128Src::check_ptr (rhs_ptr));
	assert (lhs_ptr != dst_ptr);
	assert (rhs_ptr != dst_ptr);
	assert (nbr_spl > 0);

	const int      n16 = nbr_spl & ~15;
	for (int k = 0; k < n16; k += 16)
	{
		const auto     xal = V128Src::load_f32 (lhs_ptr + k     );
		const auto     xbl = V128Src::load_f32 (lhs_ptr + k +  4);
		const auto     xcl = V128Src::load_f32 (lhs_ptr + k +  8);
		const auto     xdl = V128Src::load_f32 (lhs_ptr + k + 12);
		const auto     xar = V128Src::load_f32 (rhs_ptr + k     );
		const auto     xbr = V128Src::load_f32 (rhs_ptr + k +  4);
		const auto     xcr = V128Src::load_f32 (rhs_ptr + k +  8);
		const auto     xdr = V128Src::load_f32 (rhs_ptr + k + 12);
		const auto     ya  = op_v (xal, xar);
		const auto     yb  = op_v (xbl, xbr);
		const auto     yc  = op_v (xcl, xcr);
		const auto     yd  = op_v (xdl, xdr);
		V128Dst::store_f32 (dst_ptr + k     , ya);
		V128Dst::store_f32 (dst_ptr + k +  4, yb);
		V128Dst::store_f32 (dst_ptr + k +  8, yc);
		V128Dst::store_f32 (dst_ptr + k + 12, yd);
	}
	for (int k = n16; k < nbr_spl; ++k)
	{
		dst_ptr [k] = op_s (lhs_ptr [k], rhs_ptr [k]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_mix_Simd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
