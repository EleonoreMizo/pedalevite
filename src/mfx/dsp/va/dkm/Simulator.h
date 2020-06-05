/*****************************************************************************

        Simulator.h
        Author: Laurent de Soras, 2020

Electronic circuit simulator implementing the Nodal DK method.

Main references:

Martin Holters, Udo Zolzer, Physical Modelling of a Wah-wah Effect Pedal as a
Case Study for Application of the Nodal DK Method to Circuits with Variable
Parts, Proceedings of the 14th International Conference on Digital Audio
Effects (DAFx-11), 2011

Jaromir Macak, Real-Time Digital Simulation of Guitar Amplifiers as Audio
Effects, doctoral thesis, Brno University of Technology, 2012

Piero Rivera Benois, Simulation Framework for Analog Audio Circuits based on
Nodal DK Method, Master Thesis, Helmut Schmidt Universitat, 2013

David T. Yeh, Jonathan S. Abel, Julius O. Smith III, Automated Physical
Modeling of Nonlinear Audio Circuits For Real-Time Audio Effects - Part I:
Theoretical Development, IEEE Transactions on Audio, Speech, and Language
Processing, vol. 18, no. 4, pp. 728-737, May 2010

David T. Yeh, Automated Physical Modeling of Nonlinear Audio Circuits For
Real-Time Audio Effects - Part II: BJT and Vacuum Tube Examples, IEEE
Transactions on Audio, Speech, and Language Processing, vol. 20, no. 4,
pp. 1207-1216, May 2012

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_dkm_Simulator_HEADER_INCLUDED)
#define mfx_dsp_va_dkm_Simulator_HEADER_INCLUDED



// Define this to enable convergence statistics. Very light overhead.
#define mfx_dsp_va_dkm_Simulator_STATS

// Enables matrix display functions for debugging
#undef mfx_dsp_va_dkm_Simulator_DISPLAY



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "Eigen/Dense"
#include "mfx/dsp/va/dkm/Flt.h"

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	#include <array>
#endif // mfx_dsp_va_dkm_Simulator_STATS
#include <map>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace dkm
{



class Simulator
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef int IdNode;
	static const IdNode  _nid_invalid = -1;
	static const IdNode  _nid_gnd     = 0;

	static const int     _limit_it    = 200;

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	typedef std::array <int, _limit_it      +   1> HistIt;
	typedef std::array <int, _limit_it * 16 + 100> HistEval;
	class Stats
	{
	public:
		HistIt         _hist_it = {{ 0 }}; // Histogram for the number of NR iterations
		int            _nbr_spl_proc = 0;  // Number of processed samples since the statistics start
	};
#endif // mfx_dsp_va_dkm_Simulator_STATS

	int            add_src_v (IdNode nid_1, IdNode nid_2, Flt v);
	int            add_resistor (IdNode nid_1, IdNode nid_2, Flt r);
	int            add_pot (IdNode nid_1, IdNode nid_2, Flt rmin, Flt rmax, Flt pos);
	int            add_capacitor (IdNode nid_1, IdNode nid_2, Flt c);
	int            add_inductor (IdNode nid_1, IdNode nid_2, Flt l);
	int            add_diode (IdNode nid_1, IdNode nid_2, Flt is, Flt n);
	int            add_diode_antipar (IdNode nid_1, IdNode nid_2, Flt is1, Flt n1, Flt is2, Flt n2);
	int            add_bjt_npn (IdNode nid_e, IdNode nid_b, IdNode nid_c, Flt is, Flt n, Flt beta_f, Flt beta_r);
	int            add_output (IdNode nid_1, IdNode nid_2);

	void           prepare (double sample_freq);

	void           set_max_nbr_it (int max_it);
	void           set_src_v (int idx, Flt v);
	void           set_pot (int idx, Flt pos);
	void           process_sample ();
	Flt            get_output (int idx) const;

	void           clear_buffers ();

	int            get_nbr_nodes () const;
	int            get_nbr_src_v () const;

#if defined (mfx_dsp_va_dkm_Simulator_DISPLAY)
	void           print_all () const;
#endif // mfx_dsp_va_dkm_Simulator_DISPLAY

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	void           reset_stats ();
	void           get_stats (Stats &stats) const;
#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef Eigen::Matrix <Flt, Eigen::Dynamic, Eigen::Dynamic> TypeMatrix;
	typedef Eigen::Matrix <Flt, Eigen::Dynamic, 1> TypeVector;
	typedef TypeMatrix TypeDiagonal;

	typedef Flt JuncDataType;

	static const int  _idx_gnd = -666999666;

	class SrcV
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		Flt            _v;
	};

	class Resistor
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		Flt            _r;
	};

	class Potentiometer
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		Flt            _rmin;
		Flt            _rmax;
		Flt            _pos;
	};

	class Capacitor
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		Flt            _c;
		int            _base_idx = -1;
	};

	class Inductor
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		Flt            _l;
		int            _base_idx = -1;
	};

	class Junction
	{
	public:
		void           set_param (Flt is, Flt n, Flt imax);
		Flt            compute_nvtl2 () const;
		Flt            compute_mul_v () const;
		Flt            compute_nvt_inv () const;
		Flt            compute_vmax () const;
		Flt            compute_vcrit () const;
		Flt            _is      = Flt (1e-15);
		Flt            _n       = Flt (1);
		Flt            _imax    = Flt (100);   // Maximum current through the junction
		const Flt      _vt      = Flt (0.026); // Thermal voltage, volt
		Flt            _nvtl2   = compute_nvtl2 ();  // Precalculated stuff
		Flt            _mul_v   = compute_mul_v ();
		Flt            _nvt_inv = compute_nvt_inv ();
		Flt            _vmax    = compute_vmax ();
		Flt            _vcrit   = compute_vcrit ();
	};

	class Diode
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		int            _base_idx = -1;
		int            _idx_1    = -1;
		int            _idx_2    = -1;
		Junction       _junc;
	};

	class DiodeAntipar
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
		int            _base_idx = -1;
		int            _idx_1    = -1;
		int            _idx_2    = -1;
		std::array <Junction, 2>
		               _dir_arr;
	};

	class BjtNpn
	{
	public:
		IdNode         _nid_e;
		IdNode         _nid_b;
		IdNode         _nid_c;
		Flt            _beta_f;
		Flt            _beta_r;
		int            _base_idx = -1;
		int            _idx_e    = -1;
		int            _idx_b    = -1;
		int            _idx_c    = -1;
		Flt            _alpha_f_inv;
		Flt            _alpha_r_inv;
		Junction       _junc_be;
		Junction       _junc_bc;
	};

	class Output
	{
	public:
		IdNode         _nid_1;
		IdNode         _nid_2;
	};

	void           build_matrices ();
	void           resize_and_clear_mat_vec ();
	void           setup_src_v ();
	void           setup_res ();
	void           setup_pot ();
	void           setup_ese ();
	void           setup_nl ();
	void           setup_outputs ();
	void           add_oim_entry (TypeMatrix &m, int row, IdNode nid_1, IdNode nid_2);
	int            use_or_create_node (IdNode nid);
	int            use_node (IdNode nid) const;
	void           build_s_0_inv ();
	void           prepare_dk_const_matrices ();
	void           update_r_v ();
	void           solve_nl ();
	void           compute_nl_data (int it_cnt);
	void           compute_nl_data_diode (int it_cnt, int idx_d);
	void           compute_nl_data_diode_pair (int it_cnt, int idx_d);
	void           compute_nl_data_bjt_npn (int it_cnt, int idx_d);
	inline void    compute_nl_data_junction (JuncDataType &i, JuncDataType &di, JuncDataType v, const Junction &junc, int it_cnt);

#if defined (mfx_dsp_va_dkm_Simulator_DISPLAY)
	static void    print_vector (const TypeVector &v, const char *name_0);
	static void    print_matrix (const TypeMatrix &m, const char *name_0);
#endif // mfx_dsp_va_dkm_Simulator_DISPLAY

	Flt            _max_dif     = Flt (0.25);

	Flt            _sample_freq = 0;    // Hz, > 0. 0 = not initialised
	Flt            _reltol      = Flt (1e-3);
	Flt            _maxres      = Flt (1e-6);
	int            _max_it      = _limit_it;  // Maximum number of NR iterations. ]0 ; _limit_it]
	bool           _linear_flag = false;      // Circuit contains only linear parts

	std::map <IdNode, int>        // Node Id -> index
	               _nid_to_idx_map;

	// Parts
	std::vector <SrcV>
	               _src_v_arr;
	std::vector <Resistor>
	               _resistor_arr;
	std::vector <Potentiometer>
	               _pot_arr;
	std::vector <Capacitor>
	               _capa_arr;
	std::vector <Inductor>
	               _inductor_arr;
	std::vector <Diode>
	               _diode_arr;
	std::vector <DiodeAntipar>
	               _diode_pair_arr;
	std::vector <BjtNpn>
	               _bjt_npn_arr;
	std::vector <Output>
	               _output_arr;

	// Dimensions
	int            _nbr_nodes = 0;
	int            _nbr_res   = 0;
	int            _nbr_pot   = 0;
	int            _nbr_ese   = 0;   // Energy-storage elements (ESE)
	int            _nbr_nl    = 0;
	int            _nbr_src_v = 0;
	int            _nbr_out   = 0;

	// Main DK vectors
	TypeVector     _vec_x_cur;    // ESE states, current time step
	TypeVector     _vec_x_prv;    // ESE states, previous time step
	TypeVector     _vec_u;        // Voltage sources
	TypeVector     _vec_v_n;      // Voltages at the non-linear elements
	TypeVector     _vec_i_n;      // Currents through the non-linear elements (it is the standard direction, not following the DK-method usual direction. All equations implying i must change its sign.)
	TypeVector     _vec_v_o;      // Output voltages
	TypeVector     _vec_p;        // Input for the non-linear solving

	// Main DK matrices
	TypeMatrix     _mat_a;        // states      -> states
	TypeMatrix     _mat_b;        // voltage src -> states
	TypeMatrix     _mat_c;        // NL currents -> states

	TypeMatrix     _mat_d;        // states      -> outputs
	TypeMatrix     _mat_e;        // voltage src -> outputs
	TypeMatrix     _mat_f;        // NL currents -> outputs

	TypeMatrix     _mat_g;        // states      -> NL voltages
	TypeMatrix     _mat_h;        // voltage src -> NL voltages
	TypeMatrix     _mat_k;        // NL currents -> NL voltages

	// Parameters
	TypeDiagonal   _dia_g_r;      // Diagonal, conductances for the fixed resistors
	TypeDiagonal   _dia_g_x;      // Diagonal, ESE values: 2C/T or T/(2L)
	TypeDiagonal   _dia_z;        // Diagonal, type of the ESEs: +1 for C, -1 for L
	TypeDiagonal   _dia_r_v;      // Diagonal, potentiometer final values
	std::vector <Flt>             // Potentiometer base values: R = a * pos + b
	               _rv_val_a_arr;
	std::vector <Flt>
	               _rv_val_b_arr;
	std::vector <Flt>
	               _rv_pos_arr;

	// Oriented incidence matrices
	// Same width for all: _nbr_nodes
	TypeMatrix     _mat_n_x;
	TypeMatrix     _mat_n_o;
	TypeMatrix     _mat_n_n;
	TypeMatrix     _mat_n_u;
	TypeMatrix     _mat_n_v;
	TypeMatrix     _mat_n_r;

	// Intermediate calculations
	TypeMatrix     _mat_s_0;
	TypeMatrix     _mat_s_0_inv;
	TypeMatrix     _mat_q;
	TypeMatrix     _mat_r_v_q_inv;// (Rv + Q)^-1
	TypeMatrix     _mat_a_0;
	TypeMatrix     _mat_b_0;
	TypeMatrix     _mat_c_0;
	TypeMatrix     _mat_d_0;
	TypeMatrix     _mat_e_0;
	TypeMatrix     _mat_f_0;
	TypeMatrix     _mat_g_0;
	TypeMatrix     _mat_h_0;
	TypeMatrix     _mat_k_0;
	TypeMatrix     _mat_u_x;
	TypeMatrix     _mat_u_o;
	TypeMatrix     _mat_u_n;
	TypeMatrix     _mat_u_u;
	TypeMatrix     _mat_j_f;
	TypeMatrix     _mat_j_r;
	TypeMatrix     _dia_id_n;
	TypeVector     _vec_r;
	TypeVector     _vec_delta_x;
	TypeMatrix     _mat_abc_tmp;
	TypeMatrix     _mat_def_tmp;
	TypeMatrix     _mat_ghk_tmp;
	TypeMatrix     _mat_2zgxux;      // 2 * Z * Gx * Ux
	TypeMatrix     _mat_abc_0_tmp1;  // 2 * Z * Gx * (Nx 0)
	TypeMatrix     _mat_abc_0_tmp2;  // tmp1 * S0^-1
	TypeMatrix     _mat_def_0_tmp;   // (No 0) * S0^-1
	TypeMatrix     _mat_ghk_0_tmp;   // (Nn 0) * S0^-1
	TypeMatrix     _mat_u_tmp;       // S0^-1 * (Nv 0)T
	Eigen::PartialPivLU <TypeMatrix>
	               _decomp_delta_x;

	bool           _r_v_dirty_flag = true; // Indicates _mat_r_v and dependencies must be recomputed

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	Stats          _st;
	int            _st_nbr_it = 0;
#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Simulator &other) const = delete;
	bool           operator != (const Simulator &other) const = delete;

}; // class Simulator



}  // namespace dkm
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/dkm/Simulator.hpp"



#endif   // mfx_dsp_va_dkm_Simulator_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
