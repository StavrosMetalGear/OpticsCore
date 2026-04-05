#include "NumericalSolverOpt.h"
#include <fstream>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ═════════════════════════════════════════════════════════════════════════════
//  1D FDTD — Ez/Hy Yee grid, Mur absorbing BCs, point source, CSV snapshots
// ═════════════════════════════════════════════════════════════════════════════
void NumericalSolverOpt::solveFDTD1D(
    double dx, double dt, int numPoints, int numSteps,
    const std::vector<double>& epsilon,
    const std::vector<double>& mu,
    double sourceFreq, int sourcePos,
    const std::string& outputFilename,
    int snapshotEvery)
{
    const double c0 = 2.99792458e8;
    const double eps0 = 8.854187817e-12;
    const double mu0 = 1.25663706212e-6;

    int N = numPoints;
    if (N < 3) return;
    if (sourcePos < 0 || sourcePos >= N) sourcePos = N / 2;
    if (snapshotEvery < 1) snapshotEvery = 1;

    // Ensure epsilon/mu arrays cover all points; default to vacuum
    std::vector<double> eps_r(N, 1.0);
    std::vector<double> mu_r(N, 1.0);
    for (int i = 0; i < N && i < (int)epsilon.size(); ++i) eps_r[i] = epsilon[i];
    for (int i = 0; i < N && i < (int)mu.size(); ++i) mu_r[i] = mu[i];

    // Update coefficients
    std::vector<double> cEz(N), cHy(N);
    for (int i = 0; i < N; ++i) {
        cEz[i] = dt / (eps0 * eps_r[i] * dx);
        cHy[i] = dt / (mu0 * mu_r[i] * dx);
    }

    // Fields: Ez at integer points, Hy at half-integer points
    std::vector<double> Ez(N, 0.0);
    std::vector<double> Hy(N, 0.0);

    // Mur ABC storage: previous boundary values
    double Ez_left_prev = 0.0;
    double Ez_right_prev = 0.0;
    double mur_coeff = (c0 * dt - dx) / (c0 * dt + dx);

    std::ofstream fout(outputFilename);
    // Header: step, then x positions
    fout << "step";
    for (int i = 0; i < N; ++i)
        fout << ",x" << i;
    fout << "\n";

    for (int step = 0; step < numSteps; ++step) {
        // --- Update Hy (half-step) ---
        for (int i = 0; i < N - 1; ++i) {
            Hy[i] = Hy[i] + cHy[i] * (Ez[i + 1] - Ez[i]);
        }

        // --- Update Ez ---
        for (int i = 1; i < N - 1; ++i) {
            Ez[i] = Ez[i] + cEz[i] * (Hy[i] - Hy[i - 1]);
        }

        // --- Soft point source (additive) ---
        double t = step * dt;
        Ez[sourcePos] += std::sin(2.0 * M_PI * sourceFreq * t);

        // --- Mur absorbing boundary conditions ---
        double Ez_left_new = Ez_left_prev + mur_coeff * (Ez[1] - Ez[0]);
        double Ez_right_new = Ez_right_prev + mur_coeff * (Ez[N - 2] - Ez[N - 1]);
        Ez_left_prev = Ez[0];
        Ez_right_prev = Ez[N - 1];
        Ez[0] = Ez_left_new;
        Ez[N - 1] = Ez_right_new;

        // --- Snapshot ---
        if (step % snapshotEvery == 0) {
            fout << step;
            for (int i = 0; i < N; ++i)
                fout << "," << Ez[i];
            fout << "\n";
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  Split-step Fourier Beam Propagation Method (BPM)
// ═════════════════════════════════════════════════════════════════════════════
void NumericalSolverOpt::solveBPM(
    double dx, double dz, int numPointsX, int numStepsZ,
    double lambda, double n0,
    const std::vector<std::complex<double>>& initialField,
    const std::vector<double>& refractiveIndexProfile,
    const std::string& outputFilename,
    int snapshotEvery)
{
    int N = numPointsX;
    if (N < 2) return;
    if (snapshotEvery < 1) snapshotEvery = 1;

    double k0 = 2.0 * M_PI / lambda;
    double kz = k0 * n0;

    // Refractive index profile (default to n0)
    std::vector<double> n_prof(N, n0);
    for (int i = 0; i < N && i < (int)refractiveIndexProfile.size(); ++i)
        n_prof[i] = refractiveIndexProfile[i];

    // Initial field (pad or truncate)
    std::vector<std::complex<double>> field(N, {0.0, 0.0});
    for (int i = 0; i < N && i < (int)initialField.size(); ++i)
        field[i] = initialField[i];

    // Spatial frequency array for diffraction step
    std::vector<double> kx(N);
    double dkx = 2.0 * M_PI / (N * dx);
    for (int i = 0; i < N; ++i) {
        int ii = (i <= N / 2) ? i : i - N;
        kx[i] = ii * dkx;
    }

    // Precompute diffraction phase (free-space propagator in Fourier domain)
    std::vector<std::complex<double>> diffraction_phase(N);
    for (int i = 0; i < N; ++i) {
        double phase = -kx[i] * kx[i] * dz / (2.0 * kz);
        diffraction_phase[i] = std::exp(std::complex<double>(0.0, phase));
    }

    // Precompute refraction phase (lens term in real space)
    std::vector<std::complex<double>> refraction_phase(N);
    for (int i = 0; i < N; ++i) {
        double dn = n_prof[i] - n0;
        double phase = k0 * dn * dz;
        refraction_phase[i] = std::exp(std::complex<double>(0.0, phase));
    }

    // Temporary arrays for DFT/IDFT (simple direct DFT for generality)
    std::vector<std::complex<double>> spectrum(N);
    std::vector<std::complex<double>> temp(N);

    auto dft = [&](const std::vector<std::complex<double>>& in,
                   std::vector<std::complex<double>>& out) {
        for (int k = 0; k < N; ++k) {
            out[k] = {0.0, 0.0};
            for (int n = 0; n < N; ++n) {
                double angle = -2.0 * M_PI * k * n / N;
                out[k] += in[n] * std::exp(std::complex<double>(0.0, angle));
            }
        }
    };

    auto idft = [&](const std::vector<std::complex<double>>& in,
                    std::vector<std::complex<double>>& out) {
        for (int n = 0; n < N; ++n) {
            out[n] = {0.0, 0.0};
            for (int k = 0; k < N; ++k) {
                double angle = 2.0 * M_PI * k * n / N;
                out[n] += in[k] * std::exp(std::complex<double>(0.0, angle));
            }
            out[n] /= (double)N;
        }
    };

    std::ofstream fout(outputFilename);
    fout << "z";
    for (int i = 0; i < N; ++i)
        fout << ",x" << i;
    fout << "\n";

    // Write initial field
    fout << 0.0;
    for (int i = 0; i < N; ++i)
        fout << "," << std::abs(field[i]);
    fout << "\n";

    for (int step = 1; step <= numStepsZ; ++step) {
        // Half-step refraction
        for (int i = 0; i < N; ++i)
            field[i] *= std::sqrt(refraction_phase[i]);

        // Full-step diffraction in Fourier domain
        dft(field, spectrum);
        for (int i = 0; i < N; ++i)
            spectrum[i] *= diffraction_phase[i];
        idft(spectrum, field);

        // Half-step refraction
        for (int i = 0; i < N; ++i)
            field[i] *= std::sqrt(refraction_phase[i]);

        // Snapshot
        if (step % snapshotEvery == 0) {
            double z = step * dz;
            fout << z;
            for (int i = 0; i < N; ++i)
                fout << "," << std::abs(field[i]);
            fout << "\n";
        }
    }
}

std::vector<std::complex<double>> NumericalSolverOpt::makeGaussianBeam(
    int numPoints, double dx, double w0, double x0, double k0)
{
    std::vector<std::complex<double>> field(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double x = (i - numPoints / 2) * dx - x0;
        double env = std::exp(-x * x / (w0 * w0));
        field[i] = env * std::exp(std::complex<double>(0.0, k0 * x));
    }
    return field;
}

std::vector<std::complex<double>> NumericalSolverOpt::makePlaneWave(
    int numPoints, double dx, double k, double amplitude)
{
    std::vector<std::complex<double>> field(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double x = (i - numPoints / 2) * dx;
        field[i] = amplitude * std::exp(std::complex<double>(0.0, k * x));
    }
    return field;
}
