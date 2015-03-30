// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XBEAMRATEEXT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XBEAMRATEEXT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XBEAMRATEEXT_EXPORTS
#define XBEAMRATEEXT_API __declspec(dllexport)
#else
#define XBEAMRATEEXT_API __declspec(dllimport)
#endif

// This class is exported from the XBeamRateExt.dll
class XBEAMRATEEXT_API CXBeamRateExt {
public:
	CXBeamRateExt(void);
	// TODO: add your methods here.
};

extern XBEAMRATEEXT_API int nXBeamRateExt;

XBEAMRATEEXT_API int fnXBeamRateExt(void);
