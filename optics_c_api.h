#pragma once

// ═══════════════════════════════════════════════════════════════════════════════
//  C API wrappers for the OpticsPhysics library.
//
//  Usage:
//    OP_Handle h = op_create("HeNe", 632.8e-9, 1.0);
//    double theta = op_snell_refraction(h, 0.5, 1.0, 1.5);
//    op_destroy(h);
// ═══════════════════════════════════════════════════════════════════════════════

#include "OpticsExport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* OP_Handle;

// ── Lifecycle ───────────────────────────────────────────────────────────────
OPTICS_API OP_Handle op_create(const char* name, double wavelength, double n_medium);
OPTICS_API void      op_destroy(OP_Handle handle);

// ── Getters ─────────────────────────────────────────────────────────────────
OPTICS_API double op_get_wavelength(OP_Handle handle);
OPTICS_API double op_get_n_medium(OP_Handle handle);

// ── 1: Snell's Law ──────────────────────────────────────────────────────────
OPTICS_API double op_snell_refraction(OP_Handle h, double theta_i, double n1, double n2);
OPTICS_API double op_critical_angle(OP_Handle h, double n1, double n2);
OPTICS_API double op_brewster_angle(OP_Handle h, double n1, double n2);
OPTICS_API double op_fresnel_rs(OP_Handle h, double theta_i, double n1, double n2);
OPTICS_API double op_fresnel_rp(OP_Handle h, double theta_i, double n1, double n2);

// ── 2: Thin Lens ────────────────────────────────────────────────────────────
OPTICS_API double op_thin_lens(OP_Handle h, double obj_dist, double f);
OPTICS_API double op_magnification(OP_Handle h, double obj_dist, double img_dist);

// ── 3: Thick Lens ───────────────────────────────────────────────────────────
OPTICS_API double op_thick_lens_focal_length(OP_Handle h, double n_lens, double R1, double R2, double d);

// ── 4: Mirror Optics ────────────────────────────────────────────────────────
OPTICS_API double op_spherical_mirror_focal(OP_Handle h, double R);
OPTICS_API double op_mirror_equation(OP_Handle h, double obj_dist, double f);

// ── 6: Prism Optics ────────────────────────────────────────────────────────
OPTICS_API double op_prism_deviation(OP_Handle h, double theta_i, double n_prism, double apex);
OPTICS_API double op_prism_min_deviation(OP_Handle h, double n_prism, double apex);

// ── 7: Aberrations ─────────────────────────────────────────────────────────
OPTICS_API double op_spherical_aberration(OP_Handle h, double n, double R, double height);
OPTICS_API double op_chromatic_aberration(OP_Handle h, double f, double V);

// ── 8: Optical Instruments ──────────────────────────────────────────────────
OPTICS_API double op_telescope_magnification(OP_Handle h, double f_obj, double f_eye);
OPTICS_API double op_microscope_na(OP_Handle h, double n, double alpha);
OPTICS_API double op_rayleigh_criterion(OP_Handle h, double lambda, double NA);

// ── 9: Fiber Optics ────────────────────────────────────────────────────────
OPTICS_API double op_fiber_na(OP_Handle h, double n_core, double n_clad);
OPTICS_API double op_fiber_v_number(OP_Handle h, double a, double NA, double lambda);
OPTICS_API int    op_fiber_num_modes(OP_Handle h, double V);

// ── 10: Fermat / Eikonal ───────────────────────────────────────────────────
OPTICS_API double op_optical_path_length(OP_Handle h, double n, double d);
OPTICS_API double op_eikonal_phase(OP_Handle h, double OPL, double lambda);

// ── 11: Single Slit ─────────────────────────────────────────────────────────
OPTICS_API double op_single_slit_intensity(OP_Handle h, double theta, double a, double lambda);

// ── 12: Double Slit ─────────────────────────────────────────────────────────
OPTICS_API double op_double_slit_intensity(OP_Handle h, double theta, double a, double d, double lambda);

// ── 13: Diffraction Grating ─────────────────────────────────────────────────
OPTICS_API double op_grating_intensity(OP_Handle h, double theta, double a, double d, double lambda, int N);
OPTICS_API double op_grating_resolving_power(OP_Handle h, int m, int N);

// ── 14: Airy Pattern ───────────────────────────────────────────────────────
OPTICS_API double op_airy_intensity(OP_Handle h, double theta, double D, double lambda);
OPTICS_API double op_airy_disk_radius(OP_Handle h, double lambda, double f, double D);

// ── 15: Fresnel Diffraction ─────────────────────────────────────────────────
OPTICS_API double op_fresnel_diffraction_edge(OP_Handle h, double x, double lambda, double z);
OPTICS_API int    op_fresnel_number(OP_Handle h, double a, double lambda, double z);

// ── 16: Thin Film ───────────────────────────────────────────────────────────
OPTICS_API double op_thin_film_reflectance(OP_Handle h, double n1, double nf, double n2, double d, double lambda, double theta);
OPTICS_API double op_anti_reflection_thickness(OP_Handle h, double nf, double lambda);

// ── 17: Michelson ───────────────────────────────────────────────────────────
OPTICS_API double op_michelson_intensity(OP_Handle h, double delta_d, double lambda);
OPTICS_API double op_michelson_visibility(OP_Handle h, double I_max, double I_min);

// ── 18: Fabry-Perot ─────────────────────────────────────────────────────────
OPTICS_API double op_fabry_perot_transmission(OP_Handle h, double delta, double F);
OPTICS_API double op_fabry_perot_finesse(OP_Handle h, double R);
OPTICS_API double op_fabry_perot_fsr(OP_Handle h, double d, double n);

// ── 19: Coherence ───────────────────────────────────────────────────────────
OPTICS_API double op_temporal_coherence_length(OP_Handle h, double lambda, double delta_lambda);
OPTICS_API double op_spatial_coherence_width(OP_Handle h, double lambda, double theta_source);
OPTICS_API double op_degree_of_coherence(OP_Handle h, double tau, double tau_c);

// ── 22: Talbot Effect ───────────────────────────────────────────────────────
OPTICS_API double op_talbot_distance(OP_Handle h, double d, double lambda);

// ── 26: Malus's Law ────────────────────────────────────────────────────────
OPTICS_API double op_malus_law(OP_Handle h, double I0, double theta);

// ── 27: Birefringence ───────────────────────────────────────────────────────
OPTICS_API double op_birefringent_phase_delay(OP_Handle h, double delta_n, double d, double lambda);
OPTICS_API double op_walkoff_angle(OP_Handle h, double n_e, double n_o, double theta);

// ── 28: Optical Activity ────────────────────────────────────────────────────
OPTICS_API double op_optical_rotation(OP_Handle h, double spec_rot, double conc, double path);
OPTICS_API double op_faraday_rotation(OP_Handle h, double V, double B, double L);

// ── 31: Gaussian Beam ───────────────────────────────────────────────────────
OPTICS_API double op_gaussian_waist(OP_Handle h, double w0, double z, double lambda);
OPTICS_API double op_gaussian_rayleigh(OP_Handle h, double w0, double lambda);
OPTICS_API double op_gaussian_divergence(OP_Handle h, double w0, double lambda);
OPTICS_API double op_gaussian_gouy_phase(OP_Handle h, double z, double zR);
OPTICS_API double op_gaussian_intensity(OP_Handle h, double r, double w);

// ── 35: Laser Rate Equations ────────────────────────────────────────────────
OPTICS_API double op_laser_threshold_gain(OP_Handle h, double losses, double L_cavity);
OPTICS_API double op_laser_slope_efficiency(OP_Handle h, double sigma_e, double tau_c, double V_mode, double h_nu);

// ── 36: Cavity Modes ───────────────────────────────────────────────────────
OPTICS_API double op_cavity_fsr(OP_Handle h, double L, double n);
OPTICS_API double op_cavity_stability(OP_Handle h, double L, double R1, double R2);

// ── 37: Q-Switch / Mode-Lock ────────────────────────────────────────────────
OPTICS_API double op_mode_lock_rep_rate(OP_Handle h, double L, double n);
OPTICS_API double op_mode_lock_pulse_duration(OP_Handle h, double delta_nu);

// ── 38: SHG ─────────────────────────────────────────────────────────────────
OPTICS_API double op_shg_phase_mismatch(OP_Handle h, double n_w, double n_2w, double lambda);
OPTICS_API double op_shg_coherence_length(OP_Handle h, double n_w, double n_2w, double lambda);

// ── 39: Kerr Effect ─────────────────────────────────────────────────────────
OPTICS_API double op_kerr_index(OP_Handle h, double n0, double n2, double I);
OPTICS_API double op_b_integral(OP_Handle h, double n2, double I, double L, double lambda);
OPTICS_API double op_critical_power(OP_Handle h, double lambda, double n0, double n2);

// ── 40: OPA ─────────────────────────────────────────────────────────────────
OPTICS_API double op_opa_gain(OP_Handle h, double gamma, double L, double delta_k);
OPTICS_API double op_opa_idler_wavelength(OP_Handle h, double lambda_pump, double lambda_signal);

// ── 41: Electro-Optic ───────────────────────────────────────────────────────
OPTICS_API double op_pockels_phase_shift(OP_Handle h, double n_e, double r, double V, double lambda, double d);
OPTICS_API double op_half_wave_voltage(OP_Handle h, double lambda, double n_e, double r, double d);

// ── 42: Acousto-Optic ───────────────────────────────────────────────────────
OPTICS_API double op_bragg_angle(OP_Handle h, double lambda, double lambda_sound);
OPTICS_API double op_ao_frequency_shift(OP_Handle h, double f_acoustic);

// ── 43: Photon Statistics ───────────────────────────────────────────────────
OPTICS_API double op_poisson_distribution(double n_mean, int n);
OPTICS_API double op_thermal_distribution(double n_mean, int n);
OPTICS_API double op_mandel_q(OP_Handle h, double variance, double n_mean);

// ── 44: HBT ─────────────────────────────────────────────────────────────────
OPTICS_API double op_hbt_g2_thermal(OP_Handle h, double tau, double tau_c);

// ── 45: Beam Splitter / HOM ─────────────────────────────────────────────────
OPTICS_API double op_hom_dip(OP_Handle h, double tau, double tau_c);

// ── 46: Mach-Zehnder ────────────────────────────────────────────────────────
OPTICS_API double op_mz_output1(OP_Handle h, double phi);
OPTICS_API double op_mz_output2(OP_Handle h, double phi);
OPTICS_API double op_mz_sensitivity(OP_Handle h, double phi);

// ── 47: Squeezed Light ──────────────────────────────────────────────────────
OPTICS_API double op_squeezed_variance_x(OP_Handle h, double r);
OPTICS_API double op_squeezed_variance_p(OP_Handle h, double r);
OPTICS_API double op_squeezed_db(OP_Handle h, double r);

// ── 48: BB84 QKD ────────────────────────────────────────────────────────────
OPTICS_API double op_bb84_qber(OP_Handle h, double e_det, double e_chan);
OPTICS_API double op_bb84_secure_key_rate(OP_Handle h, double QBER, double f_err);

// ── 49: Plasmonics ──────────────────────────────────────────────────────────
OPTICS_API double op_spp_wavevector(OP_Handle h, double omega, double eps_m_re, double eps_d);
OPTICS_API double op_spr_resonance_angle(OP_Handle h, double n_prism, double eps_m_re, double eps_d);

// ── 50: Metamaterials ───────────────────────────────────────────────────────
OPTICS_API double op_metamaterial_epsilon(OP_Handle h, double omega, double omega_p, double gamma);
OPTICS_API double op_metamaterial_mu(OP_Handle h, double omega, double omega_m, double F, double gamma_m);
OPTICS_API double op_metamaterial_n(OP_Handle h, double epsilon, double mu);
OPTICS_API double op_negative_refraction(OP_Handle h, double theta_i, double n_pos, double n_neg);

#ifdef __cplusplus
}
#endif
