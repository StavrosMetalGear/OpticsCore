#include "OpticalSystem.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cassert>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace OpticsConstants;

// ═════════════════════════════════════════════════════════════════════════════
//  Constructor
// ═════════════════════════════════════════════════════════════════════════════
OpticalSystem::OpticalSystem(const std::string& name, double wavelength, double n_medium)
    : name(name), wavelength(wavelength), n_medium(n_medium) {}

// ═════════════════════════════════════════════════════════════════════════════
//  1: Snell's Law & Fresnel
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::snellRefraction(double theta_i, double n1, double n2) {
    double sinTheta_t = n1 * std::sin(theta_i) / n2;
    if (std::abs(sinTheta_t) > 1.0) return -1.0; // TIR
    return std::asin(sinTheta_t);
}

double OpticalSystem::criticalAngle(double n1, double n2) {
    if (n1 <= n2) return -1.0;
    return std::asin(n2 / n1);
}

double OpticalSystem::brewsterAngle(double n1, double n2) {
    return std::atan(n2 / n1);
}

double OpticalSystem::fresnelRs(double theta_i, double n1, double n2) {
    double ct_i = std::cos(theta_i);
    double sinTheta_t = n1 * std::sin(theta_i) / n2;
    if (std::abs(sinTheta_t) > 1.0) return 1.0;
    double ct_t = std::sqrt(1.0 - sinTheta_t * sinTheta_t);
    double num = n1 * ct_i - n2 * ct_t;
    double den = n1 * ct_i + n2 * ct_t;
    double r = num / den;
    return r * r;
}

double OpticalSystem::fresnelRp(double theta_i, double n1, double n2) {
    double ct_i = std::cos(theta_i);
    double sinTheta_t = n1 * std::sin(theta_i) / n2;
    if (std::abs(sinTheta_t) > 1.0) return 1.0;
    double ct_t = std::sqrt(1.0 - sinTheta_t * sinTheta_t);
    double num = n2 * ct_i - n1 * ct_t;
    double den = n2 * ct_i + n1 * ct_t;
    double r = num / den;
    return r * r;
}

double OpticalSystem::fresnelTs(double theta_i, double n1, double n2) {
    return 1.0 - fresnelRs(theta_i, n1, n2);
}

double OpticalSystem::fresnelTp(double theta_i, double n1, double n2) {
    return 1.0 - fresnelRp(theta_i, n1, n2);
}

void OpticalSystem::exportSnellCSV(const std::string& filename, double n1, double n2, int numPoints) {
    std::ofstream f(filename);
    f << "theta_i,theta_t,Rs,Rp,Ts,Tp\n";
    for (int i = 0; i < numPoints; ++i) {
        double theta = (M_PI / 2.0) * i / (numPoints - 1);
        double theta_t = snellRefraction(theta, n1, n2);
        f << theta << "," << theta_t << "," << fresnelRs(theta, n1, n2) << ","
          << fresnelRp(theta, n1, n2) << "," << fresnelTs(theta, n1, n2) << ","
          << fresnelTp(theta, n1, n2) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  2: Thin Lens
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::thinLensEquation(double objectDist, double focalLength) {
    if (std::abs(objectDist - focalLength) < 1e-15) return 1e30;
    return 1.0 / (1.0 / focalLength - 1.0 / objectDist);
}

double OpticalSystem::magnification(double objectDist, double imageDist) {
    if (std::abs(objectDist) < 1e-15) return 0.0;
    return -imageDist / objectDist;
}

double OpticalSystem::lensMakerEquation(double n_lens, double R1, double R2) {
    return (n_lens - 1.0) * (1.0 / R1 - 1.0 / R2);
}

double OpticalSystem::thinLensPower(double focalLength) {
    return 1.0 / focalLength; // diopters if f in meters
}

void OpticalSystem::exportThinLensCSV(const std::string& filename, double f, double objMin, double objMax, int numPoints) {
    std::ofstream file(filename);
    file << "object_dist,image_dist,magnification\n";
    for (int i = 0; i < numPoints; ++i) {
        double obj = objMin + (objMax - objMin) * i / (numPoints - 1);
        double img = thinLensEquation(obj, f);
        double mag = magnification(obj, img);
        file << obj << "," << img << "," << mag << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  3: Thick Lens & Principal Planes
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::thickLensFocalLength(double n_lens, double R1, double R2, double d) {
    double phi1 = (n_lens - 1.0) / R1;
    double phi2 = -(n_lens - 1.0) / R2;
    double phi = phi1 + phi2 - (d / n_lens) * phi1 * phi2;
    return 1.0 / phi;
}

double OpticalSystem::thickLensFrontPrincipalPlane(double n_lens, double R1, double R2, double d) {
    double phi2 = -(n_lens - 1.0) / R2;
    double f = thickLensFocalLength(n_lens, R1, R2, d);
    return -f * (d / n_lens) * phi2;
}

double OpticalSystem::thickLensBackPrincipalPlane(double n_lens, double R1, double R2, double d) {
    double phi1 = (n_lens - 1.0) / R1;
    double f = thickLensFocalLength(n_lens, R1, R2, d);
    return f * (d / n_lens) * phi1;
}

void OpticalSystem::exportThickLensCSV(const std::string& fn, double nl, double R1, double R2, double d, int np) {
    std::ofstream f(fn);
    f << "thickness,focal_length,front_principal,back_principal\n";
    for (int i = 0; i < np; ++i) {
        double t = 0.001 + d * (i + 1.0) / np;
        double fl = thickLensFocalLength(nl, R1, R2, t);
        double fp = thickLensFrontPrincipalPlane(nl, R1, R2, t);
        double bp = thickLensBackPrincipalPlane(nl, R1, R2, t);
        f << t << "," << fl << "," << fp << "," << bp << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  4: Mirror Optics
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::sphericalMirrorFocal(double R) { return R / 2.0; }

double OpticalSystem::mirrorEquation(double objectDist, double focalLength) {
    return thinLensEquation(objectDist, focalLength); // same formula
}

double OpticalSystem::mirrorMagnification(double objectDist, double imageDist) {
    return magnification(objectDist, imageDist);
}

void OpticalSystem::exportMirrorCSV(const std::string& fn, double R, double oMin, double oMax, int np) {
    std::ofstream f(fn);
    double foc = sphericalMirrorFocal(R);
    f << "object_dist,image_dist,magnification\n";
    for (int i = 0; i < np; ++i) {
        double obj = oMin + (oMax - oMin) * i / (np - 1);
        double img = mirrorEquation(obj, foc);
        double mag = mirrorMagnification(obj, img);
        f << obj << "," << img << "," << mag << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  5: ABCD Ray Transfer Matrices
// ═════════════════════════════════════════════════════════════════════════════
ABCDMatrix OpticalSystem::abcdFreeSpace(double d) {
    return {{{1.0, d}, {0.0, 1.0}}};
}

ABCDMatrix OpticalSystem::abcdThinLens(double f) {
    return {{{1.0, 0.0}, {-1.0 / f, 1.0}}};
}

ABCDMatrix OpticalSystem::abcdSphericalBoundary(double n1, double n2, double R) {
    return {{{1.0, 0.0}, {(n1 - n2) / (n2 * R), n1 / n2}}};
}

ABCDMatrix OpticalSystem::abcdFlatBoundary(double n1, double n2) {
    return {{{1.0, 0.0}, {0.0, n1 / n2}}};
}

ABCDMatrix OpticalSystem::abcdSphericalMirror(double R) {
    return {{{1.0, 0.0}, {-2.0 / R, 1.0}}};
}

ABCDMatrix OpticalSystem::abcdMultiply(const ABCDMatrix& M1, const ABCDMatrix& M2) {
    ABCDMatrix R{};
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
                R[i][j] += M1[i][k] * M2[k][j];
    return R;
}

std::pair<double, double> OpticalSystem::abcdTraceRay(const ABCDMatrix& M, double y_in, double theta_in) {
    double y_out = M[0][0] * y_in + M[0][1] * theta_in;
    double theta_out = M[1][0] * y_in + M[1][1] * theta_in;
    return {y_out, theta_out};
}

void OpticalSystem::exportABCDRayTraceCSV(const std::string& fn, const std::vector<ABCDMatrix>& elems,
                                            const std::vector<double>& lens, double y0, double th0, int np) {
    std::ofstream f(fn);
    f << "z,y,theta\n";
    double y = y0, th = th0, z = 0.0;
    f << z << "," << y << "," << th << "\n";
    for (size_t e = 0; e < elems.size(); ++e) {
        double segLen = (e < lens.size()) ? lens[e] : 0.0;
        int steps = std::max(1, np / (int)elems.size());
        for (int s = 1; s <= steps; ++s) {
            double dz = segLen * s / steps;
            auto M = abcdFreeSpace(dz);
            auto [yy, tt] = abcdTraceRay(M, y, th);
            f << (z + dz) << "," << yy << "," << tt << "\n";
        }
        auto [yn, tn] = abcdTraceRay(elems[e], y, th);
        y = yn; th = tn;
        z += segLen;
        f << z << "," << y << "," << th << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  6: Prism Optics
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::prismDeviation(double theta_i, double n_prism, double apex) {
    double sinR1 = std::sin(theta_i) / n_prism;
    if (std::abs(sinR1) > 1.0) return -1.0;
    double r1 = std::asin(sinR1);
    double r2 = apex - r1;
    double sinTheta2 = n_prism * std::sin(r2);
    if (std::abs(sinTheta2) > 1.0) return -1.0;
    double theta2 = std::asin(sinTheta2);
    return theta_i + theta2 - apex;
}

double OpticalSystem::prismMinDeviation(double n_prism, double apex) {
    return 2.0 * std::asin(n_prism * std::sin(apex / 2.0)) - apex;
}

double OpticalSystem::cauchyIndex(double A, double B, double C, double lambda) {
    double l2 = lambda * lambda;
    return A + B / l2 + C / (l2 * l2);
}

double OpticalSystem::abbeNumber(double nD, double nF, double nC) {
    return (nD - 1.0) / (nF - nC);
}

double OpticalSystem::prismAngularDispersion(double n_prism, double apex, double dn_dlambda) {
    double dmin = prismMinDeviation(n_prism, apex);
    (void)dmin;
    // dn/dlambda * base / cos(angle)
    return (2.0 * std::sin(apex / 2.0) / std::cos((apex + prismMinDeviation(n_prism, apex)) / 2.0)) * dn_dlambda;
}

void OpticalSystem::exportPrismDeviationCSV(const std::string& fn, double np, double apex, int numPts) {
    std::ofstream f(fn);
    f << "theta_i,deviation\n";
    for (int i = 0; i < numPts; ++i) {
        double theta = 0.01 + (M_PI / 2.0 - 0.02) * i / (numPts - 1);
        double dev = prismDeviation(theta, np, apex);
        f << theta << "," << dev << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  7: Aberrations
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::sphericalAberrationCoeff(double n, double R, double h_val) {
    return h_val * h_val * h_val / (2.0 * R * R) * (n - 1.0);
}

double OpticalSystem::comaCoeff(double n, double R, double h_val, double theta) {
    return h_val * h_val * theta / (2.0 * R) * (n - 1.0);
}

double OpticalSystem::astigmatismCoeff(double f, double theta) {
    return f * theta * theta;
}

double OpticalSystem::fieldCurvatureRadius(double f) { return f; }

double OpticalSystem::chromaticAberrationLongitudinal(double f, double V) {
    return -f / V;
}

void OpticalSystem::exportAberrationCSV(const std::string& fn, double f, double n, double R, int np) {
    std::ofstream file(fn);
    file << "height,spherical,coma,astigmatism,chromatic\n";
    double hMax = std::abs(R) * 0.5;
    for (int i = 0; i < np; ++i) {
        double h = hMax * (i + 1.0) / np;
        double sa = sphericalAberrationCoeff(n, R, h);
        double co = comaCoeff(n, R, h, 0.1);
        double as = astigmatismCoeff(f, h / f);
        double ch = chromaticAberrationLongitudinal(f, 50.0);
        file << h << "," << sa << "," << co << "," << as << "," << ch << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  8: Optical Instruments
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::telescopeMagnification(double f_obj, double f_eye) {
    return -f_obj / f_eye;
}

double OpticalSystem::telescopeResolvingPower(double D, double lambda) {
    return 1.22 * lambda / D; // angular resolution in radians
}

double OpticalSystem::microscopeMagnification(double f_obj, double f_eye, double L_tube) {
    return -(L_tube / f_obj) * (0.25 / f_eye); // 25 cm near point
}

double OpticalSystem::microscopeNA(double n, double alpha_val) {
    return n * std::sin(alpha_val);
}

double OpticalSystem::rayleighCriterion(double lambda, double NA) {
    return 0.61 * lambda / NA;
}

double OpticalSystem::fNumber(double f, double D) { return f / D; }

double OpticalSystem::depthOfField(double f, double N, double c_val, double s) {
    return 2.0 * N * c_val * s * s / (f * f);
}

void OpticalSystem::exportInstrumentCSV(const std::string& fn, int type, double p1, double p2, int np) {
    std::ofstream f(fn);
    if (type == 0) {
        f << "f_eye,magnification,resolving_power\n";
        for (int i = 0; i < np; ++i) {
            double fe = 0.005 + (p2 - 0.005) * i / (np - 1);
            double mag = telescopeMagnification(p1, fe);
            double res = telescopeResolvingPower(p1 / fNumber(p1, p1), 550e-9);
            f << fe << "," << mag << "," << res << "\n";
        }
    } else {
        f << "NA,resolution\n";
        for (int i = 0; i < np; ++i) {
            double na = 0.1 + 1.3 * i / (np - 1);
            double res = rayleighCriterion(550e-9, na);
            f << na << "," << res << "\n";
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  9: Fiber Optics
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::fiberNA(double n_core, double n_clad) {
    return std::sqrt(n_core * n_core - n_clad * n_clad);
}

double OpticalSystem::fiberAcceptanceAngle(double n_core, double n_clad) {
    return std::asin(fiberNA(n_core, n_clad));
}

double OpticalSystem::fiberVNumber(double a, double NA, double lambda) {
    return 2.0 * M_PI * a * NA / lambda;
}

int OpticalSystem::fiberNumberOfModes(double V) {
    return static_cast<int>(V * V / 2.0);
}

double OpticalSystem::fiberModeCutoff(double a, double NA) {
    return 2.0 * M_PI * a * NA / 2.405; // single-mode cutoff
}

double OpticalSystem::fiberAttenuationdB(double P_in, double P_out) {
    return -10.0 * std::log10(P_out / P_in);
}

double OpticalSystem::fiberDispersion(double D, double L, double delta_lambda) {
    return D * L * delta_lambda;
}

void OpticalSystem::exportFiberCSV(const std::string& fn, double nc, double ncl, double a, int np) {
    std::ofstream f(fn);
    double NA = fiberNA(nc, ncl);
    f << "wavelength,V_number,num_modes\n";
    for (int i = 0; i < np; ++i) {
        double lam = 400e-9 + (1600e-9 - 400e-9) * i / (np - 1);
        double V = fiberVNumber(a, NA, lam);
        int modes = fiberNumberOfModes(V);
        f << lam << "," << V << "," << modes << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  10: Fermat's Principle & Eikonal
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::opticalPathLength(double n, double d) { return n * d; }

double OpticalSystem::totalOPL(const std::vector<double>& ns, const std::vector<double>& ds) {
    double opl = 0.0;
    for (size_t i = 0; i < ns.size() && i < ds.size(); ++i)
        opl += ns[i] * ds[i];
    return opl;
}

double OpticalSystem::eikonalPhase(double OPL, double lambda) {
    return 2.0 * M_PI * OPL / lambda;
}

void OpticalSystem::exportOPLComparisonCSV(const std::string& fn, double n1, double n2, double d, int np) {
    std::ofstream f(fn);
    f << "distance,OPL_n1,OPL_n2,phase_n1,phase_n2\n";
    for (int i = 0; i < np; ++i) {
        double dist = d * (i + 1.0) / np;
        double opl1 = opticalPathLength(n1, dist);
        double opl2 = opticalPathLength(n2, dist);
        double ph1 = eikonalPhase(opl1, wavelength);
        double ph2 = eikonalPhase(opl2, wavelength);
        f << dist << "," << opl1 << "," << opl2 << "," << ph1 << "," << ph2 << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  11: Single Slit Diffraction
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::singleSlitIntensity(double theta, double a, double lambda) {
    if (std::abs(theta) < 1e-15) return 1.0;
    double beta = M_PI * a * std::sin(theta) / lambda;
    double sinc = std::sin(beta) / beta;
    return sinc * sinc;
}

double OpticalSystem::singleSlitMinima(int m, double a, double lambda) {
    return std::asin(m * lambda / a);
}

void OpticalSystem::exportSingleSlitCSV(const std::string& fn, double a, double lam, int np) {
    std::ofstream f(fn);
    f << "theta,intensity\n";
    for (int i = 0; i < np; ++i) {
        double theta = -M_PI / 6.0 + (M_PI / 3.0) * i / (np - 1);
        f << theta << "," << singleSlitIntensity(theta, a, lam) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  12: Double Slit Interference
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::doubleSlitIntensity(double theta, double a, double d, double lambda) {
    double single = singleSlitIntensity(theta, a, lambda);
    double delta = M_PI * d * std::sin(theta) / lambda;
    double interference = std::cos(delta);
    return single * interference * interference * 4.0;
}

double OpticalSystem::doubleSlitMaxima(int m, double d, double lambda) {
    return std::asin(m * lambda / d);
}

double OpticalSystem::doubleSlitFringeSpacing(double d, double lambda, double L) {
    return lambda * L / d;
}

void OpticalSystem::exportDoubleSlitCSV(const std::string& fn, double a, double d, double lam, int np) {
    std::ofstream f(fn);
    f << "theta,intensity\n";
    for (int i = 0; i < np; ++i) {
        double theta = -M_PI / 6.0 + (M_PI / 3.0) * i / (np - 1);
        f << theta << "," << doubleSlitIntensity(theta, a, d, lam) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  13: N-Slit Diffraction Grating
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::gratingIntensity(double theta, double a, double d, double lambda, int N) {
    double single = singleSlitIntensity(theta, a, lambda);
    double delta = M_PI * d * std::sin(theta) / lambda;
    double num = std::sin(N * delta);
    double den = std::sin(delta);
    double grating_factor = (std::abs(den) < 1e-15) ? (double)(N * N) : (num / den) * (num / den);
    return single * grating_factor / (N * N);
}

double OpticalSystem::gratingMaxima(int m, double d, double lambda) {
    return std::asin(m * lambda / d);
}

double OpticalSystem::gratingResolvingPower(int m, int N) {
    return static_cast<double>(m * N);
}

double OpticalSystem::gratingFreeSpectralRange(int m, double lambda) {
    return lambda / m;
}

double OpticalSystem::gratingAngularDispersion(int m, double d, double theta) {
    return (double)m / (d * std::cos(theta));
}

void OpticalSystem::exportGratingCSV(const std::string& fn, double a, double d, double lam, int N, int np) {
    std::ofstream f(fn);
    f << "theta,intensity\n";
    for (int i = 0; i < np; ++i) {
        double theta = -M_PI / 6.0 + (M_PI / 3.0) * i / (np - 1);
        f << theta << "," << gratingIntensity(theta, a, d, lam, N) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  14: Circular Aperture (Airy Pattern)
// ═════════════════════════════════════════════════════════════════════════════
// Simple J1(x)/x approximation for the Airy pattern
static double besselJ1(double x) {
    if (std::abs(x) < 1e-15) return 0.0;
    // Series expansion for small x
    if (std::abs(x) < 4.0) {
        double sum = 0.0;
        double term = x / 2.0;
        sum = term;
        for (int k = 1; k <= 20; ++k) {
            term *= -x * x / (4.0 * k * (k + 1));
            sum += term;
        }
        return sum;
    }
    // Asymptotic for large x
    double p1 = x - 3.0 * M_PI / 4.0;
    return std::sqrt(2.0 / (M_PI * x)) * std::cos(p1);
}

double OpticalSystem::airyIntensity(double theta, double D, double lambda) {
    double x = M_PI * D * std::sin(theta) / lambda;
    if (std::abs(x) < 1e-15) return 1.0;
    double j1 = besselJ1(x);
    return (2.0 * j1 / x) * (2.0 * j1 / x);
}

double OpticalSystem::airyFirstZero(double lambda, double D) {
    return 1.22 * lambda / D; // angular radius
}

double OpticalSystem::airyDiskRadius(double lambda, double f, double D) {
    return 1.22 * lambda * f / D;
}

double OpticalSystem::rayleighResolution(double lambda, double D) {
    return 1.22 * lambda / D;
}

double OpticalSystem::sparrowResolution(double lambda, double D) {
    return 0.95 * lambda / D;
}

void OpticalSystem::exportAiryCSV(const std::string& fn, double D, double lam, int np) {
    std::ofstream f(fn);
    f << "theta,intensity\n";
    for (int i = 0; i < np; ++i) {
        double theta = -0.01 + 0.02 * i / (np - 1);
        f << theta << "," << airyIntensity(theta, D, lam) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  15: Fresnel Diffraction
// ═════════════════════════════════════════════════════════════════════════════
std::pair<double, double> OpticalSystem::fresnelIntegrals(double u) {
    // Numerical integration via power series
    double C = 0.0, S = 0.0;
    int N = 200;
    double dt = u / N;
    for (int i = 0; i <= N; ++i) {
        double t = i * dt;
        double w = (i == 0 || i == N) ? 0.5 : 1.0; // trapezoidal
        C += w * std::cos(M_PI * t * t / 2.0) * dt;
        S += w * std::sin(M_PI * t * t / 2.0) * dt;
    }
    return {C, S};
}

double OpticalSystem::fresnelDiffractionEdge(double x, double lambda, double z) {
    double u = x * std::sqrt(2.0 / (lambda * z));
    auto [C, S] = fresnelIntegrals(u);
    double re = 0.5 + C;
    double im = 0.5 + S;
    return 0.5 * (re * re + im * im);
}

double OpticalSystem::fresnelZoneRadius(int n, double lambda, double z) {
    return std::sqrt(n * lambda * z);
}

int OpticalSystem::fresnelNumber(double a, double lambda, double z) {
    return static_cast<int>(a * a / (lambda * z));
}

void OpticalSystem::exportFresnelEdgeCSV(const std::string& fn, double lam, double z, int np) {
    std::ofstream f(fn);
    f << "x,intensity\n";
    double xMax = 5.0 * std::sqrt(lam * z);
    for (int i = 0; i < np; ++i) {
        double x = -xMax + 2.0 * xMax * i / (np - 1);
        f << x << "," << fresnelDiffractionEdge(x, lam, z) << "\n";
    }
}

void OpticalSystem::exportFresnelZonesCSV(const std::string& fn, double lam, double z, int maxZ) {
    std::ofstream f(fn);
    f << "zone,radius\n";
    for (int n = 1; n <= maxZ; ++n) {
        f << n << "," << fresnelZoneRadius(n, lam, z) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  16: Thin Film Interference
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::thinFilmReflectance(double n1, double n_film, double n2, double d,
                                           double lambda, double theta) {
    double r12 = (n1 - n_film) / (n1 + n_film);
    double r23 = (n_film - n2) / (n_film + n2);
    double cos_t = std::cos(theta);
    double delta = 4.0 * M_PI * n_film * d * cos_t / lambda;
    double num = r12 * r12 + r23 * r23 + 2.0 * r12 * r23 * std::cos(delta);
    double den = 1.0 + r12 * r12 * r23 * r23 + 2.0 * r12 * r23 * std::cos(delta);
    return num / den;
}

double OpticalSystem::thinFilmTransmittance(double n1, double n_film, double n2, double d,
                                             double lambda, double theta) {
    return 1.0 - thinFilmReflectance(n1, n_film, n2, d, lambda, theta);
}

double OpticalSystem::thinFilmConstructiveThickness(int m, double n_film, double lambda) {
    return m * lambda / (2.0 * n_film);
}

double OpticalSystem::thinFilmDestructiveThickness(int m, double n_film, double lambda) {
    return (2 * m + 1) * lambda / (4.0 * n_film);
}

double OpticalSystem::antiReflectionThickness(double n_film, double lambda) {
    return lambda / (4.0 * n_film);
}

void OpticalSystem::exportThinFilmCSV(const std::string& fn, double n1, double nf, double n2,
                                        double d, double lMin, double lMax, int np) {
    std::ofstream f(fn);
    f << "wavelength,reflectance,transmittance\n";
    for (int i = 0; i < np; ++i) {
        double lam = lMin + (lMax - lMin) * i / (np - 1);
        double R = thinFilmReflectance(n1, nf, n2, d, lam, 0.0);
        double T = thinFilmTransmittance(n1, nf, n2, d, lam, 0.0);
        f << lam << "," << R << "," << T << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  17: Michelson Interferometer
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::michelsonIntensity(double delta_d, double lambda) {
    double delta = 4.0 * M_PI * delta_d / lambda;
    return 0.5 * (1.0 + std::cos(delta));
}

double OpticalSystem::michelsonVisibility(double I_max, double I_min) {
    return (I_max - I_min) / (I_max + I_min);
}

double OpticalSystem::michelsonFringeCount(double delta_d, double lambda) {
    return 2.0 * delta_d / lambda;
}

double OpticalSystem::michelsonCoherenceLength(double delta_lambda, double lambda) {
    return lambda * lambda / delta_lambda;
}

void OpticalSystem::exportMichelsonCSV(const std::string& fn, double lam, double dMax, int np) {
    std::ofstream f(fn);
    f << "delta_d,intensity\n";
    for (int i = 0; i < np; ++i) {
        double dd = dMax * i / (np - 1);
        f << dd << "," << michelsonIntensity(dd, lam) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  18: Fabry-Perot Interferometer
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::fabryPerotTransmission(double delta, double F) {
    return 1.0 / (1.0 + F * std::sin(delta / 2.0) * std::sin(delta / 2.0));
}

double OpticalSystem::fabryPerotFinesse(double R) {
    return M_PI * std::sqrt(R) / (1.0 - R);
}

double OpticalSystem::fabryPerotFSR(double d, double n) {
    return OpticsConstants::c / (2.0 * n * d);
}

double OpticalSystem::fabryPerotResolvingPower(double m, double F_finesse) {
    return m * F_finesse;
}

double OpticalSystem::fabryPerotFWHM(double FSR, double F_finesse) {
    return FSR / F_finesse;
}

void OpticalSystem::exportFabryPerotCSV(const std::string& fn, double R, double d, double n,
                                          double lMin, double lMax, int np) {
    std::ofstream f(fn);
    double Fin = fabryPerotFinesse(R);
    double Fcoeff = 4.0 * R / ((1.0 - R) * (1.0 - R));
    f << "wavelength,transmission\n";
    for (int i = 0; i < np; ++i) {
        double lam = lMin + (lMax - lMin) * i / (np - 1);
        double delta = 4.0 * M_PI * n * d / lam;
        double T = fabryPerotTransmission(delta, Fcoeff);
        f << lam << "," << T << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  19: Coherence
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::temporalCoherenceLength(double lambda, double delta_lambda) {
    return lambda * lambda / delta_lambda;
}

double OpticalSystem::temporalCoherenceTime(double lambda, double delta_lambda) {
    return temporalCoherenceLength(lambda, delta_lambda) / OpticsConstants::c;
}

double OpticalSystem::spatialCoherenceWidth(double lambda, double theta_source) {
    return lambda / theta_source;
}

double OpticalSystem::vanCittertZernikeRadius(double lambda, double D_source, double z) {
    return 1.22 * lambda * z / D_source;
}

double OpticalSystem::degreeOfCoherence(double tau, double tau_c) {
    return std::exp(-M_PI * tau * tau / (tau_c * tau_c));
}

void OpticalSystem::exportCoherenceCSV(const std::string& fn, double lam, double dlam, int np) {
    std::ofstream f(fn);
    double tau_c = temporalCoherenceTime(lam, dlam);
    f << "tau,gamma\n";
    for (int i = 0; i < np; ++i) {
        double tau = 5.0 * tau_c * i / (np - 1);
        f << tau << "," << degreeOfCoherence(tau, tau_c) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  20: Fraunhofer Diffraction (General Aperture)
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::fraunhoferRectIntensity(double kx, double ky, double a, double b) {
    double sincX = (std::abs(kx * a / 2.0) < 1e-15) ? 1.0 : std::sin(kx * a / 2.0) / (kx * a / 2.0);
    double sincY = (std::abs(ky * b / 2.0) < 1e-15) ? 1.0 : std::sin(ky * b / 2.0) / (ky * b / 2.0);
    return sincX * sincX * sincY * sincY;
}

double OpticalSystem::fraunhoferCircIntensity(double kr, double R) {
    double x = kr * R;
    if (std::abs(x) < 1e-15) return 1.0;
    double j1 = besselJ1(x);
    return (2.0 * j1 / x) * (2.0 * j1 / x);
}

std::complex<double> OpticalSystem::fraunhoferFT1D(const std::vector<double>& aperture, double dx, double kx) {
    std::complex<double> sum(0.0, 0.0);
    for (size_t i = 0; i < aperture.size(); ++i) {
        double x = (i - aperture.size() / 2.0) * dx;
        sum += aperture[i] * std::exp(std::complex<double>(0.0, -kx * x)) * dx;
    }
    return sum;
}

void OpticalSystem::exportFraunhoferRectCSV(const std::string& fn, double a, double b, double lam, double f, int np) {
    std::ofstream file(fn);
    file << "x,intensity\n";
    double kMax = 6.0 * M_PI / a;
    for (int i = 0; i < np; ++i) {
        double kx = -kMax + 2.0 * kMax * i / (np - 1);
        file << kx << "," << fraunhoferRectIntensity(kx, 0.0, a, b) << "\n";
    }
}

void OpticalSystem::exportFraunhoferCircCSV(const std::string& fn, double R, double lam, double f, int np) {
    std::ofstream file(fn);
    file << "kr,intensity\n";
    double krMax = 6.0 * M_PI / R;
    for (int i = 0; i < np; ++i) {
        double kr = krMax * i / (np - 1);
        file << kr << "," << fraunhoferCircIntensity(kr, R) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  21: Babinet's Principle
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::babinetComplementIntensity(double I_aperture, double I_total) {
    double amp_ap = std::sqrt(I_aperture);
    double amp_tot = std::sqrt(I_total);
    double amp_comp = amp_tot - amp_ap;
    return amp_comp * amp_comp;
}

void OpticalSystem::exportBabinetCSV(const std::string& fn, double a, double lam, double z, int np) {
    std::ofstream f(fn);
    f << "theta,I_aperture,I_complement\n";
    for (int i = 0; i < np; ++i) {
        double theta = -M_PI / 6.0 + (M_PI / 3.0) * i / (np - 1);
        double I_ap = singleSlitIntensity(theta, a, lam);
        double I_comp = babinetComplementIntensity(I_ap, 1.0);
        f << theta << "," << I_ap << "," << I_comp << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  22: Talbot Effect
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::talbotDistance(double d, double lambda) {
    return 2.0 * d * d / lambda;
}

double OpticalSystem::fractionalTalbotDistance(double d, double lambda, int p, int q) {
    return talbotDistance(d, lambda) * (double)p / (double)q;
}

double OpticalSystem::lauDistance(double d1, double d2, double lambda) {
    return d1 * d2 / lambda;
}

double OpticalSystem::talbotSelfImageIntensity(double x, double d, double z, double lambda, int N_harmonics) {
    double sum = 0.5;
    double zT = talbotDistance(d, lambda);
    for (int n = 1; n <= N_harmonics; ++n) {
        double phase = 2.0 * M_PI * n * x / d - M_PI * n * n * z / zT;
        sum += std::cos(phase);
    }
    return sum * sum / ((N_harmonics + 0.5) * (N_harmonics + 0.5));
}

void OpticalSystem::exportTalbotCSV(const std::string& fn, double d, double lam, int np) {
    std::ofstream f(fn);
    double zT = talbotDistance(d, lam);
    f << "x,I_z0,I_zT,I_zT2\n";
    for (int i = 0; i < np; ++i) {
        double x = -2.0 * d + 4.0 * d * i / (np - 1);
        double I0 = talbotSelfImageIntensity(x, d, 0.0, lam, 10);
        double I1 = talbotSelfImageIntensity(x, d, zT, lam, 10);
        double I2 = talbotSelfImageIntensity(x, d, zT / 2.0, lam, 10);
        f << x << "," << I0 << "," << I1 << "," << I2 << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  23: Jones Calculus
// ═════════════════════════════════════════════════════════════════════════════
JonesVector OpticalSystem::jonesLinearH() { return {{{1.0, 0.0}, {0.0, 0.0}}}; }
JonesVector OpticalSystem::jonesLinearV() { return {{{0.0, 0.0}, {1.0, 0.0}}}; }
JonesVector OpticalSystem::jonesLinear(double theta) {
    return {{{std::cos(theta), 0.0}, {std::sin(theta), 0.0}}};
}
JonesVector OpticalSystem::jonesRCP() {
    double s = 1.0 / std::sqrt(2.0);
    return {{{s, 0.0}, {0.0, -s}}};
}
JonesVector OpticalSystem::jonesLCP() {
    double s = 1.0 / std::sqrt(2.0);
    return {{{s, 0.0}, {0.0, s}}};
}
JonesVector OpticalSystem::jonesElliptical(double alpha_val, double delta) {
    return {{{std::cos(alpha_val), 0.0}, std::polar(std::sin(alpha_val), delta)}};
}

JonesMatrix OpticalSystem::jonesLinearPolarizer(double theta) {
    double c = std::cos(theta), s = std::sin(theta);
    JonesMatrix M;
    M[0][0] = std::complex<double>(c * c, 0.0);
    M[0][1] = std::complex<double>(c * s, 0.0);
    M[1][0] = std::complex<double>(c * s, 0.0);
    M[1][1] = std::complex<double>(s * s, 0.0);
    return M;
}

JonesMatrix OpticalSystem::jonesHWP(double theta) {
    double c2 = std::cos(2.0 * theta), s2 = std::sin(2.0 * theta);
    JonesMatrix M;
    M[0][0] = std::complex<double>(c2, 0.0);
    M[0][1] = std::complex<double>(s2, 0.0);
    M[1][0] = std::complex<double>(s2, 0.0);
    M[1][1] = std::complex<double>(-c2, 0.0);
    return M;
}

JonesMatrix OpticalSystem::jonesQWP(double theta) {
    double c = std::cos(theta), s = std::sin(theta);
    std::complex<double> j(0.0, 1.0);
    JonesMatrix M;
    M[0][0] = c * c + j * s * s;
    M[0][1] = (1.0 - j) * c * s;
    M[1][0] = (1.0 - j) * c * s;
    M[1][1] = s * s + j * c * c;
    return M;
}

JonesMatrix OpticalSystem::jonesRotator(double theta) {
    double c = std::cos(theta), s = std::sin(theta);
    JonesMatrix M;
    M[0][0] = std::complex<double>(c, 0.0);
    M[0][1] = std::complex<double>(-s, 0.0);
    M[1][0] = std::complex<double>(s, 0.0);
    M[1][1] = std::complex<double>(c, 0.0);
    return M;
}

JonesMatrix OpticalSystem::jonesRetarder(double delta, double theta) {
    double c = std::cos(theta), s = std::sin(theta);
    std::complex<double> eid2 = std::exp(std::complex<double>(0.0, delta / 2.0));
    std::complex<double> emid2 = std::exp(std::complex<double>(0.0, -delta / 2.0));
    JonesMatrix M;
    M[0][0] = emid2 * c * c + eid2 * s * s;
    M[0][1] = (emid2 - eid2) * c * s;
    M[1][0] = M[0][1];
    M[1][1] = emid2 * s * s + eid2 * c * c;
    return M;
}

JonesVector OpticalSystem::jonesApply(const JonesMatrix& M, const JonesVector& v) {
    JonesVector out;
    out[0] = M[0][0] * v[0] + M[0][1] * v[1];
    out[1] = M[1][0] * v[0] + M[1][1] * v[1];
    return out;
}

JonesMatrix OpticalSystem::jonesMultiply(const JonesMatrix& A, const JonesMatrix& B) {
    JonesMatrix R{};
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
                R[i][j] += A[i][k] * B[k][j];
    return R;
}

double OpticalSystem::jonesIntensity(const JonesVector& v) {
    return std::norm(v[0]) + std::norm(v[1]);
}

void OpticalSystem::exportJonesCSV(const std::string& fn, int cfgType, double param, int np) {
    std::ofstream f(fn);
    f << "angle,intensity\n";
    for (int i = 0; i < np; ++i) {
        double theta = M_PI * i / (np - 1);
        JonesVector v = jonesLinear(theta);
        JonesMatrix M;
        if (cfgType == 0) M = jonesLinearPolarizer(param);
        else if (cfgType == 1) M = jonesHWP(param);
        else M = jonesQWP(param);
        JonesVector out = jonesApply(M, v);
        f << theta << "," << jonesIntensity(out) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  24: Stokes Parameters & Poincare Sphere
// ═════════════════════════════════════════════════════════════════════════════
StokesVector OpticalSystem::jonestoStokes(const JonesVector& v) {
    double Ex_r = v[0].real(), Ex_i = v[0].imag();
    double Ey_r = v[1].real(), Ey_i = v[1].imag();
    double S0 = Ex_r * Ex_r + Ex_i * Ex_i + Ey_r * Ey_r + Ey_i * Ey_i;
    double S1 = Ex_r * Ex_r + Ex_i * Ex_i - Ey_r * Ey_r - Ey_i * Ey_i;
    double S2 = 2.0 * (Ex_r * Ey_r + Ex_i * Ey_i);
    double S3 = 2.0 * (Ex_i * Ey_r - Ex_r * Ey_i);
    return {S0, S1, S2, S3};
}

double OpticalSystem::stokesDOP(const StokesVector& S) {
    return std::sqrt(S[1] * S[1] + S[2] * S[2] + S[3] * S[3]) / S[0];
}

double OpticalSystem::stokesDOLP(const StokesVector& S) {
    return std::sqrt(S[1] * S[1] + S[2] * S[2]) / S[0];
}

double OpticalSystem::stokesDOCP(const StokesVector& S) {
    return std::abs(S[3]) / S[0];
}

double OpticalSystem::stokesEllipticityAngle(const StokesVector& S) {
    return 0.5 * std::asin(S[3] / S[0]);
}

double OpticalSystem::stokesOrientationAngle(const StokesVector& S) {
    return 0.5 * std::atan2(S[2], S[1]);
}

std::tuple<double, double, double> OpticalSystem::stokesToPoincare(const StokesVector& S) {
    double r = std::sqrt(S[1] * S[1] + S[2] * S[2] + S[3] * S[3]);
    if (r < 1e-15) return {0.0, 0.0, 0.0};
    return {S[1] / S[0], S[2] / S[0], S[3] / S[0]};
}

void OpticalSystem::exportStokesCSV(const std::string& fn, const JonesVector& v) {
    std::ofstream f(fn);
    StokesVector S = jonestoStokes(v);
    f << "S0,S1,S2,S3,DOP,DOLP,DOCP\n";
    f << S[0] << "," << S[1] << "," << S[2] << "," << S[3] << ","
      << stokesDOP(S) << "," << stokesDOLP(S) << "," << stokesDOCP(S) << "\n";
}

void OpticalSystem::exportPoincareCSV(const std::string& fn, int numStates) {
    std::ofstream f(fn);
    f << "theta,s1,s2,s3\n";
    for (int i = 0; i < numStates; ++i) {
        double theta = M_PI * i / (numStates - 1);
        JonesVector v = jonesLinear(theta);
        StokesVector S = jonestoStokes(v);
        auto [s1, s2, s3] = stokesToPoincare(S);
        f << theta << "," << s1 << "," << s2 << "," << s3 << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  25: Mueller Matrix
// ═════════════════════════════════════════════════════════════════════════════
MuellerMatrix OpticalSystem::muellerLinearPolarizer(double theta) {
    double c2 = std::cos(2.0 * theta), s2 = std::sin(2.0 * theta);
    MuellerMatrix M{};
    M[0][0] = 0.5; M[0][1] = 0.5 * c2; M[0][2] = 0.5 * s2; M[0][3] = 0.0;
    M[1][0] = 0.5 * c2; M[1][1] = 0.5 * c2 * c2; M[1][2] = 0.5 * c2 * s2; M[1][3] = 0.0;
    M[2][0] = 0.5 * s2; M[2][1] = 0.5 * c2 * s2; M[2][2] = 0.5 * s2 * s2; M[2][3] = 0.0;
    M[3][0] = 0.0; M[3][1] = 0.0; M[3][2] = 0.0; M[3][3] = 0.0;
    return M;
}

MuellerMatrix OpticalSystem::muellerHWP(double theta) {
    return muellerRetarder(M_PI, theta);
}

MuellerMatrix OpticalSystem::muellerQWP(double theta) {
    return muellerRetarder(M_PI / 2.0, theta);
}

MuellerMatrix OpticalSystem::muellerRetarder(double delta, double theta) {
    double c2 = std::cos(2.0 * theta), s2 = std::sin(2.0 * theta);
    double cd = std::cos(delta), sd = std::sin(delta);
    MuellerMatrix M{};
    M[0][0] = 1.0;
    M[1][1] = c2 * c2 + s2 * s2 * cd;
    M[1][2] = c2 * s2 * (1.0 - cd);
    M[1][3] = -s2 * sd;
    M[2][1] = c2 * s2 * (1.0 - cd);
    M[2][2] = s2 * s2 + c2 * c2 * cd;
    M[2][3] = c2 * sd;
    M[3][1] = s2 * sd;
    M[3][2] = -c2 * sd;
    M[3][3] = cd;
    return M;
}

MuellerMatrix OpticalSystem::muellerRotator(double theta) {
    MuellerMatrix M{};
    double c2 = std::cos(2.0 * theta), s2 = std::sin(2.0 * theta);
    M[0][0] = 1.0; M[1][1] = c2; M[1][2] = s2;
    M[2][1] = -s2; M[2][2] = c2; M[3][3] = 1.0;
    return M;
}

MuellerMatrix OpticalSystem::muellerDepolarizer(double p) {
    MuellerMatrix M{};
    M[0][0] = 1.0;
    M[1][1] = p; M[2][2] = p; M[3][3] = p;
    return M;
}

StokesVector OpticalSystem::muellerApply(const MuellerMatrix& M, const StokesVector& S) {
    StokesVector out{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            out[i] += M[i][j] * S[j];
    return out;
}

MuellerMatrix OpticalSystem::muellerMultiply(const MuellerMatrix& A, const MuellerMatrix& B) {
    MuellerMatrix R{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 4; ++k)
                R[i][j] += A[i][k] * B[k][j];
    return R;
}

void OpticalSystem::exportMuellerCSV(const std::string& fn, int cfgType, double param, int np) {
    std::ofstream f(fn);
    f << "angle,S0_out,S1_out,S2_out,S3_out\n";
    for (int i = 0; i < np; ++i) {
        double theta = M_PI * i / (np - 1);
        StokesVector S_in = jonestoStokes(jonesLinear(theta));
        MuellerMatrix M;
        if (cfgType == 0) M = muellerLinearPolarizer(param);
        else if (cfgType == 1) M = muellerQWP(param);
        else M = muellerRetarder(param, 0.0);
        StokesVector S_out = muellerApply(M, S_in);
        f << theta << "," << S_out[0] << "," << S_out[1] << "," << S_out[2] << "," << S_out[3] << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  26: Malus's Law
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::malusLaw(double I0, double theta) {
    return I0 * std::cos(theta) * std::cos(theta);
}

double OpticalSystem::malusChain(double I0, const std::vector<double>& angles) {
    double I = I0;
    double prev = 0.0;
    for (auto& a : angles) {
        I *= std::cos(a - prev) * std::cos(a - prev);
        prev = a;
    }
    return I;
}

double OpticalSystem::brewsterReflectionAngle(double n1, double n2) {
    return brewsterAngle(n1, n2);
}

double OpticalSystem::reflectanceAtBrewster(double n1, double n2) {
    double theta_B = brewsterAngle(n1, n2);
    return fresnelRs(theta_B, n1, n2); // only s-pol reflects
}

void OpticalSystem::exportMalusCSV(const std::string& fn, double I0, int np) {
    std::ofstream f(fn);
    f << "angle,intensity\n";
    for (int i = 0; i < np; ++i) {
        double theta = M_PI * i / (np - 1);
        f << theta << "," << malusLaw(I0, theta) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  27: Birefringence
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::birefringentPhaseDelay(double delta_n, double d, double lambda) {
    return 2.0 * M_PI * delta_n * d / lambda;
}

double OpticalSystem::ordinaryRayAngle(double theta_i, double n_o) {
    return std::asin(std::sin(theta_i) / n_o);
}

double OpticalSystem::extraordinaryRayAngle(double theta_i, double n_e, double n_o, double opticAxisAngle) {
    double n_eff = n_o * n_e / std::sqrt(n_o * n_o * std::sin(opticAxisAngle) * std::sin(opticAxisAngle) +
                                           n_e * n_e * std::cos(opticAxisAngle) * std::cos(opticAxisAngle));
    return std::asin(std::sin(theta_i) / n_eff);
}

double OpticalSystem::walkoffAngle(double n_e, double n_o, double theta) {
    double tan_rho = (n_o * n_o - n_e * n_e) * std::sin(theta) * std::cos(theta) /
                     (n_o * n_o * std::sin(theta) * std::sin(theta) + n_e * n_e * std::cos(theta) * std::cos(theta));
    return std::atan(tan_rho);
}

void OpticalSystem::exportBirefringenceCSV(const std::string& fn, double no, double ne, double d,
                                             double lMin, double lMax, int np) {
    std::ofstream f(fn);
    double dn = std::abs(ne - no);
    f << "wavelength,phase_delay,walkoff\n";
    for (int i = 0; i < np; ++i) {
        double lam = lMin + (lMax - lMin) * i / (np - 1);
        double pd = birefringentPhaseDelay(dn, d, lam);
        double wo = walkoffAngle(ne, no, M_PI / 4.0);
        f << lam << "," << pd << "," << wo << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  28: Optical Activity
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::opticalRotation(double specificRotation, double concentration, double pathLength) {
    return specificRotation * concentration * pathLength;
}

double OpticalSystem::faradayRotation(double V, double B, double L) {
    return V * B * L;
}

double OpticalSystem::verdetConstant(double lambda, double dn_dlambda) {
    return (OpticsConstants::e / (2.0 * OpticsConstants::me * OpticsConstants::c)) * lambda * dn_dlambda;
}

void OpticalSystem::exportOpticalActivityCSV(const std::string& fn, double as, double conc, int np) {
    std::ofstream f(fn);
    f << "path_length,rotation_optical,rotation_faraday\n";
    for (int i = 0; i < np; ++i) {
        double L = 0.01 * (i + 1.0) / np;
        double rot = opticalRotation(as, conc, L);
        double frot = faradayRotation(10.0, 1.0, L);
        f << L << "," << rot << "," << frot << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  29: Fourier Transform Optics
// ═════════════════════════════════════════════════════════════════════════════
std::complex<double> OpticalSystem::fourierTransform1D(const std::vector<std::complex<double>>& field,
                                                        double dx, double fx) {
    std::complex<double> sum(0.0, 0.0);
    for (size_t i = 0; i < field.size(); ++i) {
        double x = (i - field.size() / 2.0) * dx;
        sum += field[i] * std::exp(std::complex<double>(0.0, -2.0 * M_PI * fx * x)) * dx;
    }
    return sum;
}

std::vector<std::complex<double>> OpticalSystem::dft1D(const std::vector<std::complex<double>>& field,
                                                        double dx, const std::vector<double>& fx_arr) {
    std::vector<std::complex<double>> result(fx_arr.size());
    for (size_t i = 0; i < fx_arr.size(); ++i)
        result[i] = fourierTransform1D(field, dx, fx_arr[i]);
    return result;
}

double OpticalSystem::opticalTransferFunction(double fx, double cutoff) {
    if (std::abs(fx) >= cutoff) return 0.0;
    double x = std::abs(fx) / cutoff;
    return (2.0 / M_PI) * (std::acos(x) - x * std::sqrt(1.0 - x * x));
}

double OpticalSystem::modulationTransferFunction(double fx, double cutoff) {
    return std::abs(opticalTransferFunction(fx, cutoff));
}

void OpticalSystem::exportFourierApertureCSV(const std::string& fn, double a, double lam, double f, int np) {
    std::ofstream file(fn);
    double cutoff = a / (lam * f);
    file << "fx,OTF,MTF\n";
    for (int i = 0; i < np; ++i) {
        double fx = 2.0 * cutoff * i / (np - 1);
        file << fx << "," << opticalTransferFunction(fx, cutoff) << ","
             << modulationTransferFunction(fx, cutoff) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  30: Spatial Filtering
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::lowPassFilter(double fx, double fy, double fc) {
    return (std::sqrt(fx * fx + fy * fy) <= fc) ? 1.0 : 0.0;
}

double OpticalSystem::highPassFilter(double fx, double fy, double fc) {
    return (std::sqrt(fx * fx + fy * fy) >= fc) ? 1.0 : 0.0;
}

double OpticalSystem::bandPassFilter(double fx, double fy, double f_low, double f_high) {
    double f = std::sqrt(fx * fx + fy * fy);
    return (f >= f_low && f <= f_high) ? 1.0 : 0.0;
}

void OpticalSystem::exportSpatialFilterCSV(const std::string& fn, double a, double fc, double lam, double f, int np) {
    std::ofstream file(fn);
    file << "fx,low_pass,high_pass,band_pass\n";
    double fMax = 3.0 * fc;
    for (int i = 0; i < np; ++i) {
        double fx = fMax * i / (np - 1);
        file << fx << "," << lowPassFilter(fx, 0.0, fc) << ","
             << highPassFilter(fx, 0.0, fc) << ","
             << bandPassFilter(fx, 0.0, fc * 0.5, fc * 1.5) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  31: Gaussian Beam Propagation
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::gaussianBeamWaist(double w0, double z, double lambda) {
    double zR = M_PI * w0 * w0 / lambda;
    return w0 * std::sqrt(1.0 + (z / zR) * (z / zR));
}

double OpticalSystem::gaussianRayleighRange(double w0, double lambda) {
    return M_PI * w0 * w0 / lambda;
}

double OpticalSystem::gaussianRadiusOfCurvature(double z, double zR) {
    if (std::abs(z) < 1e-15) return 1e30;
    return z * (1.0 + (zR / z) * (zR / z));
}

double OpticalSystem::gaussianGouyPhase(double z, double zR) {
    return std::atan(z / zR);
}

double OpticalSystem::gaussianDivergence(double w0, double lambda) {
    return lambda / (M_PI * w0);
}

double OpticalSystem::gaussianIntensity(double r, double w) {
    return std::exp(-2.0 * r * r / (w * w));
}

double OpticalSystem::gaussianPeakIntensity(double P, double w) {
    return 2.0 * P / (M_PI * w * w);
}

std::complex<double> OpticalSystem::gaussianComplexBeamParameter(double z, double zR) {
    return std::complex<double>(z, zR);
}

std::complex<double> OpticalSystem::gaussianABCDPropagation(std::complex<double> q_in, const ABCDMatrix& M) {
    return (M[0][0] * q_in + M[0][1]) / (M[1][0] * q_in + M[1][1]);
}

double OpticalSystem::gaussianBeamWaistAfterLens(double w0, double f, double lambda) {
    double zR = M_PI * w0 * w0 / lambda;
    return w0 / std::sqrt(1.0 + (zR / f) * (zR / f));
}

void OpticalSystem::exportGaussianBeamCSV(const std::string& fn, double w0, double lam, double zMax, int np) {
    std::ofstream f(fn);
    double zR = gaussianRayleighRange(w0, lam);
    f << "z,w,R_curv,gouy\n";
    for (int i = 0; i < np; ++i) {
        double z = -zMax + 2.0 * zMax * i / (np - 1);
        double w = gaussianBeamWaist(w0, z, lam);
        double Rc = gaussianRadiusOfCurvature(z, zR);
        double gouy = gaussianGouyPhase(z, zR);
        f << z << "," << w << "," << Rc << "," << gouy << "\n";
    }
}

void OpticalSystem::exportGaussianBeamProfileCSV(const std::string& fn, double w0, double P, double rMax, int np) {
    std::ofstream f(fn);
    f << "r,intensity\n";
    for (int i = 0; i < np; ++i) {
        double r = rMax * i / (np - 1);
        double I = gaussianPeakIntensity(P, w0) * gaussianIntensity(r, w0);
        f << r << "," << I << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  32: BPM
// ═════════════════════════════════════════════════════════════════════════════
std::vector<std::complex<double>> OpticalSystem::bpmStep(const std::vector<std::complex<double>>& field,
                                                          double dx, double dz, double lambda, double n) {
    // Simple split-step BPM placeholder
    std::vector<std::complex<double>> out(field.size());
    double k = 2.0 * M_PI * n / lambda;
    for (size_t i = 0; i < field.size(); ++i) {
        // Free-space phase
        out[i] = field[i] * std::exp(std::complex<double>(0.0, k * dz));
    }
    return out;
}

void OpticalSystem::exportBPMCSV(const std::string& fn, const std::vector<std::complex<double>>& init,
                                   double dx, double dz, double lam, double n, int nSteps, int snap) {
    std::ofstream f(fn);
    f << "step,x,amplitude\n";
    auto field = init;
    for (int s = 0; s <= nSteps; ++s) {
        if (s % snap == 0) {
            for (size_t i = 0; i < field.size(); ++i) {
                double x = (i - (int)field.size() / 2) * dx;
                f << s << "," << x << "," << std::abs(field[i]) << "\n";
            }
        }
        if (s < nSteps) field = bpmStep(field, dx, dz, lam, n);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  33: Holography
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::hologramInterference(double ref_amp, double obj_amp, double phase_diff) {
    return ref_amp * ref_amp + obj_amp * obj_amp + 2.0 * ref_amp * obj_amp * std::cos(phase_diff);
}

std::vector<double> OpticalSystem::hologramRecord(const std::vector<double>& ref,
                                                   const std::vector<double>& obj,
                                                   const std::vector<double>& phase) {
    std::vector<double> H(ref.size());
    for (size_t i = 0; i < ref.size(); ++i)
        H[i] = hologramInterference(ref[i], obj[i], phase[i]);
    return H;
}

std::vector<double> OpticalSystem::hologramReconstruct(const std::vector<double>& hologram,
                                                        const std::vector<double>& ref) {
    std::vector<double> out(hologram.size());
    for (size_t i = 0; i < hologram.size(); ++i)
        out[i] = hologram[i] * ref[i];
    return out;
}

void OpticalSystem::exportHologramCSV(const std::string& fn, double obj_amp, int np) {
    std::ofstream f(fn);
    f << "x,hologram,reconstructed\n";
    std::vector<double> ref(np), obj(np), phase(np);
    for (int i = 0; i < np; ++i) {
        double x = -1.0 + 2.0 * i / (np - 1);
        ref[i] = 1.0;
        obj[i] = obj_amp;
        phase[i] = 2.0 * M_PI * x * 5.0;
    }
    auto H = hologramRecord(ref, obj, phase);
    auto R = hologramReconstruct(H, ref);
    for (int i = 0; i < np; ++i) {
        double x = -1.0 + 2.0 * i / (np - 1);
        f << x << "," << H[i] << "," << R[i] << "\n";
    }
}

void OpticalSystem::exportSelfImagingCSV(const std::string& fn, double d, double lam, double zMax, int np) {
    std::ofstream f(fn);
    double zT = talbotDistance(d, lam);
    f << "z,intensity_x0,intensity_xd2\n";
    for (int i = 0; i < np; ++i) {
        double z = zMax * i / (np - 1);
        double I0 = talbotSelfImageIntensity(0.0, d, z, lam, 10);
        double Id = talbotSelfImageIntensity(d / 2.0, d, z, lam, 10);
        f << z << "," << I0 << "," << Id << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  35: Laser Rate Equations
// ═════════════════════════════════════════════════════════════════════════════
OpticalSystem::LaserState OpticalSystem::laserRateEquationsStep(double N, double phi, double R_pump,
    double tau_sp, double sigma_e, double V_mode, double tau_c, double dt) {
    double dN = R_pump - N / tau_sp - sigma_e * phi * N;
    double dphi = sigma_e * N * phi - phi / tau_c + N / (tau_sp * V_mode);
    return {N + dN * dt, phi + dphi * dt};
}

double OpticalSystem::laserThresholdGain(double losses, double L_cavity) {
    return losses / (2.0 * L_cavity);
}

double OpticalSystem::laserThresholdPump(double N_threshold, double tau_sp) {
    return N_threshold / tau_sp;
}

double OpticalSystem::laserSlopeEfficiency(double sigma_e, double tau_c, double V_mode, double h_nu) {
    return h_nu / (sigma_e * tau_c * V_mode);
}

void OpticalSystem::exportLaserRateCSV(const std::string& fn, double Rp, double tausp,
                                         double se, double Vm, double tauc, double tMax, double dt) {
    std::ofstream f(fn);
    f << "time,N,phi\n";
    double N = 0.0, phi = 1.0;
    for (double t = 0.0; t <= tMax; t += dt) {
        f << t << "," << N << "," << phi << "\n";
        auto state = laserRateEquationsStep(N, phi, Rp, tausp, se, Vm, tauc, dt);
        N = std::max(0.0, state.N);
        phi = std::max(0.0, state.phi);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  36: Laser Cavity Modes
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::cavityModeFSR(double L_cavity, double n) {
    return OpticsConstants::c / (2.0 * n * L_cavity);
}

double OpticalSystem::cavityModeFrequency(int q, double L_cavity, double n) {
    return q * OpticsConstants::c / (2.0 * n * L_cavity);
}

double OpticalSystem::cavityModeWavelength(int q, double L_cavity, double n) {
    return 2.0 * n * L_cavity / q;
}

double OpticalSystem::gaussianModeFrequency(int q, int m, int p, double L_cavity, double R1, double R2) {
    double g1 = 1.0 - L_cavity / R1;
    double g2 = 1.0 - L_cavity / R2;
    double fsr = OpticsConstants::c / (2.0 * L_cavity);
    return fsr * (q + (1 + m + p) * std::acos(std::sqrt(g1 * g2)) / M_PI);
}

double OpticalSystem::cavityStabilityParameter(double L_cavity, double R1, double R2) {
    double g1 = 1.0 - L_cavity / R1;
    double g2 = 1.0 - L_cavity / R2;
    return g1 * g2;
}

bool OpticalSystem::cavityIsStable(double g1, double g2) {
    double product = g1 * g2;
    return product >= 0.0 && product <= 1.0;
}

double OpticalSystem::cavityWaistSize(double lambda, double L_cavity, double g1, double g2) {
    double product = g1 * g2;
    if (product < 0.0 || product > 1.0) return 0.0;
    return std::sqrt(lambda * L_cavity / M_PI) * std::pow(g1 * g2 * (1.0 - g1 * g2) /
           ((g1 + g2 - 2.0 * g1 * g2) * (g1 + g2 - 2.0 * g1 * g2)), 0.25);
}

void OpticalSystem::exportCavityModesCSV(const std::string& fn, double L, double n, int numModes) {
    std::ofstream f(fn);
    double fsr = cavityModeFSR(L, n);
    f << "mode,frequency,wavelength\n";
    int q0 = (int)(2.0 * n * L / wavelength);
    for (int i = 0; i < numModes; ++i) {
        int q = q0 - numModes / 2 + i;
        if (q <= 0) continue;
        f << q << "," << cavityModeFrequency(q, L, n) << "," << cavityModeWavelength(q, L, n) << "\n";
    }
}

void OpticalSystem::exportStabilityDiagramCSV(const std::string& fn, int np) {
    std::ofstream f(fn);
    f << "g1,g2,stable\n";
    for (int i = 0; i < np; ++i) {
        double g1 = -1.5 + 3.0 * i / (np - 1);
        for (int j = 0; j < np; ++j) {
            double g2 = -1.5 + 3.0 * j / (np - 1);
            f << g1 << "," << g2 << "," << (cavityIsStable(g1, g2) ? 1 : 0) << "\n";
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  37: Q-Switching & Mode-Locking
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::qSwitchPulseEnergy(double N_initial, double V_mode, double h_nu) {
    return N_initial * V_mode * h_nu;
}

double OpticalSystem::qSwitchPeakPower(double E_pulse, double tau_pulse) {
    return E_pulse / tau_pulse;
}

double OpticalSystem::qSwitchPulseDuration(double tau_c, double r) {
    return tau_c * r / (r - 1.0 - std::log(r));
}

double OpticalSystem::modeLockPulseDuration(double delta_nu) {
    return 1.0 / delta_nu; // transform-limited
}

double OpticalSystem::modeLockPeakPower(double P_avg, double rep_rate, double tau_pulse) {
    return P_avg / (rep_rate * tau_pulse);
}

double OpticalSystem::modeLockRepRate(double L_cavity, double n) {
    return OpticsConstants::c / (2.0 * n * L_cavity);
}

void OpticalSystem::exportQSwitchCSV(const std::string& fn, double Ni, double Vm,
                                       double hnu, double tc, int np) {
    std::ofstream f(fn);
    f << "pump_ratio,pulse_energy,peak_power,pulse_duration\n";
    for (int i = 0; i < np; ++i) {
        double r = 1.1 + 9.9 * i / (np - 1);
        double E = qSwitchPulseEnergy(Ni * r, Vm, hnu);
        double tau = qSwitchPulseDuration(tc, r);
        double Pp = qSwitchPeakPower(E, tau);
        f << r << "," << E << "," << Pp << "," << tau << "\n";
    }
}

void OpticalSystem::exportModeLockCSV(const std::string& fn, double L, double dnu, double Pavg, int np) {
    std::ofstream f(fn);
    f << "bandwidth,pulse_duration,rep_rate,peak_power\n";
    for (int i = 0; i < np; ++i) {
        double bw = dnu * (0.1 + 9.9 * i / (np - 1));
        double tp = modeLockPulseDuration(bw);
        double rr = modeLockRepRate(L, 1.0);
        double Pp = modeLockPeakPower(Pavg, rr, tp);
        f << bw << "," << tp << "," << rr << "," << Pp << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  38: SHG
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::shgConversionEfficiency(double d_eff, double L_crystal, double lambda,
                                               double n_omega, double n_2omega, double I_pump) {
    double k = 2.0 * M_PI / lambda;
    double eta = 8.0 * M_PI * M_PI * d_eff * d_eff * L_crystal * L_crystal * I_pump /
                 (n_omega * n_omega * n_2omega * OpticsConstants::eps0 * OpticsConstants::c * lambda * lambda);
    return eta;
}

double OpticalSystem::shgPhaseMismatch(double n_omega, double n_2omega, double lambda) {
    return 4.0 * M_PI * (n_2omega - n_omega) / lambda;
}

double OpticalSystem::shgCoherenceLength(double n_omega, double n_2omega, double lambda) {
    return lambda / (4.0 * std::abs(n_2omega - n_omega));
}

double OpticalSystem::shgIntensity(double d_eff, double L, double delta_k, double I_pump,
                                    double lambda, double n_omega, double n_2omega) {
    double sinc_arg = delta_k * L / 2.0;
    double sinc = (std::abs(sinc_arg) < 1e-15) ? 1.0 : std::sin(sinc_arg) / sinc_arg;
    return shgConversionEfficiency(d_eff, L, lambda, n_omega, n_2omega, I_pump) * sinc * sinc;
}

void OpticalSystem::exportSHGCSV(const std::string& fn, double deff, double lam, double nw,
                                   double n2w, double Ip, double LMax, int np) {
    std::ofstream f(fn);
    double dk = shgPhaseMismatch(nw, n2w, lam);
    f << "crystal_length,efficiency,intensity\n";
    for (int i = 0; i < np; ++i) {
        double L = LMax * (i + 1.0) / np;
        double eta = shgConversionEfficiency(deff, L, lam, nw, n2w, Ip);
        double I2w = shgIntensity(deff, L, dk, Ip, lam, nw, n2w);
        f << L << "," << eta << "," << I2w << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  39: Kerr Effect
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::kerrRefractiveIndex(double n0, double n2, double I) {
    return n0 + n2 * I;
}

double OpticalSystem::selfPhaseModulation(double n2, double I, double L, double lambda) {
    return 2.0 * M_PI * n2 * I * L / lambda;
}

double OpticalSystem::bIntegral(double n2, double I, double L, double lambda) {
    return selfPhaseModulation(n2, I, L, lambda);
}

double OpticalSystem::criticalPowerSelfFocusing(double lambda, double n0, double n2) {
    return 3.77 * lambda * lambda / (8.0 * M_PI * n0 * n2);
}

double OpticalSystem::selfFocusingLength(double w0, double P, double P_cr) {
    if (P <= P_cr) return 1e30;
    return w0 / std::sqrt(P / P_cr - 1.0);
}

void OpticalSystem::exportKerrCSV(const std::string& fn, double n0, double n2, double L,
                                    double lam, double Imax, int np) {
    std::ofstream f(fn);
    f << "intensity,n_eff,spm_phase,B_integral\n";
    for (int i = 0; i < np; ++i) {
        double I = Imax * i / (np - 1);
        double n_eff = kerrRefractiveIndex(n0, n2, I);
        double spm = selfPhaseModulation(n2, I, L, lam);
        double B = bIntegral(n2, I, L, lam);
        f << I << "," << n_eff << "," << spm << "," << B << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  40: OPA
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::opaGain(double gamma, double L, double delta_k) {
    double g2 = gamma * gamma - (delta_k / 2.0) * (delta_k / 2.0);
    if (g2 <= 0.0) return 1.0;
    double g = std::sqrt(g2);
    double sinh_val = std::sinh(g * L);
    return 1.0 + (gamma * gamma / g2) * sinh_val * sinh_val;
}

double OpticalSystem::opaIdlerWavelength(double lambda_pump, double lambda_signal) {
    return 1.0 / (1.0 / lambda_pump - 1.0 / lambda_signal);
}

double OpticalSystem::opaEnergyConservation(double omega_pump, double omega_signal) {
    return omega_pump - omega_signal;
}

double OpticalSystem::opaMomentumMismatch(double n_p, double n_s, double n_i,
                                           double omega_p, double omega_s, double omega_i) {
    return (n_p * omega_p - n_s * omega_s - n_i * omega_i) / OpticsConstants::c;
}

void OpticalSystem::exportOPACSV(const std::string& fn, double gamma, double LMax, double dk, int np) {
    std::ofstream f(fn);
    f << "length,gain\n";
    for (int i = 0; i < np; ++i) {
        double L = LMax * (i + 1.0) / np;
        f << L << "," << opaGain(gamma, L, dk) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  41: Electro-Optic Effect
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::pockelsPhaseShift(double n_e, double r, double V, double lambda, double d) {
    return M_PI * n_e * n_e * n_e * r * V / (lambda * d);
}

double OpticalSystem::halfWaveVoltage(double lambda, double n_e, double r, double d) {
    return lambda * d / (n_e * n_e * n_e * r);
}

double OpticalSystem::kerrCellPhaseShift(double K, double lambda, double E_field, double L) {
    return 2.0 * M_PI * K * E_field * E_field * L / lambda;
}

void OpticalSystem::exportElectroOpticCSV(const std::string& fn, double ne, double r, double d,
                                            double lam, double Vmax, int np) {
    std::ofstream f(fn);
    f << "voltage,phase_shift,transmission\n";
    for (int i = 0; i < np; ++i) {
        double V = Vmax * i / (np - 1);
        double phi = pockelsPhaseShift(ne, r, V, lam, d);
        double T = std::sin(phi / 2.0) * std::sin(phi / 2.0);
        f << V << "," << phi << "," << T << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  42: Acousto-Optic
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::braggAngle(double lambda, double lambda_sound) {
    return std::asin(lambda / (2.0 * lambda_sound));
}

double OpticalSystem::acoustoOpticFrequencyShift(double f_acoustic) {
    return f_acoustic; // shifted by f_acoustic
}

double OpticalSystem::acoustoOpticDiffractionEfficiency(double eta0, double P_acoustic, double P_max) {
    return eta0 * std::sin(M_PI / 2.0 * P_acoustic / P_max) * std::sin(M_PI / 2.0 * P_acoustic / P_max);
}

double OpticalSystem::acoustoOpticBandwidth(double v_sound, double lambda, double D_beam) {
    return v_sound * v_sound / (lambda * D_beam);
}

void OpticalSystem::exportAcoustoOpticCSV(const std::string& fn, double lam, double vs,
                                            double fMin, double fMax, int np) {
    std::ofstream f(fn);
    f << "frequency,bragg_angle,sound_wavelength\n";
    for (int i = 0; i < np; ++i) {
        double freq = fMin + (fMax - fMin) * i / (np - 1);
        double lambda_s = vs / freq;
        double ba = braggAngle(lam, lambda_s);
        f << freq << "," << ba << "," << lambda_s << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  43: Photon Statistics
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::poissonDistribution(double n_mean, int n) {
    double lnP = n * std::log(n_mean) - n_mean;
    for (int k = 2; k <= n; ++k) lnP -= std::log(k);
    return std::exp(lnP);
}

double OpticalSystem::thermalDistribution(double n_mean, int n) {
    return std::pow(n_mean / (1.0 + n_mean), n) / (1.0 + n_mean);
}

double OpticalSystem::subPoissonianVariance(double n_mean, double eta) {
    return n_mean * (1.0 - eta + eta * eta);
}

double OpticalSystem::secondOrderCorrelation(double variance, double n_mean) {
    if (n_mean < 1e-15) return 1.0;
    return 1.0 + (variance - n_mean) / (n_mean * n_mean);
}

double OpticalSystem::mandelsQParameter(double variance, double n_mean) {
    return (variance - n_mean) / n_mean;
}

void OpticalSystem::exportPhotonStatisticsCSV(const std::string& fn, double n_mean, int nMax) {
    std::ofstream f(fn);
    f << "n,poisson,thermal\n";
    for (int n = 0; n <= nMax; ++n) {
        f << n << "," << poissonDistribution(n_mean, n) << ","
          << thermalDistribution(n_mean, n) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  44: HBT
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::hbtG2Coherent() { return 1.0; }

double OpticalSystem::hbtG2Thermal(double tau, double tau_c) {
    return 1.0 + std::exp(-2.0 * std::abs(tau) / tau_c);
}

double OpticalSystem::hbtG2SinglePhoton() { return 0.0; }

double OpticalSystem::hbtBunchingContrast(double tau_c, double delta_t) {
    return std::exp(-2.0 * delta_t / tau_c);
}

void OpticalSystem::exportHBTCSV(const std::string& fn, double tau_c, double tauMax, int np) {
    std::ofstream f(fn);
    f << "tau,g2_thermal,g2_coherent\n";
    for (int i = 0; i < np; ++i) {
        double tau = -tauMax + 2.0 * tauMax * i / (np - 1);
        f << tau << "," << hbtG2Thermal(tau, tau_c) << "," << hbtG2Coherent() << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  45: Beam Splitter
// ═════════════════════════════════════════════════════════════════════════════
OpticalSystem::BeamSplitterOutput OpticalSystem::beamSplitterSinglePhoton(double R, double T) {
    return {0.0, R, T, 0.0};
}

OpticalSystem::BeamSplitterOutput OpticalSystem::beamSplitterHOMEffect(double R, double T, double d) {
    double P_both = R * T * (1.0 - d);
    return {P_both, R * R + T * T * d, T * T + R * R * d, 0.0};
}

double OpticalSystem::hongOuMandelDip(double tau, double tau_c) {
    return 1.0 - std::exp(-tau * tau / (tau_c * tau_c));
}

void OpticalSystem::exportBeamSplitterCSV(const std::string& fn, double R, int np) {
    std::ofstream f(fn);
    f << "reflectance,P_A,P_B,P_both\n";
    for (int i = 0; i < np; ++i) {
        double r = (double)i / (np - 1);
        double t = 1.0 - r;
        auto out = beamSplitterSinglePhoton(r, t);
        f << r << "," << out.P_oneA << "," << out.P_oneB << "," << out.P_both << "\n";
    }
}

void OpticalSystem::exportHOMCSV(const std::string& fn, double tau_c, double tauMax, int np) {
    std::ofstream f(fn);
    f << "tau,coincidence\n";
    for (int i = 0; i < np; ++i) {
        double tau = -tauMax + 2.0 * tauMax * i / (np - 1);
        f << tau << "," << hongOuMandelDip(tau, tau_c) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  46: Mach-Zehnder
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::machZehnderOutput1(double phi) {
    return std::cos(phi / 2.0) * std::cos(phi / 2.0);
}

double OpticalSystem::machZehnderOutput2(double phi) {
    return std::sin(phi / 2.0) * std::sin(phi / 2.0);
}

double OpticalSystem::machZehnderPhaseShift(double n, double L, double lambda) {
    return 2.0 * M_PI * n * L / lambda;
}

double OpticalSystem::machZehnderSensitivity(double phi) {
    return std::abs(std::sin(phi));
}

double OpticalSystem::machZehnderQuantumNoise(int N_photons) {
    return 1.0 / std::sqrt((double)N_photons);
}

void OpticalSystem::exportMachZehnderCSV(const std::string& fn, double phiMax, int np) {
    std::ofstream f(fn);
    f << "phi,output1,output2,sensitivity\n";
    for (int i = 0; i < np; ++i) {
        double phi = phiMax * i / (np - 1);
        f << phi << "," << machZehnderOutput1(phi) << "," << machZehnderOutput2(phi) << ","
          << machZehnderSensitivity(phi) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  47: Squeezed Light
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::squeezedVarianceX(double r) { return std::exp(-2.0 * r); }
double OpticalSystem::squeezedVarianceP(double r) { return std::exp(2.0 * r); }
double OpticalSystem::squeezedPhotonNumber(double r) { return std::sinh(r) * std::sinh(r); }

double OpticalSystem::squeezedNoisePower(double r, double theta) {
    return std::exp(-2.0 * r) * std::cos(theta) * std::cos(theta) +
           std::exp(2.0 * r) * std::sin(theta) * std::sin(theta);
}

double OpticalSystem::squeezedSqueezingdB(double r) { return 10.0 * std::log10(std::exp(-2.0 * r)); }

void OpticalSystem::exportSqueezedLightCSV(const std::string& fn, double rMax, int np) {
    std::ofstream f(fn);
    f << "r,var_x,var_p,photon_number,squeezing_dB\n";
    for (int i = 0; i < np; ++i) {
        double r = rMax * i / (np - 1);
        f << r << "," << squeezedVarianceX(r) << "," << squeezedVarianceP(r) << ","
          << squeezedPhotonNumber(r) << "," << squeezedSqueezingdB(r) << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  48: BB84 QKD
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::bb84QBER(double e_detector, double e_channel) {
    return e_detector + e_channel;
}

double OpticalSystem::bb84SecureKeyRate(double QBER, double f_error) {
    if (QBER >= 0.11) return 0.0;
    auto h = [](double x) -> double {
        if (x <= 0.0 || x >= 1.0) return 0.0;
        return -x * std::log2(x) - (1.0 - x) * std::log2(1.0 - x);
    };
    return 1.0 - h(QBER) - f_error * h(QBER);
}

double OpticalSystem::bb84MaxDistance(double attenuation_dB_per_km, double eta_detector, double R_dark) {
    // Simplified: find L where QBER = 0.11
    return -10.0 * std::log10(eta_detector) / attenuation_dB_per_km;
}

double OpticalSystem::bb84MutualInformation(double QBER) {
    if (QBER <= 0.0 || QBER >= 1.0) return 1.0;
    return 1.0 + QBER * std::log2(QBER) + (1.0 - QBER) * std::log2(1.0 - QBER);
}

double OpticalSystem::bb84EveInformation(double QBER) {
    return 1.0 - bb84MutualInformation(QBER);
}

void OpticalSystem::exportBB84CSV(const std::string& fn, double e_det, double att, int np) {
    std::ofstream f(fn);
    f << "QBER,key_rate,mutual_info,eve_info\n";
    for (int i = 0; i < np; ++i) {
        double qber = 0.15 * i / (np - 1);
        double kr = bb84SecureKeyRate(qber, 1.16);
        double mi = bb84MutualInformation(qber);
        double ei = bb84EveInformation(qber);
        f << qber << "," << kr << "," << mi << "," << ei << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  49: Plasmonics
// ═════════════════════════════════════════════════════════════════════════════
std::complex<double> OpticalSystem::drude(double omega, double omega_p, double gamma) {
    return 1.0 - omega_p * omega_p / (omega * omega + std::complex<double>(0.0, gamma * omega));
}

double OpticalSystem::sppWavevector(double omega, double eps_m_re, double eps_d) {
    return omega / OpticsConstants::c * std::sqrt(eps_m_re * eps_d / (eps_m_re + eps_d));
}

double OpticalSystem::sppPropagationLength(double omega, double eps_m_re, double eps_m_im, double eps_d) {
    double k_spp = sppWavevector(omega, eps_m_re, eps_d);
    double k_imag = k_spp * eps_m_im / (2.0 * eps_m_re * eps_m_re);
    return 1.0 / (2.0 * k_imag);
}

double OpticalSystem::sppPenetrationDepthMetal(double omega, double eps_m_re, double eps_d) {
    return OpticsConstants::c / omega * std::sqrt(std::abs((eps_m_re + eps_d) / (eps_m_re * eps_m_re)));
}

double OpticalSystem::sppPenetrationDepthDielectric(double omega, double eps_m_re, double eps_d) {
    return OpticsConstants::c / omega * std::sqrt(std::abs((eps_m_re + eps_d) / (eps_d * eps_d)));
}

double OpticalSystem::sprResonanceAngle(double n_prism, double eps_m_re, double eps_d) {
    double k_spp_over_k0 = std::sqrt(eps_m_re * eps_d / (eps_m_re + eps_d));
    return std::asin(k_spp_over_k0 / n_prism);
}

double OpticalSystem::localizedSPRWavelength(double eps_inf, double omega_p, double eps_medium) {
    // Frohlich condition: eps_real = -2*eps_medium
    double omega_res = omega_p / std::sqrt(eps_inf + 2.0 * eps_medium);
    return 2.0 * M_PI * OpticsConstants::c / omega_res;
}

void OpticalSystem::exportSPPDispersionCSV(const std::string& fn, double wp, double g, double ed,
                                             double wMax, int np) {
    std::ofstream f(fn);
    f << "omega,k_spp,light_line\n";
    for (int i = 1; i <= np; ++i) {
        double w = wMax * i / np;
        auto eps = drude(w, wp, g);
        double em_re = eps.real();
        if (em_re + ed > 0.0 && em_re < 0.0) {
            double k = sppWavevector(w, em_re, ed);
            f << w << "," << k << "," << (w * std::sqrt(ed) / OpticsConstants::c) << "\n";
        }
    }
}

void OpticalSystem::exportSPRReflectanceCSV(const std::string& fn, double nPrism, double emRe,
                                              double emIm, double dMetal, double ed, int np) {
    std::ofstream f(fn);
    f << "angle,reflectance\n";
    for (int i = 0; i < np; ++i) {
        double theta = M_PI / 2.0 * i / (np - 1);
        double k0 = 2.0 * M_PI / wavelength;
        double kx = k0 * nPrism * std::sin(theta);
        double k_spp = (emRe + ed > 0.0 && emRe < 0.0) ?
            k0 * std::sqrt(emRe * ed / (emRe + ed)) : 0.0;
        double dk = kx - k_spp;
        double w = dMetal * k0;
        double R = 1.0 - 1.0 / (1.0 + (dk * dk) / (w * w * emIm * emIm));
        f << theta << "," << R << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  50: Metamaterials
// ═════════════════════════════════════════════════════════════════════════════
double OpticalSystem::metamaterialEpsilon(double omega, double omega_p, double gamma) {
    return 1.0 - omega_p * omega_p / (omega * omega + gamma * gamma);
}

double OpticalSystem::metamaterialMu(double omega, double omega_m, double F, double gamma_m) {
    return 1.0 - F * omega_m * omega_m / (omega * omega - omega_m * omega_m +
           std::complex<double>(0.0, gamma_m * omega).real());
}

double OpticalSystem::metamaterialRefractiveIndex(double epsilon, double mu) {
    double n2 = epsilon * mu;
    if (epsilon < 0.0 && mu < 0.0) return -std::sqrt(std::abs(n2));
    return std::sqrt(std::abs(n2));
}

double OpticalSystem::negativeRefractionAngle(double theta_i, double n_pos, double n_neg) {
    double sinTheta_t = n_pos * std::sin(theta_i) / n_neg;
    return std::asin(sinTheta_t); // negative n gives negative angle
}

double OpticalSystem::pendryPerfectLensResolution(double d, double lambda) {
    return d; // subwavelength resolution limited by slab thickness
}

double OpticalSystem::cloakingRadius(double R_inner, double R_outer, double r) {
    return R_inner + (R_outer - R_inner) * r / R_outer;
}

void OpticalSystem::exportMetamaterialCSV(const std::string& fn, double wp, double g,
                                            double wm, double F, double gm, double wMax, int np) {
    std::ofstream f(fn);
    f << "omega,epsilon,mu,n\n";
    for (int i = 1; i <= np; ++i) {
        double w = wMax * i / np;
        double eps = metamaterialEpsilon(w, wp, g);
        double mu = metamaterialMu(w, wm, F, gm);
        double n = metamaterialRefractiveIndex(eps, mu);
        f << w << "," << eps << "," << mu << "," << n << "\n";
    }
}

void OpticalSystem::exportNegativeRefractionCSV(const std::string& fn, double n_neg, int np) {
    std::ofstream f(fn);
    f << "theta_i,theta_t_positive,theta_t_negative\n";
    for (int i = 0; i < np; ++i) {
        double ti = (M_PI / 2.0 - 0.01) * i / (np - 1);
        double tp = snellRefraction(ti, 1.0, std::abs(n_neg));
        double tn = negativeRefractionAngle(ti, 1.0, n_neg);
        f << ti << "," << tp << "," << tn << "\n";
    }
}
