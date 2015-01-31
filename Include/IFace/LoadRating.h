#pragma once

/*****************************************************************************
INTERFACE
   ILoadRating

DESCRIPTION
   Interface for getting load rating results
*****************************************************************************/
// {C22CE4A8-5E29-4911-9F84-B772406A4955}
DEFINE_GUID(IID_ILoadRating, 
0xc22ce4a8, 0x5e29, 0x4911, 0x9f, 0x84, 0xb7, 0x72, 0x40, 0x6a, 0x49, 0x55);
interface ILoadRating : IUnknown
{
   virtual Float64 GetRatingFactor() = 0;
};
