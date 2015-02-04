#pragma once

/*****************************************************************************
INTERFACE
   IProject

DESCRIPTION
   Interface for getting this application going.
*****************************************************************************/
// {AAF586AA-D06E-446b-9EB2-CA916427AD9E}
DEFINE_GUID(IID_IProject, 
0xaaf586aa, 0xd06e, 0x446b, 0x9e, 0xb2, 0xca, 0x91, 0x64, 0x27, 0xad, 0x9e);
interface IProject : IUnknown
{
   virtual void SetProjectName(LPCTSTR strName) = 0;
   virtual LPCTSTR GetProjectName() = 0;

   virtual Float64 GetLeftOverhang() = 0;
   virtual Float64 GetRightOverhang() = 0;
   virtual IndexType GetColumnCount() = 0;
   virtual Float64 GetColumnHeight(IndexType colIdx) = 0;
   virtual xbrTypes::ColumnBaseType GetColumnBaseType(IndexType colIdx) = 0;
   virtual Float64 GetSpacing(IndexType spaceIdx) = 0;
};

/*****************************************************************************
INTERFACE
   IProjectEventSink

DESCRIPTION
   Callback interface for changes to the project data
*****************************************************************************/
// {E677C320-9E35-4ce2-9FA6-083E99F87742}
DEFINE_GUID(IID_IProjectEventSink, 
0xe677c320, 0x9e35, 0x4ce2, 0x9f, 0xa6, 0x8, 0x3e, 0x99, 0xf8, 0x77, 0x42);
interface IProjectEventSink : IUnknown
{
   virtual HRESULT OnProjectChanged() = 0;
};
