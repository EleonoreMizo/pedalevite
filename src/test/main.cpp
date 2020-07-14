
#include "fstb/def.h"

#include "fstb/msg/MsgRet.h"
#include "fstb/msg/QueueRetMgr.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/fnc.h"
#include "mfx/doc/SerRText.h"
#include "mfx/doc/SerWText.h"
#include "mfx/doc/Setup.h"
#include "mfx/dsp/ana/FreqFast.h"
#include "mfx/dsp/dly/BbdLine.h"
#include "mfx/dsp/ctrl/lfo/LfoModule.h"
#include "mfx/dsp/dyn/EnvFollowerAHR1LrSimdHelper.h"
#include "mfx/dsp/dyn/EnvFollowerAHR4SimdHelper.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/fir/RankSelA.h"
#include "mfx/dsp/fir/RankSelL.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/nz/WhiteFast.h"
#include "mfx/dsp/osc/OscSinCosStableSimd.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/dsp/rspl/InterpPhaseFpu.h"
#include "mfx/dsp/rspl/InterpPhaseSimd.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/dist1/Param.h"
#include "mfx/pi/dly2/Delay2.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dly2/Param.h"
#include "mfx/pi/nzbl/NoiseBleach.h"
#include "mfx/pi/nzbl/NoiseBleachDesc.h"
#include "mfx/pi/nzbl/Param.h"
#include "mfx/pi/nzcl/NoiseChlorine.h"
#include "mfx/pi/nzcl/NoiseChlorineDesc.h"
#include "mfx/pi/nzcl/Param.h"
#include "mfx/pi/osdet/OnsetDetect.h"
#include "mfx/pi/osdet/OnsetDetectDesc.h"
#include "mfx/pi/peq/Param.h"
#include "mfx/pi/peq/PEqDesc.h"
#include "mfx/pi/peq/PEqType.h"
#include "mfx/pi/phase1/Param.h"
#include "mfx/pi/phase1/Phaser.h"
#include "mfx/pi/phase1/PhaserDesc.h"
#include "mfx/pi/testgen/TestGen.h"
#include "mfx/pi/testgen/TestGenDesc.h"
#include "mfx/pi/testgen/Param.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/ProcInfo.h"
#include "mfx/FileIOInterface.h"
#include "mfx/FileOpWav.h"
#include "test/DrawShapers.h"
#include "test/EPSPlot.h"
#include "test/Gridaxis.h"
#include "test/TestApprox.h"
#include "test/TestConvolverFft.h"
#include "test/TestDiodeClipDAngelo.h"
#include "test/TestDiodeClipJcm.h"
#include "test/TestDiodeClipScreamer.h"
#include "test/TestDkmSimulator.h"
#include "test/TestHardclipBl.h"
#include "test/TestHash.h"
#include "test/TestHiirDesigner.h"
#include "test/TestInterpFtor.h"
#include "test/TestInterpPhase.h"
#include "test/TestLatAlgo.h"
#include "test/TestMoogLadderDAngelo.h"
#include "test/TestMoogLadderMystran.h"
#include "test/TestOscSample.h"
#include "test/TestOscSampleSyncFade.h"
#include "test/TestOscSampleSyncHard.h"
#include "test/TestOscWavetable.h"
#include "test/TestOscWavetableSub.h"
#include "test/TestOscWavetableSyncHard.h"
#include "test/TestPinkShade.h"
#include "test/TestPsu.h"
#include "test/TestRcClipGeneric.h"
#include "test/TestRemez.h"
#include "test/TestSampleMipMapper.h"
#include "test/TestMnaSimulator.h"
#include "test/TestSlidingMax.h"
#include "test/TestSlidingOp.h"
#include "test/TestSmooth.h"
#include "test/TestSvf.h"
#include "test/TestSvfAntisat.h"

#if fstb_IS (SYS, LINUX)
	#include "mfx/adrv/CbInterface.h"
	#include "mfx/adrv/DPvabDirect.h"
	#include "test/TestDisplayFrameBufSimple.h"
	#include "test/TestLedSimple.h"
	#include "test/TestUserInputPi3.h"
#endif

#include <algorithm>
#include <array>
#include <vector>

#include <cassert>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>



int	generate_test_signal (double &sample_freq, std::vector <std::vector <float> > &chn_arr)
{
	int            ret_val = 0;

#if 1

	ret_val = mfx::FileOpWav::load ("../../../src/test/samples/guitar-01.wav", chn_arr, sample_freq);
//	ret_val = mfx::FileOpWav::load ("../../../src/test/samples/guitar-02.wav", chn_arr, sample_freq);

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
		mfx::dsp::mix::Generic::clear (&chn [0], int (len));
		for (double t = 0; t < duration; t += spike_len)
		{
			const size_t   pos = size_t (t * sample_freq);
			assert (pos < len);
			chn [pos] = 1;
		}
	}

	return 0;
}



int    generate_test_signal_short_sine (double &sample_freq, std::vector <std::vector <float> > &chn_arr, double duration, double sine_duration, double freq, double vol, double fade)
{
	assert (sine_duration <= sine_duration);

	sample_freq = 44100;
	if (chn_arr.empty ())
	{
		chn_arr.resize (1);
	}
	const size_t   len    = size_t (sample_freq *      duration);
	const size_t   len_s  = size_t (sample_freq * sine_duration);
	const double   inv_fs = 1.0 / sample_freq;
	for (auto &chn : chn_arr)
	{
		chn.resize (len);
		mfx::dsp::mix::Generic::clear (&chn [0], int (len));
		for (size_t k = 0; k < len_s; ++k)
		{
			float          env = 1;
			const double   t   = k * inv_fs;
			if (t < fade)
			{
				env = 0.5f - float (cos (fstb::PI * t / fade)) * 0.5f;
			}
			else if (t > sine_duration - fade)
			{
				env = 0.5f - float (cos (fstb::PI * (sine_duration - t) / fade)) * 0.5f;
			}
			const float    vv  = float (vol) * env;
			chn [k] = float (sin (2 * fstb::PI * freq * inv_fs * k)) * vv;
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
	mfx::piapi::ProcInfo &
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
	mfx::piapi::ProcInfo
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
	assert (desc_sptr.get () != nullptr);

	_desc_sptr = desc_sptr;
}

int	PiProc::setup (mfx::piapi::PluginInterface &pi, int nbr_chn_i, int nbr_chn_o, double sample_freq, int max_block_size, int &latency)
{
	assert (_desc_sptr.get () != nullptr);
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
	_proc_info._src_arr = nullptr;
	if (nbr_chn_i > 0)
	{
		_proc_info._src_arr = &_buf_src_ptr_list [0];
	}
	_proc_info._dir_arr [mfx::piapi::Dir_IN ]._nbr_chn = nbr_chn_i;

	for (int chn = 0; chn < nbr_chn_o * nbr_o; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_dst_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._dst_arr = nullptr;
	if (nbr_chn_o > 0)
	{
		_proc_info._dst_arr = &_buf_dst_ptr_list [0];
	}
	_proc_info._dir_arr [mfx::piapi::Dir_OUT]._nbr_chn = nbr_chn_o;

	for (int chn = 0; chn < nbr_s; ++chn)
	{
		_buf_list [buf_idx].resize (mbs_alig);
		_buf_sig_ptr_list [chn] = &_buf_list [buf_idx] [0];
		++ buf_idx;
	}
	_proc_info._sig_arr = (nbr_s <= 0) ? nullptr : &_buf_sig_ptr_list [0];

	latency = 0;

	return pi.reset (sample_freq, max_block_size, latency);
}

void	PiProc::set_param_nat (int index, double val_nat)
{
	assert (_desc_sptr.get () != nullptr);
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
	for (const auto &evt_p : _evt_list)
	{
		_evt_ptr_list.push_back (&evt_p);
	}
	_proc_info._evt_arr = &_evt_ptr_list [0];
	_proc_info._nbr_evt = int (_evt_ptr_list.size ());
}

void	PiProc::reset_param ()
{
	_evt_list.clear ();
	_evt_ptr_list.clear ();
	_proc_info._evt_arr = nullptr;
	_proc_info._nbr_evt = 0;
}

mfx::piapi::ProcInfo &	PiProc::use_proc_info ()
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
	return (_buf_sig_ptr_list.empty ()) ? nullptr : &_buf_sig_ptr_list [0];
}



int	test_testgen ()
{
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr;

	int            ret_val = generate_test_signal (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::testgen::TestGen  plugin;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::testgen::TestGenDesc> ()
			)
		);
		pi_proc.setup (plugin, 1, 1, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		std::array <float *, 1> dst_arr = {{ pi_proc.use_buf_list_dst () [0] }};
		std::array <float *, 1> src_arr = {{ pi_proc.use_buf_list_src () [0] }};
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
		pi_proc.set_param_nat (mfx::pi::testgen::Param_STATE, 1);
		pi_proc.set_param_nat (mfx::pi::testgen::Param_LVL  , 0.9375);
		pi_proc.set_param_nat (mfx::pi::testgen::Param_TYPE , mfx::pi::testgen::Type_SWEEP);
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

			plugin.process_block (proc_info);
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

		ret_val = mfx::FileOpWav::save (
			"results/testgensweep0.wav", chn_arr, sample_freq, 1
		);
	}

	return ret_val;
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
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::dist1::DistoSimpleDesc> ()
			)
		);
		pi_proc.setup (dist, 1, 1, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		std::array <float *, 1> dst_arr = {{ pi_proc.use_buf_list_dst () [0] }};
		std::array <float *, 1> src_arr = {{ pi_proc.use_buf_list_src () [0] }};
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
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

		ret_val = mfx::FileOpWav::save (
			"results/t0.wav", chn_arr, sample_freq, 1
		);
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
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::phase1::PhaserDesc> ()
			)
		);
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
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

		ret_val = mfx::FileOpWav::save (
			"results/phaser0.wav", chn_arr, sample_freq, 1
		);
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
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::nzcl::NoiseChlorineDesc> ()
			)
		);
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
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

		ret_val = mfx::FileOpWav::save (
			"results/noisechlorine0.wav", chn_arr, sample_freq, 1
		);
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
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::nzbl::NoiseBleachDesc> ()
			)
		);
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
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

		ret_val = mfx::FileOpWav::save (
			"results/noisebleach0.wav", chn_arr, sample_freq, 1
		);
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

		ret_val = mfx::FileOpWav::save (
			"results/transients-envelopes.wav", dst_arr, sample_freq, 1
		);
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
	const int      nbr_param = int (settings._param_list.size ());
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
#if __cplusplus >= 201402
		sss_uptr = std::make_unique <mfx::doc::Setup> ();
#else // __cplusplus
		sss_uptr = std::unique_ptr <mfx::doc::Setup> (new mfx::doc::Setup);
#endif // __cplusplus
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
					if (slot_node.second.get () != nullptr)
					{
						auto &         slot = *(slot_node.second);
						if (slot._pi_model == "peq")
						{
						   mfx::doc::PluginSettings * settings_ptr =
								slot.test_and_get_settings (mfx::PiType_MAIN);
							if (settings_ptr != nullptr)
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
				if (settings_sptr.get () != nullptr)
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
		assert (sss_uptr.get () != nullptr);
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

		ret_val = mfx::FileOpWav::save (
			"results/bbdline0.wav", chn_arr, sample_freq, 1
		);
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



int	test_delay2 ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

//	int            ret_val = generate_test_signal (sample_freq, chn_arr);
	int            ret_val = generate_test_signal_short_sine (sample_freq, chn_arr, 5, 0.9, 110, 0.5, 0.01);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::dly2::Delay2 plugin;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::dly2::Delay2Desc> ()
			)
		);
		pi_proc.setup (plugin, nbr_chn, nbr_chn, sample_freq, max_block_size, latency);
		size_t         pos = 0;
		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
		pi_proc.set_param_nat (mfx::pi::dly2::Delay2Desc::get_line_base (1) + mfx::pi::dly2::ParamLine_VOL, 0);
		pi_proc.set_param_nat (mfx::pi::dly2::Delay2Desc::get_line_base (0) + mfx::pi::dly2::ParamLine_DLY_BASE, 1.0);
		pi_proc.set_param_nat (mfx::pi::dly2::Delay2Desc::get_line_base (0) + mfx::pi::dly2::ParamLine_PITCH, 4.0/12);
		pi_proc.set_param_nat (mfx::pi::dly2::Delay2Desc::get_line_base (0) + mfx::pi::dly2::ParamLine_FDBK, 0.95);
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

			if (pos >= (len >> 1) && pos < (len >> 1) + block_len)
			{
				pi_proc.set_param_nat (mfx::pi::dly2::Delay2Desc::get_line_base (0) + mfx::pi::dly2::ParamLine_DLY_BASE, 1.1);
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

		ret_val = mfx::FileOpWav::save (
			"results/delay20.wav", chn_arr, sample_freq, 1
		);
	}

	return ret_val;
}



int test_osdet ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

	int            ret_val = generate_test_signal (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::pi::osdet::OnsetDetect plugin;
		const int      max_block_size = 64;
		int            latency = 0;
		PiProc         pi_proc;
		pi_proc.set_desc (
			std::static_pointer_cast <mfx::piapi::PluginDescInterface> (
				std::make_shared <mfx::pi::osdet::OnsetDetectDesc> ()
			)
		);
		pi_proc.setup (plugin, nbr_chn, 0, sample_freq, max_block_size, latency);
		size_t         pos = 0;
//		float * const* dst_arr = pi_proc.use_buf_list_dst ();
		float * const* sig_arr = pi_proc.use_buf_list_sig ();
		float * const* src_arr = pi_proc.use_buf_list_src ();
		mfx::piapi::ProcInfo &   proc_info = pi_proc.use_proc_info ();
		pi_proc.reset_param ();
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
			for (int k = 0; k < block_len; ++k)
			{
				chn_arr [0] [pos + k] = sig_arr [0] [0] - sig_arr [1] [0];
			}

			pi_proc.reset_param ();
			pos += block_len;
		}
		while (pos < len);

		ret_val = mfx::FileOpWav::save (
			"results/osdet0.wav", chn_arr, sample_freq, 1
		);
	}

	return ret_val;
}



int	test_freqfast ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

	int            ret_val = generate_test_signal (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		const size_t   len = chn_arr [0].size ();
		mfx::dsp::ana::FreqFast analyser;
		const int      max_block_size = 64;

		analyser.set_sample_freq (sample_freq);

		size_t         pos = 0;
		double         phase = 0;
		do
		{
			const int      block_len =
				int (std::min (len - pos, size_t (max_block_size)));
			const float    freq = analyser.process_block (&chn_arr [0] [pos], block_len);
			for (int k = 0; k < block_len; ++k)
			{
				chn_arr [0] [pos + k] = float (sin (phase * 2 * fstb::PI / sample_freq));
				phase += freq;
			}
			pos += block_len;
		}
		while (pos < len);

		ret_val = mfx::FileOpWav::save (
			"results/freqfast0.wav", chn_arr, sample_freq, 1
		);
	}

	return ret_val;
}



int	test_envelope_detector ()
{
	static const int  nbr_chn = 1;
	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr (nbr_chn);

	int            ret_val = generate_test_signal (sample_freq, chn_arr);

	if (ret_val == 0)
	{
		static const int  order = 2;
		const size_t   len = chn_arr [0].size ();
		mfx::dsp::dyn::EnvFollowerAHR1LrSimdHelper <fstb::DataAlign <false>, order> env_hlp;
		const int      max_block_size = 64;

		env_hlp.set_atk_coef (float (
			mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (
				mfx::dsp::dyn::EnvHelper::compensate_order (0.001f, order),
				sample_freq
			)
		));
		env_hlp.set_rls_coef (float (
			mfx::dsp::dyn::EnvHelper::compute_env_coef_simple (
				mfx::dsp::dyn::EnvHelper::compensate_order (0.100f, order),
				sample_freq
			)
		));
		env_hlp.set_hold_time (fstb::round_int (sample_freq * 0.030));

		size_t         pos = 0;
		do
		{
#if 1
			const int      block_len =
				int (std::min (len - pos, size_t (max_block_size)));
			mfx::dsp::mix::Generic::mult_ip_1_1 (&chn_arr [0] [pos], &chn_arr [0] [pos], block_len);
			env_hlp.process_block (&chn_arr [0] [pos], &chn_arr [0] [pos], block_len);
			for (size_t p2 = pos; p2 < pos + block_len; ++p2)
			{
				chn_arr [0] [p2] = sqrt (chn_arr [0] [p2]);
			}
#else
			const int      block_len = 1;
			const float    x  = chn_arr [0] [pos];
			const float    y2 = env_hlp.process_sample (x * x);
			chn_arr [0] [pos] = sqrt (y2);
#endif

			pos += block_len;
		}
		while (pos < len);

		ret_val = mfx::FileOpWav::save (
			"results/envdet0.wav", chn_arr, sample_freq, 1
		);
	}

	return ret_val;
}



#if fstb_IS (SYS, LINUX)

class CbDPvabDirect
:	public mfx::adrv::CbInterface
{
public:
	CbDPvabDirect () = default;
	virtual ~CbDPvabDirect () = default;
protected:
	virtual void   do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
	{
		// Copies input to output
		mfx::dsp::mix::Generic::copy_2_2 (
			dst_arr [0], dst_arr [1], src_arr [0], src_arr [1], nbr_spl
		);
	}
	virtual void   do_notify_dropout ()
	{
		// Nothing
	}
	virtual void   do_request_exit ()
	{
		// Nothing
	}
};

int	test_adrv_dpvab ()
{
	mfx::adrv::DPvabDirect  pvab;
	CbDPvabDirect           cb;

	double            sample_freq;
	int               max_block_size;
	pvab.init (sample_freq, max_block_size, cb, 0, 0, 0);

	pvab.start ();
	std::this_thread::sleep_for (std::chrono::seconds (600));
	pvab.stop ();

	return 0;
}

#endif // LINUX



int	test_median_filter ()
{
	const int      med_len = 63;
	const int      rank    = med_len / 2;
	std::vector <float> med_buf (med_len);
	mfx::dsp::fir::RankSelA	med_flt_a;
	mfx::dsp::fir::RankSelL	med_flt_l;

	med_flt_a.set_len (med_len, 0);
	med_flt_l.set_len (med_len, 0);

	double         sample_freq;
	std::vector <std::vector <float> >  chn_arr;

	int            ret_val = generate_test_signal (sample_freq, chn_arr);
	if (ret_val == 0)
	{
		const size_t   len     = chn_arr [0].size ();
		const float *  src_ptr = chn_arr [0].data ();
		for (size_t pos = 0; pos < len && ret_val == 0; ++pos)
		{
			// Naive median filter (reference)
			for (size_t idx = 0; idx < med_len; ++idx)
			{
				if (idx > pos)
				{
					med_buf [idx] = 0;
				}
				else
				{
					med_buf [idx] = src_ptr [pos - idx];
				}
			}
			std::sort (med_buf.begin (), med_buf.end ());
			const float    med_ref = med_buf [rank];
			const float    med_a   = med_flt_a.process_sample (src_ptr [pos]);
			const float    med_l   = med_flt_l.process_sample (src_ptr [pos]);

			if (med_a != med_ref)
			{
				assert (false);
				ret_val = -1;
			}
			if (med_l != med_ref)
			{
				assert (false);
				ret_val = -1;
			}
		}
	}

	return ret_val;
}



class QueueRetData
{
public:
	void           set_data (int val)
	{
		assert (val > 0);
		_dat = val;
	}
	int            get_data () const
	{
		assert (_dat > 0);
		return _dat;
	}
	void           clear ()
	{
		if (_dat != 0)
		{
			_dat = 0;
			++ _nbr_clr;
		}
	}
	~QueueRetData ()
	{
		clear ();
	}
	static int     _nbr_clr;
private:
	int            _dat = 0;
};
int QueueRetData::_nbr_clr = 0;

int	test_queue_ret ()
{
	int            ret_val = 0;

	fstb::msg::QueueRetMgr <fstb::msg::MsgRet <QueueRetData> > queue_mgr;

	const int      idx_base = 1;
	int            idx_s = idx_base;
	int            idx_r = idx_s;
	const int      nbr_a = 1000;
	QueueRetData::_nbr_clr = 0;

	auto           q_sptr = queue_mgr.create_new_ret_queue ();
	for (int i = 0; i < nbr_a && ret_val == 0; ++i)
	{
		auto           cell_ptr = queue_mgr.use_pool ().take_cell (true);
		assert (cell_ptr != nullptr);
		cell_ptr->_val._content.set_data (idx_s);
		queue_mgr.enqueue (*cell_ptr, q_sptr);
		++ idx_s;

		if ((i & 7) == 5)
		{
			queue_mgr.flush_ret_queue (*q_sptr);
		}

		cell_ptr = nullptr;
		do
		{
			cell_ptr = queue_mgr.dequeue ();
			if (cell_ptr != nullptr)
			{
				const int      val = cell_ptr->_val._content.get_data ();
				if (val != idx_r)
				{
					printf (
						"QueueRetMgr: wrong order (received %d, expected %d).\n",
						val, idx_r
					);
					ret_val = -1;
				}
				cell_ptr->_val.ret ();
				++ idx_r;
			}
		}
		while (cell_ptr != nullptr);

	}
	queue_mgr.flush_ret_queue (*q_sptr);
	queue_mgr.kill_ret_queue (q_sptr);

	if (idx_s != idx_r || idx_r != idx_base + nbr_a)
	{
		printf ("QueueRetMgr: messages lost.\n");
		ret_val = -1;
	}
	if (QueueRetData::_nbr_clr != nbr_a)
	{
		printf ("QueueRetMgr: failed to deallocate all message's resources.\n");
		ret_val = -1;
	}

	return ret_val;
}



int test_osc_sin_cos_stable_simd ()
{
	int            ret_val = 0;

	printf ("Testing OscSinCosStableSimd...\n");

	float          step = 1e-3f;
	const float    tol  = 1e-5f;
	mfx::dsp::osc::OscSinCosStableSimd   osc;

	const int      len_vec = 10000;
	const int      len     = len_vec * 4;
	std::vector <float>  cos_arr (len);
	std::vector <float>  sin_arr (len);

	osc.clear_buffers ();
	osc.set_step (step);
	osc.process_block (&cos_arr [0], &sin_arr [0], len_vec);

	double         err_sum = 0;
	float          err_max = 0;
	for (int pos = 0; pos < len && ret_val == 0; ++pos)
	{
		const double   angle = pos * step;
		const float    c     = float (cos (angle));
		const float    s     = float (sin (angle));

		const float    dif_c = fabs (cos_arr [pos] - c);
		const float    dif_s = fabs (sin_arr [pos] - s);

		if (dif_c > tol || dif_s > tol)
		{
			printf ("Error at position %d\n", pos);
			ret_val = -1;
		}

		err_sum += dif_c + dif_s;
		err_max = std::max (err_max, dif_c);
		err_max = std::max (err_max, dif_s);
	}

	if (ret_val == 0)
	{
		const double   err = err_sum / (len * 2);
		printf ("Average error: %g. Maximum: %g\n", err, err_max);
	}

	return ret_val;
}



int	test_file_write_fs_ro ()
{
	std::string    pathname = "results/test_file_write_fs_ro.txt";
	std::string    content  = "This is a test.\n";

	std::string    pathname_tmp ("/tmp");
	const size_t   last_delim = pathname.rfind ('/');
	if (last_delim == std::string::npos)
	{
		pathname_tmp += "/";
		pathname_tmp += pathname;
	}
	else
	{
		pathname_tmp += pathname.substr (last_delim);
	}

	int            ret_val = mfx::FileIOInterface::write_txt_file_direct (
		pathname_tmp, content
	);
	if (ret_val == 0)
	{
		std::string    cmd = "sudo ";
		cmd += mfx::Cst::_rw_cmd_script_pathname;
		cmd += " ";
		cmd += "mv \'";
		cmd += pathname_tmp;
		cmd += "\' \'";
		cmd += pathname;
		cmd += "\'";

		const int      ret_val_sys = system (cmd.c_str ());
		if (ret_val_sys != 0)
		{
			ret_val = -1;
		}
	}

	return ret_val;
}



int	test_conv_int_fast ()
{
	printf ("Testing fstb::conv_int_fast():\n");

	bool           trunc_flag = true;
	bool           round_flag = true;
	bool           rnda0_flag = true;
	bool           rnd20_flag = true;
	bool           floor_flag = true;
	for (int k = -8; k <= 8; ++k)
	{
		const float    v_flt = k * 0.5f;
		const int      v_int = fstb::conv_int_fast (v_flt);
		printf ("%+6.2f -> %+3d\n", v_flt, v_int);

		if (trunc (v_flt) != v_int)
		{
			trunc_flag = false;
		}
		if (int (floor (v_flt + 0.5f)) != v_int)
		{
			round_flag = false;
		}
		if (lround (v_flt) != v_int)
		{
			rnda0_flag = false;
		}
		if (   int (floor (v_flt     + 0.5f)) != v_int
		    && int (floor (v_flt * 2 + 0.5f)) != v_int * 2 + 1)
		{
			rnd20_flag = false;
		}
		if (int (floor (v_flt)) != v_int)
		{
			floor_flag = false;
		}
	}
	printf ("Mode: ");
	if (trunc_flag)
	{
		printf ("trunc\n");
	}
	else if (round_flag)
	{
		printf ("exact round\n");
	}
	else if (rnda0_flag)
	{
		printf ("round and half-way case away from 0\n");
	}
	else if (rnd20_flag)
	{
		printf ("round and half-way case to the nearest even integer\n");
	}
	else if (floor_flag)
	{
		printf ("floor\n");
	}
	else
	{
		printf ("unknown\n");
	}

	return 0;
}



int main (int argc, char *argv [])
{
	fstb::unused (argc, argv);

	mfx::dsp::mix::Generic::setup ();

	int            ret_val = 0;

#if 1
	if (ret_val == 0) ret_val = TestHash::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestHiirDesigner::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestDkmSimulator::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestMnaSimulator::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestMoogLadderMystran::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestPinkShade::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestHardclipBl::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestMoogLadderDAngelo::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestSvfAntisat::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestRcClipGeneric::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestDiodeClipJcm::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestDiodeClipScreamer::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestDiodeClipDAngelo::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestSvf::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestSmooth::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = DrawShapers::draw_shapers ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestPsu::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_conv_int_fast ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestLatAlgo::perform_test ();
#endif

#if 0
	#if fstb_IS (SYS, LINUX)
	if (ret_val == 0) ret_val = test_file_write_fs_ro ();
	#endif
#endif

#if 0
	#if fstb_IS (SYS, LINUX)
	if (ret_val == 0) ret_val = TestUserInputPi3::perform_test ();
	#endif
#endif

#if 0
	#if fstb_IS (SYS, LINUX)
	if (ret_val == 0) ret_val = TestLedSimple::perform_test ();
	#endif
#endif

#if 0
	#if fstb_IS (SYS, LINUX)
	if (ret_val == 0) ret_val = TestDisplayFrameBufSimple::perform_test ();
	#endif
#endif

#if 0
	if (ret_val == 0) ret_val = TestApprox::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestSlidingMax::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestSlidingOp::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestOscWavetableSyncHard::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestOscWavetableSub::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestOscWavetable::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestOscSampleSyncFade::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestOscSampleSyncHard::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestOscSample::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestSampleMipMapper::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestConvolverFft::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestRemez::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestInterpFtor::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_osc_sin_cos_stable_simd ();
#endif

#if 0
	if (ret_val == 0) ret_val = TestInterpPhase <mfx::dsp::rspl::InterpPhaseFpu <4>, 6>::perform_test ();
	if (ret_val == 0) ret_val = TestInterpPhase <mfx::dsp::rspl::InterpPhaseSimd <4>, 6>::perform_test ();
	if (ret_val == 0) ret_val = TestInterpPhase <mfx::dsp::rspl::InterpPhaseFpu <12>, 6>::perform_test ();
	if (ret_val == 0) ret_val = TestInterpPhase <mfx::dsp::rspl::InterpPhaseSimd <12>, 6>::perform_test ();
	if (ret_val == 0) ret_val = TestInterpPhase <mfx::dsp::rspl::InterpPhaseFpu <64>, 6>::perform_test ();
	if (ret_val == 0) ret_val = TestInterpPhase <mfx::dsp::rspl::InterpPhaseSimd <64>, 6>::perform_test ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_queue_ret ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_median_filter ();
#endif

#if 0
	#if fstb_IS (SYS, LINUX)
	if (ret_val == 0) ret_val = test_adrv_dpvab ();
	#endif
#endif

#if 0
	if (ret_val == 0) ret_val = test_testgen ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_disto ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_phaser ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_noise_chlorine ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_noise_bleach ();
#endif

#if 0
	if (ret_val == 0) ret_val = draw_all_lfos ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_transients ();
#endif

#if 0
	if (ret_val == 0) ret_val = patch_setup_file ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_bbd_line ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_delay2 ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_osdet ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_freqfast ();
#endif

#if 0
	if (ret_val == 0) ret_val = test_envelope_detector ();
#endif

	if (ret_val == 0)
	{
		printf ("Everything is OK.\n");
	}

	return ret_val;
}
