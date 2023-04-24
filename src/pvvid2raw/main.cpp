
/*

Decodes .pvvid video capture files into raw 8-bit 4:4:4 Y'Cb'Cr' data, 30 fps.
Warning: the raw frames are dumped on the standard output.

Usage:

pvvid2raw "filename.pvvid" | x264 --stitchable --demuxer raw --input-depth 8 --input-range pc --input-res 800x480 --input-csp i444 --range pc --output-csp i444 --sar 1:1 --fps 30 --qp 0 --preset veryfast --keyint 10 --transfer bt709 --colorprim bt709 --colormatrix bt709 --stylish --output "filename.mkv" -

*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/CompressSimple.h"
#include "mfx/VidRecFmt.h"

#if fstb_SYS == fstb_SYS_WIN
	#include <io.h>
	#include <fcntl.h>
#endif

#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>



class VidDecomp
{
public:
	typedef mfx::VidRecFmt::TimestampUnit::period TsPer;
	explicit       VidDecomp (FILE *f_ptr, FILE *o_ptr, int fps_num, int fps_den);
	int            convert_pvvid ();
	int            read_comp_frame (bool &cont_flag);
	int            output_previous_frames (int frame_stop);
	int            conv_ts_to_framenum (uint64_t timestamp) const;
	static uint64_t
	               gcd (uint64_t a, uint64_t b);
private:
	FILE *         _f_ptr;
	FILE *         _o_ptr;
	int            _fps_num;
	int            _fps_den;
	int            _pic_w;
	int            _pic_h;
	std::vector <uint8_t>
	               _frame_buf;
	std::vector <uint8_t>
	               _raw_buf;
	std::vector <uint8_t>
	               _cmp_buf;
	uint64_t       _conv_num;
	uint64_t       _conv_den;
	uint64_t       _ts_cur;
	int            _frame_cur;
};



VidDecomp::VidDecomp (FILE *f_ptr, FILE *o_ptr, int fps_num, int fps_den)
:	_f_ptr (f_ptr)
,	_o_ptr (o_ptr)
,	_fps_num (fps_num)
,	_fps_den (fps_den)
,	_pic_w (0)
,	_pic_h (0)
,	_frame_buf ()
,	_raw_buf ()
,	_cmp_buf ()
,	_conv_num (TsPer::num * _fps_num)
,	_conv_den (TsPer::den * _fps_den)
,	_ts_cur (0)
,	_frame_cur (0)
{
	assert (f_ptr != nullptr);
	assert (o_ptr != nullptr);
	assert (fps_num > 0);
	assert (fps_den > 0);

	const uint64_t g = gcd (_conv_num, _conv_den);
	_conv_num /= g;
	_conv_den /= g;
}



int	VidDecomp::convert_pvvid ()
{
	int            ret_val = 0;

	mfx::VidRecFmt::HeaderFile header;
	if (ret_val == 0 && fread (&header, sizeof (header), 1, _f_ptr) != 1)
	{
		fprintf (stderr, "Error. Cannot read file header.\n");
		ret_val = -1;
	}

	if (ret_val == 0 && header._version > mfx::VidRecFmt::_fmt_version)
	{
		fprintf (
			stderr,
			"Error. Wrong file version %d.\n",
			int (header._version)
		);
		ret_val = -1;
	}
	if (ret_val == 0 && header._pix_code != mfx::VidRecFmt::PixCode_GREY8)
	{
		fprintf (
			stderr,
			"Error. Unsupported pixel format %d.\n",
			int (header._pix_code)
		);
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		_pic_w = header._width;
		_pic_h = header._height;

		_frame_buf.assign (_pic_w * _pic_h, 0);
		_raw_buf.resize (_frame_buf.size ());
		_cmp_buf.resize (_frame_buf.size () * 5 / 4);

		_ts_cur    = 0;
		_frame_cur = 0;
	}

	// Reads frames
	bool           cont_flag = true;
	while (ret_val == 0 && cont_flag)
	{
		ret_val = read_comp_frame (cont_flag);
	}

	return ret_val;
}



int	VidDecomp::read_comp_frame (bool &cont_flag)
{
	int            ret_val = 0;

	mfx::VidRecFmt::HeaderFrame   header;

	if (ret_val == 0 && fread (&header, sizeof (header), 1, _f_ptr) != 1)
	{
		fprintf (
			stderr,
			"Error. Cannot read frame header. Last frame: %d, %llu us.\n",
			_frame_cur,
			static_cast <unsigned long long> (_ts_cur)
		);
		ret_val   = -1;
	}

	int            frame_new = _frame_cur;
	uint64_t       ts_new    = _ts_cur;
	if (ret_val == 0)
	{
#if 0
		fprintf (
			stderr,
			"Decoding frame at %llu us.\n",
			static_cast <unsigned long long> (header._timestamp)
		);
#endif

		assert (header._x < _pic_w);
		assert (header._y < _pic_h);
		assert (header._x + header._w <= _pic_w);
		assert (header._y + header._h <= _pic_h);

		ts_new    = header._timestamp;
		frame_new = conv_ts_to_framenum (ts_new);

		assert (ts_new >= _ts_cur);
		if (header._w == 0 || header._h == 0)
		{
			cont_flag = false;
			++ frame_new; // Makes sure we output the very last frame
		}

		// If the compressed frame is located on an output frame after
		// the current one, generates the required output frames.
		ret_val = output_previous_frames (frame_new);
	}

	// Decodes the current compressed frame
	static const size_t cmp_hd_len = mfx::CompressSimple::_frame_header_len;
	if (ret_val == 0 && cont_flag)
	{
		if (fread (_cmp_buf.data (), cmp_hd_len, 1, _f_ptr) != 1)
		{
			fprintf (
				stderr,
				"Error. Cannot read compressed data header."
				" Last frame: %d, %llu us.\n",
				_frame_cur,
				static_cast <unsigned long long> (_ts_cur)
			);
		}
	}

	size_t         comp_frame_size = 0;
	if (ret_val == 0 && cont_flag)
	{
		comp_frame_size = mfx::CompressSimple::read_compressed_frame_size (
			_cmp_buf.data ()
		);
		assert (cmp_hd_len + comp_frame_size <= _cmp_buf.size ());
		if (fread (
			   _cmp_buf.data () + cmp_hd_len, 1, comp_frame_size, _f_ptr
		   ) != comp_frame_size
		)
		{
			fprintf (
				stderr,
				"Error. Cannot read compressed data."
				" Last frame: %d, %llu us.\n",
				_frame_cur,
				static_cast <unsigned long long> (_ts_cur)
			);
		}
	}

	if (ret_val == 0 && cont_flag)
	{
#if ! defined (NDEBUG)
		const size_t   raw_size =
			mfx::CompressSimple::compute_raw_frame_size (_cmp_buf.data ());
		assert (raw_size <= _raw_buf.size ());
#endif
		mfx::CompressSimple::decompress_frame (
			_raw_buf.data (), _cmp_buf.data ()
		);

		const uint8_t* src_ptr = _raw_buf.data ();
		uint8_t *      dst_ptr = &_frame_buf [header._y * _pic_w + header._x];
		for (int dy = 0; dy < header._h; ++dy)
		{
			memcpy (dst_ptr, src_ptr, header._w);
			src_ptr += header._w;
			dst_ptr += _pic_w;
		}

		_ts_cur = ts_new;
	}

	if (ret_val != 0)
	{
		cont_flag = false;
	}

	return ret_val;
}



int	VidDecomp::output_previous_frames (int frame_stop)
{
	int            ret_val = 0;

	const size_t   len_lum = _pic_w * _pic_h;
	const size_t   len_chr = len_lum;
	if (_frame_cur < frame_stop)
	{
		// Neutral chroma value: 128
		memset (_raw_buf.data (), 128, len_chr);
	}

	while (ret_val == 0 && _frame_cur < frame_stop)
	{
		// Luma plane
		if (fwrite (_frame_buf.data (), 1, len_lum, _o_ptr) != len_lum)
		{
			fprintf (
				stderr,
				"Error. Cannot output frame %d (luma plane).\n",
				_frame_cur
			);	
			ret_val = -1;
		}

		// Chroma planes
		for (int p = 0; p < 2 && ret_val == 0; ++p)
		{
			if (fwrite (_raw_buf.data (), 1, len_chr, _o_ptr) != len_chr)
			{
				fprintf (
					stderr,
					"Error. Cannot output frame %d (chroma plane %d).\n",
					_frame_cur,
					p
				);	
				ret_val = -1;
			}
		}

		++ _frame_cur;
	}

	return ret_val;
}



int	VidDecomp::conv_ts_to_framenum (uint64_t timestamp) const
{

	return int (timestamp * _conv_num / _conv_den);
}



uint64_t	VidDecomp::gcd (uint64_t a, uint64_t b)
{
	if (b == 0)
	{
		return a;
	}

	return gcd (b, a % b);
}



int main (int argc, char *argv [])
{
	int            ret_val = 0;

	int            fps_num = 30;
	int            fps_den = 1;

	if (argc != 2)
	{
		fprintf (stderr, "Error. Usage: pvvid2raw filename.pvvid\n");
		ret_val = -1;
	}

	FILE *         f_ptr = nullptr;
	if (ret_val == 0)
	{
		f_ptr = fstb::fopen_utf8 (argv [1], "rb");
		if (f_ptr == nullptr)
		{
			fprintf (stderr, "Error. Cannot open %s\n", argv [1]);
			ret_val = -1;
		}
	}

	if (ret_val == 0)
	{
#if fstb_SYS == fstb_SYS_WIN
		(void) _setmode (_fileno (stdout), O_BINARY);
#endif // fstb_SYS_WIN

		VidDecomp      vid_dec (f_ptr, stdout, fps_num, fps_den);
		ret_val = vid_dec.convert_pvvid ();
	}

	if (f_ptr != nullptr)
	{
		fclose (f_ptr);
		f_ptr = nullptr;
	}

	return ret_val;
}

