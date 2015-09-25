#pragma once

#include <GraphingTypes.h>
#include <IFace\AnalysisResults.h>

class CGraphDefinition
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

   CGraphDefinition();
   
   // constructor for limit states
   CGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LimitState ls);
   
   // constructor for combinations
   CGraphDefinition(IDType id,LPCTSTR name,xbrTypes::CombinedForceType comb);
   
   // constructor for product loads
   CGraphDefinition(IDType id,LPCTSTR name,xbrTypes::ProductForceType type);
   
   // constructor for vehicular live loads
   CGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);

   // constructor for live loads
   CGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType);


   // constructor for capacity
   CGraphDefinition(IDType id,LPCTSTR name);

   bool operator< (const CGraphDefinition& rOther) const
   {
      return m_ID < rOther.m_ID;
   }
};

class CGraphDefinitions
{
public:
   CGraphDefinitions();

   void AddGraphDefinition(const CGraphDefinition& def);
   IndexType GetGraphDefinitionCount() const;
   CGraphDefinition& GetGraphDefinition(IndexType graphIdx);
   const CGraphDefinition& GetGraphDefinition(IndexType graphIdx) const;
   CGraphDefinition& FindGraphDefinition(IDType graphID);
   const CGraphDefinition& FindGraphDefinition(IDType graphID) const;
   IndexType GetGraphIndex(IDType graphID) const;
   void RemoveGraphDefinition(IndexType graphIdx);
   void RemoveGraphDefinitionByID(IDType graphID);

   void Clear();

private:
   typedef std::vector<CGraphDefinition> GraphDefinitionContainer;
   typedef GraphDefinitionContainer::iterator GraphDefinitionIterator;
   typedef GraphDefinitionContainer::const_iterator ConstGraphDefinitionIterator;
   GraphDefinitionContainer m_Definitions;
};
