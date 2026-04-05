#pragma once

// ═══════════════════════════════════════════════════════════════════════════════
//  Shared physical constants for optics (SI units)
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace OpticsConstants {

    // Speed of light in vacuum
    inline constexpr double c      = 2.99792458e8;       // m/s

    // Planck constant and reduced Planck constant
    inline constexpr double h      = 6.62607015e-34;     // J·s
    inline constexpr double hbar   = 1.0545718e-34;      // J·s

    // Elementary charge
    inline constexpr double e      = 1.602176634e-19;    // C

    // Electron mass
    inline constexpr double me     = 9.1093837015e-31;   // kg

    // Vacuum permittivity
    inline constexpr double eps0   = 8.854187817e-12;    // F/m

    // Vacuum permeability
    inline constexpr double mu0    = 1.25663706212e-6;   // N/A^2

    // Boltzmann constant
    inline constexpr double kB     = 1.380649e-23;       // J/K

    // Stefan-Boltzmann constant
    inline constexpr double sigma_SB = 5.670374419e-8;   // W/(m^2 K^4)

    // Wien displacement constant
    inline constexpr double b_Wien = 2.897771955e-3;     // m·K

    // Fine-structure constant
    inline constexpr double alpha  = 1.0 / 137.035999084;

    // Bohr radius
    inline constexpr double a0     = 5.29177210903e-11;  // m

    // Energy conversions
    inline constexpr double eV     = 1.602176634e-19;    // J

    // Impedance of free space
    inline constexpr double Z0     = 376.730313668;      // Ohm

    // Common wavelengths (m)
    inline constexpr double lambda_HeNe  = 632.8e-9;     // HeNe laser
    inline constexpr double lambda_NdYAG = 1064e-9;      // Nd:YAG laser
    inline constexpr double lambda_green = 532e-9;        // Frequency-doubled Nd:YAG

} // namespace OpticsConstants
