/*****************************************************************************

        BiquadPackSimd.h
        Author: Laurent de Soras, 2016

Processes N channels with M biquads, for an almost optimal SSE use.
It allows changing the (N, M) configuration during the processing.
Changes of biquad equations can be smoothed.

Automatically initialises dsp::mix::Generic.

Template parameters:

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectF32 VD::load_f32 (const void *ptr);
	static void VD::store_f32 (void *ptr, fstb::ToolsSimd::VectF32 val);

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr);
	static fstb::ToolsSimd::VectF32 VS::load_f32 (const void *ptr);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_BiquadPackSimd_HEADER_INCLUDED)
#define mfx_dsp_iir_BiquadPackSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "mfx/dsp/iir/Biquad4SimdMorph.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <class VD, class VS>
class BiquadPackSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	VD	V128Dest;
	typedef	VS	V128Src;

	               BiquadPackSimd ();
	virtual        ~BiquadPackSimd () = default;

	void				set_config (int nbr_stages, int nbr_chn);
	void				reserve (int nbr_stages, int nbr_chn);
	void				set_buf_len (long nbr_spl);

	void				set_ramp_time (long nbr_spl);
	void				adapt_config (int nbr_stages, int nbr_chn);
	void				set_biquad (int stage, int chn, const float b [3], const float a [3], bool ramp_flag = false);
	void				process_block (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end);
	void				process_block (float * const out_ptr_arr [], const float in_ptr [], long pos_beg, long pos_end);
	void				clear_buffers ();
	void				clear_buffers_one (int stage, int chn);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum ProcConf
	{
		ProcConf_PARALLEL = 0,
		ProcConf_SERIAL,
		ProcConf_2X2,

		ProcConf_NBR_ELT,
	};

	typedef Biquad4SimdMorph <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	>					Pack4;
	typedef std::vector <Pack4, fstb::AllocAlign <float, 16> > PackList;

	class BiqInfo
	{
	public:
		float          _b [3];
		float          _a [3];
		float          _mem_x [2];
		float          _mem_y [2];
	};
	typedef std::vector <BiqInfo> BiqInfoList;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > Buffer;

	void           find_biq (int &pack_index, int &biq_index, int stage, int chn) const;
	void           set_config_internal (int nbr_stages, int nbr_chn);
	void           save_info ();
	void           load_info (int nbr_stages, int nbr_chn);

	void           process_block_parallel (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end, bool mono_source_flag);
	void           process_block_serial (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end);
	void           process_block_serial (float * const out_ptr_arr [], const float in_ptr [], long pos_beg, long pos_end);
	void           process_block_serial_one_chn (float out_ptr [], const float in_ptr [], long pos_beg, long pos_end, int &pack_index);
	void           process_block_2x2 (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end);
	void           process_block_2x2 (float * const out_ptr_arr [], const float in_ptr [], long pos_beg, long pos_end);

	static ProcConf
	               compute_proc_conf (int nbr_stages, int nbr_chn);
	static void    compute_config_info (ProcConf &proc_conf, int &nbr_packs, int &group_size, int nbr_stages, int nbr_chn);

	PackList       _pack_list;
	Buffer         _tmp_buf;      // Capacity: _buf_len * 4
	long           _buf_len;
	int            _nbr_stages;
	int            _nbr_chn;
	int            _group_size;   // Number of packs per group. Meanning depends on _proc_conf
	int            _nbr_packs;
	int            _ramp_len;
	ProcConf       _proc_conf;
	BiqInfoList    _biq_info_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BiquadPackSimd (const BiquadPackSimd &other)    = delete;
	BiquadPackSimd &
	               operator = (const BiquadPackSimd &other)        = delete;
	bool           operator == (const BiquadPackSimd &other) const = delete;
	bool           operator != (const BiquadPackSimd &other) const = delete;

}; // class BiquadPackSimd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/BiquadPackSimd.hpp"



#endif   // mfx_dsp_iir_BiquadPackSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
