
#include "fstb/def.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "mfx/doc/SerRText.h"
#include "mfx/doc/SerWText.h"
#include "mfx/doc/Setup.h"
#include "mfx/dsp/dly/BbdLine.h"
#include "mfx/dsp/ctrl/lfo/LfoModule.h"
#include "mfx/dsp/dyn/EnvFollowerAHR4SimdHelper.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/nz/WhiteFast.h"
#include	"mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/dist1/Param.h"
#include "mfx/pi/nzbl/NoiseBleach.h"
#include "mfx/pi/nzbl/NoiseBleachDesc.h"
#include "mfx/pi/nzbl/Param.h"
#include "mfx/pi/nzcl/NoiseChlorine.h"
#include "mfx/pi/nzcl/NoiseChlorineDesc.h"
#include "mfx/pi/nzcl/Param.h"
#include "mfx/pi/peq/Param.h"
#include "mfx/pi/peq/PEqDesc.h"
#include "mfx/pi/peq/PEqType.h"
#include "mfx/pi/phase1/Param.h"
#include "mfx/pi/phase1/Phaser.h"
#include "mfx/pi/phase1/PhaserDesc.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/FileIOInterface.h"
#include "test/EPSPlot.h"
#include "test/Gridaxis.h"

#include <algorithm>
#include <array>
#include <vector>

#include <cassert>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>



struct WavRiff
{
	char           _chunk_id [4];  // "RIFF"
	uint32_t       _chunk_size;
	char           _wave_id [4];   // "WAVE"
};

struct WavFmt
{
	char           _chunk_id [4];  // "fmt "
	uint32_t       _chunk_size;
	int16_t        _format_tag;
	uint16_t       _channels;
	uint32_t       _samples_per_sec;
	uint32_t       _avg_bytes_per_sec;
	uint16_t       _block_align;
	uint16_t       _bits_per_sample;

	uint16_t       _size;
	uint16_t       _valid_bits_per_sample;
	uint32_t       _channel_mask;
	uint8_t        _subformat [16];
};

struct WavData
{
	char           _chunk_id [4];  // "data"
	uint32_t       _chunk_size;
};

enum WavFormat : uint16_t
{
 	WavFormat_PCM        = 0x0001,
 	WavFormat_IEEE_FLOAT = 0x0003,
 	WavFormat_ALAW		   = 0x0006,
 	WavFormat_MULAW	   = 0x0007,
 	WavFormat_EXTENSIBLE = 0xFFFE
};



int load_wav (const char *filename_0, std::vector <std::vector <float> > &chn_arr, double &sample_freq)
{
	int            ret_val = 0;

	chn_arr.clear ();
	sample_freq = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Loads the file into memory

	FILE *         f_ptr = fopen (filename_0, "rb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	std::vector <uint8_t>   content;
	if (ret_val == 0)
	{
		fseek (f_ptr, 0, SEEK_END);
		const long     file_size = ftell (f_ptr);
		fseek (f_ptr, 0, SEEK_SET);
		content.resize (file_size);
		if (fread (&content [0], file_size, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Converts the WAV file

	// Checks the header
	const WavRiff *   riff_ptr = 0;
	if (ret_val == 0)
	{
		riff_ptr = reinterpret_cast <const WavRiff *> (&content [0]);
		if (   riff_ptr->_chunk_id [0] != 'R'
		    || riff_ptr->_chunk_id [1] != 'I'
		    || riff_ptr->_chunk_id [2] != 'F'
		    || riff_ptr->_chunk_id [3] != 'F'
		    || riff_ptr->_wave_id [0] != 'W'
		    || riff_ptr->_wave_id [1] != 'A'
		    || riff_ptr->_wave_id [2] != 'V'
		    || riff_ptr->_wave_id [3] != 'E')
		{
			ret_val = -1;
		}
	}

	// Finds the significant chunks
	size_t         pos_fmt  = 0;
	size_t         pos_data = 0;
	if (ret_val == 0)
	{
		size_t         pos = 12;
		while (pos + 8 < content.size () && (pos_fmt == 0 || pos_data == 0))
		{
			if (   content [pos    ] == 'f'
			    && content [pos + 1] == 'm'
			    && content [pos + 2] == 't'
			    && content [pos + 3] == ' ')
			{
				pos_fmt = pos;
			}
			if (   content [pos    ] == 'd'
			    && content [pos + 1] == 'a'
			    && content [pos + 2] == 't'
			    && content [pos + 3] == 'a')
			{
				pos_data = pos;
			}

			const size_t   jmp = 
				   size_t (content [pos + 4])
				+ (size_t (content [pos + 5]) <<  8)
				+ (size_t (content [pos + 6]) << 16)
				+ (size_t (content [pos + 7]) << 24);
			pos += 8 + jmp;
		}
		if (pos_fmt == 0 || pos_data == 0)
		{
			ret_val = -1;
		}
	}

	// Checks the format
	const WavFmt * fmt_ptr = 0;
	int            format  = 0;
	if (ret_val == 0)
	{
		fmt_ptr = reinterpret_cast <const WavFmt *> (&content [pos_fmt]);
		format  = fmt_ptr->_format_tag;
		if (format == WavFormat_EXTENSIBLE)
		{
			format = fmt_ptr->_subformat [0] + (fmt_ptr->_subformat [1] << 8);
		}
		if (   format != WavFormat_PCM
		    && format != WavFormat_IEEE_FLOAT)
		{
			ret_val = -1;
		}
	}
	if (ret_val == 0)
	{
		if (fmt_ptr->_bits_per_sample > 32)
		{
			ret_val = -1;
		}
	}

	// Loads and converts the sample data
	const WavData *   data_ptr = 0;
	if (ret_val == 0)
	{
		const int      nbr_chn = fmt_ptr->_channels;
		chn_arr.resize (nbr_chn);
		sample_freq = fmt_ptr->_samples_per_sec;
		const int      bytes_per_frame = fmt_ptr->_block_align;
		const int      bitdepth = fmt_ptr->_bits_per_sample;
		const int      bytes_per_spl   = (bitdepth + 7) / 8;
		const bool     float_flag = (fmt_ptr->_format_tag == WavFormat_IEEE_FLOAT);

		data_ptr = reinterpret_cast <const WavData *> (&content [pos_data]);
		const size_t   nbr_spl = data_ptr->_chunk_size / bytes_per_frame;
		const uint8_t* spl_ptr = &content [pos_data + 8];
		for (auto &chn_data : chn_arr)
		{
			chn_data.resize (nbr_spl);
		}

		for (size_t pos = 0; pos < nbr_spl; ++pos)
		{
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				const uint8_t* ptr =
					  spl_ptr
					+ pos * bytes_per_frame
					+ chn * bytes_per_spl;
				float				val = 0;
				if (float_flag)
				{
					val = *reinterpret_cast <const float *> (ptr);
				}
				else
				{
					for (int b = 0; b < bytes_per_spl; ++b)
					{
						val += ptr [b];
						val *= 1.0f / 256;
					}
					if (val >= 0.5f)
					{
						val -= 1;
					}
					val *= 2;
				}
				chn_arr [chn] [pos] = val;
			}
		}
	}

	return ret_val;
}



int save_wav (const char *filename_0, const std::vector <std::vector <float> > &chn_arr, double sample_freq, float scale = 1)
{
	assert (! chn_arr.empty ());
	assert (sample_freq > 0);

	int            ret_val = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Prepares the chunks

	const size_t   nbr_spl = chn_arr [0].size ();
	const int      nbr_chn = int (chn_arr.size ());
	for (auto &chn : chn_arr)
	{
		assert (chn.size () == nbr_spl);
	}
	const int      header_len = 8;

	WavFmt         fmt { { 'f', 'm', 't', ' ' } };
	fmt._chunk_size        = offsetof (WavFmt, _size) - header_len;
	fmt._format_tag        = WavFormat_IEEE_FLOAT;
	fmt._channels          = int (chn_arr.size ());
	fmt._samples_per_sec   = uint32_t (floor (sample_freq + 0.5f));
	fmt._block_align       = sizeof (float) * nbr_chn;
	fmt._avg_bytes_per_sec = fmt._block_align * fmt._samples_per_sec;
	fmt._bits_per_sample   = sizeof (float) * CHAR_BIT;

	WavData        data { { 'd', 'a', 't', 'a' } };
	data._chunk_size = fmt._block_align * nbr_spl;
	assert ((data._chunk_size & 1) == 0);

	WavRiff        riff { { 'R', 'I', 'F', 'F' }, 0, { 'W', 'A', 'V', 'E' } };
	riff._chunk_size = 4 + header_len * 2 + fmt._chunk_size + data._chunk_size;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Writes the file

	FILE *         f_ptr = fopen (filename_0, "wb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		if (fwrite (&riff, header_len + 4, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}
	if (ret_val == 0)
	{
		if (fwrite (&fmt, header_len + fmt._chunk_size, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}
	if (ret_val == 0)
	{
		if (fwrite (&data, header_len, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}
	std::vector <float> tmp (nbr_chn);
	for (size_t pos = 0; pos < nbr_spl && ret_val == 0; ++pos)
	{
		for (int chn = 0; chn < nbr_chn; ++chn)
		{
			tmp [chn] = chn_arr [chn] [pos] * scale;
		}
		if (fwrite (&tmp [0], sizeof (tmp [0]), nbr_chn, f_ptr) != nbr_chn)
		{
			ret_val = -1;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	return ret_val;
}



int	generate_test_signal (double &sample_freq, std::vector <std::vector <float> > &chn_arr)
{
	int            ret_val = 0;

#if 1

	ret_val = load_wav ("../../../src/test/samples/guitar-01.wav", chn_arr, sample_freq);

#else

	sample_freq = 44100;
	chn_arr.resize (1);
	const size_t   len = size_t (sample_freq * 10);
	chn_arr [0].resize (len);
	const double   freq = 200;
	const double   muls = fstb::PI * 2 * freq / sample_freq;
	const double   mule = -1 / sample_freq;
	for (size_t i = 0; i < len; ++i)
	{
		chn_arr [0] [i] = float (
			  sin (i * muls)
			* exp (i * mule)
		);
	}

#endif

	return ret_val;
}



int    generate_test_signal_noise_w (double &sample_freq, std::vector <std::vector <float> > &chn_arr)
{
	sample_freq = 44100;
	if (chn_arr.empty ())
	{
		chn_arr.resize (1);
	}
	const size_t   len = size_t (sample_freq * 10);
	mfx::dsp::nz::WhiteFast gen;
	gen.set_rough_level (0.75f);
	for (auto &chn : chn_arr)
	{
		chn.resize (len);
		gen.process_block (&chn [0], int (len));
	}

	return 0;
}



int    generate_test_signal_spikes (double &sample_freq, std::vector <std::vector <float> > &chn_arr, double duration, double spike_len)
{
	sample_freq = 44100;
	if (chn_arr.empty ())
	{
		chn_arr.resize (1);
	}
	const size_t   len = size_t (sample_freq * duration);
	for (auto &chn : chn_arr)
	{
		chn.resize (len);
		mfx::dsp::mix::Generic::clear (&chn [0], len);
		for (double t = 0; t < duration; t += spike_len)
		{
			const size_t   pos = size_t (t * sample_freq);
			assert (pos < len);
			chn [pos] = 1;
		}
	}

	return 0;
}



class PiProc
{
public:
	typedef std::shared_ptr <mfx::piapi::PluginDescInterface> DescSPtr;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	void           set_desc (DescSPtr desc_sptr);
	int            setup (mfx::piapi::PluginInterface &pi, int nbr_chn_i, int nbr_chn_o, double sample_freq, int max_block_size, int &latency);
	void           set_param_nat (int index, double val_nat);
	void           reset_param ();
	mfx::piapi::PluginInterface::ProcInfo &
	               use_proc_info ();
	float *const * use_buf_list_src () const;
	float *const * use_buf_list_dst () const;
	float *const * use_buf_list_sig () const;
private:
	DescSPtr       _desc_sptr;
	std::vector <mfx::piapi::EventTs>
	               _evt_list;
	std::vector <const mfx::piapi::EventTs *>
	               _evt_ptr_list;
	mfx::piapi::PluginInterface::ProcInfo
	               _proc_info;
	std::vector <BufAlign>
	               _buf_list;
	std::vector <float *>
	               _buf_src_ptr_list;
	std::vector <float *>
	               _buf_dst_ptr_list;
	std::vector <float *>
	               _buf_sig_ptr_list;
};

void	PiProc::set_desc (DescSPtr desc_sptr)
{
	assert (desc_sptr.get () != 0);

	_desc_sptr = desc_sptr;
}

int	PiProc::setup (mfx::piapi::PluginInterface &pi, int nbr_chn_i, int nbr_chn_o, double sample_freq, int max_block_size, int &latency)
{
	assert (_desc_sptr.get () != 0);
	const int      mbs_alig = (max_block_size + 3) & -4;
	int            nbr_i = 1;
	int            nbr_o = 1;
	int            nbr_s = 0;
	_desc_sptr->get_nbr_io (nbr_i, nbr_o, nbr_s);
	assert (nbr_chn_i >= nbr_i);
	assert (nbr_chn_o >= nbr_o);
	_buf_list.resize (nbr_chn_i * nbr_i + nbr_chn_o * nbr_o + nbr_s);
	_buf_src_ptr_list.resize (nbr_chn_i * nbr_i);
	_buf_dst_ptr_list.resize (nbr_chn_o * nbr_o);
	_buf_sig_ptr_list.resize (nbr_s);

	int            buf_idx = 0;

	for (int chn = 0; chn < nbr_chn_i * nbr_i; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_src_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._src_arr = &_buf_src_ptr_list [0];
	_proc_info._nbr_chn_arr [mfx::piapi::PluginInterface::Dir_IN ] = nbr_chn_i;

	for (int chn = 0; chn < nbr_chn_o * nbr_o; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_dst_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._dst_arr = &_buf_dst_ptr_list [0];
	_proc_info._nbr_chn_arr [mfx::piapi::PluginInterface::Dir_OUT] = nbr_chn_o;

	for (int chn = 0; chn < nbr_s; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_sig_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._sig_arr = (nbr_s <= 0) ? 0 : &_buf_sig_ptr_list [0];

	latency = 0;

	return pi.reset (sample_freq, max_block_size, latency);
}

void	PiProc::set_param_nat (int index, double val_nat)
{
	assert (_desc_sptr.get () != 0);
	assert (_evt_list.size () == _evt_ptr_list.size ());
	assert (index >= 0);
	assert (index < _desc_sptr->get_nbr_param (mfx::piapi::ParamCateg_GLOBAL));
	const mfx::piapi::ParamDescInterface & desc_param =
		_desc_sptr->get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
	const double   val_nrm = desc_param.conv_nat_to_nrm (val_nat);

	mfx::piapi::EventTs  evt;
	evt._timestamp = 0;
	evt._type      = mfx::piapi::EventType_PARAM;
	evt._evt._param._categ = mfx::piapi::ParamCateg_GLOBAL;
	evt._evt._param._note_id = 0;
	evt._evt._param._index = index;
	evt._evt._param._val   = float (val_nrm);
	_evt_list.push_back (evt);
	_evt_ptr_list.clear ();
	for (const auto &evt : _evt_list)
	{
		_evt_ptr_list.push_back (&evt);
	}
	_proc_info._evt_arr = &_evt_ptr_list [0];
	_proc_info._nbr_evt = int (_evt_ptr_list.size ());
}

void	PiProc::reset_param ()
{
	_evt_list.clear ();
	_evt_ptr_list.clear ();
	_proc_info._evt_arr = 0;
	_proc_info._nbr_evt = 0;
}

mfx::piapi::PluginInterface::ProcInfo &	PiProc::use_proc_info ()
{
	return _proc_info;
}

float * const *	PiProc::use_buf_list_src () const
{
	return &_buf_src_ptr_list [0];
}

float * const *	PiProc::use_buf_list_dst () const
{
	return &_buf_dst_ptr_list [0];
}

float * const *	PiProc::use_buf_list_sig () const
{
	return (_buf_sig_ptr_list.empty ()) ? 0 : &_buf_sig_ptr_list [0];
}



int	test_disto ()
{
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr;

	int            ret_val = generate_test_signal (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::dist1::DistoSimple   dist;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (PiProc::DescSPtr (new mfx::pi::dist1::DistoSimpleDesc));
		pi_proc.setup (dist, 1, 1, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		std::array <float *, 1> dst_arr = {{ pi_proc.use_buf_list_dst () [0] }};
		std::array <float *, 1> src_arr = {{ pi_proc.use_buf_list_src () [0] }};
		mfx::piapi::PluginInterface::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
		pi_proc.set_param_nat (mfx::pi::dist1::Param_GAIN, 100   );
		pi_proc.set_param_nat (mfx::pi::dist1::Param_BIAS,   0.30);
		do
		{
			const int      block_len =
				int (std::min (len - pos, size_t (max_block_size)));

			proc_info._nbr_spl = block_len;
		
			memcpy (
				src_arr [0],
				&chn_arr [0] [pos],
				block_len * sizeof (src_arr [0] [0])
			);

			dist.process_block (proc_info);
			pi_proc.reset_param ();

			memcpy (
				&chn_arr [0] [pos],
				dst_arr [0],
				block_len * sizeof (chn_arr [0] [pos])
			);

			proc_info._nbr_evt = 0;
			pos += block_len;
		}
		while (pos < len);

		ret_val = save_wav ("results/t0.wav", chn_arr, sample_freq, 1);
	}

	return ret_val;
}



int	test_phaser ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

	int            ret_val = generate_test_signal_noise_w (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::phase1::Phaser plugin;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (PiProc::DescSPtr (new mfx::pi::phase1::PhaserDesc));
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::PluginInterface::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
		pi_proc.set_param_nat (mfx::pi::phase1::Param_SPEED    ,  0.1);
		pi_proc.set_param_nat (mfx::pi::phase1::Param_DEPTH    ,  0);
		pi_proc.set_param_nat (mfx::pi::phase1::Param_AP_DELAY ,  0.001);
		pi_proc.set_param_nat (mfx::pi::phase1::Param_AP_COEF  ,  -0.5);
		do
		{
			const int      block_len =
				int (std::min (len - pos, size_t (max_block_size)));

			proc_info._nbr_spl = block_len;
		
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				memcpy (
					src_arr [chn],
					&chn_arr [chn] [pos],
					block_len * sizeof (src_arr [chn] [0])
				);
			}

			plugin.process_block (proc_info);

			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				memcpy (
					&chn_arr [chn] [pos],
					dst_arr [chn],
					block_len * sizeof (chn_arr [chn] [pos])
				);
			}

			pi_proc.reset_param ();
			pos += block_len;
		}
		while (pos < len);

		ret_val = save_wav ("results/phaser0.wav", chn_arr, sample_freq, 1);
	}

	return ret_val;
}



int	test_noise_chlorine ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

	int            ret_val = generate_test_signal_noise_w (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::nzcl::NoiseChlorine plugin;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (PiProc::DescSPtr (new mfx::pi::nzcl::NoiseChlorineDesc));
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::PluginInterface::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
		pi_proc.set_param_nat (mfx::pi::nzcl::Param_LVL    ,  1);
		for (int b = 0; b < mfx::pi::nzcl::Cst::_nbr_notches; ++b)
		{
			const int      base = mfx::pi::nzcl::NoiseChlorineDesc::get_base_notch (b);
			pi_proc.set_param_nat (base + mfx::pi::nzcl::ParamNotch_FREQ, 160 << b);
			pi_proc.set_param_nat (base + mfx::pi::nzcl::ParamNotch_Q,       0.33f);
			pi_proc.set_param_nat (base + mfx::pi::nzcl::ParamNotch_LVL,    16);
		}
		do
		{
			const int      block_len =
				int (std::min (len - pos, size_t (max_block_size)));

			proc_info._nbr_spl = block_len;
		
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				memcpy (
					src_arr [chn],
					&chn_arr [chn] [pos],
					block_len * sizeof (src_arr [chn] [0])
				);
			}

			plugin.process_block (proc_info);

			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				memcpy (
					&chn_arr [chn] [pos],
					dst_arr [chn],
					block_len * sizeof (chn_arr [chn] [pos])
				);
			}

			pi_proc.reset_param ();
			pos += block_len;
		}
		while (pos < len);

		ret_val = save_wav ("results/noisechlorine0.wav", chn_arr, sample_freq, 1);
	}

	return ret_val;
}



int	test_noise_bleach ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

	int            ret_val = generate_test_signal_noise_w (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::nzbl::NoiseBleach plugin;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (PiProc::DescSPtr (new mfx::pi::nzbl::NoiseBleachDesc));
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::PluginInterface::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
		pi_proc.set_param_nat (mfx::pi::nzbl::Param_LVL    ,  1);
		for (int b = 0; b < mfx::pi::nzbl::Cst::_nbr_bands; ++b)
		{
			const int      base = mfx::pi::nzbl::NoiseBleachDesc::get_base_band (b);
			pi_proc.set_param_nat (base + mfx::pi::nzbl::ParamBand_LVL, 1);
		}
		do
		{
			const int      block_len =
				int (std::min (len - pos, size_t (max_block_size)));

			proc_info._nbr_spl = block_len;
		
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				memcpy (
					src_arr [chn],
					&chn_arr [chn] [pos],
					block_len * sizeof (src_arr [chn] [0])
				);
			}

			plugin.process_block (proc_info);

			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				memcpy (
					&chn_arr [chn] [pos],
					dst_arr [chn],
					block_len * sizeof (chn_arr [chn] [pos])
				);
			}

			pi_proc.reset_param ();
			pos += block_len;
		}
		while (pos < len);

		ret_val = save_wav ("results/noisebleach0.wav", chn_arr, sample_freq, 1);
	}

	return ret_val;
}



int	test_transients ()
{
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr;
	std::vector <std::vector <float> >  dst_arr;

	int            ret_val = generate_test_signal (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		static const int  order = 2;
		typedef mfx::dsp::dyn::EnvFollowerAHR4SimdHelper <
			fstb::DataAlign <true>,
			fstb::DataAlign <true>,
			fstb::DataAlign <true>,
			order
		> EnvHelper;
		typedef std::vector <
			fstb::ToolsSimd::VectF32,
			fstb::AllocAlign <fstb::ToolsSimd::VectF32, 16>
		> BufAlign;

		const size_t   len     = chn_arr [0].size ();
		const int      buf_len = 256;
		BufAlign       buf_src (buf_len);
		BufAlign       buf_dst (buf_len);
		size_t         buf_pos = 0;
		dst_arr.resize (4);
		for (auto &chn : dst_arr)
		{
			chn.resize (len);
		}
		EnvHelper      env;
		const double   min_freq = 50; // Hz
		const int      hold_time = fstb::round_int (sample_freq / min_freq);

		// Attack, fast envelope
		env.set_atk_coef (0, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.0001f, order), sample_freq)));
		env.set_rls_coef (0, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.050f , order), sample_freq)));
		env.set_hold_time (0, hold_time);

		// Attack, slow envelope
		env.set_atk_coef (1, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.050f , order), sample_freq)));
		env.set_rls_coef (1, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.050f , order), sample_freq)));
		env.set_hold_time (1, hold_time);

		// Sustain, fast envelope
		env.set_atk_coef (2, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.005f , order), sample_freq)));
		env.set_rls_coef (2, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.200f , order), sample_freq)));
		env.set_hold_time (2, hold_time);

		// Sustain, slow envelope
		env.set_atk_coef (3, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.005f , order), sample_freq)));
		env.set_rls_coef (3, float (mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (mfx::dsp::dyn::EnvHelper::compensate_order (0.600f , order), sample_freq)));
		env.set_hold_time (3, hold_time);

		mfx::dsp::iir::OnePole  hpf;
		const float    bs [2] = { 0.125f, 3 };
		const float    as [2] = { 1     , 1 };
		float          bz [2];
		float          az [2];
		mfx::dsp::iir::TransSZBilin::map_s_to_z_one_pole (
			bz, az, bs, as, 2000, sample_freq
		);
		hpf.set_z_eq (bz, az);

		for (size_t pos = 0; pos < len; ++pos)
		{
			float          a = chn_arr [0] [pos];
			a = hpf.process_sample (a);

			auto           x = fstb::ToolsSimd::set1_f32 (fabs (a));
			x = env.process_sample (x);
			for (int chn = 0; chn < 4; ++chn)
			{
				dst_arr [chn] [pos] = fstb::ToolsSimd::Shift <0>::extract (x);
				x = fstb::ToolsSimd::Shift <-1>::rotate (x);
			}
		}

		ret_val = save_wav ("results/transients-envelopes.wav", dst_arr, sample_freq, 1);
	}

	return ret_val;
}



int	save_lfo_drawing (std::string pathname, const std::vector <double> &data_arr, int nbr_periods)
{
	int            ret_val = 0;

	const size_t   nbr_points = data_arr.size ();

	const double   margin_x = 0;
	const double   margin_y = 0;
	const double   width    = 576;
	const double   height   = 64;
	EPSPlot        plot (
		pathname.c_str (),
		0, 0,
		float (width),
		float (height)
	);

	Gridaxis			grid;
	grid.set_size (float (width - margin_x * 2), float (height - margin_y * 2));
	grid.set_plot (plot, float (margin_x), float (margin_y));
	grid.set_grid (true);
	grid.use_axis (Gridaxis::Direction_H).set_scale (0.0, nbr_periods);
	grid.use_axis (Gridaxis::Direction_H).set_tick_dist (1);
	grid.use_axis (Gridaxis::Direction_H).activate_tick (true);
	grid.use_axis (Gridaxis::Direction_V).set_scale (-1, 1);
	grid.use_axis (Gridaxis::Direction_V).set_tick_dist (0.25);
	grid.use_axis (Gridaxis::Direction_V).activate_tick (true);
	grid.render_background ();

	plot.setFontSize (8);
	plot.setLineWidth (1.0);

	std::vector <double> x_arr (nbr_points);
	const double   scale = double (nbr_periods) / double (nbr_points);
	for (size_t pos = 0; pos < nbr_points; ++pos)
	{
		x_arr [pos] = double (pos) * scale;
	}
	grid.render_curve (&x_arr [0], &data_arr [0], long (nbr_points));

	return ret_val;
}



void	run_lfo (int &ret_val, std::string pathname, int points_per_period, int nbr_periods, mfx::dsp::ctrl::lfo::LfoModule::Type type, double chaos, double ph_dist, double ph_dist_ofs, bool inv_flag, bool unipolar_flag, double var0, double var1, double snh, double smooth, double var0end = -1, double var1end = -1)
{
	if (ret_val == 0)
	{
		const int      nbr_spl = points_per_period * nbr_periods;
		std::vector <double> data_arr (nbr_spl, 0);

		mfx::dsp::ctrl::lfo::LfoModule   lfo;
		lfo.set_type (type);
		lfo.set_sample_freq (points_per_period);
		lfo.set_period (1);
		lfo.set_chaos (chaos);
		lfo.set_phase_dist (ph_dist);
		lfo.set_phase_dist_offset (ph_dist_ofs);
		lfo.set_sign (inv_flag);
		lfo.set_polarity (unipolar_flag);
		lfo.set_variation (0, var0);
		lfo.set_variation (1, var1);
		lfo.set_snh (snh);
		lfo.set_smooth (smooth);

		const double  scale = 1.0 / nbr_spl;
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const double   r = pos * scale;
			if (var0end >= 0)
			{
				lfo.set_variation (0, var0 + r * (var0end - var0));
			}
			if (var1end >= 0)
			{
				lfo.set_variation (1, var1 + r * (var1end - var1));
			}
			const double   val = lfo.get_val ();
			data_arr [pos] = val;
			lfo.tick (1);
		}

		ret_val = save_lfo_drawing (pathname, data_arr, nbr_periods);
	}
}



int	draw_all_lfos ()
{
	int            ret_val = 0;

	const std::string path ("results/");

	run_lfo (
		ret_val, path + "lfo-sine.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SINE, 0, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-triangle.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_TRIANGLE, 0, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-parabola.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_PARABOLA, 0, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-square.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SQUARE, 0, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-saw.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SAW, 0, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-nphase-050-015.eps", 1024, 4,
		mfx::dsp::ctrl::lfo::LfoModule::Type_N_PHASE, 0, 0.5, 0, false, false, 0.5, 0.15, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-nphase-060-000.eps", 1024, 4,
		mfx::dsp::ctrl::lfo::LfoModule::Type_N_PHASE, 0, 0.5, 0, false, false, 0.6, 0.0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-biphase-050-025.eps", 1024, 2,
		mfx::dsp::ctrl::lfo::LfoModule::Type_BIPHASE, 0, 0.5, 0, false, false, 0.5, 0.25, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-biphase-0625-003125.eps", 1024, 2,
		mfx::dsp::ctrl::lfo::LfoModule::Type_BIPHASE, 0, 0.5, 0, false, false, 0.625, 0.03125, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-varislope-025-000.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_VARISLOPE, 0, 0.5, 0, false, false, 0.25, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-varislope-025-050.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_VARISLOPE, 0, 0.5, 0, false, false, 0.25, 0.5, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-varislope-025-morph.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_VARISLOPE, 0, 0.5, 0, false, false, 0.25, 0.0, 0, 0, -1, 1.0
	);
	run_lfo (
		ret_val, path + "lfo-noise.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_NOISE_FLT2, 0, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-saw-inv.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SAW, 0, 0.5, 0, true, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-square-uni.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SQUARE, 0, 0.5, 0, false, true, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-sine-phdist-025-000.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SINE, 0, 0.25, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-sine-phdist-025-025.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SINE, 0, 0.25, 0.25, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-sine-snh.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SINE, 0, 0.5, 0, false, false, 0, 0, 0.15, 0
	);
	run_lfo (
		ret_val, path + "lfo-square-smooth.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SQUARE, 0, 0.5, 0, false, false, 0, 0, 0, 0.15
	);
	run_lfo (
		ret_val, path + "lfo-sine-chaos.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SINE, 0.35, 0.5, 0, false, false, 0, 0, 0, 0
	);
	run_lfo (
		ret_val, path + "lfo-saw-chaos.eps", 1024, 8,
		mfx::dsp::ctrl::lfo::LfoModule::Type_SAW, 0.35, 0.5, 0, false, false, 0, 0, 0, 0
	);

	return ret_val;
}



void	patch_setup_file_fix_peq_freq (mfx::doc::PluginSettings &settings, const mfx::piapi::ParamDescInterface &desc_freq, const mfx::piapi::ParamDescInterface &desc_type, const mfx::piapi::ParamDescInterface &desc_gain)
{
	const int      nbr_param = settings._param_list.size ();
	const int      nbr_bands = nbr_param / mfx::pi::peq::Param_NBR_ELT;

	for (int b_cnt = 0; b_cnt < nbr_bands; ++b_cnt)
	{
		const int	   index_base = b_cnt * mfx::pi::peq::Param_NBR_ELT;
		const int      index_type = index_base + mfx::pi::peq::Param_TYPE;
		const int      index_freq = index_base + mfx::pi::peq::Param_FREQ;
		const int      index_gain = index_base + mfx::pi::peq::Param_GAIN;

		const float    type_nrm = settings._param_list [index_type];
		const double   type_nat = desc_type.conv_nrm_to_nat (type_nrm);
		const mfx::pi::peq::PEqType   type =
			mfx::pi::peq::PEqType (fstb::round_int (type_nat));

		if (type == mfx::pi::peq::PEqType_PEAK)
		{
			const float    gain_nrm = settings._param_list [index_gain];
			const double   gain_nat = desc_gain.conv_nrm_to_nat (gain_nrm);
			if (! fstb::is_eq_rel (gain_nat, 1.0, 1e-3))
			{
				float          freq_nrm = settings._param_list [index_freq];
				double         freq_nat = desc_freq.conv_nrm_to_nat (freq_nrm);

				freq_nat *= 0.5;

				freq_nrm = float (desc_freq.conv_nat_to_nrm (freq_nat));
				settings._param_list [index_freq] = freq_nrm;
			}
		}
	}
}

int	patch_setup_file ()
{
	int            ret_val = 0;

	const std::string pathname_load = "../../../etc/config/current"; // Put the settings file here
	const std::string pathname_save = pathname_load + ".patched";

	std::string    content;
	std::unique_ptr <mfx::doc::Setup> sss_uptr;

	// Loading
	ret_val = mfx::FileIOInterface::read_txt_file_direct (
		pathname_load, content
	);

	if (ret_val == 0)
	{
		mfx::doc::SerRText   ser_r;
		ser_r.start (content);
		sss_uptr = std::unique_ptr <mfx::doc::Setup> (new mfx::doc::Setup);
		sss_uptr->ser_read (ser_r);
		ret_val = ser_r.terminate ();
	}
	
	// Put the code to change the settings here
	if (ret_val == 0)
	{
#if 1

		// In all PEq settings, divides by 2 the frequencies for all active
		// bell curves to preserve the tones after bugfix b995bbb221.

		mfx::pi::peq::PEqDesc <4> desc_pi;
		const mfx::piapi::ParamDescInterface & desc_freq =
			desc_pi.get_param_info (
				mfx::piapi::ParamCateg_GLOBAL, mfx::pi::peq::Param_FREQ
			);
		const mfx::piapi::ParamDescInterface & desc_type =
			desc_pi.get_param_info (
				mfx::piapi::ParamCateg_GLOBAL, mfx::pi::peq::Param_TYPE
			);
		const mfx::piapi::ParamDescInterface & desc_gain =
			desc_pi.get_param_info (
				mfx::piapi::ParamCateg_GLOBAL, mfx::pi::peq::Param_GAIN
			);

		for (auto &bank : sss_uptr->_bank_arr)
		{
			for (auto &preset : bank._preset_arr)
			{
				for (auto &slot_node : preset._slot_map)
				{
					if (slot_node.second.get () != 0)
					{
						auto &         slot = *(slot_node.second);
						if (slot._pi_model == "peq")
						{
						   mfx::doc::PluginSettings * settings_ptr =
								slot.test_and_get_settings (mfx::PiType_MAIN);
							if (settings_ptr != 0)
							{
								patch_setup_file_fix_peq_freq  (
									*settings_ptr, desc_freq, desc_type, desc_gain
								);
							}
						}
					}
				}
			}
		}

		auto           it_peq = sss_uptr->_map_plugin_settings.find ("peq");
		if (it_peq != sss_uptr->_map_plugin_settings.end ())
		{
			for (auto &settings_sptr : it_peq->second._cell_arr)
			{
				if (settings_sptr.get () != 0)
				{
					patch_setup_file_fix_peq_freq  (
						settings_sptr->_main, desc_freq, desc_type, desc_gain
					);
				}
			}
		}

#endif

	}

	// Saving
	mfx::doc::SerWText   ser_w;
	if (ret_val == 0)
	{
		assert (sss_uptr.get () != 0);
		ser_w.clear ();
		sss_uptr->ser_write (ser_w);
		ret_val = ser_w.terminate ();
	}
	if (ret_val == 0)
	{
		content = ser_w.use_content ();
		ret_val = mfx::FileIOInterface::write_txt_file_direct (
			pathname_save, content
		);
	}

	if (ret_val == 0)
	{
		fprintf (stderr, "Patched settings successfully.\n");
	}
	else
	{
		fprintf (stderr, "*** An error occured when patching the settings ***\n");
	}

	return ret_val;
}



int	test_bbd_line ()
{
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr;

#if 1
	int            ret_val = generate_test_signal (sample_freq, chn_arr);
	float          fdbk    = 0.75f;
#elif 0
	int            ret_val = generate_test_signal_spikes (sample_freq, chn_arr, 86.6667, 0.1);
	float          fdbk    = 0.75f;
#else
	int            ret_val = generate_test_signal_spikes (sample_freq, chn_arr, 20, 2.6973);
	float          fdbk    = 1;
#endif

	mfx::dsp::dly::BbdLine delay;
	if (ret_val == 0)
	{
		chn_arr.resize (1);

		mfx::dsp::rspl::InterpolatorHermite43  interp;

		int            bbd_size  = 4096;
		delay.init (bbd_size, interp, 0);
		delay.clear_buffers ();

		const int      buf_len   =
			bbd_size << -mfx::dsp::dly::BbdLine::_min_speed_l2;
		std::vector <float>  buf (buf_len);
		const float    spd_min   = 1.0f / (1 << -mfx::dsp::dly::BbdLine::_min_speed_l2);
		const float    spd_max   = 64;

		float          dly_time  = float (bbd_size);
		float          speed     = 1;
		double         osc_freq  = 0.1; // Hz

		const int      nbr_spl   = int (chn_arr [0].size ());
		int            block_pos = 0;
		while (block_pos < nbr_spl)
		{
			const float    c =
				float (cos (osc_freq * 2 * fstb::PI * block_pos / sample_freq));
			speed = spd_min + (1 - c) * 0.5f * (spd_max - spd_min);

			delay.set_speed (speed);

			const int      rem_len   = nbr_spl - block_pos;
			const int      max_len   =
				delay.estimate_max_one_shot_proc_w_feedback (dly_time);
			const int      max_len2  = 999999999; // 64;
			const int      block_len =
				std::min (std::min (std::min (rem_len, max_len), buf_len), max_len2);

			delay.read_block (&buf [0], block_len, dly_time, dly_time, 0);
			mfx::dsp::mix::Generic::scale_1_v (&buf [0], block_len, fdbk);
			if (block_pos < nbr_spl * 3 / 4)
			{
				mfx::dsp::mix::Generic::mix_1_1 (
					&buf [0],
					&chn_arr [0] [block_pos],
					block_len
				);
			}
			else
			{
				fdbk = 1.0f;
			}
			mfx::dsp::mix::Generic::copy_1_1 (
				&chn_arr [0] [block_pos],
				&buf [0],
				block_len
			);
			delay.push_block (&buf [0], block_len);

			block_pos += block_len;
		}

		ret_val = save_wav ("results/bbdline0.wav", chn_arr, sample_freq, 1);
	}

	if (ret_val == 0)
	{
		fprintf (stderr, "test_bbd_line successful.\n");
	}
	else
	{
		fprintf (stderr, "*** An error occured in test_bbd_line ***\n");
	}

	return ret_val;
}




int main (int argc, char *argv [])
{
	mfx::dsp::mix::Generic::setup ();

	int            ret_val = 0;

#if 0
	test_disto ();
#endif

#if 0
	test_phaser ();
#endif

#if 0
	test_noise_chlorine ();
#endif

#if 0
	test_noise_bleach ();
#endif

#if 0
	draw_all_lfos ();
#endif

#if 0
	test_transients ();
#endif

#if 0
	patch_setup_file ();
#endif

#if 1
	test_bbd_line ();
#endif


	return ret_val;
}
