#pragma once

#include "OpticsExport.h"
#include "OpticsConstants.h"

#include <string>
#include <vector>
#include <complex>
#include <array>
#include <utility>
#include <tuple>
#include <cmath>

// ═══════════════════════════════════════════════════════════════════════════════
//  OpticalSystem — physics engine for 50 optics simulations
// ═══════════════════════════════════════════════════════════════════════════════

// 2x2 complex matrix for Jones calculus
using JonesMatrix  = std::array<std::array<std::complex<double>, 2>, 2>;
using JonesVector  = std::array<std::complex<double>, 2>;

// 2x2 real matrix for ray (ABCD) optics
using ABCDMatrix   = std::array<std::array<double, 2>, 2>;

// 4x4 Mueller matrix (real)
using MuellerMatrix = std::array<std::array<double, 4>, 4>;

// Stokes vector
using StokesVector  = std::array<double, 4>;

class OPTICS_API OpticalSystem {
public:
    // ── Constructor ─────────────────────────────────────────────────────
    OpticalSystem(const std::string& name, double wavelength, double n_medium);

    std::string name;
    double wavelength;   // m  (default 632.8e-9  HeNe)
    double n_medium;     // refractive index of surrounding medium

    // ═════════════════════════════════════════════════════════════════════
    //  GEOMETRIC OPTICS  (Options 1–10)
    // ═════════════════════════════════════════════════════════════════════

    // --- 1: Snell's Law ---
    double snellRefraction(double theta_i, double n1, double n2);
    double criticalAngle(double n1, double n2);
    double brewsterAngle(double n1, double n2);
    double fresnelRs(double theta_i, double n1, double n2);
    double fresnelRp(double theta_i, double n1, double n2);
    double fresnelTs(double theta_i, double n1, double n2);
    double fresnelTp(double theta_i, double n1, double n2);
    void exportSnellCSV(const std::string& filename, double n1, double n2, int numPoints);

    // --- 2: Thin Lens ---
    double thinLensEquation(double objectDist, double focalLength);
    double magnification(double objectDist, double imageDist);
    double lensMakerEquation(double n_lens, double R1, double R2);
    double thinLensPower(double focalLength);
    void exportThinLensCSV(const std::string& filename, double f, double objMin, double objMax, int numPoints);

    // --- 3: Thick Lens & Principal Planes ---
    double thickLensFocalLength(double n_lens, double R1, double R2, double d);
    double thickLensFrontPrincipalPlane(double n_lens, double R1, double R2, double d);
    double thickLensBackPrincipalPlane(double n_lens, double R1, double R2, double d);
    void exportThickLensCSV(const std::string& filename, double n_lens, double R1, double R2, double d, int numPoints);

    // --- 4: Mirror Optics ---
    double sphericalMirrorFocal(double R);
    double mirrorEquation(double objectDist, double focalLength);
    double mirrorMagnification(double objectDist, double imageDist);
    void exportMirrorCSV(const std::string& filename, double R, double objMin, double objMax, int numPoints);

    // --- 5: ABCD Ray Transfer Matrices ---
    ABCDMatrix abcdFreeSpace(double d);
    ABCDMatrix abcdThinLens(double f);
    ABCDMatrix abcdSphericalBoundary(double n1, double n2, double R);
    ABCDMatrix abcdFlatBoundary(double n1, double n2);
    ABCDMatrix abcdSphericalMirror(double R);
    ABCDMatrix abcdMultiply(const ABCDMatrix& M1, const ABCDMatrix& M2);
    std::pair<double, double> abcdTraceRay(const ABCDMatrix& M, double y_in, double theta_in);
    void exportABCDRayTraceCSV(const std::string& filename, const std::vector<ABCDMatrix>& elements,
                                const std::vector<double>& lengths, double y0, double theta0, int numPoints);

    // --- 6: Prism Optics ---
    double prismDeviation(double theta_i, double n_prism, double apex);
    double prismMinDeviation(double n_prism, double apex);
    double cauchyIndex(double A, double B, double C, double lambda);
    double abbeNumber(double nD, double nF, double nC);
    double prismAngularDispersion(double n_prism, double apex, double dn_dlambda);
    void exportPrismDeviationCSV(const std::string& filename, double n_prism, double apex, int numPoints);

    // --- 7: Aberrations ---
    double sphericalAberrationCoeff(double n, double R, double h);
    double comaCoeff(double n, double R, double h, double theta);
    double astigmatismCoeff(double f, double theta);
    double fieldCurvatureRadius(double f);
    double chromaticAberrationLongitudinal(double f, double V);
    void exportAberrationCSV(const std::string& filename, double f, double n, double R, int numPoints);

    // --- 8: Optical Instruments ---
    double telescopeMagnification(double f_obj, double f_eye);
    double telescopeResolvingPower(double D, double lambda);
    double microscopeMagnification(double f_obj, double f_eye, double L_tube);
    double microscopeNA(double n, double alpha);
    double rayleighCriterion(double lambda, double NA);
    double fNumber(double f, double D);
    double depthOfField(double f, double N, double c, double s);
    void exportInstrumentCSV(const std::string& filename, int type, double param1, double param2, int numPoints);

    // --- 9: Fiber Optics ---
    double fiberNA(double n_core, double n_clad);
    double fiberAcceptanceAngle(double n_core, double n_clad);
    double fiberVNumber(double a, double NA, double lambda);
    int fiberNumberOfModes(double V);
    double fiberModeCutoff(double a, double NA);
    double fiberAttenuationdB(double P_in, double P_out);
    double fiberDispersion(double D, double L, double delta_lambda);
    void exportFiberCSV(const std::string& filename, double n_core, double n_clad, double a, int numPoints);

    // --- 10: Fermat's Principle & Eikonal ---
    double opticalPathLength(double n, double d);
    double totalOPL(const std::vector<double>& n_segments, const std::vector<double>& d_segments);
    double eikonalPhase(double OPL, double lambda);
    void exportOPLComparisonCSV(const std::string& filename, double n1, double n2, double d, int numPoints);

    // ═════════════════════════════════════════════════════════════════════
    //  WAVE OPTICS  (Options 11–22)
    // ═════════════════════════════════════════════════════════════════════

    // --- 11: Single Slit Diffraction ---
    double singleSlitIntensity(double theta, double a, double lambda);
    double singleSlitMinima(int m, double a, double lambda);
    void exportSingleSlitCSV(const std::string& filename, double a, double lambda, int numPoints);

    // --- 12: Double Slit Interference ---
    double doubleSlitIntensity(double theta, double a, double d, double lambda);
    double doubleSlitMaxima(int m, double d, double lambda);
    double doubleSlitFringeSpacing(double d, double lambda, double L);
    void exportDoubleSlitCSV(const std::string& filename, double a, double d, double lambda, int numPoints);

    // --- 13: N-Slit Diffraction Grating ---
    double gratingIntensity(double theta, double a, double d, double lambda, int N);
    double gratingMaxima(int m, double d, double lambda);
    double gratingResolvingPower(int m, int N);
    double gratingFreeSpectralRange(int m, double lambda);
    double gratingAngularDispersion(int m, double d, double theta);
    void exportGratingCSV(const std::string& filename, double a, double d, double lambda, int N, int numPoints);

    // --- 14: Circular Aperture (Airy Pattern) ---
    double airyIntensity(double theta, double D, double lambda);
    double airyFirstZero(double lambda, double D);
    double airyDiskRadius(double lambda, double f, double D);
    double rayleighResolution(double lambda, double D);
    double sparrowResolution(double lambda, double D);
    void exportAiryCSV(const std::string& filename, double D, double lambda, int numPoints);

    // --- 15: Fresnel Diffraction ---
    std::pair<double, double> fresnelIntegrals(double u);
    double fresnelDiffractionEdge(double x, double lambda, double z);
    double fresnelZoneRadius(int n, double lambda, double z);
    int fresnelNumber(double a, double lambda, double z);
    void exportFresnelEdgeCSV(const std::string& filename, double lambda, double z, int numPoints);
    void exportFresnelZonesCSV(const std::string& filename, double lambda, double z, int maxZones);

    // --- 16: Thin Film Interference ---
    double thinFilmReflectance(double n1, double n_film, double n2, double d, double lambda, double theta);
    double thinFilmTransmittance(double n1, double n_film, double n2, double d, double lambda, double theta);
    double thinFilmConstructiveThickness(int m, double n_film, double lambda);
    double thinFilmDestructiveThickness(int m, double n_film, double lambda);
    double antiReflectionThickness(double n_film, double lambda);
    void exportThinFilmCSV(const std::string& filename, double n1, double n_film, double n2, double d,
                           double lambdaMin, double lambdaMax, int numPoints);

    // --- 17: Michelson Interferometer ---
    double michelsonIntensity(double delta_d, double lambda);
    double michelsonVisibility(double I_max, double I_min);
    double michelsonFringeCount(double delta_d, double lambda);
    double michelsonCoherenceLength(double delta_lambda, double lambda);
    void exportMichelsonCSV(const std::string& filename, double lambda, double dMax, int numPoints);

    // --- 18: Fabry-Perot Interferometer ---
    double fabryPerotTransmission(double delta, double F);
    double fabryPerotFinesse(double R);
    double fabryPerotFSR(double d, double n);
    double fabryPerotResolvingPower(double m, double F_finesse);
    double fabryPerotFWHM(double FSR, double F_finesse);
    void exportFabryPerotCSV(const std::string& filename, double R, double d, double n,
                             double lambdaMin, double lambdaMax, int numPoints);

    // --- 19: Coherence (Temporal & Spatial) ---
    double temporalCoherenceLength(double lambda, double delta_lambda);
    double temporalCoherenceTime(double lambda, double delta_lambda);
    double spatialCoherenceWidth(double lambda, double theta_source);
    double vanCittertZernikeRadius(double lambda, double D_source, double z);
    double degreeOfCoherence(double tau, double tau_c);
    void exportCoherenceCSV(const std::string& filename, double lambda, double delta_lambda, int numPoints);

    // --- 20: Fraunhofer Diffraction (General Aperture) ---
    double fraunhoferRectIntensity(double kx, double ky, double a, double b);
    double fraunhoferCircIntensity(double kr, double R);
    std::complex<double> fraunhoferFT1D(const std::vector<double>& aperture, double dx, double kx);
    void exportFraunhoferRectCSV(const std::string& filename, double a, double b, double lambda, double f, int numPoints);
    void exportFraunhoferCircCSV(const std::string& filename, double R, double lambda, double f, int numPoints);

    // --- 21: Babinet's Principle ---
    double babinetComplementIntensity(double I_aperture, double I_total);
    void exportBabinetCSV(const std::string& filename, double a, double lambda, double z, int numPoints);

    // --- 22: Talbot Effect ---
    double talbotDistance(double d, double lambda);
    double fractionalTalbotDistance(double d, double lambda, int p, int q);
    double talbotSelfImageIntensity(double x, double d, double z, double lambda, int N_harmonics);
    void exportTalbotCSV(const std::string& filename, double d, double lambda, int numPoints);

    // ═════════════════════════════════════════════════════════════════════
    //  POLARIZATION  (Options 23–28)
    // ═════════════════════════════════════════════════════════════════════

    // --- 23: Jones Calculus ---
    JonesVector jonesLinearH();
    JonesVector jonesLinearV();
    JonesVector jonesLinear(double theta);
    JonesVector jonesRCP();
    JonesVector jonesLCP();
    JonesVector jonesElliptical(double alpha, double delta);
    JonesMatrix jonesLinearPolarizer(double theta);
    JonesMatrix jonesHWP(double theta);
    JonesMatrix jonesQWP(double theta);
    JonesMatrix jonesRotator(double theta);
    JonesMatrix jonesRetarder(double delta, double theta);
    JonesVector jonesApply(const JonesMatrix& M, const JonesVector& v);
    JonesMatrix jonesMultiply(const JonesMatrix& A, const JonesMatrix& B);
    double jonesIntensity(const JonesVector& v);
    void exportJonesCSV(const std::string& filename, int configType, double param, int numPoints);

    // --- 24: Stokes Parameters & Poincare Sphere ---
    StokesVector jonestoStokes(const JonesVector& v);
    double stokesDOP(const StokesVector& S);
    double stokesDOLP(const StokesVector& S);
    double stokesDOCP(const StokesVector& S);
    double stokesEllipticityAngle(const StokesVector& S);
    double stokesOrientationAngle(const StokesVector& S);
    std::tuple<double, double, double> stokesToPoincare(const StokesVector& S);
    void exportStokesCSV(const std::string& filename, const JonesVector& v);
    void exportPoincareCSV(const std::string& filename, int numStates);

    // --- 25: Mueller Matrix ---
    MuellerMatrix muellerLinearPolarizer(double theta);
    MuellerMatrix muellerHWP(double theta);
    MuellerMatrix muellerQWP(double theta);
    MuellerMatrix muellerRetarder(double delta, double theta);
    MuellerMatrix muellerRotator(double theta);
    MuellerMatrix muellerDepolarizer(double p);
    StokesVector muellerApply(const MuellerMatrix& M, const StokesVector& S);
    MuellerMatrix muellerMultiply(const MuellerMatrix& A, const MuellerMatrix& B);
    void exportMuellerCSV(const std::string& filename, int configType, double param, int numPoints);

    // --- 26: Malus's Law & Brewster Angle ---
    double malusLaw(double I0, double theta);
    double malusChain(double I0, const std::vector<double>& angles);
    double brewsterReflectionAngle(double n1, double n2);
    double reflectanceAtBrewster(double n1, double n2);
    void exportMalusCSV(const std::string& filename, double I0, int numPoints);

    // --- 27: Birefringence ---
    double birefringentPhaseDelay(double delta_n, double d, double lambda);
    double ordinaryRayAngle(double theta_i, double n_o);
    double extraordinaryRayAngle(double theta_i, double n_e, double n_o, double opticAxisAngle);
    double walkoffAngle(double n_e, double n_o, double theta);
    void exportBirefringenceCSV(const std::string& filename, double n_o, double n_e, double d,
                                double lambdaMin, double lambdaMax, int numPoints);

    // --- 28: Optical Activity ---
    double opticalRotation(double specificRotation, double concentration, double pathLength);
    double faradayRotation(double V, double B, double L);
    double verdetConstant(double lambda, double dn_dlambda);
    void exportOpticalActivityCSV(const std::string& filename, double alpha_spec, double c, int numPoints);

    // ═════════════════════════════════════════════════════════════════════
    //  FOURIER OPTICS & BEAMS  (Options 29–34)
    // ═════════════════════════════════════════════════════════════════════

    // --- 29: Fourier Transform Optics ---
    std::complex<double> fourierTransform1D(const std::vector<std::complex<double>>& field, double dx, double fx);
    std::vector<std::complex<double>> dft1D(const std::vector<std::complex<double>>& field, double dx,
                                            const std::vector<double>& fx_arr);
    double opticalTransferFunction(double fx, double cutoff);
    double modulationTransferFunction(double fx, double cutoff);
    void exportFourierApertureCSV(const std::string& filename, double a, double lambda, double f, int numPoints);

    // --- 30: Spatial Filtering ---
    double lowPassFilter(double fx, double fy, double fc);
    double highPassFilter(double fx, double fy, double fc);
    double bandPassFilter(double fx, double fy, double f_low, double f_high);
    void exportSpatialFilterCSV(const std::string& filename, double a, double fc, double lambda, double f, int numPoints);

    // --- 31: Gaussian Beam Propagation ---
    double gaussianBeamWaist(double w0, double z, double lambda);
    double gaussianRayleighRange(double w0, double lambda);
    double gaussianRadiusOfCurvature(double z, double zR);
    double gaussianGouyPhase(double z, double zR);
    double gaussianDivergence(double w0, double lambda);
    double gaussianIntensity(double r, double w);
    double gaussianPeakIntensity(double P, double w);
    std::complex<double> gaussianComplexBeamParameter(double z, double zR);
    std::complex<double> gaussianABCDPropagation(std::complex<double> q_in, const ABCDMatrix& M);
    double gaussianBeamWaistAfterLens(double w0, double f, double lambda);
    void exportGaussianBeamCSV(const std::string& filename, double w0, double lambda, double zMax, int numPoints);
    void exportGaussianBeamProfileCSV(const std::string& filename, double w0, double P, double rMax, int numPoints);

    // --- 32: Beam Propagation (BPM) ---
    std::vector<std::complex<double>> bpmStep(const std::vector<std::complex<double>>& field,
                                               double dx, double dz, double lambda, double n);
    void exportBPMCSV(const std::string& filename, const std::vector<std::complex<double>>& initial,
                      double dx, double dz, double lambda, double n, int numSteps, int snapshotEvery);

    // --- 33: Holography ---
    double hologramInterference(double ref_amp, double obj_amp, double phase_diff);
    std::vector<double> hologramRecord(const std::vector<double>& ref, const std::vector<double>& obj,
                                       const std::vector<double>& phase_diff);
    std::vector<double> hologramReconstruct(const std::vector<double>& hologram,
                                            const std::vector<double>& ref);
    void exportHologramCSV(const std::string& filename, double obj_amp, int numPoints);

    // --- 34: Talbot & Self-Imaging (Extended) ---
    double lauDistance(double d1, double d2, double lambda);
    void exportSelfImagingCSV(const std::string& filename, double d, double lambda, double zMax, int numPoints);

    // ═════════════════════════════════════════════════════════════════════
    //  LASERS & NONLINEAR OPTICS  (Options 35–42)
    // ═════════════════════════════════════════════════════════════════════

    // --- 35: Laser Rate Equations ---
    struct LaserState { double N; double phi; };
    LaserState laserRateEquationsStep(double N, double phi, double R_pump, double tau_sp,
                                      double sigma_e, double V_mode, double tau_c, double dt);
    double laserThresholdGain(double losses, double L_cavity);
    double laserThresholdPump(double N_threshold, double tau_sp);
    double laserSlopeEfficiency(double sigma_e, double tau_c, double V_mode, double h_nu);
    void exportLaserRateCSV(const std::string& filename, double R_pump, double tau_sp,
                            double sigma_e, double V_mode, double tau_c, double tMax, double dt);

    // --- 36: Laser Cavity Modes ---
    double cavityModeFSR(double L_cavity, double n);
    double cavityModeFrequency(int q, double L_cavity, double n);
    double cavityModeWavelength(int q, double L_cavity, double n);
    double gaussianModeFrequency(int q, int m, int p, double L_cavity, double R1, double R2);
    double cavityStabilityParameter(double L_cavity, double R1, double R2);
    bool cavityIsStable(double g1, double g2);
    double cavityWaistSize(double lambda, double L_cavity, double g1, double g2);
    void exportCavityModesCSV(const std::string& filename, double L, double n, int numModes);
    void exportStabilityDiagramCSV(const std::string& filename, int numPoints);

    // --- 37: Q-Switching & Mode-Locking ---
    double qSwitchPulseEnergy(double N_initial, double V_mode, double h_nu);
    double qSwitchPeakPower(double E_pulse, double tau_pulse);
    double qSwitchPulseDuration(double tau_c, double r);
    double modeLockPulseDuration(double delta_nu);
    double modeLockPeakPower(double P_avg, double rep_rate, double tau_pulse);
    double modeLockRepRate(double L_cavity, double n);
    void exportQSwitchCSV(const std::string& filename, double N_init, double V_mode,
                          double h_nu, double tau_c, int numPoints);
    void exportModeLockCSV(const std::string& filename, double L, double delta_nu, double P_avg, int numPoints);

    // --- 38: Second Harmonic Generation (SHG) ---
    double shgConversionEfficiency(double d_eff, double L_crystal, double lambda, double n_omega,
                                   double n_2omega, double I_pump);
    double shgPhaseMismatch(double n_omega, double n_2omega, double lambda);
    double shgCoherenceLength(double n_omega, double n_2omega, double lambda);
    double shgIntensity(double d_eff, double L, double delta_k, double I_pump, double lambda,
                        double n_omega, double n_2omega);
    void exportSHGCSV(const std::string& filename, double d_eff, double lambda, double n_omega,
                      double n_2omega, double I_pump, double LMax, int numPoints);

    // --- 39: Kerr Effect & Self-Phase Modulation ---
    double kerrRefractiveIndex(double n0, double n2, double I);
    double selfPhaseModulation(double n2, double I, double L, double lambda);
    double bIntegral(double n2, double I, double L, double lambda);
    double criticalPowerSelfFocusing(double lambda, double n0, double n2);
    double selfFocusingLength(double w0, double P, double P_cr);
    void exportKerrCSV(const std::string& filename, double n0, double n2, double L, double lambda,
                       double Imax, int numPoints);

    // --- 40: Optical Parametric Processes ---
    double opaGain(double gamma, double L, double delta_k);
    double opaIdlerWavelength(double lambda_pump, double lambda_signal);
    double opaEnergyConservation(double omega_pump, double omega_signal);
    double opaMomentumMismatch(double n_p, double n_s, double n_i, double omega_p,
                               double omega_s, double omega_i);
    void exportOPACSV(const std::string& filename, double gamma, double LMax, double delta_k, int numPoints);

    // --- 41: Electro-Optic Effect ---
    double pockelsPhaseShift(double n_e, double r, double V, double lambda, double d);
    double halfWaveVoltage(double lambda, double n_e, double r, double d);
    double kerrCellPhaseShift(double K, double lambda, double E_field, double L);
    void exportElectroOpticCSV(const std::string& filename, double n_e, double r, double d,
                               double lambda, double Vmax, int numPoints);

    // --- 42: Acousto-Optic Effect ---
    double braggAngle(double lambda, double lambda_sound);
    double acoustoOpticFrequencyShift(double f_acoustic);
    double acoustoOpticDiffractionEfficiency(double eta0, double P_acoustic, double P_max);
    double acoustoOpticBandwidth(double v_sound, double lambda, double D_beam);
    void exportAcoustoOpticCSV(const std::string& filename, double lambda, double v_sound,
                               double fMin, double fMax, int numPoints);

    // ═════════════════════════════════════════════════════════════════════
    //  MODERN & QUANTUM OPTICS  (Options 43–50)
    // ═════════════════════════════════════════════════════════════════════

    // --- 43: Photon Statistics ---
    static double poissonDistribution(double n_mean, int n);
    static double thermalDistribution(double n_mean, int n);
    static double subPoissonianVariance(double n_mean, double eta);
    double secondOrderCorrelation(double variance, double n_mean);
    double mandelsQParameter(double variance, double n_mean);
    void exportPhotonStatisticsCSV(const std::string& filename, double n_mean, int nMax);

    // --- 44: Hanbury Brown-Twiss (HBT) ---
    double hbtG2Coherent();
    double hbtG2Thermal(double tau, double tau_c);
    double hbtG2SinglePhoton();
    double hbtBunchingContrast(double tau_c, double delta_t);
    void exportHBTCSV(const std::string& filename, double tau_c, double tauMax, int numPoints);

    // --- 45: Beam Splitter Quantum Model ---
    struct BeamSplitterOutput {
        double P_both;
        double P_oneA;
        double P_oneB;
        double P_none;
    };
    BeamSplitterOutput beamSplitterSinglePhoton(double R, double T);
    BeamSplitterOutput beamSplitterHOMEffect(double R, double T, double distinguishability);
    double hongOuMandelDip(double tau, double tau_c);
    void exportBeamSplitterCSV(const std::string& filename, double R, int numPoints);
    void exportHOMCSV(const std::string& filename, double tau_c, double tauMax, int numPoints);

    // --- 46: Mach-Zehnder Interferometer ---
    double machZehnderOutput1(double phi);
    double machZehnderOutput2(double phi);
    double machZehnderPhaseShift(double n, double L, double lambda);
    double machZehnderSensitivity(double phi);
    double machZehnderQuantumNoise(int N_photons);
    void exportMachZehnderCSV(const std::string& filename, double phiMax, int numPoints);

    // --- 47: Squeezed Light ---
    double squeezedVarianceX(double r);
    double squeezedVarianceP(double r);
    double squeezedPhotonNumber(double r);
    double squeezedNoisePower(double r, double theta);
    double squeezedSqueezingdB(double r);
    void exportSqueezedLightCSV(const std::string& filename, double rMax, int numPoints);

    // --- 48: Quantum Key Distribution (BB84) ---
    double bb84QBER(double e_detector, double e_channel);
    double bb84SecureKeyRate(double QBER, double f_error);
    double bb84MaxDistance(double attenuation_dB_per_km, double eta_detector, double R_dark);
    double bb84MutualInformation(double QBER);
    double bb84EveInformation(double QBER);
    void exportBB84CSV(const std::string& filename, double e_det, double attenuation, int numPoints);

    // --- 49: Plasmonics & Surface Plasmon Resonance ---
    std::complex<double> drude(double omega, double omega_p, double gamma);
    double sppWavevector(double omega, double eps_metal_re, double eps_dielectric);
    double sppPropagationLength(double omega, double eps_metal_re, double eps_metal_im, double eps_d);
    double sppPenetrationDepthMetal(double omega, double eps_metal_re, double eps_d);
    double sppPenetrationDepthDielectric(double omega, double eps_metal_re, double eps_d);
    double sprResonanceAngle(double n_prism, double eps_metal_re, double eps_d);
    double localizedSPRWavelength(double eps_inf, double omega_p, double eps_medium);
    void exportSPPDispersionCSV(const std::string& filename, double omega_p, double gamma, double eps_d,
                                double omegaMax, int numPoints);
    void exportSPRReflectanceCSV(const std::string& filename, double n_prism, double eps_metal_re,
                                  double eps_metal_im, double d_metal, double eps_d, int numPoints);

    // --- 50: Metamaterials & Negative Refraction ---
    double metamaterialEpsilon(double omega, double omega_p, double gamma);
    double metamaterialMu(double omega, double omega_m, double F, double gamma_m);
    double metamaterialRefractiveIndex(double epsilon, double mu);
    double negativeRefractionAngle(double theta_i, double n_positive, double n_negative);
    double pendryPerfectLensResolution(double d, double lambda);
    double cloakingRadius(double R_inner, double R_outer, double r);
    void exportMetamaterialCSV(const std::string& filename, double omega_p, double gamma,
                               double omega_m, double F, double gamma_m, double omegaMax, int numPoints);
    void exportNegativeRefractionCSV(const std::string& filename, double n_neg, int numPoints);
};
