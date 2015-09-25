#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <PgsExt\PierData2.h>

xbrTypes::SuperstructureConnectionType XBREXTFUNC GetSuperstructureConnectionType(pgsTypes::BoundaryConditionType bcType);
xbrTypes::SuperstructureConnectionType XBREXTFUNC GetSuperstructureConnectionType(pgsTypes::PierSegmentConnectionType connType);
bool XBREXTFUNC IsStandAlone();
bool XBREXTFUNC IsPGSExtension();
bool XBREXTFUNC CanModelPier(PierIDType pierID,StatusGroupIDType statusGroupID,StatusCallbackIDType callbackID);

void XBREXTFUNC GetLaneInfo(Float64 Wcc,Float64* pWlane,IndexType* pnLanes,Float64* pWloadedLane);
