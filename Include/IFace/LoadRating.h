#pragma once

/*****************************************************************************
INTERFACE
   ILoadRating

DESCRIPTION
   Interface for getting load rating results
*****************************************************************************/
// {15281829-5F1E-4a78-9A9C-E619A7B1E0F6}
DEFINE_GUID(IID_ILoadRating, 
0x15281829, 0x5f1e, 0x4a78, 0x9a, 0x9c, 0xe6, 0x19, 0xa7, 0xb1, 0xe0, 0xf6);
interface ILoadRating : IUnknown
{
   virtual Float64 GetRatingFactor() = 0;
};
