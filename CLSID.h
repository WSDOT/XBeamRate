///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2021  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_GUID(CLSID_XBeamRateAppPlugin, 0xB195CE70, 0x2D27, 0x4fa6, 0xB0, 0xE5, 0x99, 0xD0, 0x90, 0x22, 0xD6, 0x62);
struct __declspec(uuid("{B195CE70-2D27-4fa6-B0E5-99D09022D662}")) XBeamRateAppPlugin;

DEFINE_GUID(CLSID_XBeamRateComponentInfo, 0x8CB5644D, 0x5CC5, 0x4fca, 0xA8, 0x6C, 0x0A, 0x1D, 0x56, 0x6D, 0x30, 0xCB);
struct __declspec(uuid("{8CB5644D-5CC5-4fca-A86C-0A1D566D30CB}")) XBeamRateComponentInfo;

DEFINE_GUID(CLSID_XBeamRateAgent, 0x60BF2930, 0x673C, 0x4d29, 0xB6, 0x54, 0x8A, 0x2E, 0x08, 0x79, 0xDE, 0x2B);
struct __declspec(uuid("{60BF2930-673C-4d29-B654-8A2E0879DE2B}")) XBeamRateAgent;

DEFINE_GUID(CLSID_PGSuperComponentInfo, 0xDA6D93DF, 0x1907, 0x4e12, 0xA6, 0x49, 0x30, 0xE2, 0x07, 0x8F, 0xFC, 0x8A);
struct __declspec(uuid("{DA6D93DF-1907-4e12-A649-30E2078FFC8A}")) PGSuperComponentInfo;

DEFINE_GUID(CLSID_PGSpliceComponentInfo, 0xD9B7C5C6, 0x0233, 0x48cb, 0x89, 0xF5, 0xC8, 0x5A, 0xD9, 0xA8, 0x2E, 0x2D);
struct __declspec(uuid("{D9B7C5C6-0233-48cb-89F5-C85AD9A82E2D}")) PGSpliceComponentInfo;

DEFINE_GUID(CLSID_PGSuperDataExporter,0xB3773B17, 0x08A0, 0x4f2b, 0x99, 0xE8, 0xC8, 0x07, 0x7C, 0x3D, 0x21, 0x66);
struct __declspec(uuid("{B3773B17-08A0-4f2b-99E8-C8077C3D2166}")) PGSuperDataExporter;
