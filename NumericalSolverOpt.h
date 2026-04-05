#pragma once

#include "OpticsExport.h"

#include <vector>
#include <string>
#include <complex>

class OPTICS_API NumericalSolverOpt {
public:
    // 1D FDTD (Finite-Difference Time-Domain) for EM wave propagation
    static void solveFDTD1D(
        double dx, double dt, int numPoints, int numSteps,
        const std::vector<double>& epsilon,
        const std::vector<double>& mu,
        double sourceFreq, int sourcePos,
        const std::string& outputFilename,
        int snapshotEvery = 10
    );

    // Beam Propagation Method (split-step Fourier)
    static void solveBPM(
        double dx, double dz, int numPointsX, int numStepsZ,
        double lambda, double n0,
        const std::vector<std::complex<double>>& initialField,
        const std::vector<double>& refractiveIndexProfile,
        const std::string& outputFilename,
        int snapshotEvery = 10
    );

    // Helper: Gaussian beam initial field
    static std::vector<std::complex<double>> makeGaussianBeam(
        int numPoints, double dx, double w0, double x0, double k0 = 0.0
    );

    // Helper: plane wave initial field
    static std::vector<std::complex<double>> makePlaneWave(
        int numPoints, double dx, double k, double amplitude = 1.0
    );
};
