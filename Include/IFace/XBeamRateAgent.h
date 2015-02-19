#pragma once

namespace XBR
{

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

}; //