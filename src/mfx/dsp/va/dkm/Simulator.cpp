/*****************************************************************************

        Simulator.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/Approx.h"
#include "lal/op.h"
#include "mfx/dsp/va/dkm/Simulator.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace dkm
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Simulator::add_src_v (IdNode nid_1, IdNode nid_2, Flt v)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_src_v_arr.size ());
	_src_v_arr.push_back (SrcV { nid_1, nid_2, v });

	return idx;
}



int	Simulator::add_resistor (IdNode nid_1, IdNode nid_2, Flt r)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);
	assert (r > 0);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_resistor_arr.size ());
	_resistor_arr.push_back (Resistor { nid_1, nid_2, r });

	return idx;
}



int	Simulator::add_pot (IdNode nid_1, IdNode nid_2, Flt rmin, Flt rmax, Flt pos)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);
	assert (rmin >= 0);
	assert (rmax >= 0);
	assert (pos >= 0);
	assert (pos <= 1);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_pot_arr.size ());
	_pot_arr.push_back (Potentiometer { nid_1, nid_2, rmin, rmax, pos });

	_rv_val_a_arr.push_back (rmax - rmin);
	_rv_val_b_arr.push_back (rmin);
	_rv_pos_arr.push_back (pos);

	return idx;
}



int	Simulator::add_capacitor (IdNode nid_1, IdNode nid_2, Flt c)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);
	assert (c > 0);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_capa_arr.size ());
	Capacitor      part;
	part._nid_1 = nid_1;
	part._nid_2 = nid_2;
	part._c     = c;
	_capa_arr.push_back (part);

	return idx;
}



int	Simulator::add_inductor (IdNode nid_1, IdNode nid_2, Flt l)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);
	assert (l > 0);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_inductor_arr.size ());
	Inductor       part;
	part._nid_1 = nid_1;
	part._nid_2 = nid_2;
	part._l     = l;
	_inductor_arr.push_back (part);

	return idx;
}



int	Simulator::add_diode (IdNode nid_1, IdNode nid_2, Flt is, Flt n)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);
	assert (n > 0);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_diode_arr.size ());
	Diode          part;
	part._nid_1 = nid_1;
	part._nid_2 = nid_2;
	part._junc.set_param (is, n, 100);
	_diode_arr.push_back (part);

	return idx;
}



int	Simulator::add_diode_antipar (IdNode nid_1, IdNode nid_2, Flt is1, Flt n1, Flt is2, Flt n2)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);
	assert (is1 >= 1e-20f);
	assert (is1 <= 1e-1f);
	assert (n1 > 0);
	assert (is2 >= 1e-20f);
	assert (is2 <= 1e-1f);
	assert (n2 > 0);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_diode_pair_arr.size ());
	DiodeAntipar   part;
	part._nid_1 = nid_1;
	part._nid_2 = nid_2;
	part._dir_arr [0].set_param (is1, n1, 100);
	part._dir_arr [1].set_param (is2, n2, 100);
	_diode_pair_arr.push_back (part);

	return idx;
}



int	Simulator::add_bjt_npn (IdNode nid_e, IdNode nid_b, IdNode nid_c, Flt is, Flt n, Flt beta_f, Flt beta_r)
{
	assert (nid_e >= 0);
	assert (nid_b >= 0);
	assert (nid_c >= 0);
	assert (nid_b != nid_e || nid_b != nid_c);
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);
	assert (n > 0);
	assert (beta_f > 0);
	assert (beta_r > 0);

	use_or_create_node (nid_e);
	use_or_create_node (nid_b);
	use_or_create_node (nid_c);

	const int      idx = int (_bjt_npn_arr.size ());
	BjtNpn         part;
	part._nid_e  = nid_e;
	part._nid_b  = nid_b;
	part._nid_c  = nid_c;
	part._beta_f = beta_f;
	part._beta_r = beta_r;
	part._junc_be.set_param (is, n, 100);
	part._junc_bc.set_param (is, n, 100);
	part._alpha_f_inv = (1 + beta_f) / beta_f;
	part._alpha_r_inv = (1 + beta_r) / beta_r;
	_bjt_npn_arr.push_back (part);

	return idx;
}



int	Simulator::add_output (IdNode nid_1, IdNode nid_2)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);

	use_or_create_node (nid_1);
	use_or_create_node (nid_2);

	const int      idx = int (_output_arr.size ());
	_output_arr.push_back (Output { nid_1, nid_2 });

	return idx;
}



void	Simulator::prepare (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = Flt (sample_freq);

	build_matrices ();
}



void	Simulator::update_sample_freq (double sample_freq)
{
	assert (_sample_freq > 0);
	assert (sample_freq > 0);

	_sample_freq = Flt (sample_freq);

	// Updates sampling-rate dependent matrices
	setup_ese ();

	finalize_matrices ();
}



void	Simulator::set_reordering_jacobian (const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
#if ! defined (NDEBUG)
	assert (_sample_freq > 0);
	assert (r_arr.size () == _j_r_r_arr.size ());
	assert (c_arr.size () == _j_r_c_arr.size ());
	const int      nr = int (r_arr.size ());
	const int      nc = int (c_arr.size ());
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == nr * (nr - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == nc * (nc - 1) / 2);
#endif

	_j_r_r_arr = r_arr;
	_j_r_c_arr = c_arr;
}



void	Simulator::set_max_nbr_it (int max_it)
{
	assert (max_it > 0);
	assert (max_it <= _limit_it);

	_max_it = max_it;
}



void	Simulator::set_src_v (int idx, Flt v)
{
	assert (idx >= 0);
	assert (idx < _nbr_src_v);

	_vec_u [idx] = v;
}



void	Simulator::set_pot (int idx, Flt pos)
{
	assert (idx >= 0);
	assert (idx < _nbr_pot);
	assert (pos >= 0);
	assert (pos <= 1);

	_rv_pos_arr [idx] = pos;
	_r_v_dirty_flag   = true;
}



void	Simulator::process_sample ()
{
#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	_st_nbr_it = 0;
#endif //mfx_dsp_va_dkm_Simulator_STATS

	if (_r_v_dirty_flag)
	{
		update_r_v ();
	}

	// Linear system
	if (_linear_flag)
	{
		// _vec_v_o   = _mat_e * _vec_u + _mat_d * _vec_x_prv;
		lal::mul (_tmp_v_o_u, _mat_e, _vec_u);
		lal::mul (_tmp_v_o_x, _mat_d, _vec_x_prv);
		lal::add (_vec_v_o, _tmp_v_o_u, _tmp_v_o_x);

		// _vec_x_cur = _mat_b * _vec_u + _mat_a * _vec_x_prv;
		lal::mul (_tmp_x_u, _mat_b, _vec_u);
		lal::mul (_tmp_x_x, _mat_a, _vec_x_prv);
		lal::add (_vec_x_cur, _tmp_x_u, _tmp_x_x);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
		_st_nbr_it = 1;
#endif //mfx_dsp_va_dkm_Simulator_STATS
	}

	// Non-linear system
	else
	{
		// Finds voltages across the non-linear elements
		// _vec_p = _mat_h * _vec_u + _mat_g * _vec_x_prv;
		lal::mul (_tmp_p_u, _mat_h, _vec_u);
		lal::mul (_tmp_p_x, _mat_g, _vec_x_prv);
		lal::add (_vec_p, _tmp_p_u, _tmp_p_x);
		solve_nl ();

		// _vec_v_o   = _mat_e * _vec_u - _mat_f * _vec_i_n + _mat_d * _vec_x_prv;
		lal::mul (_tmp_v_o_u, _mat_e, _vec_u);
		lal::mul (_tmp_v_o_x, _mat_d, _vec_x_prv);
		lal::add (_vec_v_o, _tmp_v_o_u, _tmp_v_o_x);
		lal::mul (_tmp_v_o_i, _mat_f, _vec_i_n);
		lal::sub (_vec_v_o, _vec_v_o, _tmp_v_o_i);

		// _vec_x_cur = _mat_b * _vec_u - _mat_c * _vec_i_n + _mat_a * _vec_x_prv;
		lal::mul (_tmp_x_u, _mat_b, _vec_u);
		lal::mul (_tmp_x_x, _mat_a, _vec_x_prv);
		lal::add (_vec_x_cur, _tmp_x_u, _tmp_x_x);
		lal::mul (_tmp_x_i, _mat_c, _vec_i_n);
		lal::sub (_vec_x_cur, _vec_x_cur, _tmp_x_i);
	}

	lal::copy (_vec_x_prv, _vec_x_cur);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	++ _st._hist_it [_st_nbr_it];
	++ _st._nbr_spl_proc;
#endif // mfx_dsp_va_dkm_Simulator_STATS
}



Flt	Simulator::get_output (int idx) const
{
	assert (idx >= 0);
	assert (idx < _nbr_out);

	return _vec_v_o [idx];
}



void	Simulator::clear_buffers ()
{
	std::fill (_vec_x_cur.begin (), _vec_x_cur.end (), Flt (0));
	std::fill (_vec_x_prv.begin (), _vec_x_prv.end (), Flt (0));
	std::fill (_vec_v_o.begin ()  , _vec_v_o.end ()  , Flt (0));
	std::fill (_vec_v_n.begin ()  , _vec_v_n.end ()  , Flt (0));
}



int	Simulator::get_nbr_nodes () const
{
	return _nbr_nodes;
}



int	Simulator::get_nbr_src_v () const
{
	return _nbr_src_v;
}



int	Simulator::get_nbr_non_lin () const
{
	return _nbr_nl;
}



int	Simulator::get_nbr_res () const
{
	return _nbr_res;
}



int	Simulator::get_nbr_pot () const
{
	return _nbr_pot;
}



int	Simulator::get_nbr_ese () const
{
	return _nbr_ese;
}



int	Simulator::get_nbr_out () const
{
	return _nbr_out;
}



#if defined (mfx_dsp_va_dkm_Simulator_DISPLAY)

std::string	Simulator::print_all () const
{
	std::string    txt;

	txt += print_vector (_vec_x_cur     , "_vec_x_cur"     );
	txt += print_vector (_vec_x_prv     , "_vec_x_prv"     );
	txt += print_vector (_vec_u         , "_vec_u"         );
	txt += print_vector (_vec_v_n       , "_vec_v_n"       );
	txt += print_vector (_vec_i_n       , "_vec_i_n"       );
	txt += print_vector (_vec_v_o       , "_vec_v_o"       );
	txt += print_vector (_vec_p         , "_vec_p"         );
	txt += print_matrix (_mat_a         , "_mat_a"         );
	txt += print_matrix (_mat_b         , "_mat_b"         );
	txt += print_matrix (_mat_c         , "_mat_c"         );
	txt += print_matrix (_mat_d         , "_mat_d"         );
	txt += print_matrix (_mat_e         , "_mat_e"         );
	txt += print_matrix (_mat_f         , "_mat_f"         );
	txt += print_matrix (_mat_g         , "_mat_g"         );
	txt += print_matrix (_mat_h         , "_mat_h"         );
	txt += print_matrix (_mat_k         , "_mat_k"         );
	txt += print_vector (_dia_g_r       , "_dia_g_r"       );
	txt += print_vector (_dia_g_x       , "_dia_g_x"       );
	txt += print_vector (_dia_z         , "_dia_z"         );
	txt += print_vector (_dia_r_v       , "_dia_r_v"       );
	txt += print_matrix (_mat_n_x       , "_mat_n_x"       );
	txt += print_matrix (_mat_n_o       , "_mat_n_o"       );
	txt += print_matrix (_mat_n_n       , "_mat_n_n"       );
	txt += print_matrix (_mat_n_u       , "_mat_n_u"       );
	txt += print_matrix (_mat_n_v       , "_mat_n_v"       );
	txt += print_matrix (_mat_n_r       , "_mat_n_r"       );
	txt += print_matrix (_mat_s_0       , "_mat_s_0"       );
	txt += print_matrix (_mat_s_0_inv   , "_mat_s_0_inv"   );
	txt += print_matrix (_mat_q         , "_mat_q"         );
	txt += print_matrix (_mat_r_v_q_inv , "_mat_r_v_q_inv" );
	txt += print_matrix (_mat_a_0       , "_mat_a_0"       );
	txt += print_matrix (_mat_b_0       , "_mat_b_0"       );
	txt += print_matrix (_mat_c_0       , "_mat_c_0"       );
	txt += print_matrix (_mat_d_0       , "_mat_d_0"       );
	txt += print_matrix (_mat_e_0       , "_mat_e_0"       );
	txt += print_matrix (_mat_f_0       , "_mat_f_0"       );
	txt += print_matrix (_mat_g_0       , "_mat_g_0"       );
	txt += print_matrix (_mat_h_0       , "_mat_h_0"       );
	txt += print_matrix (_mat_k_0       , "_mat_k_0"       );
	txt += print_matrix (_mat_u_x       , "_mat_u_x"       );
	txt += print_matrix (_mat_u_o       , "_mat_u_o"       );
	txt += print_matrix (_mat_u_n       , "_mat_u_n"       );
	txt += print_matrix (_mat_u_u       , "_mat_u_u"       );
	txt += print_matrix (_mat_j_f       , "_mat_j_f"       );
	txt += print_matrix (_mat_j_r       , "_mat_j_r"       );
	txt += print_matrix (_dia_id_n      , "_dia_id_n"      );
	txt += print_vector (_vec_r_neg     , "_vec_r_neg"     );
	txt += print_vector (_vec_delta_x   , "_vec_delta_x"   );
	txt += print_matrix (_mat_abc_tmp   , "_mat_abc_tmp"   );
	txt += print_matrix (_mat_def_tmp   , "_mat_def_tmp"   );
	txt += print_matrix (_mat_ghk_tmp   , "_mat_ghk_tmp"   );
	txt += print_matrix (_mat_abc_0_tmp1, "_mat_abc_0_tmp1");
	txt += print_matrix (_mat_abc_0_tmp2, "_mat_abc_0_tmp2");
	txt += print_matrix (_mat_def_0_tmp , "_mat_def_0_tmp" );
	txt += print_matrix (_mat_ghk_0_tmp , "_mat_ghk_0_tmp" );
	txt += print_matrix (_mat_u_tmp     , "_mat_u_tmp"     );

	return txt;
}

#endif // mfx_dsp_va_dkm_Simulator_DISPLAY



#if defined (mfx_dsp_va_dkm_Simulator_STATS)

void	Simulator::reset_stats ()
{
	_st._hist_it.fill (0);
	_st._nbr_spl_proc = 0;
#if defined (mfx_dsp_va_dkm_Simulator_STATS_PIV)
	_st._piv_map.clear ();
#endif
}



void	Simulator::get_stats (Stats &stats) const
{
	stats = _st;
}

#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Simulator::build_matrices ()
{
	// Finds the number of elements
	_nbr_nodes   = int (_nid_to_idx_map.size ());
	_nbr_res     = int (_resistor_arr.size ());
	_nbr_pot     = int (_pot_arr.size ());
	_nbr_ese     = int (_capa_arr.size () + _inductor_arr.size ());
	_nbr_nl      = int (
		_diode_arr.size () + _diode_pair_arr.size () + _bjt_npn_arr.size () * 2
	);
	_nbr_src_v   = int (_src_v_arr.size ());
	_nbr_out     = int (_output_arr.size ());

	_linear_flag = (_nbr_nl == 0);

	// Sets everything up
	resize_and_clear_mat_vec ();

	// Fills matrices (excepted potentiometers)
	setup_src_v ();
	setup_res ();
	setup_pot ();
	setup_ese ();
	setup_nl ();
	setup_outputs ();

	// Last things
	finalize_matrices ();
}



void	Simulator::resize_and_clear_mat_vec ()
{
	const int      msize = _nbr_nodes + _nbr_src_v;

	// Resizes all matrices and vectors
	_vec_x_cur.resize (_nbr_ese);
	_vec_x_prv.resize (_nbr_ese);
	_vec_u.resize (_nbr_src_v);
	_vec_v_n.resize (_nbr_nl);
	_vec_i_n.resize (_vec_v_n.size ());
	_vec_v_o.resize (_nbr_out);
	_vec_p.resize (_nbr_nl);

	_mat_a.resize (_nbr_ese, _nbr_ese);
	_mat_b.resize (_nbr_ese, _nbr_src_v);
	_mat_c.resize (_nbr_ese, _nbr_nl);

	_mat_d.resize (_nbr_out, _nbr_ese);
	_mat_e.resize (_nbr_out, _nbr_src_v);
	_mat_f.resize (_nbr_out, _nbr_nl);

	_mat_g.resize (_nbr_nl , _nbr_ese);
	_mat_h.resize (_nbr_nl , _nbr_src_v);
	_mat_k.resize (_nbr_nl , _nbr_nl);

	_dia_g_r.resize (_nbr_res);
	_dia_g_x.resize (_nbr_ese);
	_dia_z.resize (_nbr_ese);
	_dia_r_v.resize (_nbr_pot);

	_mat_n_x.resize (_nbr_ese  , _nbr_nodes);
	_mat_n_o.resize (_nbr_out  , _nbr_nodes);
	_mat_n_n.resize (_nbr_nl   , _nbr_nodes);
	_mat_n_u.resize (_nbr_src_v, _nbr_nodes);
	_mat_n_v.resize (_nbr_pot  , _nbr_nodes);
	_mat_n_r.resize (_nbr_res  , _nbr_nodes);

	_mat_s_0.resize (msize, msize);
	_mat_s_0_inv.resize (_mat_s_0);
	_mat_q.resize (int (_dia_r_v.size ()), int (_dia_r_v.size ()));
	_mat_r_v_q_inv.resize (_mat_q);
	_mat_r_v_q_lu.resize (_mat_q);
	_r_v_q_r_arr.resize (_mat_r_v_q_lu.get_rows ());
	_r_v_q_c_arr.resize (_mat_r_v_q_lu.get_cols ());
	for (int k = 0; k < int (_dia_r_v.size ()); ++k)
	{
		_r_v_q_r_arr [k] = k;
		_r_v_q_c_arr [k] = k;
	}
	_r_v_q_y.resize (_dia_r_v.size ());
	_mat_a_0.resize (_mat_a);
	_mat_b_0.resize (_mat_b);
	_mat_c_0.resize (_mat_c);
	_mat_d_0.resize (_mat_d);
	_mat_e_0.resize (_mat_e);
	_mat_f_0.resize (_mat_f);
	_mat_g_0.resize (_mat_g);
	_mat_h_0.resize (_mat_h);
	_mat_k_0.resize (_mat_k);
	_mat_u_x.resize (_nbr_ese, _nbr_pot);
	_mat_u_o.resize (_nbr_out, _nbr_pot);
	_mat_u_n.resize (_nbr_nl, _nbr_pot);
	_mat_u_u.resize (_nbr_src_v, _nbr_pot);
	_mat_j_f.resize (_nbr_nl, _nbr_nl);
	_mat_j_r.resize (_mat_j_f);
	_dia_id_n.resize (_mat_j_f);
	_vec_r_neg.resize (_vec_v_n.size ());
	_vec_r_neg_tmp1.resize (_vec_r_neg.size ());
	_vec_r_neg_tmp2.resize (_vec_r_neg.size ());
	_vec_delta_x.resize (_vec_v_n.size ());
	_mat_abc_tmp.resize (_nbr_ese, _nbr_pot);
	_mat_def_tmp.resize (_nbr_out, _nbr_pot);
	_mat_ghk_tmp.resize (_nbr_nl , _nbr_pot);
	_mat_abc_0_tmp1.resize (_mat_n_x);
	_mat_abc_0_tmp2.resize (_mat_n_x);
	_mat_def_0_tmp.resize (_mat_n_o);
	_mat_ghk_0_tmp.resize (_mat_n_n);
	_mat_u_tmp.resize (_nbr_nodes, _nbr_pot);
	_j_r_r_arr.resize (_nbr_nl);
	_j_r_c_arr.resize (_nbr_nl);
	for (int k = 0; k < _nbr_nl; ++k)
	{
		_j_r_r_arr [k] = k;
		_j_r_c_arr [k] = k;
	}
	_j_r_y.resize (_nbr_nl);

	// Temporary data
	_tmp_v_o_u.resize (_vec_v_o.size ());
	_tmp_v_o_x.resize (_vec_v_o.size ());
	_tmp_v_o_i.resize (_vec_v_o.size ());
	_tmp_x_u.resize (_vec_x_cur.size ());
	_tmp_x_x.resize (_vec_x_cur.size ());
	_tmp_x_i.resize (_vec_x_cur.size ());
	_tmp_p_u.resize (_vec_p.size ());
	_tmp_p_x.resize (_vec_p.size ());

	// Resets everything
	lal::fill (_vec_x_cur, Flt (0));
	lal::fill (_vec_x_prv, Flt (0));
	lal::fill (_vec_u, Flt (0));
	lal::fill (_vec_v_n, Flt (0));
	lal::fill (_vec_i_n, Flt (0));
	lal::fill (_vec_v_o, Flt (0));
	lal::fill (_vec_p, Flt (0));

	lal::fill (_mat_a, Flt (0));
	lal::fill (_mat_b, Flt (0));
	lal::fill (_mat_c, Flt (0));

	lal::fill (_mat_d, Flt (0));
	lal::fill (_mat_e, Flt (0));
	lal::fill (_mat_f, Flt (0));

	lal::fill (_mat_g, Flt (0));
	lal::fill (_mat_h, Flt (0));
	lal::fill (_mat_k, Flt (0));

	lal::fill (_dia_g_r, Flt (0));
	lal::fill (_dia_g_x, Flt (0));
	lal::fill (_dia_z, Flt (0));
	lal::fill (_dia_r_v, Flt (0));

	lal::fill (_mat_n_x, Flt (0));
	lal::fill (_mat_n_o, Flt (0));
	lal::fill (_mat_n_n, Flt (0));
	lal::fill (_mat_n_u, Flt (0));
	lal::fill (_mat_n_v, Flt (0));
	lal::fill (_mat_n_r, Flt (0));

	lal::fill (_mat_s_0, Flt (0));
	lal::fill (_mat_s_0_inv, Flt (0));
	lal::fill (_mat_q, Flt (0));
	lal::fill (_mat_r_v_q_inv, Flt (0));
	lal::fill (_mat_a_0, Flt (0));
	lal::fill (_mat_b_0, Flt (0));
	lal::fill (_mat_c_0, Flt (0));
	lal::fill (_mat_d_0, Flt (0));
	lal::fill (_mat_e_0, Flt (0));
	lal::fill (_mat_f_0, Flt (0));
	lal::fill (_mat_g_0, Flt (0));
	lal::fill (_mat_h_0, Flt (0));
	lal::fill (_mat_k_0, Flt (0));
	lal::fill (_mat_u_x, Flt (0));
	lal::fill (_mat_u_o, Flt (0));
	lal::fill (_mat_u_n, Flt (0));
	lal::fill (_mat_u_u, Flt (0));
	lal::fill (_mat_j_f, Flt (0));
	lal::fill (_mat_j_r, Flt (0));
	_dia_id_n.set_id ();
	lal::fill (_vec_r_neg, Flt (0));
	lal::fill (_vec_delta_x, Flt (0));
	lal::fill (_mat_abc_tmp, Flt (0));
	lal::fill (_mat_def_tmp, Flt (0));
	lal::fill (_mat_ghk_tmp, Flt (0));
	lal::fill (_mat_abc_0_tmp1, Flt (0));
	lal::fill (_mat_abc_0_tmp2, Flt (0));
	lal::fill (_mat_def_0_tmp, Flt (0));
	lal::fill (_mat_ghk_0_tmp, Flt (0));
	lal::fill (_mat_u_tmp, Flt (0));
}



void	Simulator::setup_src_v ()
{
	for (int idx = 0; idx < _nbr_src_v; ++idx)
	{
		auto &         elt = _src_v_arr [idx];
		add_oim_entry (_mat_n_u, idx, elt._nid_1, elt._nid_2);
		_vec_u [idx] = elt._v;
	}
}



void	Simulator::setup_res ()
{
	for (int idx = 0; idx < _nbr_res; ++idx)
	{
		auto &         elt = _resistor_arr [idx];
		add_oim_entry (_mat_n_r, idx, elt._nid_1, elt._nid_2);
		_dia_g_r [idx] = 1.f / elt._r;
	}
}



void	Simulator::setup_pot ()
{
	for (int idx = 0; idx < _nbr_pot; ++idx)
	{
		auto &         elt = _pot_arr [idx];
		add_oim_entry (_mat_n_v, idx, elt._nid_1, elt._nid_2);
	}
}



void	Simulator::setup_ese ()
{
	int            idx = 0;

	for (int c_cnt = 0; c_cnt < int (_capa_arr.size ()); ++c_cnt)
	{
		auto &         elt = _capa_arr [c_cnt];
		add_oim_entry (_mat_n_x, idx, elt._nid_1, elt._nid_2);
		_dia_g_x [idx] = Flt (2 * elt._c * _sample_freq);
		_dia_z [idx]   = Flt (+1);
		elt._base_idx  = idx;
		++ idx;
	}

	for (int l_cnt = 0; l_cnt < int (_inductor_arr.size ()); ++l_cnt)
	{
		auto &         elt = _inductor_arr [l_cnt];
		add_oim_entry (_mat_n_x, idx, elt._nid_1, elt._nid_2);
		_dia_g_x [idx] = Flt (_sample_freq / (2 * elt._l));
		_dia_z [idx]   = Flt (-1);
		elt._base_idx  = idx;
		++ idx;
	}

	assert (idx == _nbr_ese);
}



void	Simulator::setup_nl ()
{
	int            idx = 0;

	for (int d_cnt = 0; d_cnt < int (_diode_arr.size ()); ++d_cnt)
	{
		auto &         elt = _diode_arr [d_cnt];
		add_oim_entry (_mat_n_n, idx, elt._nid_1, elt._nid_2);
		elt._base_idx = idx;
		elt._idx_1    = use_node (elt._nid_1);
		elt._idx_2    = use_node (elt._nid_2);
		++ idx;
	}

	for (int dp_cnt = 0; dp_cnt < int (_diode_pair_arr.size ()); ++dp_cnt)
	{
		auto &         elt = _diode_pair_arr [dp_cnt];
		add_oim_entry (_mat_n_n, idx, elt._nid_1, elt._nid_2);
		elt._base_idx = idx;
		elt._idx_1    = use_node (elt._nid_1);
		elt._idx_2    = use_node (elt._nid_2);
		++ idx;
	}

	for (int npn_cnt = 0; npn_cnt < int (_bjt_npn_arr.size ()); ++npn_cnt)
	{
		auto &         elt = _bjt_npn_arr [npn_cnt];
		add_oim_entry (_mat_n_n, idx    , elt._nid_b, elt._nid_e);
		add_oim_entry (_mat_n_n, idx + 1, elt._nid_b, elt._nid_c);
		elt._base_idx = idx;
		elt._idx_e    = use_node (elt._nid_e);
		elt._idx_b    = use_node (elt._nid_b);
		elt._idx_c    = use_node (elt._nid_c);
		idx += 2;
	}

	assert (idx == _nbr_nl);
}



void	Simulator::setup_outputs ()
{
	for (int idx = 0; idx < _nbr_out; ++idx)
	{
		auto &         elt = _output_arr [idx];
		add_oim_entry (_mat_n_o, idx, elt._nid_1, elt._nid_2);
	}
}



void	Simulator::add_oim_entry (TypeMatrix &m, int row, IdNode nid_1, IdNode nid_2)
{
	assert (m.get_cols () == _nbr_nodes);
	assert (row >= 0);
	assert (row < m.get_rows ());
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_1 != nid_2);

	if (nid_1 != _nid_gnd)
	{
		auto           it  = _nid_to_idx_map.find (nid_1);
		assert (it != _nid_to_idx_map.end ());
		const int      idx = it->second;
		m (row, idx) = +1;
	}

	if (nid_2 != _nid_gnd)
	{
		auto           it  = _nid_to_idx_map.find (nid_2);
		assert (it != _nid_to_idx_map.end ());
		const int      idx = it->second;
		m (row, idx) = -1;
	}
}



int	Simulator::use_or_create_node (IdNode nid)
{
	assert (nid >= 0);

	int            idx = _idx_gnd;
	if (nid != _nid_gnd)
	{
		auto           it = _nid_to_idx_map.find (nid);
		if (it != _nid_to_idx_map.end ())
		{
			idx = it->second;
		}
		else
		{
			// Not found? Creates a new node
			idx = _nbr_nodes;
			_nid_to_idx_map [nid] = idx;
			++ _nbr_nodes;
		}
	}

	return idx;
}



int	Simulator::use_node (IdNode nid) const
{
	int            idx = _idx_gnd;
	if (nid != _nid_gnd)
	{
		auto           it = _nid_to_idx_map.find (nid);
		if (it == _nid_to_idx_map.end ())
		{
			assert (false);
		}
		else
		{
			idx = it->second;
		}
	}

	return idx;
}



void	Simulator::finalize_matrices ()
{
	build_s_0_inv ();
	prepare_dk_const_matrices ();
	update_r_v ();
}



void	Simulator::build_s_0_inv ()
{
	auto           s_0_tl =
		_mat_s_0.make_sub (0, 0, _nbr_nodes, _nbr_nodes);
	auto           s_0_tr =
		_mat_s_0.make_sub (0, _nbr_nodes, _nbr_nodes, _nbr_src_v);
	auto           s_0_bl =
		_mat_s_0.make_sub (_nbr_nodes, 0, _nbr_src_v, _nbr_nodes);
	auto           s_0_br =
		_mat_s_0.make_sub (_nbr_nodes, _nbr_nodes, _nbr_src_v, _nbr_src_v);

	// _mat_s_0.topLeftCorner (_nbr_nodes, _nbr_nodes) =
	// 	  _mat_n_r.transpose () * _dia_g_r * _mat_n_r
	// 	+ _mat_n_x.transpose () * _dia_g_x * _mat_n_x;
	TypeMatrix     tmp;
	TypeMatrix     tmp_tl_r;
	mul_oim (tmp_tl_r, tmp, _mat_n_r, _dia_g_r, _mat_n_r);
	TypeMatrix     tmp_tl_x;
	mul_oim (tmp_tl_x, tmp, _mat_n_x, _dia_g_x, _mat_n_x);
	lal::add (s_0_tl, tmp_tl_r, tmp_tl_x);

	lal::transpose (s_0_tr, _mat_n_u);
	lal::copy (s_0_bl, _mat_n_u);
	lal::fill (s_0_br, Flt (0));

	// If the matrix is not invertible, there are probably some unconnected
	// nodes (for example, potentiometers absent from S0). Then one should
	// add large resistors to make S0 invertible.
	// Note: determinant calculation is not reliable in float, so it is skipped
	// in this case.
//	assert ((sizeof (Flt) == sizeof (float)) || _mat_s_0.determinant () != 0);

	const int      mat_sz = _mat_s_0.get_rows ();
	std::vector <int> r_arr (mat_sz);
	std::vector <int> c_arr (mat_sz);
	for (int k = 0; k < mat_sz; ++k)
	{
		r_arr [k] = k;
		c_arr [k] = k;
	}
	lal::copy (tmp, _mat_s_0);
	lal::decompose_lu (tmp, r_arr, c_arr);
	TypeVector    y;
	lal::invert (_mat_s_0_inv, y, tmp, r_arr, c_arr);
}



// Requires _mat_s_0_inv to be ready
void	Simulator::prepare_dk_const_matrices ()
{
	TypeVector     tmp_vec;
	TypeMatrix     tmp_mat;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	// _mat_abc_0_tmp1 = 2 * _dia_z * _dia_g_x * _mat_n_x;
	lal::mul_cw (tmp_vec, _dia_z, _dia_g_x);
	lal::mul (tmp_vec, tmp_vec, Flt (2));
	lal::vec_to_mat (tmp_mat, tmp_vec, lal::Dir::D);
	lal::mul (_mat_abc_0_tmp1, tmp_mat, _mat_n_x);

	lal::mul (
		_mat_abc_0_tmp2,
		_mat_abc_0_tmp1,
		_mat_s_0_inv.make_sub (0, 0, _nbr_nodes, _nbr_nodes)
	);

	// _mat_a_0 = _mat_abc_0_tmp2 * _mat_n_x.transpose () - _dia_z;
	lal::mul_transp_rhs (_mat_a_0, _mat_abc_0_tmp2, _mat_n_x);
	lal::vec_to_mat (tmp_mat, _dia_z, lal::Dir::D);
	lal::sub (_mat_a_0, _mat_a_0, tmp_mat);

	lal::mul_transp_rhs (_mat_c_0, _mat_abc_0_tmp2, _mat_n_n);

	lal::mul (
		_mat_b_0,
		_mat_abc_0_tmp1,
		_mat_s_0_inv.make_sub (0, _nbr_nodes, _nbr_nodes, _nbr_src_v)
	);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	lal::mul (
		_mat_def_0_tmp,
		_mat_n_o,
		_mat_s_0_inv.make_sub (0, 0, _nbr_nodes, _nbr_nodes)
	);

	lal::mul_transp_rhs (_mat_d_0, _mat_def_0_tmp, _mat_n_x);
	lal::mul_transp_rhs (_mat_f_0, _mat_def_0_tmp, _mat_n_n);
	lal::mul (
		_mat_e_0,
		_mat_n_o,
		_mat_s_0_inv.make_sub (0, _nbr_nodes, _nbr_nodes, _nbr_src_v)
	);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	lal::mul (
		_mat_ghk_0_tmp,
		_mat_n_n,
		_mat_s_0_inv.make_sub (0, 0, _nbr_nodes, _nbr_nodes)
	);
	lal::mul_transp_rhs (_mat_g_0, _mat_ghk_0_tmp, _mat_n_x);
	lal::mul_transp_rhs (_mat_k_0, _mat_ghk_0_tmp, _mat_n_n);
	lal::mul (
		_mat_h_0,
		_mat_n_n,
		_mat_s_0_inv.make_sub (0, _nbr_nodes, _nbr_nodes, _nbr_src_v)
	);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	lal::mul_transp_rhs (
		_mat_u_tmp,
		_mat_s_0_inv.make_sub (0, 0, _nbr_nodes, _nbr_nodes),
		_mat_n_v
	);
	lal::mul (_mat_u_x, _mat_n_x, _mat_u_tmp);
	lal::mul (_mat_u_o, _mat_n_o, _mat_u_tmp);
	lal::mul (_mat_u_n, _mat_n_n, _mat_u_tmp);
	lal::mul_transp_rhs (
		_mat_u_u,
		_mat_s_0_inv.make_sub (_nbr_nodes, 0, _nbr_src_v, _nbr_nodes),
		_mat_n_v
	);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	if (_nbr_pot > 0)
	{
		lal::mul (
			tmp_mat,
			_mat_n_v,
			_mat_s_0_inv.make_sub (0, 0, _nbr_nodes, _nbr_nodes)
		);
		lal::mul_transp_rhs (_mat_q, tmp_mat, _mat_n_v);

		// _mat_2zgxux = _dia_z * _dia_g_x * _mat_u_x * 2;
		lal::mul_cw (tmp_vec, _dia_z, _dia_g_x);
		lal::mul (tmp_vec, tmp_vec, Flt (2));
		lal::vec_to_mat (tmp_mat, tmp_vec, lal::Dir::D);
		lal::mul (_mat_2zgxux, tmp_mat, _mat_u_x);
	}
}



void	Simulator::update_r_v ()
{
	lal::copy (_mat_a, _mat_a_0);
	lal::copy (_mat_b, _mat_b_0);
	lal::copy (_mat_d, _mat_d_0);
	lal::copy (_mat_e, _mat_e_0);
	lal::copy (_mat_g, _mat_g_0);
	lal::copy (_mat_h, _mat_h_0);
	if (! _linear_flag)
	{
		lal::copy (_mat_c, _mat_c_0);
		lal::copy (_mat_f, _mat_f_0);
		lal::copy (_mat_k, _mat_k_0);
	}

	if (_nbr_pot > 0)
	{
		// Updates Rv
		for (int idx = 0; idx < _nbr_pot; ++idx)
		{
			const Flt      a   = _rv_val_a_arr [idx];
			const Flt      b   = _rv_val_b_arr [idx];
			const Flt      pos = _rv_pos_arr [idx];
			_dia_r_v [idx] = a * pos + b;
		}

		// (Rv + Q)^-1
		lal::vec_to_mat (_mat_r_v_q_lu, _dia_r_v, lal::Dir::D);
		lal::add (_mat_r_v_q_lu, _mat_r_v_q_lu, _mat_q);
		lal::decompose_lu (_mat_r_v_q_lu, _r_v_q_r_arr, _r_v_q_c_arr);
		lal::invert (
			_mat_r_v_q_inv, _r_v_q_y,
			_mat_r_v_q_lu, _r_v_q_r_arr, _r_v_q_c_arr
		);

		// Updates the main DK matrices
		lal::mul (_mat_abc_tmp, _mat_2zgxux, _mat_r_v_q_inv);
		lal::mul (_mat_def_tmp,    _mat_u_o, _mat_r_v_q_inv);
		lal::mul (_mat_ghk_tmp,    _mat_u_n, _mat_r_v_q_inv);
		lal::mul_transp_rhs (_mat_a, _mat_abc_tmp, _mat_u_x, lal::res_sub <Flt>);
		lal::mul_transp_rhs (_mat_b, _mat_abc_tmp, _mat_u_u, lal::res_sub <Flt>);
		lal::mul_transp_rhs (_mat_d, _mat_def_tmp, _mat_u_x, lal::res_sub <Flt>);
		lal::mul_transp_rhs (_mat_e, _mat_def_tmp, _mat_u_u, lal::res_sub <Flt>);
		lal::mul_transp_rhs (_mat_g, _mat_ghk_tmp, _mat_u_x, lal::res_sub <Flt>);
		lal::mul_transp_rhs (_mat_h, _mat_ghk_tmp, _mat_u_u, lal::res_sub <Flt>);
		if (! _linear_flag)
		{
			lal::mul_transp_rhs (_mat_c, _mat_abc_tmp, _mat_u_n, lal::res_sub <Flt>);
			lal::mul_transp_rhs (_mat_f, _mat_def_tmp, _mat_u_n, lal::res_sub <Flt>);
			lal::mul_transp_rhs (_mat_k, _mat_ghk_tmp, _mat_u_n, lal::res_sub <Flt>);
		}
	}

	_r_v_dirty_flag = false;
}



// Outputs _vec_i_n
// Starts with the previous _vec_v_n value
void	Simulator::solve_nl ()
{
	bool           cont_flag    = false;
	bool           res_chg_flag = false;
	int            it_cnt       = 0;
	do
	{
		compute_nl_data (it_cnt);

#if defined (mfx_dsp_va_dkm_Simulator_STATS) \
 && defined (mfx_dsp_va_dkm_Simulator_STATS_PIV)
		// Manual LU decomposition and traversal, to build statistics
		decompose_lu (_mat_j_r, _j_r_r_arr);
		auto           ib = _st._piv_map.insert (std::make_pair (_j_r_r_arr, 1));
		if (! ib.second)
		{
			++ ib.first->second;
		}
		traverse_lu (_vec_delta_x, _vec_r_neg, _mat_j_r, _j_r_r_arr, _j_r_y);
#else
		lal::decompose_lu (_mat_j_r, _j_r_r_arr, _j_r_c_arr);
		lal::traverse_lu (
			_vec_delta_x, _j_r_y, _vec_r_neg, _mat_j_r, _j_r_r_arr, _j_r_c_arr
		);
#endif

#if 1
		// Difference limiting. Helps convergence when the signal input is high
		// and the solution of the NL equations moves quickly.
		res_chg_flag = false;
		for (int k = 0; k < int (_vec_delta_x.size ()); ++k)
		{
			Flt            delta = _vec_delta_x [k];
			if (delta > _max_dif)
			{
				delta        = _max_dif;
				res_chg_flag = true;
			}
			else if (delta < -_max_dif)
			{
				delta        = -_max_dif;
				res_chg_flag = true;
			}
			_vec_delta_x [k] = delta;
		}
#endif

		const Flt      dif_abs = lal::norm_inf (_vec_delta_x);
		const Flt      dif_rel = lal::norm_inf (_vec_r_neg);

		lal::add (_vec_v_n, _vec_v_n, _vec_delta_x);
		++ it_cnt;

		cont_flag = (
			   dif_abs >= _maxres
			&& dif_rel >= _reltol
			&& it_cnt  <  _max_it
		);
	}
	while (cont_flag);

	// Fixes again i_n if delta has been modified
	if (res_chg_flag)
	{
		compute_nl_data (it_cnt);
	}

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	_st_nbr_it = it_cnt;
#endif //mfx_dsp_va_dkm_Simulator_STATS
}



// Fills _vec_i_n, _mat_j_f, _mat_j_r, _vec_r_neg
void	Simulator::compute_nl_data (int it_cnt)
{
	assert (it_cnt >= 0);

	_mat_j_f.set_zero ();

	for (int d_cnt = 0; d_cnt < int (_diode_arr.size ()); ++d_cnt)
	{
		compute_nl_data_diode (it_cnt, d_cnt);
	}
	for (int d_cnt = 0; d_cnt < int (_diode_pair_arr.size ()); ++d_cnt)
	{
		compute_nl_data_diode_pair (it_cnt, d_cnt);
	}
	for (int d_cnt = 0; d_cnt < int (_bjt_npn_arr.size ()); ++d_cnt)
	{
		compute_nl_data_bjt_npn (it_cnt, d_cnt);
	}

	// _vec_r_neg =   _mat_k * _vec_i_n + _vec_v_n - _vec_p;
	lal::mul (_vec_r_neg_tmp1, _mat_k, _vec_i_n);
	lal::sub (_vec_r_neg_tmp2, _vec_v_n, _vec_p);
	lal::add (_vec_r_neg, _vec_r_neg_tmp1, _vec_r_neg_tmp2);
#if 0
	// _mat_j_r   = - _mat_k * _mat_j_f - _dia_id_n;
	lal::mul (_mat_j_r, mat_k, _mat_j_f);
	lal::add (_mat_j_r, _mat_j_r, _dia_id_n);
	lal::neg (_mat_j_r, _mat_j_r);
#else
	// Attempt to optimize the multiplication with the sparse _mat_j_f
	const int      ofs = int (_diode_arr.size () + _diode_pair_arr.size ());
	const Flt *    data_k_ptr   = _mat_k.get_data ();
	const int      stride_k     = _mat_k.get_stride ();
	const Flt *    data_j_f_ptr = _mat_j_f.get_data ();
	const int      stride_j_f   = _mat_j_f.get_stride ();
	Flt *          data_j_r_ptr = _mat_j_r.get_data ();
	const int      stride_j_r   = _mat_j_r.get_stride ();
	for (int d = 0; d < ofs; ++d)
	{
		const Flt      j_f_d_d_neg = -_mat_j_f (d, d);
		const Flt *    k_i_d_ptr   = data_k_ptr + d;
		Flt *          j_r_i_d_ptr = data_j_r_ptr + d;
		for (int i = 0; i < _mat_j_r.get_rows (); ++i)
		{
			*j_r_i_d_ptr = *k_i_d_ptr * j_f_d_d_neg;
			k_i_d_ptr   += stride_k;
			j_r_i_d_ptr += stride_j_r;
		}
		_mat_j_r (d, d) -= 1;
	}
	for (int d_cnt = 0; d_cnt < int (_bjt_npn_arr.size ()); ++d_cnt)
	{
		const int      d = ofs + d_cnt * 2;
		const Flt *    j_f_d_d_ptr   = data_j_f_ptr + d * stride_j_f + d;
		const Flt      j_f_d0_d0_neg = -j_f_d_d_ptr [0];
		const Flt      j_f_d0_d1_neg = -j_f_d_d_ptr [1];
		const Flt      j_f_d1_d0_neg = -j_f_d_d_ptr [stride_j_f    ];
		const Flt      j_f_d1_d1_neg = -j_f_d_d_ptr [stride_j_f + 1];
		const Flt *    k_i_d_ptr     = data_k_ptr + d;
		Flt *          j_r_i_d_ptr   = data_j_r_ptr + d;
		for (int i = 0; i < _mat_j_r.get_rows (); ++i)
		{
			const Flt      k_i_d0 = k_i_d_ptr [0];
			const Flt      k_i_d1 = k_i_d_ptr [1];
			j_r_i_d_ptr [0] =
				  k_i_d0 * j_f_d0_d0_neg
				+ k_i_d1 * j_f_d1_d0_neg;
			j_r_i_d_ptr [1] =
				  k_i_d0 * j_f_d0_d1_neg
				+ k_i_d1 * j_f_d1_d1_neg;
			k_i_d_ptr    += stride_k;
			j_r_i_d_ptr  += stride_j_r;
		}
		_mat_j_r (d    , d    ) -= 1;
		_mat_j_r (d + 1, d + 1) -= 1;
	}
#endif
}



void	Simulator::compute_nl_data_diode (int it_cnt, int idx_d)
{
	assert (idx_d >= 0);
	assert (idx_d < int (_diode_arr.size ()));

	Diode &        elt    = _diode_arr [idx_d];
	const int      idx_n  = elt._base_idx;
	const Flt      v      = _vec_v_n [idx_n];

	JuncDataType   i;
	JuncDataType   di;
	compute_nl_data_junction (i, di, v, elt._junc, it_cnt);

	_vec_i_n [idx_n]        = Flt (i );
	_mat_j_f (idx_n, idx_n) = Flt (di);
}



void	Simulator::compute_nl_data_diode_pair (int it_cnt, int idx_d)
{
	assert (idx_d >= 0);
	assert (idx_d < int (_diode_pair_arr.size ()));

	DiodeAntipar & elt    = _diode_pair_arr [idx_d];
	const int      idx_n  = elt._base_idx;
	const Flt      v      = _vec_v_n [idx_n];
	Flt            v_abs  = v;
	int            dir    = 0;
	if (v < 0)
	{
		v_abs = -v_abs;
		dir   = 1;
	}

	JuncDataType   i;
	JuncDataType   di;
	compute_nl_data_junction (i, di, v_abs, elt._dir_arr [dir], it_cnt);
	if (v < 0)
	{
		i = -i;
	}

	_vec_i_n [idx_n]        = Flt (i );
	_mat_j_f (idx_n, idx_n) = Flt (di);
}



void	Simulator::compute_nl_data_bjt_npn (int it_cnt, int idx_d)
{
	assert (idx_d >= 0);
	assert (idx_d < int (_bjt_npn_arr.size ()));

	BjtNpn &       elt    = _bjt_npn_arr [idx_d];
	const int      idx_n  = elt._base_idx;
	const Flt      vbe    = _vec_v_n [idx_n    ];
	const Flt      vbc    = _vec_v_n [idx_n + 1];

	JuncDataType   ide;
	JuncDataType   dide;
	compute_nl_data_junction (ide, dide, vbe, elt._junc_be, it_cnt);
	JuncDataType   idc;
	JuncDataType   didc;
	compute_nl_data_junction (idc, didc, vbc, elt._junc_bc, it_cnt);

	const JuncDataType   afi = elt._alpha_f_inv;
	const JuncDataType   ari = elt._alpha_r_inv;

	const JuncDataType   ibe = ide * afi - idc;
	const JuncDataType   ibc = idc * ari - ide;

	const JuncDataType   die_dvbe = dide * afi;
	const JuncDataType   die_dvbc = -didc;
	const JuncDataType   dic_dvbe = -dide;
	const JuncDataType   dic_dvbc = didc * ari;

	_vec_i_n [idx_n    ]            = Flt (ibe     );
	_vec_i_n [idx_n + 1]            = Flt (ibc     );
	_mat_j_f (idx_n,     idx_n)     = Flt (die_dvbe);
	_mat_j_f (idx_n,     idx_n + 1) = Flt (die_dvbc);
	_mat_j_f (idx_n + 1, idx_n)     = Flt (dic_dvbe);
	_mat_j_f (idx_n + 1, idx_n + 1) = Flt (dic_dvbc);
}



void	Simulator::compute_nl_data_junction (JuncDataType &i, JuncDataType &di, JuncDataType v, const Junction &junc, int it_cnt)
{
	fstb::unused (it_cnt);

	const JuncDataType   is = junc._is;
	const JuncDataType   va = v * junc._mul_v;
	const JuncDataType   e  =
		(va <= -127.f) ? 0.f : is * fstb::Approx::exp2_5th (float (va));

	i  = e - is;

	// When v is negative, di/dv is very tiny. So we use the "line through the
	// origin" method to avoid too small or null derivative.
	di = (v < 0) ? i / v : e * junc._nvt_inv;
}



// In-place LU decomposition.
// On output, lu contains both the L and U matrices, but L is without the
// identity diagonal row. The rows are kept at their original location.
// r is the reordering vector indicating the order of the matrix rows for up
// and down traversal.
void	Simulator::decompose_lu (TypeMatrixRm &lu, std::vector <int> &r)
{
	const int      n = int (lu.get_rows ());
	assert (int (lu.get_cols ()) == n);
	assert (int (r.size ()) == n);

	// Sets up the reordering vector
	for (int k = 0; k < n; ++ k)
	{
		r [k] = k;
	}

	Flt *          data_ptr = lu.get_data ();
	const int      stride   = lu.get_stride ();

	for (int k = 0; k < n - 1; ++k)
	{
		// Finds a suitable pivot
		int         idx = k;
		double      mag = fabs (lu (r [k], k));
		for (int j = k + 1; j < n; ++j)
		{
			const double mag_tst = fabs (lu (r [j], k));
			if (mag_tst > mag)
			{
				mag = mag_tst;
				idx = j;
			}
		}
		assert (mag > 0);

		// Swaps rows to put the pivot on k
		std::swap (r [k], r [idx]);

		// Subtracts the other rows
		const int       r_k     = r [k];
		Flt *           r_k_ptr = data_ptr + r_k * stride;
		const double    ukk_inv = 1. / r_k_ptr [k];
		for (int j = k + 1; j < n; ++j)
		{
			const int       r_j     = r [j];
			Flt *           r_j_ptr = data_ptr + r_j * stride;

			// L
			const double    ljk     = r_j_ptr [k] * ukk_inv;
			r_j_ptr [k] = ljk;

			// U
			for (int d = k + 1; d < n; ++d)
			{
				r_j_ptr [d] -= ljk * r_k_ptr [d];
			}
		}
	}
}



// Solves lu * x = b
// x  = unknown vector
// b  = right-hand side of the equation
// lu = main matrix, in an LU-decomposed form (see decompose_lu)
// r  = row-reordering indexes
// y  = temporary vector
void	Simulator::traverse_lu (TypeVector &x, const TypeVector &b, const TypeMatrixRm &lu, const std::vector <int> &r, TypeVector &y)
{
	const int      n = int (b.size ());
	assert (int (x.size ()) == n);
	assert (lu.get_rows () == n);
	assert (lu.get_cols () == n);
	assert (int (r.size ()) == n);
	assert (int (y.size ()) == n);

	const Flt *    data_ptr = lu.get_data ();
	const int      stride   = lu.get_stride ();

	// Down: L * y = b
	for (int i = 0; i < n; ++i)
	{
		const int      r_i     = r [i];
		const Flt *    r_i_ptr = data_ptr + r_i * stride;
		double         s       = b [r_i];
		for (int j = 0; j < i; ++j)
		{
			s -= y [j] * r_i_ptr [j];
		}
		y [i] = s;
	}

	// Up: U * x = y
	for (int i = n - 1; i >= 0; --i)
	{
		const int      r_i     = r [i];
		const Flt *    r_i_ptr = data_ptr + r_i * stride;
		double         s       = y [i];
		for (int j = i + 1; j < n; ++j)
		{
			s -= x [j] * r_i_ptr [j];
		}
		assert (r_i_ptr [i] != 0);
		x [i] = s / r_i_ptr [i];
	}
}



template <typename D>
void	Simulator::mul_oim (D &dst, D &tmp, const TypeMatrix &lhs, const TypeVector &vec, const TypeMatrix &rhs)
{
	lal::mul_transp_lhs_dia (tmp, lhs, vec);
	lal::mul (dst, tmp, rhs);
}



#if defined (mfx_dsp_va_dkm_Simulator_DISPLAY)

std::string	Simulator::print_vector (const TypeVector &v, const char *name_0)
{
	std::string    txt;

	txt += name_0;
	txt += " = ";
	const int      len = int (v.size ());
	if (len == 0)
	{
		txt += "<null>";
	}
	else
	{
		txt += "(";
		char           txt_0 [1023+1];
		for (int i = 0; i < len; ++i)
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0),
				"%10g%s", v [i], (i == len - 1) ? ")" : "\t"
			);
			txt += txt_0;
		}
	}
	txt += "\n";

	return txt;
}



std::string	Simulator::print_matrix (const TypeMatrix &m, const char *name_0)
{
	std::string    txt;

	const int      rows = m.get_rows ();
	const int      cols = m.get_cols ();
	txt += name_0;
	txt += " =";
	if (rows * cols == 0)
	{
		txt += " <null>\n";
	}
	else
	{
		txt += "\n";
		char           txt_0 [1023+1];
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
				fstb::snprintf4all (txt_0, sizeof (txt_0), "\t%10g", m (i, j));
				txt += txt_0;
			}
			if (cols > 0)
			{
				txt += "\n";
			}
		}
	}

	return txt;
}

#endif // mfx_dsp_va_dkm_Simulator_DISPLAY



void	Simulator::Junction::set_param (Flt is, Flt n, Flt imax)
{
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);
	assert (n >= 0);
	assert (imax > 0);

	_is      = is;
	_n       = n;
	_imax    = imax;
	_nvtl2   = compute_nvtl2 ();
	_mul_v   = compute_mul_v ();
	_nvt_inv = compute_nvt_inv ();
	_vmax    = compute_vmax ();
	_vcrit   = compute_vcrit ();
}



Flt	Simulator::Junction::compute_nvtl2 () const
{
	return Flt (_n * _vt * fstb::LN2);
}



Flt	Simulator::Junction::compute_mul_v () const
{
	return Flt (1. / (_n * _vt * fstb::LN2));
}



Flt	Simulator::Junction::compute_nvt_inv () const
{
	return Flt (1. / (_n * _vt));
}



Flt	Simulator::Junction::compute_vmax () const
{
	return _n * _vt * Flt (log (1 + _imax / _is));
}



// (3.55) from http://qucs.sourceforge.net/tech/node16.html
Flt	Simulator::Junction::compute_vcrit () const
{
	const Flt      nvt = _n * _vt;

	return Flt (nvt * log (nvt / (_is * Flt (fstb::SQRT2))));
}



}  // namespace dkm
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
