#pragma once
#include "OpticalSystem.h"
#include "NumericalSolverOpt.h"
#include <string>
#include <vector>

// ── Data for a single plot curve ────────────────────────────────────────────
struct PlotCurve {
    std::vector<double> x;
    std::vector<double> y;
    std::string label;
};

// ── Main GUI application ────────────────────────────────────────────────────
class GuiApp {
public:
    GuiApp();
    void render();               // called once per frame

private:
    OpticalSystem system;        // shared across all simulations
    int  selectedSim = -1;       // -1 = nothing selected
    std::string resultText;      // text shown below parameters

    // Plot data (rebuilt on each "Compute")
    std::vector<PlotCurve> plotCurves;
    std::string plotTitle  = "Plot";
    std::string plotXLabel = "x";
    std::string plotYLabel = "y";

    void clearPlot();
    void addCurve(const std::string& label,
                  const std::vector<double>& x,
                  const std::vector<double>& y);

    // Layout helpers
    void renderSidebar();
    void renderParameters();
    void renderPlot();

    // One function per menu option (0-indexed internally)
    void renderSim01_SnellFresnel();
    void renderSim02_ThinLens();
    void renderSim03_ThickLens();
    void renderSim04_Mirror();
    void renderSim05_ABCD();
    void renderSim06_Prism();
    void renderSim07_Aberrations();
    void renderSim08_Instruments();
    void renderSim09_Fiber();
    void renderSim10_Fermat();
    void renderSim11_SingleSlit();
    void renderSim12_DoubleSlit();
    void renderSim13_Grating();
    void renderSim14_Airy();
    void renderSim15_FresnelDiffr();
    void renderSim16_ThinFilm();
    void renderSim17_Michelson();
    void renderSim18_FabryPerot();
    void renderSim19_Coherence();
    void renderSim20_FraunhoferGen();
    void renderSim21_Babinet();
    void renderSim22_Talbot();
    void renderSim23_Jones();
    void renderSim24_Stokes();
    void renderSim25_Mueller();
    void renderSim26_Malus();
    void renderSim27_Birefringence();
    void renderSim28_OpticalActivity();
    void renderSim29_FourierOptics();
    void renderSim30_SpatialFilter();
    void renderSim31_GaussianBeam();
    void renderSim32_BPM();
    void renderSim33_Holography();
    void renderSim34_SelfImaging();
    void renderSim35_LaserRate();
    void renderSim36_CavityModes();
    void renderSim37_QSwitchModeLock();
    void renderSim38_SHG();
    void renderSim39_KerrSPM();
    void renderSim40_OPA();
    void renderSim41_ElectroOptic();
    void renderSim42_AcoustoOptic();
    void renderSim43_PhotonStats();
    void renderSim44_HBT();
    void renderSim45_BeamSplitter();
    void renderSim46_MachZehnder();
    void renderSim47_SqueezedLight();
    void renderSim48_BB84();
    void renderSim49_Plasmonics();
    void renderSim50_Metamaterials();
};
