#pragma once

namespace XBR
{

/*****************************************************************************
INTERFACE
   IProject

DESCRIPTION
   Interface for getting this application going.
*****************************************************************************/
// {2600A729-D7E6-44f6-9F9B-DF086FF9E53B}
DEFINE_GUID(IID_IProject, 
0x2600a729, 0xd7e6, 0x44f6, 0x9f, 0x9b, 0xdf, 0x8, 0x6f, 0xf9, 0xe5, 0x3b);
interface IProject : IUnknown
{
   virtual void SetProjectName(LPCTSTR strName) = 0;
   virtual LPCTSTR GetProjectName() = 0;

   virtual void SetOverhangs(Float64 left,Float64 right) = 0;
   virtual Float64 GetLeftOverhang() = 0;
   virtual Float64 GetRightOverhang() = 0;

   virtual void SetColumns(IndexType nColumns,Float64 height,Float64 spacing) = 0;
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
// {9DD03140-A788-4e46-A283-0B343956A619}
DEFINE_GUID(IID_IProjectEventSink, 
0x9dd03140, 0xa788, 0x4e46, 0xa2, 0x83, 0xb, 0x34, 0x39, 0x56, 0xa6, 0x19);
interface IProjectEventSink : IUnknown
{
   virtual HRESULT OnProjectChanged() = 0;
};

}; //