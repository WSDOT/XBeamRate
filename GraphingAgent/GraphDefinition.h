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
      XBRProductForceType          ProductLoadType;
      XBRCombinedForceType         CombinedLoadType;
      pgsTypes::LoadRatingType     LiveLoadType;
   } m_LoadType;

   VehicleIndexType m_VehicleIndex;

   CGraphDefinition();
   
   // constructor for limit states
   CGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LimitState ls);
   
   // constructor for combinations
   CGraphDefinition(IDType id,LPCTSTR name,XBRCombinedForceType comb);
   
   // constructor for product loads
   CGraphDefinition(IDType id,LPCTSTR name,XBRProductForceType type);
   
   // constructor for vehicular live loads
   CGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIndex);

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
