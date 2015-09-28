#pragma once

#include <GraphingTypes.h>
#include <IFace\AnalysisResults.h>

class CXBRAnalysisResultsGraphDefinition
{
public:
   IDType m_ID;
   std::_tstring m_Name;
   GraphType m_GraphType;

   union LoadType 
   {
      pgsTypes::LimitState         LimitStateType;
      xbrTypes::ProductForceType   ProductLoadType;
      xbrTypes::CombinedForceType  CombinedLoadType;
      pgsTypes::LoadRatingType     LiveLoadType;
   } m_LoadType;

   VehicleIndexType m_VehicleIndex;

   CXBRAnalysisResultsGraphDefinition();
   
   // constructor for limit states
   CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LimitState ls);
   
   // constructor for combinations
   CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,xbrTypes::CombinedForceType comb);
   
   // constructor for product loads
   CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,xbrTypes::ProductForceType type);
   
   // constructor for vehicular live loads
   CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);

   // constructor for live loads
   CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType);


   // constructor for capacity
   CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name);

   bool operator< (const CXBRAnalysisResultsGraphDefinition& rOther) const
   {
      return m_ID < rOther.m_ID;
   }
};

class CXBRAnalysisResultsGraphDefinitions
{
public:
   CXBRAnalysisResultsGraphDefinitions();

   void AddGraphDefinition(const CXBRAnalysisResultsGraphDefinition& def);
   IndexType GetGraphDefinitionCount() const;
   CXBRAnalysisResultsGraphDefinition& GetGraphDefinition(IndexType graphIdx);
   const CXBRAnalysisResultsGraphDefinition& GetGraphDefinition(IndexType graphIdx) const;
   CXBRAnalysisResultsGraphDefinition& FindGraphDefinition(IDType graphID);
   const CXBRAnalysisResultsGraphDefinition& FindGraphDefinition(IDType graphID) const;
   IndexType GetGraphIndex(IDType graphID) const;
   void RemoveGraphDefinition(IndexType graphIdx);
   void RemoveGraphDefinitionByID(IDType graphID);

   void Clear();

private:
   typedef std::vector<CXBRAnalysisResultsGraphDefinition> GraphDefinitionContainer;
   typedef GraphDefinitionContainer::iterator GraphDefinitionIterator;
   typedef GraphDefinitionContainer::const_iterator ConstGraphDefinitionIterator;
   GraphDefinitionContainer m_Definitions;
};
