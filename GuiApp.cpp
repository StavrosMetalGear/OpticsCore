#ifdef _MSC_VER
#include "pch.h"
#endif
#include "GuiApp.h"
#include "imgui.h"
#include "implot.h"

#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <complex>
#include <numeric>

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

// ── Constructor ─────────────────────────────────────────────────────────────
GuiApp::GuiApp()
    : system("HeNe Laser", 632.8e-9, 1.0) {}

// ── Plot helpers ────────────────────────────────────────────────────────────
void GuiApp::clearPlot() { plotCurves.clear(); }

void GuiApp::addCurve(const std::string& label,
                      const std::vector<double>& x,
                      const std::vector<double>& y)
{
    plotCurves.push_back({x, y, label});
}

// ── Main render (called every frame) ────────────────────────────────────────
void GuiApp::render()
{
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);

    ImGui::Begin("OpticsCore", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    float sideW  = 300.0f;
    float totalH = ImGui::GetContentRegionAvail().y;

    // ── Left: sidebar ───────────────────────────────────────────────────
    ImGui::BeginChild("##Sidebar", ImVec2(sideW, 0), true);
    renderSidebar();
    ImGui::EndChild();

    ImGui::SameLine();

    // ── Right: params (top) + plot (bottom) ─────────────────────────────
    ImGui::BeginGroup();
    float rightW = ImGui::GetContentRegionAvail().x;
    float paramH = totalH * 0.50f;

    ImGui::BeginChild("##Params", ImVec2(rightW, paramH), true);
    renderParameters();
    ImGui::EndChild();

    ImGui::BeginChild("##Plot", ImVec2(rightW, 0), true);
    renderPlot();
    ImGui::EndChild();

    ImGui::EndGroup();
    ImGui::End();
}

// ── Sidebar: simulation list + system configuration ─────────────────────────
void GuiApp::renderSidebar()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "OpticsCore");
    ImGui::Separator();

    // System configuration
    if (ImGui::CollapsingHeader("Optical System", ImGuiTreeNodeFlags_DefaultOpen)) {
        static char nameBuffer[64] = "HeNe Laser";
        ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));

        static double wl = 632.8e-9;
        static double n  = 1.0;
        ImGui::InputDouble("Wavelength (m)", &wl, 0, 0, "%.3e");
        ImGui::InputDouble("n (medium)",     &n,  0, 0, "%.4f");

        if (ImGui::Button("Apply")) {
            system.name       = nameBuffer;
            system.wavelength = wl;
            system.n_medium   = n;
        }
    }

    ImGui::Separator();
    ImGui::Text("Select Simulation:");
    ImGui::Spacing();

    static const char* names[] = {
        " 1  Snell's Law & Fresnel",
        " 2  Thin Lens",
        " 3  Thick Lens & Planes",
        " 4  Mirror Optics",
        " 5  ABCD Ray Matrices",
        " 6  Prism Optics",
        " 7  Aberrations",
        " 8  Optical Instruments",
        " 9  Fiber Optics",
        "10  Fermat & Eikonal",
        "11  Single Slit Diffraction",
        "12  Double Slit Interference",
        "13  Diffraction Grating",
        "14  Airy Pattern",
        "15  Fresnel Diffraction",
        "16  Thin Film Interference",
        "17  Michelson Interferometer",
        "18  Fabry-Perot",
        "19  Coherence",
        "20  Fraunhofer (General)",
        "21  Babinet's Principle",
        "22  Talbot Effect",
        "23  Jones Calculus",
        "24  Stokes & Poincare",
        "25  Mueller Matrices",
        "26  Malus's Law",
        "27  Birefringence",
        "28  Optical Activity",
        "29  Fourier Optics",
        "30  Spatial Filtering",
        "31  Gaussian Beam",
        "32  Beam Propagation",
        "33  Holography",
        "34  Self-Imaging",
        "35  Laser Rate Equations",
        "36  Cavity Modes",
        "37  Q-Switch / Mode-Lock",
        "38  Second Harmonic (SHG)",
        "39  Kerr & SPM",
        "40  Optical Parametric",
        "41  Electro-Optic Effect",
        "42  Acousto-Optic Effect",
        "43  Photon Statistics",
        "44  HBT Correlation",
        "45  Beam Splitter / HOM",
        "46  Mach-Zehnder",
        "47  Squeezed Light",
        "48  QKD (BB84)",
        "49  Plasmonics / SPR",
        "50  Metamaterials"
    };

    for (int i = 0; i < 50; ++i) {
        if (ImGui::Selectable(names[i], selectedSim == i))
            selectedSim = i;
    }
}

// ── Parameters panel: dispatch to the active simulation ─────────────────────
void GuiApp::renderParameters()
{
    if (selectedSim < 0) {
        ImGui::TextWrapped("Select a simulation from the sidebar.");
        return;
    }

    switch (selectedSim) {
    case  0: renderSim01_SnellFresnel();                  break;
    case  1: renderSim02_ThinLens();                      break;
    case  2: renderSim03_ThickLens();                     break;
    case  3: renderSim04_Mirror();                        break;
    case  4: renderSim05_ABCD();                          break;
    case  5: renderSim06_Prism();                         break;
    case  6: renderSim07_Aberrations();                   break;
    case  7: renderSim08_Instruments();                   break;
    case  8: renderSim09_Fiber();                         break;
    case  9: renderSim10_Fermat();                        break;
    case 10: renderSim11_SingleSlit();                    break;
    case 11: renderSim12_DoubleSlit();                    break;
    case 12: renderSim13_Grating();                       break;
    case 13: renderSim14_Airy();                          break;
    case 14: renderSim15_FresnelDiffr();                  break;
    case 15: renderSim16_ThinFilm();                      break;
    case 16: renderSim17_Michelson();                     break;
    case 17: renderSim18_FabryPerot();                    break;
    case 18: renderSim19_Coherence();                     break;
    case 19: renderSim20_FraunhoferGen();                 break;
    case 20: renderSim21_Babinet();                       break;
    case 21: renderSim22_Talbot();                        break;
    case 22: renderSim23_Jones();                         break;
    case 23: renderSim24_Stokes();                        break;
    case 24: renderSim25_Mueller();                       break;
    case 25: renderSim26_Malus();                         break;
    case 26: renderSim27_Birefringence();                 break;
    case 27: renderSim28_OpticalActivity();               break;
    case 28: renderSim29_FourierOptics();                 break;
    case 29: renderSim30_SpatialFilter();                 break;
    case 30: renderSim31_GaussianBeam();                  break;
    case 31: renderSim32_BPM();                           break;
    case 32: renderSim33_Holography();                    break;
    case 33: renderSim34_SelfImaging();                   break;
    case 34: renderSim35_LaserRate();                     break;
    case 35: renderSim36_CavityModes();                   break;
    case 36: renderSim37_QSwitchModeLock();               break;
    case 37: renderSim38_SHG();                           break;
    case 38: renderSim39_KerrSPM();                       break;
    case 39: renderSim40_OPA();                           break;
    case 40: renderSim41_ElectroOptic();                  break;
    case 41: renderSim42_AcoustoOptic();                  break;
    case 42: renderSim43_PhotonStats();                   break;
    case 43: renderSim44_HBT();                           break;
    case 44: renderSim45_BeamSplitter();                  break;
    case 45: renderSim46_MachZehnder();                   break;
    case 46: renderSim47_SqueezedLight();                 break;
    case 47: renderSim48_BB84();                          break;
    case 48: renderSim49_Plasmonics();                    break;
    case 49: renderSim50_Metamaterials();                 break;
    default: break;
    }

    // Show accumulated result text
    if (!resultText.empty()) {
        ImGui::Separator();
        ImGui::TextWrapped("%s", resultText.c_str());
    }
}

// ── Plot panel ──────────────────────────────────────────────────────────────
void GuiApp::renderPlot()
{
    if (plotCurves.empty()) {
        ImGui::TextDisabled("No plot data. Press 'Compute' in a simulation.");
        return;
    }

    if (ImPlot::BeginPlot(plotTitle.c_str(), ImVec2(-1, -1))) {
        ImPlot::SetupAxes(plotXLabel.c_str(), plotYLabel.c_str());
        for (auto& c : plotCurves)
            ImPlot::PlotLine(c.label.c_str(), c.x.data(), c.y.data(), (int)c.x.size());
        ImPlot::EndPlot();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  SIMULATION PANELS
// ═══════════════════════════════════════════════════════════════════════════

// ── 1: Snell's Law & Fresnel Equations ──
void GuiApp::renderSim01_SnellFresnel()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Snell's Law & Fresnel Equations");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Snell's Law & Fresnel Equations"))
    {
        ImGui::TextWrapped(
            "Snell's Law: n1 sin(theta_i) = n2 sin(theta_t). "
            "When n1 > n2, total internal reflection (TIR) occurs above the critical angle "
            "theta_c = arcsin(n2/n1). Brewster's angle theta_B = arctan(n2/n1) is where "
            "p-polarized reflectance vanishes. "
            "Fresnel equations give reflectance Rs, Rp and transmittance Ts, Tp "
            "for s- and p-polarizations as functions of incidence angle.");
    }

    static double n1 = 1.0;
    static double n2 = 1.5;
    static int numPoints = 200;
    ImGui::InputDouble("n1##Sim01", &n1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n2##Sim01", &n2, 0.01, 0.1, "%.4f");
    ImGui::SliderInt("Points##Sim01", &numPoints, 50, 1000);

    if (ImGui::Button("Compute##Sim01"))
    {
        clearPlot();
        std::vector<double> angles, rsVec, rpVec, tsVec, tpVec;
        double maxAngle = M_PI / 2.0 * 0.99;
        for (int i = 0; i < numPoints; ++i) {
            double theta = maxAngle * i / (numPoints - 1);
            double deg = theta * 180.0 / M_PI;
            angles.push_back(deg);
            rsVec.push_back(system.fresnelRs(theta, n1, n2));
            rpVec.push_back(system.fresnelRp(theta, n1, n2));
            tsVec.push_back(system.fresnelTs(theta, n1, n2));
            tpVec.push_back(system.fresnelTp(theta, n1, n2));
        }
        addCurve("Rs", angles, rsVec);
        addCurve("Rp", angles, rpVec);
        addCurve("Ts", angles, tsVec);
        addCurve("Tp", angles, tpVec);

        std::ostringstream oss;
        double crit = system.criticalAngle(n1, n2);
        double brew = system.brewsterAngle(n1, n2);
        oss << "Critical angle: " << (crit < 0 ? "N/A (n1 <= n2)" : std::to_string(crit * 180.0 / M_PI) + " deg") << "\n";
        oss << "Brewster angle: " << brew * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
        plotTitle = "Fresnel Coefficients";
        plotXLabel = "Angle (deg)";
        plotYLabel = "Reflectance / Transmittance";
    }
    if (ImGui::Button("Export CSV##Sim01"))
        system.exportSnellCSV("snell_fresnel.csv", n1, n2, numPoints);
}

// ── 2: Thin Lens ──
void GuiApp::renderSim02_ThinLens()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Thin Lens");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Thin Lens"))
    {
        ImGui::TextWrapped(
            "Thin lens equation: 1/f = 1/do + 1/di. "
            "Magnification M = -di/do. "
            "Lensmaker's equation: 1/f = (n-1)(1/R1 - 1/R2). "
            "Positive f = converging lens, negative = diverging. "
            "Real images form when do > f; virtual images when do < f.");
    }

    static double f = 0.1;
    static double objMin = 0.05;
    static double objMax = 1.0;
    static double n_lens = 1.5;
    static double R1 = 0.2;
    static double R2 = -0.2;
    static int numPoints = 200;
    ImGui::InputDouble("Focal length (m)##Sim02", &f, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Obj min (m)##Sim02", &objMin, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Obj max (m)##Sim02", &objMax, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n_lens##Sim02", &n_lens, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("R1 (m)##Sim02", &R1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("R2 (m)##Sim02", &R2, 0.01, 0.1, "%.4f");
    ImGui::SliderInt("Points##Sim02", &numPoints, 50, 1000);

    if (ImGui::Button("Compute##Sim02"))
    {
        clearPlot();
        std::vector<double> doVec, diVec, magVec;
        for (int i = 0; i < numPoints; ++i) {
            double dobj = objMin + (objMax - objMin) * i / (numPoints - 1);
            double di = system.thinLensEquation(dobj, f);
            double mag = system.magnification(dobj, di);
            doVec.push_back(dobj);
            diVec.push_back(di);
            magVec.push_back(mag);
        }
        addCurve("Image dist", doVec, diVec);
        addCurve("Magnification", doVec, magVec);

        std::ostringstream oss;
        double fCalc = 1.0 / system.lensMakerEquation(n_lens, R1, R2);
        oss << "Lensmaker f = " << fCalc << " m\n";
        oss << "Power = " << system.thinLensPower(f) << " D\n";
        resultText = oss.str();
        plotTitle = "Thin Lens Imaging";
        plotXLabel = "Object distance (m)";
        plotYLabel = "Image distance (m) / Magnification";
    }
    if (ImGui::Button("Export CSV##Sim02"))
        system.exportThinLensCSV("thin_lens.csv", f, objMin, objMax, numPoints);
}

// ── 3: Thick Lens & Principal Planes ──
void GuiApp::renderSim03_ThickLens()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Thick Lens & Principal Planes");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Thick Lens & Principal Planes"))
    {
        ImGui::TextWrapped(
            "For a thick lens with surfaces R1, R2 and thickness d, the effective focal length "
            "accounts for the separation between refracting surfaces. Principal planes H, H' are "
            "reference planes from which focal length is measured. The system can be treated as a "
            "thin lens located at the principal planes.");
    }

    static double n_lens = 1.5;
    static double R1 = 0.1;
    static double R2 = -0.1;
    static double d = 0.01;
    static int numPoints = 200;
    ImGui::InputDouble("n_lens##Sim03", &n_lens, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("R1 (m)##Sim03", &R1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("R2 (m)##Sim03", &R2, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Thickness d (m)##Sim03", &d, 0.001, 0.01, "%.4f");
    ImGui::SliderInt("Points##Sim03", &numPoints, 50, 500);

    if (ImGui::Button("Compute##Sim03"))
    {
        clearPlot();
        std::vector<double> dVec, fVec;
        for (int i = 0; i < numPoints; ++i) {
            double dd = 0.001 + 0.05 * i / (numPoints - 1);
            double ff = system.thickLensFocalLength(n_lens, R1, R2, dd);
            dVec.push_back(dd * 1000.0);
            fVec.push_back(ff * 1000.0);
        }
        addCurve("EFL vs thickness", dVec, fVec);

        double efl = system.thickLensFocalLength(n_lens, R1, R2, d);
        double fpp = system.thickLensFrontPrincipalPlane(n_lens, R1, R2, d);
        double bpp = system.thickLensBackPrincipalPlane(n_lens, R1, R2, d);
        std::ostringstream oss;
        oss << "EFL = " << efl * 1000.0 << " mm\n";
        oss << "Front principal plane H = " << fpp * 1000.0 << " mm\n";
        oss << "Back principal plane H' = " << bpp * 1000.0 << " mm\n";
        resultText = oss.str();
        plotTitle = "Thick Lens EFL vs Thickness";
        plotXLabel = "Thickness (mm)";
        plotYLabel = "EFL (mm)";
    }
    if (ImGui::Button("Export CSV##Sim03"))
        system.exportThickLensCSV("thick_lens.csv", n_lens, R1, R2, d, numPoints);
}

// ── 4: Mirror Optics ──
void GuiApp::renderSim04_Mirror()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Mirror Optics");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Mirror Optics"))
    {
        ImGui::TextWrapped(
            "Spherical mirror focal length f = R/2. Mirror equation: 1/f = 1/do + 1/di. "
            "Concave mirrors (R > 0) converge light; convex mirrors (R < 0) diverge. "
            "Magnification M = -di/do. Real images are inverted; virtual images are upright.");
    }

    static double R = 0.2;
    static double objMin = 0.05;
    static double objMax = 1.0;
    static int numPoints = 200;
    ImGui::InputDouble("Radius of curvature R (m)##Sim04", &R, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Obj min (m)##Sim04", &objMin, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Obj max (m)##Sim04", &objMax, 0.01, 0.1, "%.4f");
    ImGui::SliderInt("Points##Sim04", &numPoints, 50, 1000);

    if (ImGui::Button("Compute##Sim04"))
    {
        clearPlot();
        double f = system.sphericalMirrorFocal(R);
        std::vector<double> doVec, diVec, magVec;
        for (int i = 0; i < numPoints; ++i) {
            double dobj = objMin + (objMax - objMin) * i / (numPoints - 1);
            double di = system.mirrorEquation(dobj, f);
            double mag = system.mirrorMagnification(dobj, di);
            doVec.push_back(dobj);
            diVec.push_back(di);
            magVec.push_back(mag);
        }
        addCurve("Image dist", doVec, diVec);
        addCurve("Magnification", doVec, magVec);

        std::ostringstream oss;
        oss << "Focal length f = R/2 = " << f << " m\n";
        resultText = oss.str();
        plotTitle = "Mirror Imaging";
        plotXLabel = "Object distance (m)";
        plotYLabel = "Image dist (m) / Magnification";
    }
    if (ImGui::Button("Export CSV##Sim04"))
        system.exportMirrorCSV("mirror.csv", R, objMin, objMax, numPoints);
}

// ── 5: ABCD Ray Transfer Matrices ──
void GuiApp::renderSim05_ABCD()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "ABCD Ray Transfer Matrices");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: ABCD Ray Transfer Matrices"))
    {
        ImGui::TextWrapped(
            "A ray is described by (y, theta). ABCD matrices transform rays through "
            "optical elements: free space [[1,d],[0,1]], thin lens [[1,0],[-1/f,1]], "
            "spherical mirror [[1,0],[-2/R,1]]. Multiply matrices for sequential elements. "
            "System matrix M gives output ray (y',theta') = M * (y,theta).");
    }

    static double d1 = 0.1;
    static double f_lens = 0.05;
    static double d2 = 0.1;
    static double y0 = 0.01;
    static double theta0 = 0.0;
    static int numPoints = 100;
    ImGui::InputDouble("Free space d1 (m)##Sim05", &d1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Lens f (m)##Sim05", &f_lens, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Free space d2 (m)##Sim05", &d2, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("y0 (m)##Sim05", &y0, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("theta0 (rad)##Sim05", &theta0, 0.001, 0.01, "%.4f");
    ImGui::SliderInt("Points##Sim05", &numPoints, 10, 500);

    if (ImGui::Button("Compute##Sim05"))
    {
        clearPlot();
        std::vector<double> zVec, yVec;
        double y = y0, th = theta0;
        double totalLen = d1 + d2;
        double dz = totalLen / numPoints;
        for (int i = 0; i <= numPoints; ++i) {
            double z = dz * i;
            zVec.push_back(z);
            if (z <= d1) {
                auto [yy, tt] = system.abcdTraceRay(system.abcdFreeSpace(z), y0, theta0);
                yVec.push_back(yy);
            } else {
                auto M1 = system.abcdFreeSpace(d1);
                auto ML = system.abcdThinLens(f_lens);
                auto M2 = system.abcdFreeSpace(z - d1);
                auto Msys = system.abcdMultiply(M2, system.abcdMultiply(ML, M1));
                auto [yy, tt] = system.abcdTraceRay(Msys, y0, theta0);
                yVec.push_back(yy);
            }
        }
        addCurve("Ray height", zVec, yVec);

        auto Mtotal = system.abcdMultiply(system.abcdFreeSpace(d2),
                        system.abcdMultiply(system.abcdThinLens(f_lens), system.abcdFreeSpace(d1)));
        auto [yf, tf] = system.abcdTraceRay(Mtotal, y0, theta0);
        std::ostringstream oss;
        oss << "Output: y = " << yf * 1000.0 << " mm, theta = " << tf << " rad\n";
        oss << "ABCD: [[" << Mtotal[0][0] << ", " << Mtotal[0][1] << "], [" << Mtotal[1][0] << ", " << Mtotal[1][1] << "]]\n";
        resultText = oss.str();
        plotTitle = "Ray Trace through Lens";
        plotXLabel = "z (m)";
        plotYLabel = "y (m)";
    }
    if (ImGui::Button("Export CSV##Sim05")) {
        std::vector<ABCDMatrix> elems = {system.abcdFreeSpace(d1), system.abcdThinLens(f_lens), system.abcdFreeSpace(d2)};
        std::vector<double> lengths = {d1, 0.0, d2};
        system.exportABCDRayTraceCSV("abcd_ray.csv", elems, lengths, y0, theta0, numPoints);
    }
}

// ── 6: Prism Optics ──
void GuiApp::renderSim06_Prism()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Prism Optics");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Prism Optics"))
    {
        ImGui::TextWrapped(
            "A prism with apex angle A and index n deflects light by deviation angle "
            "delta = theta_i + theta_t - A. Minimum deviation occurs at symmetric passage: "
            "delta_min = 2*arcsin(n*sin(A/2)) - A. Cauchy dispersion: n(lambda) = A + B/lambda^2. "
            "Abbe number V = (nD-1)/(nF-nC) quantifies dispersion.");
    }

    static double n_prism = 1.5;
    static double apex = 60.0;
    static int numPoints = 200;
    ImGui::InputDouble("n_prism##Sim06", &n_prism, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Apex angle (deg)##Sim06", &apex, 1.0, 5.0, "%.2f");
    ImGui::SliderInt("Points##Sim06", &numPoints, 50, 500);

    if (ImGui::Button("Compute##Sim06"))
    {
        clearPlot();
        double apexRad = apex * M_PI / 180.0;
        std::vector<double> angles, devs;
        for (int i = 0; i < numPoints; ++i) {
            double theta = 0.01 + (M_PI / 2.0 - 0.02) * i / (numPoints - 1);
            double dev = system.prismDeviation(theta, n_prism, apexRad);
            if (dev >= 0.0) {
                angles.push_back(theta * 180.0 / M_PI);
                devs.push_back(dev * 180.0 / M_PI);
            }
        }
        addCurve("Deviation", angles, devs);

        double dmin = system.prismMinDeviation(n_prism, apexRad);
        std::ostringstream oss;
        oss << "Min deviation = " << dmin * 180.0 / M_PI << " deg\n";
        oss << "Abbe number (example nD=1.5230, nF=1.5280, nC=1.5200) = "
            << system.abbeNumber(1.5230, 1.5280, 1.5200) << "\n";
        resultText = oss.str();
        plotTitle = "Prism Deviation";
        plotXLabel = "Incidence angle (deg)";
        plotYLabel = "Deviation (deg)";
    }
    if (ImGui::Button("Export CSV##Sim06"))
        system.exportPrismDeviationCSV("prism.csv", n_prism, apex * M_PI / 180.0, numPoints);
}

// ── 7: Aberrations ──
void GuiApp::renderSim07_Aberrations()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Aberrations");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Aberrations"))
    {
        ImGui::TextWrapped(
            "Seidel aberrations: spherical (h^3 dependence on ray height), coma (off-axis "
            "comet-like blur), astigmatism (different foci for sagittal/tangential planes), "
            "field curvature (Petzval surface), and chromatic aberration (focus shift with wavelength, "
            "longitudinal CA = -f/V where V is Abbe number).");
    }

    static double f = 0.1;
    static double n = 1.5;
    static double R = 0.1;
    static double V = 60.0;
    static int numPoints = 200;
    ImGui::InputDouble("Focal length f (m)##Sim07", &f, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n##Sim07", &n, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("R (m)##Sim07", &R, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Abbe V##Sim07", &V, 1.0, 10.0, "%.2f");
    ImGui::SliderInt("Points##Sim07", &numPoints, 50, 500);

    if (ImGui::Button("Compute##Sim07"))
    {
        clearPlot();
        std::vector<double> hVec, saVec, comaVec;
        for (int i = 0; i < numPoints; ++i) {
            double h_val = 0.001 + 0.05 * i / (numPoints - 1);
            hVec.push_back(h_val * 1000.0);
            saVec.push_back(system.sphericalAberrationCoeff(n, R, h_val) * 1e6);
            comaVec.push_back(system.comaCoeff(n, R, h_val, 0.1) * 1e6);
        }
        addCurve("Spherical", hVec, saVec);
        addCurve("Coma (0.1 rad)", hVec, comaVec);

        std::ostringstream oss;
        oss << "Chromatic aberration (longitudinal) = " << system.chromaticAberrationLongitudinal(f, V) * 1000.0 << " mm\n";
        oss << "Field curvature radius = " << system.fieldCurvatureRadius(f) * 1000.0 << " mm\n";
        oss << "Astigmatism coeff (0.1 rad) = " << system.astigmatismCoeff(f, 0.1) * 1e6 << " um\n";
        resultText = oss.str();
        plotTitle = "Aberration Coefficients";
        plotXLabel = "Ray height (mm)";
        plotYLabel = "Aberration (um)";
    }
    if (ImGui::Button("Export CSV##Sim07"))
        system.exportAberrationCSV("aberrations.csv", f, n, R, numPoints);
}

// ── 8: Optical Instruments ──
void GuiApp::renderSim08_Instruments()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Optical Instruments");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Optical Instruments"))
    {
        ImGui::TextWrapped(
            "Telescope magnification M = -f_obj/f_eye. Resolving power theta = 1.22 lambda/D. "
            "Microscope: M = -(L/f_obj)(25cm/f_eye), NA = n*sin(alpha). "
            "Rayleigh criterion: d_min = 0.61 lambda/NA. f-number N = f/D. "
            "Depth of field DOF = 2Ncs^2/f^2.");
    }

    static int mode = 0;
    const char* modes[] = {"Telescope", "Microscope"};
    ImGui::Combo("Mode##Sim08", &mode, modes, 2);

    static double f_obj = 1.0, f_eye = 0.025, D_aperture = 0.1;
    static double L_tube = 0.16, n_med = 1.0, alpha_half = 0.5;
    static double lambda = 550e-9;
    ImGui::InputDouble("f_obj (m)##Sim08", &f_obj, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("f_eye (m)##Sim08", &f_eye, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Aperture D (m)##Sim08", &D_aperture, 0.01, 0.1, "%.4f");
    if (mode == 1) {
        ImGui::InputDouble("Tube length (m)##Sim08", &L_tube, 0.01, 0.1, "%.4f");
        ImGui::InputDouble("n medium##Sim08", &n_med, 0.1, 0.5, "%.4f");
        ImGui::InputDouble("Half-angle (rad)##Sim08", &alpha_half, 0.01, 0.1, "%.4f");
    }
    ImGui::InputDouble("Lambda (m)##Sim08", &lambda, 1e-9, 1e-8, "%.3e");

    if (ImGui::Button("Compute##Sim08"))
    {
        clearPlot();
        std::ostringstream oss;
        if (mode == 0) {
            double mag = system.telescopeMagnification(f_obj, f_eye);
            double res = system.telescopeResolvingPower(D_aperture, lambda);
            double fN = system.fNumber(f_obj, D_aperture);
            oss << "Telescope magnification = " << mag << "x\n";
            oss << "Angular resolution = " << res * 1e6 << " urad\n";
            oss << "f/# = " << fN << "\n";

            std::vector<double> dVec, resVec;
            for (int i = 1; i <= 100; ++i) {
                double dd = 0.01 * i;
                dVec.push_back(dd * 100.0);
                resVec.push_back(system.telescopeResolvingPower(dd, lambda) * 1e6);
            }
            addCurve("Resolution vs D", dVec, resVec);
            plotTitle = "Telescope Resolution";
            plotXLabel = "Aperture (cm)";
            plotYLabel = "Resolution (urad)";
        } else {
            double mag = system.microscopeMagnification(f_obj, f_eye, L_tube);
            double na = system.microscopeNA(n_med, alpha_half);
            double res = system.rayleighCriterion(lambda, na);
            oss << "Microscope magnification = " << mag << "x\n";
            oss << "NA = " << na << "\n";
            oss << "Rayleigh resolution = " << res * 1e9 << " nm\n";

            std::vector<double> naVec, resVec;
            for (int i = 1; i <= 100; ++i) {
                double na_i = 0.01 * i * 1.5;
                naVec.push_back(na_i);
                resVec.push_back(system.rayleighCriterion(lambda, na_i) * 1e9);
            }
            addCurve("Resolution vs NA", naVec, resVec);
            plotTitle = "Microscope Resolution";
            plotXLabel = "NA";
            plotYLabel = "Resolution (nm)";
        }
        resultText = oss.str();
    }
    if (ImGui::Button("Export CSV##Sim08"))
        system.exportInstrumentCSV("instruments.csv", mode, f_obj, f_eye, 100);
}

// ── 9: Fiber Optics ──
void GuiApp::renderSim09_Fiber()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Fiber Optics");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Fiber Optics"))
    {
        ImGui::TextWrapped(
            "Fiber NA = sqrt(n_core^2 - n_clad^2). Acceptance angle theta_a = arcsin(NA). "
            "V-number V = 2*pi*a*NA/lambda determines mode count (~V^2/2 for step-index). "
            "Single-mode cutoff at V = 2.405. Attenuation in dB = -10 log10(Pout/Pin). "
            "Dispersion: pulse broadening dt = D * L * delta_lambda.");
    }

    static double n_core = 1.48;
    static double n_clad = 1.46;
    static double a_radius = 25e-6;
    static double lambda = 1550e-9;
    static int numPoints = 200;
    ImGui::InputDouble("n_core##Sim09", &n_core, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("n_clad##Sim09", &n_clad, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Core radius a (m)##Sim09", &a_radius, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim09", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim09", &numPoints, 50, 500);

    if (ImGui::Button("Compute##Sim09"))
    {
        clearPlot();
        double na = system.fiberNA(n_core, n_clad);
        std::vector<double> lamVec, vVec;
        for (int i = 0; i < numPoints; ++i) {
            double lam = 400e-9 + (1700e-9 - 400e-9) * i / (numPoints - 1);
            double v = system.fiberVNumber(a_radius, na, lam);
            lamVec.push_back(lam * 1e9);
            vVec.push_back(v);
        }
        addCurve("V-number", lamVec, vVec);

        double V = system.fiberVNumber(a_radius, na, lambda);
        int modes = system.fiberNumberOfModes(V);
        double cutoff = system.fiberModeCutoff(a_radius, na);

        std::ostringstream oss;
        oss << "NA = " << na << "\n";
        oss << "Acceptance angle = " << system.fiberAcceptanceAngle(n_core, n_clad) * 180.0 / M_PI << " deg\n";
        oss << "V-number = " << V << "\n";
        oss << "Number of modes ~ " << modes << "\n";
        oss << "Single-mode cutoff wavelength = " << cutoff * 1e9 << " nm\n";
        resultText = oss.str();
        plotTitle = "Fiber V-number vs Wavelength";
        plotXLabel = "Wavelength (nm)";
        plotYLabel = "V-number";
    }
    if (ImGui::Button("Export CSV##Sim09"))
        system.exportFiberCSV("fiber.csv", n_core, n_clad, a_radius, numPoints);
}

// ── 10: Fermat's Principle & Eikonal ──
void GuiApp::renderSim10_Fermat()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Fermat's Principle & Eikonal");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Fermat's Principle & Eikonal"))
    {
        ImGui::TextWrapped(
            "Fermat's principle: light travels the path of stationary optical path length (OPL). "
            "OPL = integral n(s) ds. For uniform media, OPL = n * d. "
            "Phase phi = 2*pi*OPL/lambda. The eikonal equation relates the gradient of phase "
            "to the refractive index. Snell's law is a direct consequence of Fermat's principle.");
    }

    static double n1 = 1.0;
    static double n2 = 1.5;
    static double d_total = 0.1;
    static int numPoints = 200;
    ImGui::InputDouble("n1##Sim10", &n1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n2##Sim10", &n2, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Total path (m)##Sim10", &d_total, 0.01, 0.1, "%.4f");
    ImGui::SliderInt("Points##Sim10", &numPoints, 50, 500);

    if (ImGui::Button("Compute##Sim10"))
    {
        clearPlot();
        double lambda = system.wavelength;
        std::vector<double> dVec, opl1Vec, opl2Vec, phaseVec;
        for (int i = 0; i < numPoints; ++i) {
            double d = d_total * i / (numPoints - 1);
            dVec.push_back(d * 1000.0);
            opl1Vec.push_back(system.opticalPathLength(n1, d));
            opl2Vec.push_back(system.opticalPathLength(n2, d));
            phaseVec.push_back(system.eikonalPhase(system.opticalPathLength(n1, d), lambda));
        }
        addCurve("OPL (n1)", dVec, opl1Vec);
        addCurve("OPL (n2)", dVec, opl2Vec);

        std::vector<double> ns = {n1, n2};
        std::vector<double> ds = {d_total / 2.0, d_total / 2.0};
        double totalOpl = system.totalOPL(ns, ds);
        std::ostringstream oss;
        oss << "Total OPL (half n1 + half n2) = " << totalOpl << " m\n";
        oss << "Phase (n1 path) = " << system.eikonalPhase(system.opticalPathLength(n1, d_total), lambda) << " rad\n";
        resultText = oss.str();
        plotTitle = "Optical Path Length";
        plotXLabel = "Distance (mm)";
        plotYLabel = "OPL (m)";
    }
    if (ImGui::Button("Export CSV##Sim10"))
        system.exportOPLComparisonCSV("fermat_opl.csv", n1, n2, d_total, numPoints);
}

// ── 11: Single Slit Diffraction ──
void GuiApp::renderSim11_SingleSlit()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Single Slit Diffraction");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Single Slit Diffraction"))
    {
        ImGui::TextWrapped(
            "Fraunhofer diffraction from a slit of width a: I(theta) = I0 * sinc^2(beta) "
            "where beta = pi*a*sin(theta)/lambda. Minima at sin(theta) = m*lambda/a (m = +/-1,2,...). "
            "Central maximum has angular width 2*lambda/a.");
    }

    static double a = 50e-6;
    static double lambda = 632.8e-9;
    static int numPoints = 500;
    ImGui::InputDouble("Slit width a (m)##Sim11", &a, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim11", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim11", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim11"))
    {
        clearPlot();
        std::vector<double> angles, intensity;
        double maxAngle = 5.0 * lambda / a;
        for (int i = 0; i < numPoints; ++i) {
            double theta = -maxAngle + 2.0 * maxAngle * i / (numPoints - 1);
            angles.push_back(theta * 1e3);
            intensity.push_back(system.singleSlitIntensity(theta, a, lambda));
        }
        addCurve("I(theta)", angles, intensity);

        std::ostringstream oss;
        oss << "First minimum at theta = +/-" << system.singleSlitMinima(1, a, lambda) * 1e3 << " mrad\n";
        oss << "Second minimum at theta = +/-" << system.singleSlitMinima(2, a, lambda) * 1e3 << " mrad\n";
        resultText = oss.str();
        plotTitle = "Single Slit Diffraction";
        plotXLabel = "Angle (mrad)";
        plotYLabel = "Normalized Intensity";
    }
    if (ImGui::Button("Export CSV##Sim11"))
        system.exportSingleSlitCSV("single_slit.csv", a, lambda, numPoints);
}

// ── 12: Double Slit Interference ──
void GuiApp::renderSim12_DoubleSlit()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Double Slit Interference");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Double Slit Interference"))
    {
        ImGui::TextWrapped(
            "Two slits of width a separated by d produce a combined diffraction-interference pattern: "
            "I = I_single * cos^2(pi*d*sin(theta)/lambda). The single-slit envelope modulates "
            "the double-slit fringes. Fringe spacing on screen: dx = lambda*L/d.");
    }

    static double a = 20e-6;
    static double d = 100e-6;
    static double lambda = 632.8e-9;
    static int numPoints = 500;
    ImGui::InputDouble("Slit width a (m)##Sim12", &a, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Slit separation d (m)##Sim12", &d, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim12", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim12", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim12"))
    {
        clearPlot();
        std::vector<double> angles, intensity, envelope;
        double maxAngle = 5.0 * lambda / a;
        for (int i = 0; i < numPoints; ++i) {
            double theta = -maxAngle + 2.0 * maxAngle * i / (numPoints - 1);
            angles.push_back(theta * 1e3);
            intensity.push_back(system.doubleSlitIntensity(theta, a, d, lambda));
            envelope.push_back(system.singleSlitIntensity(theta, a, lambda) * 4.0);
        }
        addCurve("Double slit I", angles, intensity);
        addCurve("Envelope", angles, envelope);

        std::ostringstream oss;
        oss << "Fringe spacing (at L=1m) = " << system.doubleSlitFringeSpacing(d, lambda, 1.0) * 1e3 << " mm\n";
        oss << "First max at theta = " << system.doubleSlitMaxima(1, d, lambda) * 1e3 << " mrad\n";
        resultText = oss.str();
        plotTitle = "Double Slit Interference";
        plotXLabel = "Angle (mrad)";
        plotYLabel = "Intensity (a.u.)";
    }
    if (ImGui::Button("Export CSV##Sim12"))
        system.exportDoubleSlitCSV("double_slit.csv", a, d, lambda, numPoints);
}

// ── 13: N-Slit Diffraction Grating ──
void GuiApp::renderSim13_Grating()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "N-Slit Diffraction Grating");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: N-Slit Diffraction Grating"))
    {
        ImGui::TextWrapped(
            "N slits of width a, period d: intensity includes (sin(N*delta)/sin(delta))^2 factor. "
            "Principal maxima at d*sin(theta)=m*lambda. Resolving power R = mN. "
            "FSR = lambda/m. Angular dispersion d(theta)/d(lambda) = m/(d*cos(theta)).");
    }

    static double a = 5e-6;
    static double d = 10e-6;
    static double lambda = 632.8e-9;
    static int N = 10;
    static int numPoints = 1000;
    ImGui::InputDouble("Slit width a (m)##Sim13", &a, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Period d (m)##Sim13", &d, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim13", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("N slits##Sim13", &N, 2, 100);
    ImGui::SliderInt("Points##Sim13", &numPoints, 200, 5000);

    if (ImGui::Button("Compute##Sim13"))
    {
        clearPlot();
        std::vector<double> angles, intensity;
        double maxAngle = 3.0 * lambda / d;
        for (int i = 0; i < numPoints; ++i) {
            double theta = -maxAngle + 2.0 * maxAngle * i / (numPoints - 1);
            angles.push_back(theta * 1e3);
            intensity.push_back(system.gratingIntensity(theta, a, d, lambda, N));
        }
        addCurve("Grating I", angles, intensity);

        std::ostringstream oss;
        oss << "Resolving power (m=1) = " << system.gratingResolvingPower(1, N) << "\n";
        oss << "FSR (m=1) = " << system.gratingFreeSpectralRange(1, lambda) * 1e9 << " nm\n";
        oss << "Angular dispersion (m=1) = " << system.gratingAngularDispersion(1, d, 0.0) << " rad/m\n";
        resultText = oss.str();
        plotTitle = "Diffraction Grating";
        plotXLabel = "Angle (mrad)";
        plotYLabel = "Normalized Intensity";
    }
    if (ImGui::Button("Export CSV##Sim13"))
        system.exportGratingCSV("grating.csv", a, d, lambda, N, numPoints);
}

// ── 14: Airy Pattern (Circular Aperture) ──
void GuiApp::renderSim14_Airy()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Airy Pattern (Circular Aperture)");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Airy Pattern (Circular Aperture)"))
    {
        ImGui::TextWrapped(
            "Circular aperture of diameter D produces the Airy pattern: I = (2*J1(x)/x)^2 "
            "where x = pi*D*sin(theta)/lambda. First zero (Airy disk) at 1.22*lambda/D. "
            "Rayleigh criterion: two points resolved when separated by the Airy disk radius. "
            "Sparrow criterion gives slightly tighter limit at 0.95*lambda/D.");
    }

    static double D = 0.01;
    static double lambda = 632.8e-9;
    static int numPoints = 500;
    ImGui::InputDouble("Diameter D (m)##Sim14", &D, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Wavelength (m)##Sim14", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim14", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim14"))
    {
        clearPlot();
        std::vector<double> angles, intensity;
        double maxAngle = 5.0 * lambda / D;
        for (int i = 0; i < numPoints; ++i) {
            double theta = -maxAngle + 2.0 * maxAngle * i / (numPoints - 1);
            angles.push_back(theta * 1e6);
            intensity.push_back(system.airyIntensity(theta, D, lambda));
        }
        addCurve("Airy pattern", angles, intensity);

        std::ostringstream oss;
        oss << "Airy disk angular radius = " << system.airyFirstZero(lambda, D) * 1e6 << " urad\n";
        oss << "Airy disk radius (f=1m) = " << system.airyDiskRadius(lambda, 1.0, D) * 1e6 << " um\n";
        oss << "Rayleigh resolution = " << system.rayleighResolution(lambda, D) * 1e6 << " urad\n";
        oss << "Sparrow resolution = " << system.sparrowResolution(lambda, D) * 1e6 << " urad\n";
        resultText = oss.str();
        plotTitle = "Airy Diffraction Pattern";
        plotXLabel = "Angle (urad)";
        plotYLabel = "Normalized Intensity";
    }
    if (ImGui::Button("Export CSV##Sim14"))
        system.exportAiryCSV("airy.csv", D, lambda, numPoints);
}

// ── 15: Fresnel Diffraction ──
void GuiApp::renderSim15_FresnelDiffr()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Fresnel Diffraction");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Fresnel Diffraction"))
    {
        ImGui::TextWrapped(
            "Near-field (Fresnel) diffraction: intensity computed via Fresnel integrals C(u) and S(u). "
            "Edge diffraction shows oscillations near the shadow boundary. "
            "Fresnel zones: radius r_n = sqrt(n*lambda*z). "
            "Fresnel number N_F = a^2/(lambda*z) determines near/far field regime.");
    }

    static double lambda = 632.8e-9;
    static double z = 1.0;
    static double xRange = 5e-3;
    static int numPoints = 500;
    ImGui::InputDouble("Wavelength (m)##Sim15", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Distance z (m)##Sim15", &z, 0.1, 1.0, "%.4f");
    ImGui::InputDouble("x range (m)##Sim15", &xRange, 1e-4, 1e-3, "%.3e");
    ImGui::SliderInt("Points##Sim15", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim15"))
    {
        clearPlot();
        std::vector<double> xVec, iVec;
        for (int i = 0; i < numPoints; ++i) {
            double x = -xRange + 2.0 * xRange * i / (numPoints - 1);
            xVec.push_back(x * 1e3);
            iVec.push_back(system.fresnelDiffractionEdge(x, lambda, z));
        }
        addCurve("Edge diffraction", xVec, iVec);

        std::ostringstream oss;
        for (int n = 1; n <= 5; ++n)
            oss << "Zone " << n << " radius = " << system.fresnelZoneRadius(n, lambda, z) * 1e3 << " mm\n";
        oss << "Fresnel number (a=1mm) = " << system.fresnelNumber(1e-3, lambda, z) << "\n";
        resultText = oss.str();
        plotTitle = "Fresnel Edge Diffraction";
        plotXLabel = "x (mm)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim15"))
        system.exportFresnelEdgeCSV("fresnel_edge.csv", lambda, z, numPoints);
}

// ── 16: Thin Film Interference ──
void GuiApp::renderSim16_ThinFilm()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Thin Film Interference");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Thin Film Interference"))
    {
        ImGui::TextWrapped(
            "Light reflected from top and bottom of a thin film interferes. "
            "Reflectance depends on n1, n_film, n2, thickness d, wavelength, and angle. "
            "Constructive interference at d = m*lambda/(2*n_film). "
            "Anti-reflection coating: d = lambda/(4*n_film), n_film = sqrt(n1*n2).");
    }

    static double n1 = 1.0;
    static double n_film = 1.38;
    static double n2 = 1.5;
    static double d = 100e-9;
    static double lambdaMin = 400e-9;
    static double lambdaMax = 800e-9;
    static int numPoints = 500;
    ImGui::InputDouble("n1##Sim16", &n1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n_film##Sim16", &n_film, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n2 (substrate)##Sim16", &n2, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Film thickness (m)##Sim16", &d, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Lambda min (m)##Sim16", &lambdaMin, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Lambda max (m)##Sim16", &lambdaMax, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim16", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim16"))
    {
        clearPlot();
        std::vector<double> lamVec, rVec, tVec;
        for (int i = 0; i < numPoints; ++i) {
            double lam = lambdaMin + (lambdaMax - lambdaMin) * i / (numPoints - 1);
            lamVec.push_back(lam * 1e9);
            rVec.push_back(system.thinFilmReflectance(n1, n_film, n2, d, lam, 0.0));
            tVec.push_back(system.thinFilmTransmittance(n1, n_film, n2, d, lam, 0.0));
        }
        addCurve("Reflectance", lamVec, rVec);
        addCurve("Transmittance", lamVec, tVec);

        std::ostringstream oss;
        oss << "AR coating thickness (550nm) = " << system.antiReflectionThickness(n_film, 550e-9) * 1e9 << " nm\n";
        resultText = oss.str();
        plotTitle = "Thin Film Spectral Response";
        plotXLabel = "Wavelength (nm)";
        plotYLabel = "R / T";
    }
    if (ImGui::Button("Export CSV##Sim16"))
        system.exportThinFilmCSV("thin_film.csv", n1, n_film, n2, d, lambdaMin, lambdaMax, numPoints);
}

// ── 17: Michelson Interferometer ──
void GuiApp::renderSim17_Michelson()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Michelson Interferometer");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Michelson Interferometer"))
    {
        ImGui::TextWrapped(
            "Michelson interferometer splits a beam into two arms and recombines. "
            "Intensity I = I0/2 * (1 + cos(4*pi*delta_d/lambda)). "
            "Visibility V = (Imax-Imin)/(Imax+Imin). "
            "Coherence length L_c = lambda^2/delta_lambda.");
    }

    static double lambda = 632.8e-9;
    static double dMax = 5e-6;
    static int numPoints = 500;
    ImGui::InputDouble("Wavelength (m)##Sim17", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Max mirror displacement (m)##Sim17", &dMax, 1e-7, 1e-6, "%.3e");
    ImGui::SliderInt("Points##Sim17", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim17"))
    {
        clearPlot();
        std::vector<double> dVec, iVec;
        for (int i = 0; i < numPoints; ++i) {
            double dd = dMax * i / (numPoints - 1);
            dVec.push_back(dd * 1e6);
            iVec.push_back(system.michelsonIntensity(dd, lambda));
        }
        addCurve("Intensity", dVec, iVec);

        std::ostringstream oss;
        oss << "Fringes in range = " << system.michelsonFringeCount(dMax, lambda) << "\n";
        oss << "Coherence length (1nm bw) = " << system.michelsonCoherenceLength(1e-9, lambda) * 1e3 << " mm\n";
        oss << "Visibility (ideal) = " << system.michelsonVisibility(1.0, 0.0) << "\n";
        resultText = oss.str();
        plotTitle = "Michelson Fringes";
        plotXLabel = "Mirror displacement (um)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim17"))
        system.exportMichelsonCSV("michelson.csv", lambda, dMax, numPoints);
}

// ── 18: Fabry-Perot Interferometer ──
void GuiApp::renderSim18_FabryPerot()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Fabry-Perot Interferometer");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Fabry-Perot Interferometer"))
    {
        ImGui::TextWrapped(
            "Fabry-Perot etalon: transmission T = 1/(1 + F*sin^2(delta/2)) "
            "where F = 4R/(1-R)^2 is the coefficient of finesse. "
            "Finesse F_finesse = pi*sqrt(R)/(1-R). FSR = c/(2nd). "
            "Resolving power = m * F_finesse.");
    }

    static double R = 0.9;
    static double d = 0.01;
    static double n = 1.0;
    static int numPoints = 1000;
    ImGui::InputDouble("Reflectance R##Sim18", &R, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Spacing d (m)##Sim18", &d, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("n (medium)##Sim18", &n, 0.01, 0.1, "%.4f");
    ImGui::SliderInt("Points##Sim18", &numPoints, 200, 5000);

    if (ImGui::Button("Compute##Sim18"))
    {
        clearPlot();
        double F = 4.0 * R / ((1.0 - R) * (1.0 - R));
        std::vector<double> deltaVec, tVec;
        for (int i = 0; i < numPoints; ++i) {
            double delta = 4.0 * M_PI * i / (numPoints - 1);
            deltaVec.push_back(delta);
            tVec.push_back(system.fabryPerotTransmission(delta, F));
        }
        addCurve("Transmission", deltaVec, tVec);

        double finesse = system.fabryPerotFinesse(R);
        double fsr = system.fabryPerotFSR(d, n);
        std::ostringstream oss;
        oss << "Finesse = " << finesse << "\n";
        oss << "FSR = " << fsr * 1e-9 << " GHz\n";
        oss << "FWHM = " << system.fabryPerotFWHM(fsr, finesse) * 1e-6 << " MHz\n";
        oss << "Resolving power (m=1000) = " << system.fabryPerotResolvingPower(1000, finesse) << "\n";
        resultText = oss.str();
        plotTitle = "Fabry-Perot Transmission";
        plotXLabel = "Phase (rad)";
        plotYLabel = "Transmission";
    }
    if (ImGui::Button("Export CSV##Sim18"))
        system.exportFabryPerotCSV("fabry_perot.csv", R, d, n, 400e-9, 800e-9, numPoints);
}

// ── 19: Coherence (Temporal & Spatial) ──
void GuiApp::renderSim19_Coherence()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Coherence (Temporal & Spatial)");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Coherence (Temporal & Spatial)"))
    {
        ImGui::TextWrapped(
            "Temporal coherence length L_c = lambda^2/delta_lambda. "
            "Coherence time tau_c = L_c/c. Degree of coherence gamma(tau) = exp(-pi tau^2/tau_c^2). "
            "Spatial coherence: van Cittert-Zernike theorem relates coherence width to source size. "
            "Spatial coherence width ~ lambda/theta_source.");
    }

    static double lambda = 632.8e-9;
    static double delta_lambda = 0.01e-9;
    static double theta_source = 1e-3;
    static int numPoints = 500;
    ImGui::InputDouble("Wavelength (m)##Sim19", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Bandwidth (m)##Sim19", &delta_lambda, 1e-12, 1e-10, "%.3e");
    ImGui::InputDouble("Source angular size (rad)##Sim19", &theta_source, 1e-4, 1e-3, "%.3e");
    ImGui::SliderInt("Points##Sim19", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim19"))
    {
        clearPlot();
        double tau_c = system.temporalCoherenceTime(lambda, delta_lambda);
        std::vector<double> tauVec, gammaVec;
        for (int i = 0; i < numPoints; ++i) {
            double tau = 3.0 * tau_c * i / (numPoints - 1);
            tauVec.push_back(tau * 1e12);
            gammaVec.push_back(system.degreeOfCoherence(tau, tau_c));
        }
        addCurve("gamma(tau)", tauVec, gammaVec);

        std::ostringstream oss;
        oss << "Temporal coherence length = " << system.temporalCoherenceLength(lambda, delta_lambda) * 1e3 << " mm\n";
        oss << "Coherence time = " << tau_c * 1e12 << " ps\n";
        oss << "Spatial coherence width = " << system.spatialCoherenceWidth(lambda, theta_source) * 1e3 << " mm\n";
        oss << "vCZ radius (z=1m, D=1mm) = " << system.vanCittertZernikeRadius(lambda, 1e-3, 1.0) * 1e3 << " mm\n";
        resultText = oss.str();
        plotTitle = "Degree of Coherence";
        plotXLabel = "Delay (ps)";
        plotYLabel = "|gamma(tau)|";
    }
    if (ImGui::Button("Export CSV##Sim19"))
        system.exportCoherenceCSV("coherence.csv", lambda, delta_lambda, numPoints);
}

// ── 20: Fraunhofer Diffraction (General) ──
void GuiApp::renderSim20_FraunhoferGen()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Fraunhofer Diffraction (General)");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Fraunhofer Diffraction (General)"))
    {
        ImGui::TextWrapped(
            "Far-field diffraction pattern is the Fourier transform of the aperture function. "
            "Rectangular aperture: sinc(kx*a/2)*sinc(ky*b/2). "
            "Circular aperture: (2*J1(kr*R)/(kr*R))^2. "
            "OTF is the autocorrelation of the pupil function. MTF = |OTF|.");
    }

    static int mode = 0;
    const char* modes[] = {"Rectangular", "Circular"};
    ImGui::Combo("Aperture##Sim20", &mode, modes, 2);

    static double a = 1e-3, b = 0.5e-3, R_ap = 0.5e-3;
    static double lambda = 632.8e-9, f = 1.0;
    static int numPoints = 500;
    if (mode == 0) {
        ImGui::InputDouble("Width a (m)##Sim20", &a, 1e-4, 1e-3, "%.3e");
        ImGui::InputDouble("Height b (m)##Sim20", &b, 1e-4, 1e-3, "%.3e");
    } else {
        ImGui::InputDouble("Radius R (m)##Sim20", &R_ap, 1e-4, 1e-3, "%.3e");
    }
    ImGui::InputDouble("Wavelength (m)##Sim20", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Focal length (m)##Sim20", &f, 0.1, 1.0, "%.4f");
    ImGui::SliderInt("Points##Sim20", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim20"))
    {
        clearPlot();
        std::vector<double> xVec, iVec;
        double k = 2.0 * M_PI / lambda;
        if (mode == 0) {
            double xMax = 5.0 * lambda * f / a;
            for (int i = 0; i < numPoints; ++i) {
                double x = -xMax + 2.0 * xMax * i / (numPoints - 1);
                double kx = k * x / f;
                xVec.push_back(x * 1e3);
                iVec.push_back(system.fraunhoferRectIntensity(kx, 0, a, b));
            }
            addCurve("Rect diffraction", xVec, iVec);
        } else {
            double xMax = 5.0 * lambda * f / (2.0 * R_ap);
            for (int i = 0; i < numPoints; ++i) {
                double x = -xMax + 2.0 * xMax * i / (numPoints - 1);
                double kr = k * std::abs(x) / f;
                xVec.push_back(x * 1e3);
                iVec.push_back(system.fraunhoferCircIntensity(kr, R_ap));
            }
            addCurve("Circ diffraction", xVec, iVec);
        }

        std::ostringstream oss;
        double cutoff = 1.0 / (lambda * f);
        oss << "MTF cutoff freq = " << cutoff << " cycles/m\n";
        oss << "OTF(0) = " << system.opticalTransferFunction(0, cutoff) << "\n";
        resultText = oss.str();
        plotTitle = "Fraunhofer Diffraction";
        plotXLabel = "Position (mm)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim20")) {
        if (mode == 0)
            system.exportFraunhoferRectCSV("fraunhofer_rect.csv", a, b, lambda, f, numPoints);
        else
            system.exportFraunhoferCircCSV("fraunhofer_circ.csv", R_ap, lambda, f, numPoints);
    }
}

// ── 21: Babinet's Principle ──
void GuiApp::renderSim21_Babinet()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Babinet's Principle");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Babinet's Principle"))
    {
        ImGui::TextWrapped(
            "Babinet's principle: the diffraction pattern of an aperture and its complement "
            "(opaque obstacle of same shape) are related. Away from the direct beam, "
            "I_complement = I_aperture. The sum of the two fields equals the unobstructed field.");
    }

    static double a = 50e-6;
    static double lambda = 632.8e-9;
    static double z = 1.0;
    static int numPoints = 500;
    ImGui::InputDouble("Slit width a (m)##Sim21", &a, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim21", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Distance z (m)##Sim21", &z, 0.1, 1.0, "%.4f");
    ImGui::SliderInt("Points##Sim21", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim21"))
    {
        clearPlot();
        std::vector<double> angles, iAp, iComp;
        double maxAngle = 5.0 * lambda / a;
        for (int i = 0; i < numPoints; ++i) {
            double theta = -maxAngle + 2.0 * maxAngle * i / (numPoints - 1);
            double I_ap = system.singleSlitIntensity(theta, a, lambda);
            double I_comp = system.babinetComplementIntensity(I_ap, 1.0);
            angles.push_back(theta * 1e3);
            iAp.push_back(I_ap);
            iComp.push_back(I_comp);
        }
        addCurve("Aperture", angles, iAp);
        addCurve("Complement", angles, iComp);

        std::ostringstream oss;
        oss << "At theta=0: I_aperture = 1.0, I_complement = 0.0\n";
        oss << "Away from axis: I_complement mirrors I_aperture\n";
        resultText = oss.str();
        plotTitle = "Babinet's Principle";
        plotXLabel = "Angle (mrad)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim21"))
        system.exportBabinetCSV("babinet.csv", a, lambda, z, numPoints);
}

// ── 22: Talbot Effect ──
void GuiApp::renderSim22_Talbot()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Talbot Effect");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Talbot Effect"))
    {
        ImGui::TextWrapped(
            "A periodic grating self-images at the Talbot distance z_T = 2d^2/lambda. "
            "Fractional Talbot images occur at z = z_T * p/q with modified contrast. "
            "The self-imaging effect arises from the periodic phase structure of the grating.");
    }

    static double d = 10e-6;
    static double lambda = 632.8e-9;
    static int numPoints = 500;
    static int N_harmonics = 10;
    ImGui::InputDouble("Period d (m)##Sim22", &d, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim22", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Harmonics##Sim22", &N_harmonics, 1, 50);
    ImGui::SliderInt("Points##Sim22", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim22"))
    {
        clearPlot();
        double zT = system.talbotDistance(d, lambda);
        std::vector<double> xVec, i0Vec, iHalfVec, iFullVec;
        for (int i = 0; i < numPoints; ++i) {
            double x = d * i / (numPoints - 1);
            xVec.push_back(x * 1e6);
            i0Vec.push_back(system.talbotSelfImageIntensity(x, d, 0.0, lambda, N_harmonics));
            iHalfVec.push_back(system.talbotSelfImageIntensity(x, d, zT / 2.0, lambda, N_harmonics));
            iFullVec.push_back(system.talbotSelfImageIntensity(x, d, zT, lambda, N_harmonics));
        }
        addCurve("z = 0", xVec, i0Vec);
        addCurve("z = zT/2", xVec, iHalfVec);
        addCurve("z = zT", xVec, iFullVec);

        std::ostringstream oss;
        oss << "Talbot distance z_T = " << zT * 1e3 << " mm\n";
        oss << "Fractional z_T/2 = " << system.fractionalTalbotDistance(d, lambda, 1, 2) * 1e3 << " mm\n";
        resultText = oss.str();
        plotTitle = "Talbot Self-Imaging";
        plotXLabel = "x (um)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim22"))
        system.exportTalbotCSV("talbot.csv", d, lambda, numPoints);
}

// ── 23: Jones Calculus ──
void GuiApp::renderSim23_Jones()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Jones Calculus");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Jones Calculus"))
    {
        ImGui::TextWrapped(
            "Jones vectors describe fully polarized light: H=[1,0], V=[0,1], "
            "RCP=[1,-i]/sqrt(2), LCP=[1,i]/sqrt(2). Jones matrices represent optical elements: "
            "linear polarizer, HWP, QWP, general retarder. Apply via v_out = M * v_in. "
            "Intensity I = |Ex|^2 + |Ey|^2.");
    }

    static int inputState = 0;
    const char* states[] = {"H", "V", "45 deg", "RCP", "LCP"};
    ImGui::Combo("Input state##Sim23", &inputState, states, 5);

    static int element = 0;
    const char* elements[] = {"Linear Polarizer", "HWP", "QWP", "General Retarder"};
    ImGui::Combo("Element##Sim23", &element, elements, 4);

    static double angle = 0.0;
    static double retardance = 90.0;
    ImGui::InputDouble("Angle (deg)##Sim23", &angle, 1.0, 10.0, "%.2f");
    if (element == 3)
        ImGui::InputDouble("Retardance (deg)##Sim23", &retardance, 1.0, 10.0, "%.2f");

    if (ImGui::Button("Compute##Sim23"))
    {
        clearPlot();
        JonesVector v;
        switch (inputState) {
        case 0: v = system.jonesLinearH(); break;
        case 1: v = system.jonesLinearV(); break;
        case 2: v = system.jonesLinear(M_PI / 4.0); break;
        case 3: v = system.jonesRCP(); break;
        case 4: v = system.jonesLCP(); break;
        }

        double aRad = angle * M_PI / 180.0;
        JonesMatrix M;
        switch (element) {
        case 0: M = system.jonesLinearPolarizer(aRad); break;
        case 1: M = system.jonesHWP(aRad); break;
        case 2: M = system.jonesQWP(aRad); break;
        case 3: M = system.jonesRetarder(retardance * M_PI / 180.0, aRad); break;
        }

        std::vector<double> angVec, intVec;
        for (int i = 0; i < 360; ++i) {
            double a_i = i * M_PI / 180.0;
            JonesMatrix Mi;
            switch (element) {
            case 0: Mi = system.jonesLinearPolarizer(a_i); break;
            case 1: Mi = system.jonesHWP(a_i); break;
            case 2: Mi = system.jonesQWP(a_i); break;
            case 3: Mi = system.jonesRetarder(retardance * M_PI / 180.0, a_i); break;
            }
            JonesVector out = system.jonesApply(Mi, v);
            angVec.push_back((double)i);
            intVec.push_back(system.jonesIntensity(out));
        }
        addCurve("Intensity vs angle", angVec, intVec);

        JonesVector vout = system.jonesApply(M, v);
        std::ostringstream oss;
        oss << "Output Jones vector:\n";
        oss << "  Ex = (" << vout[0].real() << ", " << vout[0].imag() << ")\n";
        oss << "  Ey = (" << vout[1].real() << ", " << vout[1].imag() << ")\n";
        oss << "Output intensity = " << system.jonesIntensity(vout) << "\n";
        resultText = oss.str();
        plotTitle = "Jones: Intensity vs Element Angle";
        plotXLabel = "Element angle (deg)";
        plotYLabel = "Output Intensity";
    }
    if (ImGui::Button("Export CSV##Sim23"))
        system.exportJonesCSV("jones.csv", element, angle, 360);
}

// ── 24: Stokes Parameters & Poincare Sphere ──
void GuiApp::renderSim24_Stokes()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Stokes Parameters & Poincare Sphere");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Stokes Parameters & Poincare Sphere"))
    {
        ImGui::TextWrapped(
            "Stokes vector S = (S0, S1, S2, S3) describes any polarization state including partially polarized. "
            "DOP = sqrt(S1^2+S2^2+S3^2)/S0. DOLP and DOCP measure linear and circular components. "
            "Poincare sphere: S1/S0, S2/S0, S3/S0 map to a unit sphere. "
            "Ellipticity angle chi = 0.5*arcsin(S3/S0), orientation psi = 0.5*arctan(S2/S1).");
    }

    static double alpha = 45.0;
    static double delta = 0.0;
    ImGui::InputDouble("Alpha (deg)##Sim24", &alpha, 1.0, 10.0, "%.2f");
    ImGui::InputDouble("Phase delta (deg)##Sim24", &delta, 1.0, 10.0, "%.2f");

    if (ImGui::Button("Compute##Sim24"))
    {
        clearPlot();
        std::vector<double> delVec, s1Vec, s2Vec, s3Vec;
        for (int i = 0; i < 360; ++i) {
            double d_i = i * M_PI / 180.0;
            JonesVector v = system.jonesElliptical(alpha * M_PI / 180.0, d_i);
            StokesVector S = system.jonestoStokes(v);
            delVec.push_back((double)i);
            s1Vec.push_back(S[1] / S[0]);
            s2Vec.push_back(S[2] / S[0]);
            s3Vec.push_back(S[3] / S[0]);
        }
        addCurve("S1/S0", delVec, s1Vec);
        addCurve("S2/S0", delVec, s2Vec);
        addCurve("S3/S0", delVec, s3Vec);

        JonesVector v = system.jonesElliptical(alpha * M_PI / 180.0, delta * M_PI / 180.0);
        StokesVector S = system.jonestoStokes(v);
        auto [px, py, pz] = system.stokesToPoincare(S);
        std::ostringstream oss;
        oss << "Stokes: [" << S[0] << ", " << S[1] << ", " << S[2] << ", " << S[3] << "]\n";
        oss << "DOP = " << system.stokesDOP(S) << "\n";
        oss << "DOLP = " << system.stokesDOLP(S) << ", DOCP = " << system.stokesDOCP(S) << "\n";
        oss << "Ellipticity angle = " << system.stokesEllipticityAngle(S) * 180.0 / M_PI << " deg\n";
        oss << "Orientation = " << system.stokesOrientationAngle(S) * 180.0 / M_PI << " deg\n";
        oss << "Poincare: (" << px << ", " << py << ", " << pz << ")\n";
        resultText = oss.str();
        plotTitle = "Normalized Stokes vs Phase";
        plotXLabel = "Phase delta (deg)";
        plotYLabel = "S_i / S_0";
    }
    if (ImGui::Button("Export CSV##Sim24"))
        system.exportPoincareCSV("poincare.csv", 360);
}

// ── 25: Mueller Matrices ──
void GuiApp::renderSim25_Mueller()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Mueller Matrices");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Mueller Matrices"))
    {
        ImGui::TextWrapped(
            "Mueller matrices (4x4 real) transform Stokes vectors: S_out = M * S_in. "
            "Handles partially polarized light. Elements: linear polarizer, retarder (HWP, QWP), "
            "rotator, depolarizer. Cascade by matrix multiplication.");
    }

    static int element = 0;
    const char* elements[] = {"Linear Polarizer", "HWP", "QWP", "Retarder", "Depolarizer"};
    ImGui::Combo("Element##Sim25", &element, elements, 5);

    static double angle = 0.0;
    static double retardance = 90.0;
    static double depol = 0.5;
    ImGui::InputDouble("Angle (deg)##Sim25", &angle, 1.0, 10.0, "%.2f");
    if (element == 3) ImGui::InputDouble("Retardance (deg)##Sim25", &retardance, 1.0, 10.0, "%.2f");
    if (element == 4) ImGui::InputDouble("Depol factor p##Sim25", &depol, 0.01, 0.1, "%.3f");

    if (ImGui::Button("Compute##Sim25"))
    {
        clearPlot();
        StokesVector Sin = {1.0, 1.0, 0.0, 0.0};
        std::vector<double> angVec, s0Vec, s1Vec;
        for (int i = 0; i < 360; ++i) {
            double a_i = i * M_PI / 180.0;
            MuellerMatrix M;
            switch (element) {
            case 0: M = system.muellerLinearPolarizer(a_i); break;
            case 1: M = system.muellerHWP(a_i); break;
            case 2: M = system.muellerQWP(a_i); break;
            case 3: M = system.muellerRetarder(retardance * M_PI / 180.0, a_i); break;
            case 4: M = system.muellerDepolarizer(depol); break;
            }
            StokesVector Sout = system.muellerApply(M, Sin);
            angVec.push_back((double)i);
            s0Vec.push_back(Sout[0]);
            s1Vec.push_back(Sout[1]);
        }
        addCurve("S0 out", angVec, s0Vec);
        addCurve("S1 out", angVec, s1Vec);

        double aRad = angle * M_PI / 180.0;
        MuellerMatrix M;
        switch (element) {
        case 0: M = system.muellerLinearPolarizer(aRad); break;
        case 1: M = system.muellerHWP(aRad); break;
        case 2: M = system.muellerQWP(aRad); break;
        case 3: M = system.muellerRetarder(retardance * M_PI / 180.0, aRad); break;
        case 4: M = system.muellerDepolarizer(depol); break;
        }
        StokesVector Sout = system.muellerApply(M, Sin);
        std::ostringstream oss;
        oss << "Input: [1, 1, 0, 0] (H-polarized)\n";
        oss << "Output: [" << Sout[0] << ", " << Sout[1] << ", " << Sout[2] << ", " << Sout[3] << "]\n";
        oss << "DOP = " << system.stokesDOP(Sout) << "\n";
        resultText = oss.str();
        plotTitle = "Mueller: Output vs Element Angle";
        plotXLabel = "Element angle (deg)";
        plotYLabel = "Stokes parameter";
    }
    if (ImGui::Button("Export CSV##Sim25"))
        system.exportMuellerCSV("mueller.csv", element, angle, 360);
}

// ── 26: Malus's Law & Brewster Angle ──
void GuiApp::renderSim26_Malus()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Malus's Law & Brewster Angle");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Malus's Law & Brewster Angle"))
    {
        ImGui::TextWrapped(
            "Malus's law: I = I0 * cos^2(theta) for light through a polarizer at angle theta. "
            "Chain of polarizers: each attenuates by cos^2 of the relative angle. "
            "At Brewster's angle, p-polarized light has zero reflectance.");
    }

    static double I0 = 1.0;
    static double n1 = 1.0;
    static double n2 = 1.5;
    static int numPoints = 360;
    ImGui::InputDouble("I0##Sim26", &I0, 0.1, 1.0, "%.4f");
    ImGui::InputDouble("n1##Sim26", &n1, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n2##Sim26", &n2, 0.01, 0.1, "%.4f");

    if (ImGui::Button("Compute##Sim26"))
    {
        clearPlot();
        std::vector<double> angVec, iVec;
        for (int i = 0; i < numPoints; ++i) {
            double theta = M_PI * i / (numPoints - 1);
            angVec.push_back(theta * 180.0 / M_PI);
            iVec.push_back(system.malusLaw(I0, theta));
        }
        addCurve("I(theta)", angVec, iVec);

        std::vector<double> chain = {M_PI / 6.0, M_PI / 3.0, M_PI / 2.0};
        double I_chain = system.malusChain(I0, chain);
        double brew = system.brewsterReflectionAngle(n1, n2);
        double R_brew = system.reflectanceAtBrewster(n1, n2);
        std::ostringstream oss;
        oss << "I through 30/60/90 deg chain = " << I_chain << "\n";
        oss << "Brewster angle = " << brew * 180.0 / M_PI << " deg\n";
        oss << "Rs at Brewster = " << R_brew << "\n";
        resultText = oss.str();
        plotTitle = "Malus's Law";
        plotXLabel = "Polarizer angle (deg)";
        plotYLabel = "Transmitted Intensity";
    }
    if (ImGui::Button("Export CSV##Sim26"))
        system.exportMalusCSV("malus.csv", I0, numPoints);
}

// ── 27: Birefringence ──
void GuiApp::renderSim27_Birefringence()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Birefringence");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Birefringence"))
    {
        ImGui::TextWrapped(
            "Birefringent crystals have different refractive indices for ordinary (n_o) and "
            "extraordinary (n_e) rays. Phase delay delta = 2*pi*(n_e-n_o)*d/lambda. "
            "Walkoff angle describes separation of o- and e-rays. "
            "Used in waveplates: QWP at d = lambda/(4*delta_n), HWP at d = lambda/(2*delta_n).");
    }

    static double n_o = 1.544;
    static double n_e = 1.553;
    static double d = 0.001;
    static double lambdaMin = 400e-9;
    static double lambdaMax = 800e-9;
    static int numPoints = 500;
    ImGui::InputDouble("n_o##Sim27", &n_o, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("n_e##Sim27", &n_e, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Thickness d (m)##Sim27", &d, 1e-4, 1e-3, "%.4f");
    ImGui::InputDouble("Lambda min (m)##Sim27", &lambdaMin, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Lambda max (m)##Sim27", &lambdaMax, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim27", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim27"))
    {
        clearPlot();
        double delta_n = n_e - n_o;
        std::vector<double> lamVec, phaseVec;
        for (int i = 0; i < numPoints; ++i) {
            double lam = lambdaMin + (lambdaMax - lambdaMin) * i / (numPoints - 1);
            lamVec.push_back(lam * 1e9);
            phaseVec.push_back(system.birefringentPhaseDelay(delta_n, d, lam) / M_PI);
        }
        addCurve("Phase delay (pi)", lamVec, phaseVec);

        double wo = system.walkoffAngle(n_e, n_o, M_PI / 4.0);
        std::ostringstream oss;
        oss << "Birefringence delta_n = " << delta_n << "\n";
        oss << "Phase delay at 633nm = " << system.birefringentPhaseDelay(delta_n, d, 632.8e-9) / M_PI << " pi\n";
        oss << "Walkoff angle (45 deg) = " << wo * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
        plotTitle = "Birefringent Phase Delay";
        plotXLabel = "Wavelength (nm)";
        plotYLabel = "Phase delay (multiples of pi)";
    }
    if (ImGui::Button("Export CSV##Sim27"))
        system.exportBirefringenceCSV("birefringence.csv", n_o, n_e, d, lambdaMin, lambdaMax, numPoints);
}

// ── 28: Optical Activity ──
void GuiApp::renderSim28_OpticalActivity()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Optical Activity");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Optical Activity"))
    {
        ImGui::TextWrapped(
            "Optical activity: rotation of polarization plane by chiral media. "
            "alpha = [alpha] * c * L (specific rotation * concentration * path). "
            "Faraday effect: theta_F = V * B * L where V is Verdet constant. "
            "Non-reciprocal: Faraday rotation does not reverse on back-propagation.");
    }

    static double alpha_spec = 66.5;
    static double concentration = 0.1;
    static double pathLength = 0.1;
    static double V_verdet = 3.0;
    static double B_field = 1.0;
    static int numPoints = 200;
    ImGui::InputDouble("Specific rotation (deg/(dm*g/mL))##Sim28", &alpha_spec, 1.0, 10.0, "%.2f");
    ImGui::InputDouble("Concentration (g/mL)##Sim28", &concentration, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Path length (m)##Sim28", &pathLength, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("Verdet constant (rad/(T*m))##Sim28", &V_verdet, 0.1, 1.0, "%.2f");
    ImGui::InputDouble("B field (T)##Sim28", &B_field, 0.1, 1.0, "%.2f");
    ImGui::SliderInt("Points##Sim28", &numPoints, 50, 500);

    if (ImGui::Button("Compute##Sim28"))
    {
        clearPlot();
        std::vector<double> cVec, rotVec;
        for (int i = 0; i < numPoints; ++i) {
            double c = 0.01 + 0.5 * i / (numPoints - 1);
            cVec.push_back(c);
            rotVec.push_back(system.opticalRotation(alpha_spec, c, pathLength));
        }
        addCurve("Rotation vs conc", cVec, rotVec);

        double rot_opt = system.opticalRotation(alpha_spec, concentration, pathLength);
        double rot_far = system.faradayRotation(V_verdet, B_field, pathLength);
        std::ostringstream oss;
        oss << "Optical rotation = " << rot_opt << " deg\n";
        oss << "Faraday rotation = " << rot_far * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
        plotTitle = "Optical Rotation";
        plotXLabel = "Concentration (g/mL)";
        plotYLabel = "Rotation (deg)";
    }
    if (ImGui::Button("Export CSV##Sim28"))
        system.exportOpticalActivityCSV("optical_activity.csv", alpha_spec, concentration, numPoints);
}

// ── 29: Fourier Transform Optics ──
void GuiApp::renderSim29_FourierOptics()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Fourier Transform Optics");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Fourier Transform Optics"))
    {
        ImGui::TextWrapped(
            "A lens performs a Fourier transform: the field in the back focal plane is the FT "
            "of the input field. OTF = autocorrelation of pupil / area. MTF = |OTF|. "
            "Cutoff frequency fc ~ D/(lambda*f) for incoherent imaging.");
    }

    static double a = 1e-3;
    static double lambda = 632.8e-9;
    static double f = 0.5;
    static int numPoints = 500;
    ImGui::InputDouble("Aperture width (m)##Sim29", &a, 1e-4, 1e-3, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim29", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Focal length (m)##Sim29", &f, 0.1, 1.0, "%.4f");
    ImGui::SliderInt("Points##Sim29", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim29"))
    {
        clearPlot();
        double cutoff = a / (lambda * f);
        std::vector<double> fxVec, otfVec, mtfVec;
        for (int i = 0; i < numPoints; ++i) {
            double fx = -1.5 * cutoff + 3.0 * cutoff * i / (numPoints - 1);
            fxVec.push_back(fx * 1e-3);
            otfVec.push_back(system.opticalTransferFunction(fx, cutoff));
            mtfVec.push_back(system.modulationTransferFunction(fx, cutoff));
        }
        addCurve("OTF", fxVec, otfVec);
        addCurve("MTF", fxVec, mtfVec);

        std::ostringstream oss;
        oss << "Cutoff frequency = " << cutoff * 1e-3 << " cycles/mm\n";
        resultText = oss.str();
        plotTitle = "OTF / MTF";
        plotXLabel = "Spatial frequency (cycles/mm)";
        plotYLabel = "Transfer function";
    }
    if (ImGui::Button("Export CSV##Sim29"))
        system.exportFourierApertureCSV("fourier_optics.csv", a, lambda, f, numPoints);
}

// ── 30: Spatial Filtering ──
void GuiApp::renderSim30_SpatialFilter()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Spatial Filtering");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Spatial Filtering"))
    {
        ImGui::TextWrapped(
            "Spatial filtering in the Fourier plane of a 4f system. Low-pass: blocks high frequencies "
            "(smooths image). High-pass: blocks DC (edge detection). Band-pass: selects specific "
            "spatial frequency range. Filter function H(fx,fy) multiplies the spectrum.");
    }

    static int filterType = 0;
    const char* filters[] = {"Low-pass", "High-pass", "Band-pass"};
    ImGui::Combo("Filter type##Sim30", &filterType, filters, 3);

    static double fc = 100.0;
    static double f_low = 50.0, f_high = 200.0;
    if (filterType < 2) ImGui::InputDouble("Cutoff freq (cycles/m)##Sim30", &fc, 10.0, 100.0, "%.1f");
    if (filterType == 2) {
        ImGui::InputDouble("f_low (cycles/m)##Sim30", &f_low, 10.0, 100.0, "%.1f");
        ImGui::InputDouble("f_high (cycles/m)##Sim30", &f_high, 10.0, 100.0, "%.1f");
    }
    static int numPoints = 500;
    ImGui::SliderInt("Points##Sim30", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim30"))
    {
        clearPlot();
        std::vector<double> fxVec, hVec;
        double fMax = (filterType == 2) ? f_high * 3.0 : fc * 3.0;
        for (int i = 0; i < numPoints; ++i) {
            double fx = -fMax + 2.0 * fMax * i / (numPoints - 1);
            fxVec.push_back(fx);
            switch (filterType) {
            case 0: hVec.push_back(system.lowPassFilter(fx, 0, fc)); break;
            case 1: hVec.push_back(system.highPassFilter(fx, 0, fc)); break;
            case 2: hVec.push_back(system.bandPassFilter(fx, 0, f_low, f_high)); break;
            }
        }
        addCurve("H(fx)", fxVec, hVec);

        std::ostringstream oss;
        oss << "Filter type: " << filters[filterType] << "\n";
        resultText = oss.str();
        plotTitle = "Spatial Filter Response";
        plotXLabel = "Spatial frequency (cycles/m)";
        plotYLabel = "Filter H(fx)";
    }
    if (ImGui::Button("Export CSV##Sim30"))
        system.exportSpatialFilterCSV("spatial_filter.csv", 1e-3, fc, 632.8e-9, 0.5, numPoints);
}

// ── 31: Gaussian Beam Propagation ──
void GuiApp::renderSim31_GaussianBeam()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Gaussian Beam Propagation");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Gaussian Beam Propagation"))
    {
        ImGui::TextWrapped(
            "Gaussian beam: w(z) = w0*sqrt(1+(z/zR)^2), zR = pi*w0^2/lambda. "
            "Radius of curvature R(z) = z(1+(zR/z)^2). Gouy phase = arctan(z/zR). "
            "Far-field divergence theta = lambda/(pi*w0). Complex beam parameter q = z + i*zR. "
            "ABCD propagation: q_out = (A*q+B)/(C*q+D).");
    }

    static int mode = 0;
    const char* modes[] = {"Propagation w(z)", "Beam Profile I(r)"};
    ImGui::Combo("Mode##Sim31", &mode, modes, 2);

    static double w0 = 0.5e-3;
    static double lambda = 632.8e-9;
    static double zMax = 2.0;
    static double P = 1e-3;
    static int numPoints = 500;
    ImGui::InputDouble("Waist w0 (m)##Sim31", &w0, 1e-5, 1e-4, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim31", &lambda, 1e-9, 1e-8, "%.3e");
    if (mode == 0) ImGui::InputDouble("z max (m)##Sim31", &zMax, 0.1, 1.0, "%.4f");
    if (mode == 1) ImGui::InputDouble("Power (W)##Sim31", &P, 1e-4, 1e-3, "%.3e");
    ImGui::SliderInt("Points##Sim31", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim31"))
    {
        clearPlot();
        double zR = system.gaussianRayleighRange(w0, lambda);
        if (mode == 0) {
            std::vector<double> zVec, wVec, negWVec;
            for (int i = 0; i < numPoints; ++i) {
                double z = -zMax + 2.0 * zMax * i / (numPoints - 1);
                double w = system.gaussianBeamWaist(w0, z, lambda);
                zVec.push_back(z * 100.0);
                wVec.push_back(w * 1e3);
                negWVec.push_back(-w * 1e3);
            }
            addCurve("w(z)", zVec, wVec);
            addCurve("-w(z)", zVec, negWVec);
            plotTitle = "Gaussian Beam Envelope";
            plotXLabel = "z (cm)";
            plotYLabel = "w (mm)";
        } else {
            double w = system.gaussianBeamWaist(w0, 0, lambda);
            std::vector<double> rVec, iVec;
            double rMax = 3.0 * w;
            for (int i = 0; i < numPoints; ++i) {
                double r = -rMax + 2.0 * rMax * i / (numPoints - 1);
                rVec.push_back(r * 1e3);
                iVec.push_back(system.gaussianPeakIntensity(P, w) * system.gaussianIntensity(std::abs(r), w));
            }
            addCurve("I(r)", rVec, iVec);
            plotTitle = "Gaussian Beam Profile";
            plotXLabel = "r (mm)";
            plotYLabel = "Intensity (W/m^2)";
        }

        std::ostringstream oss;
        oss << "Rayleigh range zR = " << zR * 100.0 << " cm\n";
        oss << "Divergence = " << system.gaussianDivergence(w0, lambda) * 1e3 << " mrad\n";
        oss << "Gouy phase at zR = " << system.gaussianGouyPhase(zR, zR) * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
    }
    if (ImGui::Button("Export CSV##Sim31"))
        system.exportGaussianBeamCSV("gaussian_beam.csv", w0, lambda, zMax, numPoints);
}

// ── 32: Beam Propagation Method ──
void GuiApp::renderSim32_BPM()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Beam Propagation Method");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Beam Propagation Method"))
    {
        ImGui::TextWrapped(
            "BPM propagates a field through an inhomogeneous medium using split-step Fourier. "
            "Each step: apply phase from refractive index profile, then propagate in free space "
            "via FFT. Useful for waveguide simulations and fiber mode analysis.");
    }

    static double w0 = 10e-6;
    static double lambda = 1550e-9;
    static double dx = 0.5e-6;
    static double dz = 1e-6;
    static int Nx = 256;
    static int Nz = 100;
    ImGui::InputDouble("Beam waist w0 (m)##Sim32", &w0, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim32", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("dx (m)##Sim32", &dx, 1e-7, 1e-6, "%.3e");
    ImGui::InputDouble("dz (m)##Sim32", &dz, 1e-7, 1e-6, "%.3e");
    ImGui::SliderInt("Nx##Sim32", &Nx, 64, 1024);
    ImGui::SliderInt("Nz steps##Sim32", &Nz, 10, 500);

    if (ImGui::Button("Compute##Sim32"))
    {
        clearPlot();
        auto field = NumericalSolverOpt::makeGaussianBeam(Nx, dx, w0, 0.0);
        std::vector<double> xVec, initI, finalI;
        auto current = field;
        for (int step = 0; step < Nz; ++step)
            current = system.bpmStep(current, dx, dz, lambda, 1.45);

        for (int i = 0; i < Nx; ++i) {
            double x = (i - Nx / 2) * dx;
            xVec.push_back(x * 1e6);
            initI.push_back(std::norm(field[i]));
            finalI.push_back(std::norm(current[i]));
        }
        addCurve("Initial |E|^2", xVec, initI);
        addCurve("Final |E|^2", xVec, finalI);

        std::ostringstream oss;
        oss << "Propagated " << Nz << " steps, dz = " << dz * 1e6 << " um\n";
        oss << "Total distance = " << Nz * dz * 1e6 << " um\n";
        resultText = oss.str();
        plotTitle = "BPM Field Profile";
        plotXLabel = "x (um)";
        plotYLabel = "|E|^2";
    }
    if (ImGui::Button("Export CSV##Sim32")) {
        auto field = NumericalSolverOpt::makeGaussianBeam(Nx, dx, w0, 0.0);
        system.exportBPMCSV("bpm.csv", field, dx, dz, lambda, 1.45, Nz, 10);
    }
}

// ── 33: Holography ──
void GuiApp::renderSim33_Holography()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Holography");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Holography"))
    {
        ImGui::TextWrapped(
            "Holography records the interference between a reference beam and an object beam. "
            "Hologram intensity: I = |R|^2 + |O|^2 + R*O* + R*O. "
            "Reconstruction: illuminate hologram with reference beam to recover object wavefront.");
    }

    static double ref_amp = 1.0;
    static double obj_amp = 0.5;
    static int numPoints = 500;
    ImGui::InputDouble("Reference amplitude##Sim33", &ref_amp, 0.1, 0.5, "%.3f");
    ImGui::InputDouble("Object amplitude##Sim33", &obj_amp, 0.1, 0.5, "%.3f");
    ImGui::SliderInt("Points##Sim33", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim33"))
    {
        clearPlot();
        std::vector<double> ref(numPoints), obj(numPoints), phase(numPoints);
        std::vector<double> xVec, holoVec, reconVec;
        for (int i = 0; i < numPoints; ++i) {
            double x = -1.0 + 2.0 * i / (numPoints - 1);
            ref[i] = ref_amp;
            obj[i] = obj_amp;
            phase[i] = 2.0 * M_PI * x * 50.0;
            xVec.push_back(x);
        }
        auto hologram = system.hologramRecord(ref, obj, phase);
        auto recon = system.hologramReconstruct(hologram, ref);
        for (int i = 0; i < numPoints; ++i) {
            holoVec.push_back(hologram[i]);
            reconVec.push_back(recon[i]);
        }
        addCurve("Hologram", xVec, holoVec);
        addCurve("Reconstruction", xVec, reconVec);

        std::ostringstream oss;
        oss << "Interference at phase=0: " << system.hologramInterference(ref_amp, obj_amp, 0) << "\n";
        oss << "Interference at phase=pi: " << system.hologramInterference(ref_amp, obj_amp, M_PI) << "\n";
        resultText = oss.str();
        plotTitle = "Hologram Recording & Reconstruction";
        plotXLabel = "Position (a.u.)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim33"))
        system.exportHologramCSV("hologram.csv", obj_amp, numPoints);
}

// ── 34: Self-Imaging & Lau Effect ──
void GuiApp::renderSim34_SelfImaging()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Self-Imaging & Lau Effect");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Self-Imaging & Lau Effect"))
    {
        ImGui::TextWrapped(
            "Extension of Talbot effect. Lau effect: two gratings at specific distances produce "
            "interference fringes with spatially incoherent light. "
            "Lau distance relates grating periods to the required separation.");
    }

    static double d = 20e-6;
    static double lambda = 632.8e-9;
    static double zMax = 0.01;
    static int numPoints = 500;
    ImGui::InputDouble("Grating period d (m)##Sim34", &d, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim34", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("z max (m)##Sim34", &zMax, 0.001, 0.01, "%.4f");
    ImGui::SliderInt("Points##Sim34", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim34"))
    {
        clearPlot();
        double zT = system.talbotDistance(d, lambda);
        std::vector<double> zVec, iVec;
        for (int i = 0; i < numPoints; ++i) {
            double z = zMax * i / (numPoints - 1);
            zVec.push_back(z * 1e3);
            iVec.push_back(system.talbotSelfImageIntensity(d / 4.0, d, z, lambda, 10));
        }
        addCurve("I(z) at x=d/4", zVec, iVec);

        std::ostringstream oss;
        oss << "Talbot distance = " << zT * 1e3 << " mm\n";
        oss << "Lau distance (d1=d2=d) = " << system.lauDistance(d, d * 1.001, lambda) * 1e3 << " mm\n";
        resultText = oss.str();
        plotTitle = "Self-Imaging vs Distance";
        plotXLabel = "z (mm)";
        plotYLabel = "Intensity";
    }
    if (ImGui::Button("Export CSV##Sim34"))
        system.exportSelfImagingCSV("self_imaging.csv", d, lambda, zMax, numPoints);
}

// ── 35: Laser Rate Equations ──
void GuiApp::renderSim35_LaserRate()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Laser Rate Equations");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Laser Rate Equations"))
    {
        ImGui::TextWrapped(
            "Two coupled rate equations: dN/dt = R_pump - N/tau_sp - sigma_e*phi*N, "
            "dphi/dt = sigma_e*N*phi - phi/tau_c + N/(tau_sp*V_mode). "
            "Above threshold pump rate, population inversion clamps and photon density grows. "
            "Relaxation oscillations appear in transient regime.");
    }

    static double R_pump = 1e24;
    static double tau_sp = 230e-6;
    static double sigma_e = 2.8e-19;
    static double V_mode = 1e-9;
    static double tau_c = 1e-9;
    static double tMax = 1e-3;
    static double dt = 1e-7;
    ImGui::InputDouble("Pump rate (m^-3/s)##Sim35", &R_pump, 1e22, 1e23, "%.2e");
    ImGui::InputDouble("tau_sp (s)##Sim35", &tau_sp, 1e-6, 1e-5, "%.3e");
    ImGui::InputDouble("sigma_e (m^2)##Sim35", &sigma_e, 1e-20, 1e-19, "%.3e");
    ImGui::InputDouble("V_mode (m^3)##Sim35", &V_mode, 1e-10, 1e-9, "%.3e");
    ImGui::InputDouble("tau_c (s)##Sim35", &tau_c, 1e-10, 1e-9, "%.3e");
    ImGui::InputDouble("t max (s)##Sim35", &tMax, 1e-4, 1e-3, "%.3e");
    ImGui::InputDouble("dt (s)##Sim35", &dt, 1e-8, 1e-7, "%.3e");

    if (ImGui::Button("Compute##Sim35"))
    {
        clearPlot();
        int steps = (int)(tMax / dt);
        if (steps > 100000) steps = 100000;
        std::vector<double> tVec, nVec, phiVec;
        double N = 0.0, phi = 1.0;
        for (int i = 0; i < steps; ++i) {
            double t = i * dt;
            if (i % std::max(1, steps / 1000) == 0) {
                tVec.push_back(t * 1e6);
                nVec.push_back(N);
                phiVec.push_back(phi);
            }
            auto state = system.laserRateEquationsStep(N, phi, R_pump, tau_sp, sigma_e, V_mode, tau_c, dt);
            N = state.N;
            phi = state.phi;
            if (N < 0) N = 0;
            if (phi < 0) phi = 0;
        }
        addCurve("Photon density", tVec, phiVec);

        double N_th = 1.0 / (sigma_e * tau_c);
        std::ostringstream oss;
        oss << "Threshold N = " << N_th << " m^-3\n";
        oss << "Threshold pump = " << system.laserThresholdPump(N_th, tau_sp) << " m^-3/s\n";
        resultText = oss.str();
        plotTitle = "Laser Dynamics";
        plotXLabel = "Time (us)";
        plotYLabel = "Photon density";
    }
    if (ImGui::Button("Export CSV##Sim35"))
        system.exportLaserRateCSV("laser_rate.csv", R_pump, tau_sp, sigma_e, V_mode, tau_c, tMax, dt);
}

// ── 36: Laser Cavity Modes ──
void GuiApp::renderSim36_CavityModes()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Laser Cavity Modes");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Laser Cavity Modes"))
    {
        ImGui::TextWrapped(
            "Longitudinal modes: freq_q = q*c/(2nL), FSR = c/(2nL). "
            "Transverse modes (TEM_mp): freq offset from Gouy phase. "
            "Stability: 0 <= g1*g2 <= 1 where g_i = 1 - L/R_i. "
            "Cavity waist depends on g-parameters and wavelength.");
    }

    static double L = 0.3;
    static double n = 1.0;
    static double R1 = 1.0;
    static double R2 = 1.0;
    static int numModes = 20;
    ImGui::InputDouble("Cavity length L (m)##Sim36", &L, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n##Sim36", &n, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("R1 (m)##Sim36", &R1, 0.1, 1.0, "%.4f");
    ImGui::InputDouble("R2 (m)##Sim36", &R2, 0.1, 1.0, "%.4f");
    ImGui::SliderInt("Modes##Sim36", &numModes, 5, 100);

    if (ImGui::Button("Compute##Sim36"))
    {
        clearPlot();
        double fsr = system.cavityModeFSR(L, n);
        double g1 = 1.0 - L / R1, g2 = 1.0 - L / R2;
        double stab = system.cavityStabilityParameter(L, R1, R2);

        std::vector<double> gVec, stabVec;
        for (int i = 0; i < 200; ++i) {
            double g = -2.0 + 4.0 * i / 199.0;
            gVec.push_back(g);
            stabVec.push_back((g * g2 >= 0.0 && g * g2 <= 1.0) ? 1.0 : 0.0);
        }
        addCurve("Stable region", gVec, stabVec);

        std::ostringstream oss;
        oss << "FSR = " << fsr * 1e-6 << " MHz\n";
        oss << "g1 = " << g1 << ", g2 = " << g2 << "\n";
        oss << "g1*g2 = " << stab << " (" << (system.cavityIsStable(g1, g2) ? "STABLE" : "UNSTABLE") << ")\n";
        double w_cav = system.cavityWaistSize(system.wavelength, L, g1, g2);
        oss << "Cavity waist = " << w_cav * 1e6 << " um\n";
        resultText = oss.str();
        plotTitle = "Cavity Stability";
        plotXLabel = "g1";
        plotYLabel = "Stable (1/0)";
    }
    if (ImGui::Button("Export CSV##Sim36"))
        system.exportCavityModesCSV("cavity_modes.csv", L, n, numModes);
}

// ── 37: Q-Switching & Mode-Locking ──
void GuiApp::renderSim37_QSwitchModeLock()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Q-Switching & Mode-Locking");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Q-Switching & Mode-Locking"))
    {
        ImGui::TextWrapped(
            "Q-switching: stored energy released in a giant pulse. E = N*V*h*nu, "
            "peak power P = E/tau_pulse, duration tau ~ tau_c*r/(r-1-ln(r)). "
            "Mode-locking: phase-locked modes produce ultrashort pulses. "
            "Duration ~ 1/delta_nu, rep rate = c/(2nL), peak power = P_avg/(rep*tau).");
    }

    static int mode = 0;
    const char* modes[] = {"Q-Switching", "Mode-Locking"};
    ImGui::Combo("Mode##Sim37", &mode, modes, 2);

    static double N_init = 1e23;
    static double V_mode = 1e-9;
    static double h_nu = 2e-19;
    static double tau_c = 10e-9;
    static double r = 3.0;
    static double L_cav = 0.3;
    static double delta_nu = 10e12;
    static double P_avg = 1.0;
    if (mode == 0) {
        ImGui::InputDouble("N_initial (m^-3)##Sim37", &N_init, 1e21, 1e22, "%.2e");
        ImGui::InputDouble("V_mode (m^3)##Sim37", &V_mode, 1e-10, 1e-9, "%.3e");
        ImGui::InputDouble("h*nu (J)##Sim37", &h_nu, 1e-20, 1e-19, "%.3e");
        ImGui::InputDouble("tau_c (s)##Sim37", &tau_c, 1e-10, 1e-9, "%.3e");
        ImGui::InputDouble("r (pump ratio)##Sim37", &r, 0.1, 1.0, "%.2f");
    } else {
        ImGui::InputDouble("Cavity L (m)##Sim37", &L_cav, 0.01, 0.1, "%.4f");
        ImGui::InputDouble("Bandwidth (Hz)##Sim37", &delta_nu, 1e11, 1e12, "%.2e");
        ImGui::InputDouble("P_avg (W)##Sim37", &P_avg, 0.1, 1.0, "%.3f");
    }

    if (ImGui::Button("Compute##Sim37"))
    {
        clearPlot();
        std::ostringstream oss;
        if (mode == 0) {
            double E = system.qSwitchPulseEnergy(N_init, V_mode, h_nu);
            double tau = system.qSwitchPulseDuration(tau_c, r);
            double Ppk = system.qSwitchPeakPower(E, tau);
            std::vector<double> rVec, eVec;
            for (int i = 1; i <= 100; ++i) {
                double ri = 1.01 + 10.0 * i / 100.0;
                rVec.push_back(ri);
                double ti = system.qSwitchPulseDuration(tau_c, ri);
                eVec.push_back(system.qSwitchPeakPower(E, ti) * 1e-6);
            }
            addCurve("Peak power (MW)", rVec, eVec);
            oss << "Pulse energy = " << E * 1e3 << " mJ\n";
            oss << "Pulse duration = " << tau * 1e9 << " ns\n";
            oss << "Peak power = " << Ppk * 1e-6 << " MW\n";
            plotTitle = "Q-Switch Peak Power vs Pump Ratio";
            plotXLabel = "r (pump/threshold)";
            plotYLabel = "Peak power (MW)";
        } else {
            double tau = system.modeLockPulseDuration(delta_nu);
            double rep = system.modeLockRepRate(L_cav, 1.0);
            double Ppk = system.modeLockPeakPower(P_avg, rep, tau);
            std::vector<double> bwVec, durVec;
            for (int i = 1; i <= 100; ++i) {
                double bw = 1e11 * i;
                bwVec.push_back(bw * 1e-12);
                durVec.push_back(system.modeLockPulseDuration(bw) * 1e15);
            }
            addCurve("Pulse duration (fs)", bwVec, durVec);
            oss << "Pulse duration = " << tau * 1e15 << " fs\n";
            oss << "Rep rate = " << rep * 1e-6 << " MHz\n";
            oss << "Peak power = " << Ppk * 1e-3 << " kW\n";
            plotTitle = "Transform-Limited Pulse";
            plotXLabel = "Bandwidth (THz)";
            plotYLabel = "Duration (fs)";
        }
        resultText = oss.str();
    }
    if (ImGui::Button("Export CSV##Sim37")) {
        if (mode == 0)
            system.exportQSwitchCSV("qswitch.csv", N_init, V_mode, h_nu, tau_c, 100);
        else
            system.exportModeLockCSV("modelock.csv", L_cav, delta_nu, P_avg, 100);
    }
}

// ── 38: Second Harmonic Generation ──
void GuiApp::renderSim38_SHG()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Second Harmonic Generation");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Second Harmonic Generation"))
    {
        ImGui::TextWrapped(
            "SHG converts two photons at omega to one at 2*omega in a nonlinear crystal. "
            "Efficiency depends on d_eff, crystal length, phase mismatch delta_k. "
            "Phase matching: delta_k = 4*pi*(n_2w - n_w)/lambda. "
            "Coherence length L_c = lambda/(4|n_2w - n_w|). Tuning curve: sinc^2(delta_k*L/2).");
    }

    static double d_eff = 2e-12;
    static double lambda = 1064e-9;
    static double n_omega = 1.494;
    static double n_2omega = 1.527;
    static double I_pump = 1e12;
    static double LMax = 0.02;
    static int numPoints = 500;
    ImGui::InputDouble("d_eff (m/V)##Sim38", &d_eff, 1e-13, 1e-12, "%.3e");
    ImGui::InputDouble("Lambda pump (m)##Sim38", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("n(omega)##Sim38", &n_omega, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("n(2omega)##Sim38", &n_2omega, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("I_pump (W/m^2)##Sim38", &I_pump, 1e10, 1e11, "%.2e");
    ImGui::InputDouble("Crystal L max (m)##Sim38", &LMax, 0.001, 0.01, "%.4f");
    ImGui::SliderInt("Points##Sim38", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim38"))
    {
        clearPlot();
        double dk = system.shgPhaseMismatch(n_omega, n_2omega, lambda);
        std::vector<double> lVec, effVec;
        for (int i = 0; i < numPoints; ++i) {
            double L = LMax * i / (numPoints - 1);
            lVec.push_back(L * 1e3);
            effVec.push_back(system.shgIntensity(d_eff, L, dk, I_pump, lambda, n_omega, n_2omega));
        }
        addCurve("SHG efficiency", lVec, effVec);

        std::ostringstream oss;
        oss << "Phase mismatch dk = " << dk << " rad/m\n";
        oss << "Coherence length = " << system.shgCoherenceLength(n_omega, n_2omega, lambda) * 1e6 << " um\n";
        oss << "Max efficiency (phase matched) = " << system.shgConversionEfficiency(d_eff, LMax, lambda, n_omega, n_2omega, I_pump) << "\n";
        resultText = oss.str();
        plotTitle = "SHG vs Crystal Length";
        plotXLabel = "Crystal length (mm)";
        plotYLabel = "Conversion efficiency";
    }
    if (ImGui::Button("Export CSV##Sim38"))
        system.exportSHGCSV("shg.csv", d_eff, lambda, n_omega, n_2omega, I_pump, LMax, numPoints);
}

// ── 39: Kerr Effect & Self-Phase Modulation ──
void GuiApp::renderSim39_KerrSPM()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Kerr Effect & Self-Phase Modulation");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Kerr Effect & Self-Phase Modulation"))
    {
        ImGui::TextWrapped(
            "Optical Kerr effect: n(I) = n0 + n2*I. Self-phase modulation phi_NL = 2*pi*n2*I*L/lambda. "
            "B-integral = cumulative nonlinear phase. Critical power for self-focusing: "
            "P_cr = 3.77*lambda^2/(8*pi*n0*n2). Self-focusing distance z_sf = w0/sqrt(P/Pcr - 1).");
    }

    static double n0 = 1.45;
    static double n2 = 2.7e-20;
    static double L = 0.01;
    static double lambda = 800e-9;
    static double Imax = 1e16;
    static int numPoints = 500;
    ImGui::InputDouble("n0##Sim39", &n0, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n2 (m^2/W)##Sim39", &n2, 1e-21, 1e-20, "%.3e");
    ImGui::InputDouble("Length L (m)##Sim39", &L, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Wavelength (m)##Sim39", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("I max (W/m^2)##Sim39", &Imax, 1e14, 1e15, "%.2e");
    ImGui::SliderInt("Points##Sim39", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim39"))
    {
        clearPlot();
        std::vector<double> iVec, nVec, phiVec;
        for (int i = 0; i < numPoints; ++i) {
            double I = Imax * i / (numPoints - 1);
            iVec.push_back(I * 1e-12);
            nVec.push_back(system.kerrRefractiveIndex(n0, n2, I));
            phiVec.push_back(system.selfPhaseModulation(n2, I, L, lambda));
        }
        addCurve("SPM phase (rad)", iVec, phiVec);

        double Pcr = system.criticalPowerSelfFocusing(lambda, n0, n2);
        std::ostringstream oss;
        oss << "B-integral at Imax = " << system.bIntegral(n2, Imax, L, lambda) << " rad\n";
        oss << "Critical power = " << Pcr * 1e-6 << " MW\n";
        resultText = oss.str();
        plotTitle = "Self-Phase Modulation";
        plotXLabel = "Intensity (TW/m^2)";
        plotYLabel = "Phase (rad)";
    }
    if (ImGui::Button("Export CSV##Sim39"))
        system.exportKerrCSV("kerr_spm.csv", n0, n2, L, lambda, Imax, numPoints);
}

// ── 40: Optical Parametric Processes ──
void GuiApp::renderSim40_OPA()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Optical Parametric Processes");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Optical Parametric Processes"))
    {
        ImGui::TextWrapped(
            "OPA: pump photon splits into signal + idler. Energy conservation: omega_p = omega_s + omega_i. "
            "Momentum conservation (phase matching): n_p*omega_p = n_s*omega_s + n_i*omega_i. "
            "Parametric gain G = 1 + (gamma^2/g^2)*sinh^2(gL) where g^2 = gamma^2 - (dk/2)^2. "
            "Idler wavelength: 1/lambda_i = 1/lambda_p - 1/lambda_s.");
    }

    static double gamma = 1000.0;
    static double LMax = 0.02;
    static double delta_k = 0.0;
    static double lambda_pump = 532e-9;
    static double lambda_signal = 800e-9;
    static int numPoints = 500;
    ImGui::InputDouble("Gain coeff gamma (m^-1)##Sim40", &gamma, 10.0, 100.0, "%.1f");
    ImGui::InputDouble("L max (m)##Sim40", &LMax, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Phase mismatch dk (m^-1)##Sim40", &delta_k, 10.0, 100.0, "%.1f");
    ImGui::InputDouble("Pump wavelength (m)##Sim40", &lambda_pump, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Signal wavelength (m)##Sim40", &lambda_signal, 1e-9, 1e-8, "%.3e");
    ImGui::SliderInt("Points##Sim40", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim40"))
    {
        clearPlot();
        std::vector<double> lVec, gVec;
        for (int i = 0; i < numPoints; ++i) {
            double L = LMax * i / (numPoints - 1);
            lVec.push_back(L * 1e3);
            gVec.push_back(system.opaGain(gamma, L, delta_k));
        }
        addCurve("OPA Gain", lVec, gVec);

        double lambda_idler = system.opaIdlerWavelength(lambda_pump, lambda_signal);
        std::ostringstream oss;
        oss << "Idler wavelength = " << lambda_idler * 1e9 << " nm\n";
        oss << "Gain at Lmax = " << system.opaGain(gamma, LMax, delta_k) << "\n";
        resultText = oss.str();
        plotTitle = "OPA Gain vs Crystal Length";
        plotXLabel = "Crystal length (mm)";
        plotYLabel = "Gain";
    }
    if (ImGui::Button("Export CSV##Sim40"))
        system.exportOPACSV("opa.csv", gamma, LMax, delta_k, numPoints);
}

// ── 41: Electro-Optic Effect ──
void GuiApp::renderSim41_ElectroOptic()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Electro-Optic Effect");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Electro-Optic Effect"))
    {
        ImGui::TextWrapped(
            "Pockels effect (linear EO): phase shift phi = pi*n_e^3*r*V/(lambda*d). "
            "Half-wave voltage V_pi = lambda*d/(n_e^3*r). "
            "Kerr cell (quadratic EO): phi = 2*pi*K*E^2*L/lambda. "
            "Used in modulators, Q-switches, and pulse pickers.");
    }

    static double n_e = 2.2;
    static double r = 30.8e-12;
    static double d_cryst = 3e-3;
    static double lambda = 632.8e-9;
    static double Vmax = 1000.0;
    static int numPoints = 500;
    ImGui::InputDouble("n_e##Sim41", &n_e, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("r (m/V)##Sim41", &r, 1e-13, 1e-12, "%.3e");
    ImGui::InputDouble("Crystal thickness d (m)##Sim41", &d_cryst, 1e-4, 1e-3, "%.3e");
    ImGui::InputDouble("Wavelength (m)##Sim41", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("V max (V)##Sim41", &Vmax, 10.0, 100.0, "%.1f");
    ImGui::SliderInt("Points##Sim41", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim41"))
    {
        clearPlot();
        std::vector<double> vVec, phiVec, transVec;
        for (int i = 0; i < numPoints; ++i) {
            double V = Vmax * i / (numPoints - 1);
            double phi = system.pockelsPhaseShift(n_e, r, V, lambda, d_cryst);
            vVec.push_back(V);
            phiVec.push_back(phi / M_PI);
            transVec.push_back(std::sin(phi / 2.0) * std::sin(phi / 2.0));
        }
        addCurve("Phase (pi)", vVec, phiVec);
        addCurve("Transmission", vVec, transVec);

        double Vpi = system.halfWaveVoltage(lambda, n_e, r, d_cryst);
        std::ostringstream oss;
        oss << "Half-wave voltage V_pi = " << Vpi << " V\n";
        resultText = oss.str();
        plotTitle = "Electro-Optic Modulator";
        plotXLabel = "Voltage (V)";
        plotYLabel = "Phase (pi) / Transmission";
    }
    if (ImGui::Button("Export CSV##Sim41"))
        system.exportElectroOpticCSV("electro_optic.csv", n_e, r, d_cryst, lambda, Vmax, numPoints);
}

// ── 42: Acousto-Optic Effect ──
void GuiApp::renderSim42_AcoustoOptic()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Acousto-Optic Effect");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Acousto-Optic Effect"))
    {
        ImGui::TextWrapped(
            "Sound waves create a refractive index grating in a medium. "
            "Bragg angle: sin(theta_B) = lambda/(2*lambda_sound). "
            "Frequency shift = f_acoustic. Diffraction efficiency depends on acoustic power. "
            "Bandwidth ~ v_s^2/(lambda*D_beam).");
    }

    static double lambda = 632.8e-9;
    static double v_sound = 5960.0;
    static double fMin = 50e6;
    static double fMax = 200e6;
    static int numPoints = 500;
    ImGui::InputDouble("Wavelength (m)##Sim42", &lambda, 1e-9, 1e-8, "%.3e");
    ImGui::InputDouble("Sound velocity (m/s)##Sim42", &v_sound, 100.0, 1000.0, "%.1f");
    ImGui::InputDouble("f min (Hz)##Sim42", &fMin, 1e6, 1e7, "%.2e");
    ImGui::InputDouble("f max (Hz)##Sim42", &fMax, 1e6, 1e7, "%.2e");
    ImGui::SliderInt("Points##Sim42", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim42"))
    {
        clearPlot();
        std::vector<double> freqVec, braggVec;
        for (int i = 0; i < numPoints; ++i) {
            double f = fMin + (fMax - fMin) * i / (numPoints - 1);
            double lambda_sound = v_sound / f;
            freqVec.push_back(f * 1e-6);
            braggVec.push_back(system.braggAngle(lambda, lambda_sound) * 1e3);
        }
        addCurve("Bragg angle (mrad)", freqVec, braggVec);

        double bw = system.acoustoOpticBandwidth(v_sound, lambda, 1e-3);
        std::ostringstream oss;
        oss << "Bragg angle at 100MHz = " << system.braggAngle(lambda, v_sound / 100e6) * 1e3 << " mrad\n";
        oss << "Bandwidth (D=1mm) = " << bw * 1e-6 << " MHz\n";
        resultText = oss.str();
        plotTitle = "Bragg Angle vs Frequency";
        plotXLabel = "Acoustic frequency (MHz)";
        plotYLabel = "Bragg angle (mrad)";
    }
    if (ImGui::Button("Export CSV##Sim42"))
        system.exportAcoustoOpticCSV("acousto_optic.csv", lambda, v_sound, fMin, fMax, numPoints);
}

// ── 43: Photon Statistics ──
void GuiApp::renderSim43_PhotonStats()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Photon Statistics");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Photon Statistics"))
    {
        ImGui::TextWrapped(
            "Coherent light: Poisson distribution P(n) = n_bar^n * exp(-n_bar) / n!. "
            "Thermal light: Bose-Einstein P(n) = n_bar^n / (1+n_bar)^(n+1). "
            "g(2)(0) = 1 + (var - mean)/mean^2: coherent=1, thermal=2, sub-Poissonian<1. "
            "Mandel Q = (var-mean)/mean: Q=0 Poisson, Q>0 super, Q<0 sub-Poissonian.");
    }

    static double n_mean = 5.0;
    static int nMax = 20;
    ImGui::InputDouble("Mean photon number##Sim43", &n_mean, 0.5, 2.0, "%.2f");
    ImGui::SliderInt("n max##Sim43", &nMax, 5, 50);

    if (ImGui::Button("Compute##Sim43"))
    {
        clearPlot();
        std::vector<double> nVec, poisVec, thermVec;
        for (int n = 0; n <= nMax; ++n) {
            nVec.push_back((double)n);
            poisVec.push_back(OpticalSystem::poissonDistribution(n_mean, n));
            thermVec.push_back(OpticalSystem::thermalDistribution(n_mean, n));
        }
        addCurve("Poisson", nVec, poisVec);
        addCurve("Thermal", nVec, thermVec);

        double var_p = n_mean;
        double var_t = n_mean * (1.0 + n_mean);
        std::ostringstream oss;
        oss << "Poisson: g(2)=" << system.secondOrderCorrelation(var_p, n_mean) << ", Q=" << system.mandelsQParameter(var_p, n_mean) << "\n";
        oss << "Thermal: g(2)=" << system.secondOrderCorrelation(var_t, n_mean) << ", Q=" << system.mandelsQParameter(var_t, n_mean) << "\n";
        resultText = oss.str();
        plotTitle = "Photon Number Distribution";
        plotXLabel = "n";
        plotYLabel = "P(n)";
    }
    if (ImGui::Button("Export CSV##Sim43"))
        system.exportPhotonStatisticsCSV("photon_stats.csv", n_mean, nMax);
}

// ── 44: Hanbury Brown-Twiss ──
void GuiApp::renderSim44_HBT()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Hanbury Brown-Twiss");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Hanbury Brown-Twiss"))
    {
        ImGui::TextWrapped(
            "HBT measures g(2)(tau) = <I(t)I(t+tau)>/<I>^2. "
            "Coherent: g(2)=1 (flat). Thermal: g(2)(0)=2, decays to 1 with tau_c. "
            "Single photon: g(2)(0)=0 (antibunching). "
            "Bunching contrast = exp(-2*delta_t/tau_c).");
    }

    static double tau_c = 1e-9;
    static double tauMax = 5e-9;
    static int numPoints = 500;
    ImGui::InputDouble("Coherence time tau_c (s)##Sim44", &tau_c, 1e-10, 1e-9, "%.3e");
    ImGui::InputDouble("tau max (s)##Sim44", &tauMax, 1e-10, 1e-9, "%.3e");
    ImGui::SliderInt("Points##Sim44", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim44"))
    {
        clearPlot();
        std::vector<double> tauVec, thermalVec, cohVec, spVec;
        for (int i = 0; i < numPoints; ++i) {
            double tau = -tauMax + 2.0 * tauMax * i / (numPoints - 1);
            tauVec.push_back(tau * 1e9);
            thermalVec.push_back(system.hbtG2Thermal(tau, tau_c));
            cohVec.push_back(system.hbtG2Coherent());
            spVec.push_back(system.hbtG2SinglePhoton());
        }
        addCurve("Thermal g(2)", tauVec, thermalVec);
        addCurve("Coherent g(2)", tauVec, cohVec);
        addCurve("Single photon g(2)", tauVec, spVec);

        std::ostringstream oss;
        oss << "g(2)(0) thermal = " << system.hbtG2Thermal(0, tau_c) << "\n";
        oss << "g(2)(0) coherent = " << system.hbtG2Coherent() << "\n";
        oss << "g(2)(0) single photon = " << system.hbtG2SinglePhoton() << "\n";
        oss << "Bunching contrast (dt=tau_c) = " << system.hbtBunchingContrast(tau_c, tau_c) << "\n";
        resultText = oss.str();
        plotTitle = "HBT g(2)(tau)";
        plotXLabel = "Delay (ns)";
        plotYLabel = "g(2)(tau)";
    }
    if (ImGui::Button("Export CSV##Sim44"))
        system.exportHBTCSV("hbt.csv", tau_c, tauMax, numPoints);
}

// ── 45: Beam Splitter & HOM Effect ──
void GuiApp::renderSim45_BeamSplitter()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Beam Splitter & HOM Effect");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Beam Splitter & HOM Effect"))
    {
        ImGui::TextWrapped(
            "Quantum beam splitter: single photon has probabilities R and T for each output. "
            "Hong-Ou-Mandel effect: two indistinguishable photons entering a 50:50 BS always "
            "exit together (bunching). HOM dip: coincidence rate drops to zero at tau=0 "
            "for identical photons.");
    }

    static int mode = 0;
    const char* modes[] = {"Single Photon BS", "HOM Dip"};
    ImGui::Combo("Mode##Sim45", &mode, modes, 2);

    static double R = 0.5;
    static double tau_c = 1e-12;
    static double tauMax = 5e-12;
    static int numPoints = 500;
    ImGui::InputDouble("Reflectance R##Sim45", &R, 0.01, 0.1, "%.3f");
    if (mode == 1) {
        ImGui::InputDouble("tau_c (s)##Sim45", &tau_c, 1e-13, 1e-12, "%.3e");
        ImGui::InputDouble("tau max (s)##Sim45", &tauMax, 1e-13, 1e-12, "%.3e");
    }
    ImGui::SliderInt("Points##Sim45", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim45"))
    {
        clearPlot();
        std::ostringstream oss;
        if (mode == 0) {
            std::vector<double> rVec, paVec, pbVec;
            for (int i = 0; i < numPoints; ++i) {
                double r_i = (double)i / (numPoints - 1);
                double T = 1.0 - r_i;
                auto out = system.beamSplitterSinglePhoton(r_i, T);
                rVec.push_back(r_i);
                paVec.push_back(out.P_oneA);
                pbVec.push_back(out.P_oneB);
            }
            addCurve("P(det A)", rVec, paVec);
            addCurve("P(det B)", rVec, pbVec);

            auto bs = system.beamSplitterSinglePhoton(R, 1.0 - R);
            oss << "P(A)=" << bs.P_oneA << ", P(B)=" << bs.P_oneB << "\n";
            plotTitle = "Single Photon Beam Splitter";
            plotXLabel = "Reflectance";
            plotYLabel = "Detection probability";
        } else {
            std::vector<double> tauVec, dipVec;
            for (int i = 0; i < numPoints; ++i) {
                double tau = -tauMax + 2.0 * tauMax * i / (numPoints - 1);
                tauVec.push_back(tau * 1e12);
                dipVec.push_back(system.hongOuMandelDip(tau, tau_c));
            }
            addCurve("HOM dip", tauVec, dipVec);

            oss << "HOM dip at tau=0: " << system.hongOuMandelDip(0, tau_c) << "\n";
            oss << "Width ~ tau_c = " << tau_c * 1e12 << " ps\n";
            plotTitle = "Hong-Ou-Mandel Dip";
            plotXLabel = "Delay (ps)";
            plotYLabel = "Coincidence rate";
        }
        resultText = oss.str();
    }
    if (ImGui::Button("Export CSV##Sim45")) {
        if (mode == 0)
            system.exportBeamSplitterCSV("beam_splitter.csv", R, numPoints);
        else
            system.exportHOMCSV("hom.csv", tau_c, tauMax, numPoints);
    }
}

// ── 46: Mach-Zehnder Interferometer ──
void GuiApp::renderSim46_MachZehnder()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Mach-Zehnder Interferometer");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Mach-Zehnder Interferometer"))
    {
        ImGui::TextWrapped(
            "MZI: two beam splitters and two mirrors. Output ports: "
            "I1 = cos^2(phi/2), I2 = sin^2(phi/2). Phase phi = 2*pi*n*L/lambda. "
            "Sensitivity |dI/dphi| = |sin(phi)|. "
            "Quantum noise limit (shot noise): delta_phi = 1/sqrt(N_photons).");
    }

    static double phiMax = 4.0 * M_PI;
    static int N_photons = 10000;
    static int numPoints = 500;
    ImGui::InputDouble("Phase max (rad)##Sim46", &phiMax, 0.1, 1.0, "%.2f");
    ImGui::SliderInt("N photons##Sim46", &N_photons, 100, 1000000);
    ImGui::SliderInt("Points##Sim46", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim46"))
    {
        clearPlot();
        std::vector<double> phiVec, i1Vec, i2Vec;
        for (int i = 0; i < numPoints; ++i) {
            double phi = phiMax * i / (numPoints - 1);
            phiVec.push_back(phi);
            i1Vec.push_back(system.machZehnderOutput1(phi));
            i2Vec.push_back(system.machZehnderOutput2(phi));
        }
        addCurve("Port 1", phiVec, i1Vec);
        addCurve("Port 2", phiVec, i2Vec);

        std::ostringstream oss;
        oss << "Phase sensitivity at pi/2 = " << system.machZehnderSensitivity(M_PI / 2.0) << "\n";
        oss << "Quantum noise limit = " << system.machZehnderQuantumNoise(N_photons) << " rad\n";
        oss << "Phase for n=1.5, L=1mm, 633nm = " << system.machZehnderPhaseShift(1.5, 1e-3, 632.8e-9) << " rad\n";
        resultText = oss.str();
        plotTitle = "Mach-Zehnder Output";
        plotXLabel = "Phase (rad)";
        plotYLabel = "Normalized Intensity";
    }
    if (ImGui::Button("Export CSV##Sim46"))
        system.exportMachZehnderCSV("mach_zehnder.csv", phiMax, numPoints);
}

// ── 47: Squeezed Light ──
void GuiApp::renderSim47_SqueezedLight()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Squeezed Light");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Squeezed Light"))
    {
        ImGui::TextWrapped(
            "Squeezed states have reduced noise in one quadrature at the expense of the other. "
            "Var(X) = exp(-2r), Var(P) = exp(2r) where r is squeeze parameter. "
            "Photon number <n> = sinh^2(r). Noise power N(theta) depends on quadrature angle. "
            "Squeezing in dB = -10*log10(exp(-2r)).");
    }

    static double rMax = 3.0;
    static int numPoints = 500;
    ImGui::InputDouble("r max##Sim47", &rMax, 0.1, 1.0, "%.2f");
    ImGui::SliderInt("Points##Sim47", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim47"))
    {
        clearPlot();
        std::vector<double> rVec, varXVec, varPVec, nVec;
        for (int i = 0; i < numPoints; ++i) {
            double r = rMax * i / (numPoints - 1);
            rVec.push_back(r);
            varXVec.push_back(system.squeezedVarianceX(r));
            varPVec.push_back(system.squeezedVarianceP(r));
            nVec.push_back(system.squeezedPhotonNumber(r));
        }
        addCurve("Var(X)", rVec, varXVec);
        addCurve("Var(P)", rVec, varPVec);

        std::ostringstream oss;
        oss << "At r=1: Var(X) = " << system.squeezedVarianceX(1.0) << ", Var(P) = " << system.squeezedVarianceP(1.0) << "\n";
        oss << "Photon number at r=1: " << system.squeezedPhotonNumber(1.0) << "\n";
        oss << "Squeezing at r=1: " << system.squeezedSqueezingdB(1.0) << " dB\n";
        resultText = oss.str();
        plotTitle = "Squeezed State Variances";
        plotXLabel = "Squeeze parameter r";
        plotYLabel = "Variance";
    }
    if (ImGui::Button("Export CSV##Sim47"))
        system.exportSqueezedLightCSV("squeezed.csv", rMax, numPoints);
}

// ── 48: Quantum Key Distribution (BB84) ──
void GuiApp::renderSim48_BB84()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Quantum Key Distribution (BB84)");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Quantum Key Distribution (BB84)"))
    {
        ImGui::TextWrapped(
            "BB84 protocol: Alice sends qubits in random bases, Bob measures in random bases. "
            "QBER = e_detector + e_channel. Secure key rate R = 1 - h(QBER) - f*h(QBER). "
            "Max QBER for security: 11%. Max distance limited by fiber attenuation.");
    }

    static double e_det = 0.01;
    static double attenuation = 0.2;
    static double eta_det = 0.1;
    static double f_error = 1.16;
    static int numPoints = 500;
    ImGui::InputDouble("Detector error e_det##Sim48", &e_det, 0.001, 0.01, "%.4f");
    ImGui::InputDouble("Attenuation (dB/km)##Sim48", &attenuation, 0.01, 0.1, "%.3f");
    ImGui::InputDouble("Detector efficiency##Sim48", &eta_det, 0.01, 0.1, "%.3f");
    ImGui::InputDouble("Error correction f##Sim48", &f_error, 0.01, 0.1, "%.3f");
    ImGui::SliderInt("Points##Sim48", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim48"))
    {
        clearPlot();
        std::vector<double> qberVec, rateVec, miVec, eveVec;
        for (int i = 0; i < numPoints; ++i) {
            double qber = 0.15 * i / (numPoints - 1);
            qberVec.push_back(qber * 100.0);
            rateVec.push_back(system.bb84SecureKeyRate(qber, f_error));
            miVec.push_back(system.bb84MutualInformation(qber));
            eveVec.push_back(system.bb84EveInformation(qber));
        }
        addCurve("Key rate", qberVec, rateVec);
        addCurve("Mutual info", qberVec, miVec);
        addCurve("Eve info", qberVec, eveVec);

        double qber = system.bb84QBER(e_det, 0.02);
        double maxDist = system.bb84MaxDistance(attenuation, eta_det, 1e-6);
        std::ostringstream oss;
        oss << "QBER (e_det + 2% channel) = " << qber * 100.0 << "%\n";
        oss << "Secure key rate = " << system.bb84SecureKeyRate(qber, f_error) << "\n";
        oss << "Max distance = " << maxDist << " km\n";
        resultText = oss.str();
        plotTitle = "BB84 Key Rate vs QBER";
        plotXLabel = "QBER (%)";
        plotYLabel = "Rate / Information";
    }
    if (ImGui::Button("Export CSV##Sim48"))
        system.exportBB84CSV("bb84.csv", e_det, attenuation, numPoints);
}

// ── 49: Plasmonics & SPR ──
void GuiApp::renderSim49_Plasmonics()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Plasmonics & SPR");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Plasmonics & SPR"))
    {
        ImGui::TextWrapped(
            "Drude model: eps(omega) = 1 - omega_p^2/(omega^2 + i*gamma*omega). "
            "SPP: surface wave at metal-dielectric interface, k_spp = (omega/c)*sqrt(eps_m*eps_d/(eps_m+eps_d)). "
            "SPR angle for Kretschmann config: sin(theta) = k_spp/(n_prism*k0). "
            "LSPR: Froehlich condition eps_real = -2*eps_medium.");
    }

    static double omega_p = 1.37e16;
    static double gamma_d = 3.23e13;
    static double eps_d = 1.77;
    static double n_prism = 1.515;
    static int numPoints = 500;
    ImGui::InputDouble("omega_p (rad/s)##Sim49", &omega_p, 1e14, 1e15, "%.3e");
    ImGui::InputDouble("gamma (rad/s)##Sim49", &gamma_d, 1e12, 1e13, "%.3e");
    ImGui::InputDouble("eps_dielectric##Sim49", &eps_d, 0.01, 0.1, "%.4f");
    ImGui::InputDouble("n_prism##Sim49", &n_prism, 0.01, 0.1, "%.4f");
    ImGui::SliderInt("Points##Sim49", &numPoints, 100, 2000);

    if (ImGui::Button("Compute##Sim49"))
    {
        clearPlot();
        std::vector<double> omegaVec, kVec;
        double omMax = omega_p * 0.8;
        for (int i = 1; i < numPoints; ++i) {
            double om = omMax * i / numPoints;
            auto eps = system.drude(om, omega_p, gamma_d);
            double em_re = eps.real();
            if (em_re < -eps_d) {
                omegaVec.push_back(om / omega_p);
                kVec.push_back(system.sppWavevector(om, em_re, eps_d) / (om / OpticsConstants::c));
            }
        }
        addCurve("k_spp / k_0", omegaVec, kVec);

        double om_test = omega_p * 0.5;
        auto eps_test = system.drude(om_test, omega_p, gamma_d);
        double spr_angle = system.sprResonanceAngle(n_prism, eps_test.real(), eps_d);
        std::ostringstream oss;
        oss << "SPR angle (at omega_p/2) = " << spr_angle * 180.0 / M_PI << " deg\n";
        oss << "LSPR wavelength (eps_inf=1, eps_m=1.77) = " << system.localizedSPRWavelength(1.0, omega_p, eps_d) * 1e9 << " nm\n";
        resultText = oss.str();
        plotTitle = "SPP Dispersion";
        plotXLabel = "omega / omega_p";
        plotYLabel = "k_spp / k_0";
    }
    if (ImGui::Button("Export CSV##Sim49"))
        system.exportSPPDispersionCSV("spp_dispersion.csv", omega_p, gamma_d, eps_d, omega_p * 0.8, numPoints);
}

// ── 50: Metamaterials & Negative Refraction ──
void GuiApp::renderSim50_Metamaterials()
{
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1, 1), "Metamaterials & Negative Refraction");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Theory: Metamaterials & Negative Refraction"))
    {
        ImGui::TextWrapped(
            "Metamaterials can have negative epsilon (from Drude model) and negative mu "
            "(from magnetic resonance). When both are negative, n = -sqrt(|eps*mu|). "
            "Pendry perfect lens: resolution limited by slab thickness d, not wavelength. "
            "Transformation optics enables cloaking via coordinate transformations.");
    }

    static double omega_p = 1e16;
    static double gamma_e = 1e13;
    static double omega_m = 5e15;
    static double F = 0.56;
    static double gamma_m = 1e13;
    static double omegaMax = 2e16;
    static int numPoints = 1000;
    ImGui::InputDouble("omega_p (rad/s)##Sim50", &omega_p, 1e14, 1e15, "%.3e");
    ImGui::InputDouble("gamma_e (rad/s)##Sim50", &gamma_e, 1e12, 1e13, "%.3e");
    ImGui::InputDouble("omega_m (rad/s)##Sim50", &omega_m, 1e14, 1e15, "%.3e");
    ImGui::InputDouble("F (filling)##Sim50", &F, 0.01, 0.1, "%.3f");
    ImGui::InputDouble("gamma_m (rad/s)##Sim50", &gamma_m, 1e12, 1e13, "%.3e");
    ImGui::InputDouble("omega max (rad/s)##Sim50", &omegaMax, 1e15, 1e16, "%.3e");
    ImGui::SliderInt("Points##Sim50", &numPoints, 200, 5000);

    if (ImGui::Button("Compute##Sim50"))
    {
        clearPlot();
        std::vector<double> omVec, epsVec, muVec, nVec;
        for (int i = 1; i < numPoints; ++i) {
            double om = omegaMax * i / numPoints;
            double eps = system.metamaterialEpsilon(om, omega_p, gamma_e);
            double mu = system.metamaterialMu(om, omega_m, F, gamma_m);
            double n = system.metamaterialRefractiveIndex(eps, mu);
            omVec.push_back(om * 1e-15);
            epsVec.push_back(eps);
            muVec.push_back(mu);
            nVec.push_back(n);
        }
        addCurve("epsilon", omVec, epsVec);
        addCurve("mu", omVec, muVec);
        addCurve("n", omVec, nVec);

        std::ostringstream oss;
        oss << "Pendry lens resolution (d=100nm) = " << system.pendryPerfectLensResolution(100e-9, 500e-9) * 1e9 << " nm\n";
        oss << "Neg. refraction: theta_i=30 deg, n=-1 -> theta_t = " << system.negativeRefractionAngle(M_PI / 6.0, 1.0, -1.0) * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
        plotTitle = "Metamaterial Parameters";
        plotXLabel = "omega (PHz)";
        plotYLabel = "epsilon / mu / n";
    }
    if (ImGui::Button("Export CSV##Sim50"))
        system.exportMetamaterialCSV("metamaterial.csv", omega_p, gamma_e, omega_m, F, gamma_m, omegaMax, numPoints);
}

