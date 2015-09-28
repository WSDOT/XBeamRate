#pragma once

#include <GraphingTypes.h>
#include <IFace\AnalysisResults.h>

class CXBRLiveLoadGraphDefinition
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

   CXBRLiveLoadGraphDefinition();
   
   // constructor for limit states
   CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LimitState ls);
   
   // constructor for combinations
   CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,xbrTypes::CombinedForceType comb);
   
   // constructor for product loads
   CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,xbrTypes::ProductForceType type);
   
   // constructor for vehicular live loads
   CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);

   // constructor for live loads
   CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType);


   // constructor for capacity
   CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name);

   bool operator< (const CXBRLiveLoadGraphDefinition& rOther) const
   {
      return m_ID < rOther.m_ID;
   }
};

class CXBRLiveLoadGraphDefinitions
{
public:
   CXBRLiveLoadGraphDefinitions();

   void AddGraphDefinition(const CXBRLiveLoadGraphDefinition& def);
   IndexType GetGraphDefinitionCount() const;
   CXBRLiveLoadGraphDefinition& GetGraphDefinition(IndexType graphIdx);
   const CXBRLiveLoadGraphDefinition& GetGraphDefinition(IndexType graphIdx) const;
   CXBRLiveLoadGraphDefinition& FindGraphDefinition(IDType graphID);
   const CXBRLiveLoadGraphDefinition& FindGraphDefinition(IDType graphID) const;
   IndexType GetGraphIndex(IDType graphID) const;
   void RemoveGraphDefinition(IndexType graphIdx);
   void RemoveGraphDefinitionByID(IDType graphID);

   void Clear();

private:
   typedef std::vector<CXBRLiveLoadGraphDefinition> GraphDefinitionContainer;
   typedef GraphDefinitionContainer::iterator GraphDefinitionIterator;
   typedef GraphDefinitionContainer::const_iterator ConstGraphDefinitionIterator;
   GraphDefinitionContainer m_Definitions;
};
