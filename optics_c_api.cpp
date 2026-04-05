#include "optics_c_api.h"
#include "OpticalSystem.h"

static OpticalSystem* op(OP_Handle h) { return static_cast<OpticalSystem*>(h); }

// ── Lifecycle ───────────────────────────────────────────────────────────────
OP_Handle op_create(const char* name, double wavelength, double n_medium) {
    return new OpticalSystem(name ? name : "", wavelength, n_medium);
}
void op_destroy(OP_Handle handle) { delete op(handle); }

// ── Getters ─────────────────────────────────────────────────────────────────
double op_get_wavelength(OP_Handle h) { return op(h)->wavelength; }
double op_get_n_medium(OP_Handle h)   { return op(h)->n_medium; }

// ── 1: Snell ────────────────────────────────────────────────────────────────
double op_snell_refraction(OP_Handle h, double ti, double n1, double n2) { return op(h)->snellRefraction(ti, n1, n2); }
double op_critical_angle(OP_Handle h, double n1, double n2) { return op(h)->criticalAngle(n1, n2); }
double op_brewster_angle(OP_Handle h, double n1, double n2) { return op(h)->brewsterAngle(n1, n2); }
double op_fresnel_rs(OP_Handle h, double ti, double n1, double n2) { return op(h)->fresnelRs(ti, n1, n2); }
double op_fresnel_rp(OP_Handle h, double ti, double n1, double n2) { return op(h)->fresnelRp(ti, n1, n2); }

// ── 2: Thin Lens ────────────────────────────────────────────────────────────
double op_thin_lens(OP_Handle h, double od, double f) { return op(h)->thinLensEquation(od, f); }
double op_magnification(OP_Handle h, double od, double id) { return op(h)->magnification(od, id); }

// ── 3: Thick Lens ───────────────────────────────────────────────────────────
double op_thick_lens_focal_length(OP_Handle h, double nl, double R1, double R2, double d) { return op(h)->thickLensFocalLength(nl, R1, R2, d); }

// ── 4: Mirror ───────────────────────────────────────────────────────────────
double op_spherical_mirror_focal(OP_Handle h, double R) { return op(h)->sphericalMirrorFocal(R); }
double op_mirror_equation(OP_Handle h, double od, double f) { return op(h)->mirrorEquation(od, f); }

// ── 6: Prism ────────────────────────────────────────────────────────────────
double op_prism_deviation(OP_Handle h, double ti, double np, double apex) { return op(h)->prismDeviation(ti, np, apex); }
double op_prism_min_deviation(OP_Handle h, double np, double apex) { return op(h)->prismMinDeviation(np, apex); }

// ── 7: Aberrations ──────────────────────────────────────────────────────────
double op_spherical_aberration(OP_Handle h, double n, double R, double ht) { return op(h)->sphericalAberrationCoeff(n, R, ht); }
double op_chromatic_aberration(OP_Handle h, double f, double V) { return op(h)->chromaticAberrationLongitudinal(f, V); }

// ── 8: Instruments ──────────────────────────────────────────────────────────
double op_telescope_magnification(OP_Handle h, double fo, double fe) { return op(h)->telescopeMagnification(fo, fe); }
double op_microscope_na(OP_Handle h, double n, double a) { return op(h)->microscopeNA(n, a); }
double op_rayleigh_criterion(OP_Handle h, double lam, double NA) { return op(h)->rayleighCriterion(lam, NA); }

// ── 9: Fiber ────────────────────────────────────────────────────────────────
double op_fiber_na(OP_Handle h, double nc, double ncl) { return op(h)->fiberNA(nc, ncl); }
double op_fiber_v_number(OP_Handle h, double a, double NA, double lam) { return op(h)->fiberVNumber(a, NA, lam); }
int    op_fiber_num_modes(OP_Handle h, double V) { return op(h)->fiberNumberOfModes(V); }

// ── 10: Fermat / Eikonal ────────────────────────────────────────────────────
double op_optical_path_length(OP_Handle h, double n, double d) { return op(h)->opticalPathLength(n, d); }
double op_eikonal_phase(OP_Handle h, double OPL, double lam) { return op(h)->eikonalPhase(OPL, lam); }

// ── 11: Single Slit ─────────────────────────────────────────────────────────
double op_single_slit_intensity(OP_Handle h, double t, double a, double l) { return op(h)->singleSlitIntensity(t, a, l); }

// ── 12: Double Slit ─────────────────────────────────────────────────────────
double op_double_slit_intensity(OP_Handle h, double t, double a, double d, double l) { return op(h)->doubleSlitIntensity(t, a, d, l); }

// ── 13: Grating ─────────────────────────────────────────────────────────────
double op_grating_intensity(OP_Handle h, double t, double a, double d, double l, int N) { return op(h)->gratingIntensity(t, a, d, l, N); }
double op_grating_resolving_power(OP_Handle h, int m, int N) { return op(h)->gratingResolvingPower(m, N); }

// ── 14: Airy ────────────────────────────────────────────────────────────────
double op_airy_intensity(OP_Handle h, double t, double D, double l) { return op(h)->airyIntensity(t, D, l); }
double op_airy_disk_radius(OP_Handle h, double l, double f, double D) { return op(h)->airyDiskRadius(l, f, D); }

// ── 15: Fresnel Diffraction ─────────────────────────────────────────────────
double op_fresnel_diffraction_edge(OP_Handle h, double x, double l, double z) { return op(h)->fresnelDiffractionEdge(x, l, z); }
int    op_fresnel_number(OP_Handle h, double a, double l, double z) { return op(h)->fresnelNumber(a, l, z); }

// ── 16: Thin Film ───────────────────────────────────────────────────────────
double op_thin_film_reflectance(OP_Handle h, double n1, double nf, double n2, double d, double l, double t) { return op(h)->thinFilmReflectance(n1, nf, n2, d, l, t); }
double op_anti_reflection_thickness(OP_Handle h, double nf, double l) { return op(h)->antiReflectionThickness(nf, l); }

// ── 17: Michelson ───────────────────────────────────────────────────────────
double op_michelson_intensity(OP_Handle h, double dd, double l) { return op(h)->michelsonIntensity(dd, l); }
double op_michelson_visibility(OP_Handle h, double Imax, double Imin) { return op(h)->michelsonVisibility(Imax, Imin); }

// ── 18: Fabry-Perot ─────────────────────────────────────────────────────────
double op_fabry_perot_transmission(OP_Handle h, double d, double F) { return op(h)->fabryPerotTransmission(d, F); }
double op_fabry_perot_finesse(OP_Handle h, double R) { return op(h)->fabryPerotFinesse(R); }
double op_fabry_perot_fsr(OP_Handle h, double d, double n) { return op(h)->fabryPerotFSR(d, n); }

// ── 19: Coherence ───────────────────────────────────────────────────────────
double op_temporal_coherence_length(OP_Handle h, double l, double dl) { return op(h)->temporalCoherenceLength(l, dl); }
double op_spatial_coherence_width(OP_Handle h, double l, double ts) { return op(h)->spatialCoherenceWidth(l, ts); }
double op_degree_of_coherence(OP_Handle h, double tau, double tc) { return op(h)->degreeOfCoherence(tau, tc); }

// ── 22: Talbot ──────────────────────────────────────────────────────────────
double op_talbot_distance(OP_Handle h, double d, double l) { return op(h)->talbotDistance(d, l); }

// ── 26: Malus ───────────────────────────────────────────────────────────────
double op_malus_law(OP_Handle h, double I0, double t) { return op(h)->malusLaw(I0, t); }

// ── 27: Birefringence ───────────────────────────────────────────────────────
double op_birefringent_phase_delay(OP_Handle h, double dn, double d, double l) { return op(h)->birefringentPhaseDelay(dn, d, l); }
double op_walkoff_angle(OP_Handle h, double ne, double no, double t) { return op(h)->walkoffAngle(ne, no, t); }

// ── 28: Optical Activity ────────────────────────────────────────────────────
double op_optical_rotation(OP_Handle h, double sr, double c, double p) { return op(h)->opticalRotation(sr, c, p); }
double op_faraday_rotation(OP_Handle h, double V, double B, double L) { return op(h)->faradayRotation(V, B, L); }

// ── 31: Gaussian Beam ───────────────────────────────────────────────────────
double op_gaussian_waist(OP_Handle h, double w0, double z, double l) { return op(h)->gaussianBeamWaist(w0, z, l); }
double op_gaussian_rayleigh(OP_Handle h, double w0, double l) { return op(h)->gaussianRayleighRange(w0, l); }
double op_gaussian_divergence(OP_Handle h, double w0, double l) { return op(h)->gaussianDivergence(w0, l); }
double op_gaussian_gouy_phase(OP_Handle h, double z, double zR) { return op(h)->gaussianGouyPhase(z, zR); }
double op_gaussian_intensity(OP_Handle h, double r, double w) { return op(h)->gaussianIntensity(r, w); }

// ── 35: Laser Rate Equations ────────────────────────────────────────────────
double op_laser_threshold_gain(OP_Handle h, double losses, double L) { return op(h)->laserThresholdGain(losses, L); }
double op_laser_slope_efficiency(OP_Handle h, double se, double tc, double Vm, double hnu) { return op(h)->laserSlopeEfficiency(se, tc, Vm, hnu); }

// ── 36: Cavity Modes ────────────────────────────────────────────────────────
double op_cavity_fsr(OP_Handle h, double L, double n) { return op(h)->cavityModeFSR(L, n); }
double op_cavity_stability(OP_Handle h, double L, double R1, double R2) { return op(h)->cavityStabilityParameter(L, R1, R2); }

// ── 37: Q-Switch / Mode-Lock ────────────────────────────────────────────────
double op_mode_lock_rep_rate(OP_Handle h, double L, double n) { return op(h)->modeLockRepRate(L, n); }
double op_mode_lock_pulse_duration(OP_Handle h, double dnu) { return op(h)->modeLockPulseDuration(dnu); }

// ── 38: SHG ─────────────────────────────────────────────────────────────────
double op_shg_phase_mismatch(OP_Handle h, double nw, double n2w, double l) { return op(h)->shgPhaseMismatch(nw, n2w, l); }
double op_shg_coherence_length(OP_Handle h, double nw, double n2w, double l) { return op(h)->shgCoherenceLength(nw, n2w, l); }

// ── 39: Kerr Effect ─────────────────────────────────────────────────────────
double op_kerr_index(OP_Handle h, double n0, double n2, double I) { return op(h)->kerrRefractiveIndex(n0, n2, I); }
double op_b_integral(OP_Handle h, double n2, double I, double L, double l) { return op(h)->bIntegral(n2, I, L, l); }
double op_critical_power(OP_Handle h, double l, double n0, double n2) { return op(h)->criticalPowerSelfFocusing(l, n0, n2); }

// ── 40: OPA ─────────────────────────────────────────────────────────────────
double op_opa_gain(OP_Handle h, double g, double L, double dk) { return op(h)->opaGain(g, L, dk); }
double op_opa_idler_wavelength(OP_Handle h, double lp, double ls) { return op(h)->opaIdlerWavelength(lp, ls); }

// ── 41: Electro-Optic ───────────────────────────────────────────────────────
double op_pockels_phase_shift(OP_Handle h, double ne, double r, double V, double l, double d) { return op(h)->pockelsPhaseShift(ne, r, V, l, d); }
double op_half_wave_voltage(OP_Handle h, double l, double ne, double r, double d) { return op(h)->halfWaveVoltage(l, ne, r, d); }

// ── 42: Acousto-Optic ───────────────────────────────────────────────────────
double op_bragg_angle(OP_Handle h, double l, double ls) { return op(h)->braggAngle(l, ls); }
double op_ao_frequency_shift(OP_Handle h, double fa) { return op(h)->acoustoOpticFrequencyShift(fa); }

// ── 43: Photon Statistics ───────────────────────────────────────────────────
double op_poisson_distribution(double nm, int n) { return OpticalSystem::poissonDistribution(nm, n); }
double op_thermal_distribution(double nm, int n) { return OpticalSystem::thermalDistribution(nm, n); }
double op_mandel_q(OP_Handle h, double var, double nm) { return op(h)->mandelsQParameter(var, nm); }

// ── 44: HBT ─────────────────────────────────────────────────────────────────
double op_hbt_g2_thermal(OP_Handle h, double tau, double tc) { return op(h)->hbtG2Thermal(tau, tc); }

// ── 45: Beam Splitter / HOM ─────────────────────────────────────────────────
double op_hom_dip(OP_Handle h, double tau, double tc) { return op(h)->hongOuMandelDip(tau, tc); }

// ── 46: Mach-Zehnder ────────────────────────────────────────────────────────
double op_mz_output1(OP_Handle h, double phi) { return op(h)->machZehnderOutput1(phi); }
double op_mz_output2(OP_Handle h, double phi) { return op(h)->machZehnderOutput2(phi); }
double op_mz_sensitivity(OP_Handle h, double phi) { return op(h)->machZehnderSensitivity(phi); }

// ── 47: Squeezed Light ──────────────────────────────────────────────────────
double op_squeezed_variance_x(OP_Handle h, double r) { return op(h)->squeezedVarianceX(r); }
double op_squeezed_variance_p(OP_Handle h, double r) { return op(h)->squeezedVarianceP(r); }
double op_squeezed_db(OP_Handle h, double r) { return op(h)->squeezedSqueezingdB(r); }

// ── 48: BB84 QKD ────────────────────────────────────────────────────────────
double op_bb84_qber(OP_Handle h, double ed, double ec) { return op(h)->bb84QBER(ed, ec); }
double op_bb84_secure_key_rate(OP_Handle h, double Q, double fe) { return op(h)->bb84SecureKeyRate(Q, fe); }

// ── 49: Plasmonics ──────────────────────────────────────────────────────────
double op_spp_wavevector(OP_Handle h, double w, double emr, double ed) { return op(h)->sppWavevector(w, emr, ed); }
double op_spr_resonance_angle(OP_Handle h, double np, double emr, double ed) { return op(h)->sprResonanceAngle(np, emr, ed); }

// ── 50: Metamaterials ───────────────────────────────────────────────────────
double op_metamaterial_epsilon(OP_Handle h, double w, double wp, double g) { return op(h)->metamaterialEpsilon(w, wp, g); }
double op_metamaterial_mu(OP_Handle h, double w, double wm, double F, double gm) { return op(h)->metamaterialMu(w, wm, F, gm); }
double op_metamaterial_n(OP_Handle h, double eps, double mu) { return op(h)->metamaterialRefractiveIndex(eps, mu); }
double op_negative_refraction(OP_Handle h, double ti, double np, double nn) { return op(h)->negativeRefractionAngle(ti, np, nn); }
