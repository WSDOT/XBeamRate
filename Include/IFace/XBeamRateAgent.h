#pragma once

#include <WBFLUnitServer.h>

/*****************************************************************************
INTERFACE
   IXBeamRateAgent

DESCRIPTION
   This is a dummy interface used to determing if XBeam Rate is acting
   as a stand alone application or an extension to PGSuper/PGSplice.
   This interface is implemented on XBeamRateAgent. If a request for this
   interface fails, the XBeam Rate is acting as a stand alone application.
*****************************************************************************/
// {D07B5589-24FA-48b3-8C15-1371C80353AF}
DEFINE_GUID(IID_IXBeamRateAgent, 
0xd07b5589, 0x24fa, 0x48b3, 0x8c, 0x15, 0x13, 0x71, 0xc8, 0x3, 0x53, 0xaf);
interface IXBeamRateAgent : IUnknown
{
   // always returns true. If this interface is implemented
   // XBeam Rate is an extension to PGSuper/PGSplice
   virtual bool IsExtendingPGSuper() = 0;
};

/*****************************************************************************
INTERFACE
   IXBeamRate

DESCRIPTION
*****************************************************************************/
// {3AF5D94F-6CC6-4aec-907E-6663BB1317E8}
DEFINE_GUID(IID_IXBeamRate, 
0x3af5d94f, 0x6cc6, 0x4aec, 0x90, 0x7e, 0x66, 0x63, 0xbb, 0x13, 0x17, 0xe8);
interface IXBeamRate : IUnknown
{
   virtual void GetUnitServer(IUnitServer** ppUnitServer) = 0;
};
