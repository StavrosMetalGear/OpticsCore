# OpticsCore

A comprehensive, interactive optics simulation engine and GUI application built with **C++17**. OpticsCore covers **50 physics simulations** spanning geometric optics, wave optics, polarization, Fourier optics, laser physics, nonlinear optics, quantum optics, plasmonics, and metamaterials — all visualized in real time with [Dear ImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot).

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue) ![CMake](https://img.shields.io/badge/CMake-3.15%2B-green) ![License](https://img.shields.io/badge/license-MIT-lightgrey)

---

## Features

### Geometric Optics (1–10)
| # | Simulation | Description |
|---|-----------|-------------|
| 1 | Snell's Law & Fresnel | Refraction, critical/Brewster angles, Fresnel coefficients |
| 2 | Thin Lens | Thin-lens equation, magnification, lens-maker equation |
| 3 | Thick Lens & Planes | Thick-lens focal length and principal planes |
| 4 | Mirror Optics | Spherical mirrors, mirror equation, magnification |
| 5 | ABCD Ray Matrices | Ray transfer matrices, multi-element ray tracing |
| 6 | Prism Optics | Deviation, Cauchy dispersion, Abbe number |
| 7 | Aberrations | Spherical, coma, astigmatism, field curvature, chromatic |
| 8 | Optical Instruments | Telescope, microscope, f-number, depth of field |
| 9 | Fiber Optics | NA, V-number, mode count, attenuation, dispersion |
| 10 | Fermat & Eikonal | Optical path length, eikonal phase |

### Wave Optics (11–22)
| # | Simulation | Description |
|---|-----------|-------------|
| 11 | Single Slit Diffraction | Fraunhofer single-slit intensity and minima |
| 12 | Double Slit Interference | Young's experiment, fringe spacing |
| 13 | Diffraction Grating | N-slit grating, resolving power, angular dispersion |
| 14 | Airy Pattern | Circular aperture, Rayleigh/Sparrow resolution |
| 15 | Fresnel Diffraction | Fresnel integrals, zone plates, edge diffraction |
| 16 | Thin Film Interference | Reflectance/transmittance, anti-reflection coatings |
| 17 | Michelson Interferometer | Fringe count, visibility, coherence length |
| 18 | Fabry-Perot | Transmission, finesse, FSR, resolving power |
| 19 | Coherence | Temporal/spatial coherence, van Cittert-Zernike |
| 20 | Fraunhofer (General) | Rectangular/circular aperture Fourier transforms |
| 21 | Babinet's Principle | Complementary screen diffraction |
| 22 | Talbot Effect | Self-imaging, fractional Talbot distances |

### Polarization (23–28)
| # | Simulation | Description |
|---|-----------|-------------|
| 23 | Jones Calculus | Jones vectors/matrices, polarizers, wave plates |
| 24 | Stokes & Poincaré | Stokes parameters, DOP, Poincaré sphere |
| 25 | Mueller Matrices | Mueller calculus for partially polarized light |
| 26 | Malus's Law | Polarizer chains, Brewster reflection |
| 27 | Birefringence | Phase delay, walk-off, ordinary/extraordinary rays |
| 28 | Optical Activity | Specific rotation, Faraday effect, Verdet constant |

### Fourier Optics & Beams (29–34)
| # | Simulation | Description |
|---|-----------|-------------|
| 29 | Fourier Optics | DFT of apertures, OTF/MTF |
| 30 | Spatial Filtering | Low-pass, high-pass, band-pass filters |
| 31 | Gaussian Beam | Waist, Rayleigh range, Gouy phase, ABCD propagation |
| 32 | Beam Propagation | Beam Propagation Method (BPM) simulation |
| 33 | Holography | Interference recording and reconstruction |
| 34 | Self-Imaging | Talbot / Lau self-imaging distances |

### Lasers & Nonlinear Optics (35–42)
| # | Simulation | Description |
|---|-----------|-------------|
| 35 | Laser Rate Equations | Gain medium dynamics, threshold, slope efficiency |
| 36 | Cavity Modes | FSR, transverse modes, stability diagram |
| 37 | Q-Switch / Mode-Lock | Pulse energy, peak power, pulse duration |
| 38 | Second Harmonic (SHG) | Phase matching, conversion efficiency |
| 39 | Kerr & SPM | Nonlinear refractive index, B-integral, self-focusing |
| 40 | Optical Parametric | OPA gain, idler wavelength, phase mismatch |
| 41 | Electro-Optic Effect | Pockels cell, half-wave voltage, Kerr cell |
| 42 | Acousto-Optic Effect | Bragg diffraction, frequency shift, bandwidth |

### Modern & Quantum Optics (43–50)
| # | Simulation | Description |
|---|-----------|-------------|
| 43 | Photon Statistics | Poisson, thermal, sub-Poissonian distributions |
| 44 | HBT Correlation | Hanbury Brown-Twiss, g²(τ), bunching |
| 45 | Beam Splitter / HOM | Quantum beam splitter, Hong-Ou-Mandel dip |
| 46 | Mach-Zehnder | Quantum interferometry, phase sensitivity |
| 47 | Squeezed Light | Quadrature squeezing, noise reduction |
| 48 | QKD (BB84) | Quantum key distribution, QBER, secure key rate |
| 49 | Plasmonics / SPR | Drude model, SPP dispersion, SPR resonance |
| 50 | Metamaterials | Negative refraction, perfect lens, cloaking |

---

## Architecture

```
OpticsCore/
├── CMakeLists.txt          # Build configuration (CMake 3.15+)
├── OpticalSystem.h/.cpp    # Physics engine — all 50 simulation computations
├── OpticsConstants.h       # SI physical constants (c, h, ħ, e, ε₀, μ₀, kB …)
├── OpticsExport.h          # DLL/shared-library export macros
├── NumericalSolverOpt.h/.cpp  # Numerical methods (BPM, Gaussian beams)
├── optics_c_api.h/.cpp     # C-compatible API wrappers
├── GuiApp.h/.cpp           # Dear ImGui / ImPlot GUI (sidebar + parameter panels + plots)
├── main.cpp                # GLFW window, OpenGL 3.3 context, main loop
└── pch.h                   # Precompiled header (MSVC)
```

The project is split into two layers:

- **`OpticsPhysics`** — a static library containing all physics computations, physical constants, and the C API. It has **zero GUI dependencies** and can be linked into any application.
- **`OpticsCore`** — the GUI executable that links `OpticsPhysics` with Dear ImGui, ImPlot, GLFW, and OpenGL to provide an interactive simulation environment.

---

## Building

### Prerequisites

| Tool | Minimum Version |
|------|----------------|
| CMake | 3.15 |
| C++ compiler | C++17 support (MSVC 2019+, GCC 9+, Clang 10+) |
| OpenGL | 3.3 core profile |
| Git | Required for FetchContent |

All other dependencies (**GLFW 3.4**, **Dear ImGui v1.91.8**, **ImPlot v0.16**) are fetched automatically via CMake `FetchContent`.

### Build Steps

```bash
# Clone the repository
git clone https://github.com/StavrosMetalGear/OpticsCore.git
cd OpticsCore

# Configure (Ninja generator recommended)
cmake -B build -G Ninja

# Build
cmake --build build

# Run
./build/OpticsCore
```

On **Windows with MSVC**, you can also open the folder directly in Visual Studio (CMake support) or use:

```powershell
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\Release\OpticsCore.exe
```

---

## Usage

1. Launch **OpticsCore** — a 1440×900 window opens with a sidebar, parameter panel, and plot area.
2. **Configure** the optical system (name, wavelength, refractive index) in the sidebar header.
3. **Select** any of the 50 simulations from the sidebar list.
4. **Adjust** parameters in the top-right panel and observe real-time plots in the bottom-right panel.
5. Most simulations support **CSV export** for external analysis.

---

## C API

The `optics_c_api.h` header exposes a C-compatible interface for embedding the physics engine in other languages or applications:

```c
#include "optics_c_api.h"

OP_Handle h = op_create("HeNe", 632.8e-9, 1.0);
double theta = op_snell_refraction(h, 0.5, 1.0, 1.5);
op_destroy(h);
```

---

## Physical Constants

All constants are defined in `OpticsConstants.h` using `inline constexpr` (SI units):

| Constant | Symbol | Value |
|----------|--------|-------|
| Speed of light | `c` | 2.998 × 10⁸ m/s |
| Planck constant | `h` | 6.626 × 10⁻³⁴ J·s |
| Reduced Planck | `ħ` | 1.055 × 10⁻³⁴ J·s |
| Elementary charge | `e` | 1.602 × 10⁻¹⁹ C |
| Vacuum permittivity | `ε₀` | 8.854 × 10⁻¹² F/m |
| Vacuum permeability | `μ₀` | 1.257 × 10⁻⁶ N/A² |
| Boltzmann constant | `kB` | 1.381 × 10⁻²³ J/K |
| Fine-structure | `α` | 1/137.036 |

Common laser wavelengths are also included (HeNe 632.8 nm, Nd:YAG 1064 nm, green 532 nm).

---

## License

This project is provided as-is for educational and research purposes.
