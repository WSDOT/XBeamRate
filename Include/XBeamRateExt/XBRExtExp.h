#pragma once

// Define BUILDXBREXTLIB when building this library
// For static builds, define ENGTOOLSLIB
// For static binding, define ENGTOOLSLIB
// For dynamic binding, nothing is required to be defined

#if defined (BUILDXBREXTLIB) && !defined(XBREXTLIB)
#define XBREXTCLASS __declspec(dllexport)
#define XBREXTFUNC  __declspec(dllexport)
#define XBREXTTPL   template class XBREXTCLASS
#elif defined(XBREXTLIB)
#define XBREXTCLASS
#define XBREXTFUNC
#define XBREXTTPL
#else
#define XBREXTCLASS __declspec(dllimport)
#define XBREXTFUNC
#define XBREXTTPL   extern template class XBREXTCLASS
#endif

#include <XBeamRateExt\AutoLib.h>
