#pragma once

// ═══════════════════════════════════════════════════════════════════════════════
//  DLL / shared-library export macros for the OpticsPhysics library.
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef OPTICS_PHYSICS_SHARED
    #ifdef _MSC_VER
        #ifdef OPTICS_PHYSICS_EXPORTS
            #define OPTICS_API __declspec(dllexport)
        #else
            #define OPTICS_API __declspec(dllimport)
        #endif
    #else
        #define OPTICS_API __attribute__((visibility("default")))
    #endif
#else
    #define OPTICS_API
#endif
