#ifndef JCON_H
#define JCON_H

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
    #define JCON_HELPER_DLL_IMPORT __declspec(dllimport)
    #define JCON_HELPER_DLL_EXPORT __declspec(dllexport)
    #define JCON_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define JCON_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define JCON_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define JCON_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #else
        #define JCON_HELPER_DLL_IMPORT
        #define JCON_HELPER_DLL_EXPORT
        #define JCON_HELPER_DLL_LOCAL
    #endif
#endif

// Use the generic helper definitions above to define JCON_API and JCON_LOCAL.
// JCON_API is used for the public API symbols. It either DLL imports or DLL
// exports (or does nothing for static build) JCON_LOCAL is used for non-api
// symbols.

#ifdef JCON_DLL // defined if JCON is compiled as a DLL
    #ifdef JCON_DLL_EXPORTS // defined if building the DLL (vs. using it)
        #define JCON_API JCON_HELPER_DLL_EXPORT
    #else
        #define JCON_API JCON_HELPER_DLL_IMPORT
    #endif // JCON_DLL_EXPORTS
    #define JCON_LOCAL JCON_HELPER_DLL_LOCAL
#else // JCON_DLL is not defined: this means JCON is a static lib.
    #define JCON_API
    #define JCON_LOCAL
#endif // JCON_DLL

#endif
