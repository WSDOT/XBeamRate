
#include "stdafx.h"
#include "LiveLoadGraphDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition()
{
}

CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition(
IDType id,
LPCTSTR name,
pgsTypes::LimitState ls
): m_ID(id),m_Name(name)
{
   m_GraphType = graphLimitState;
   m_LoadType.LimitStateType = ls;
}

// constructor for combinations
CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,xbrTypes::CombinedForceType comb) :
m_ID(id), m_Name(name)
{
   m_GraphType = graphCombined;
   m_LoadType.CombinedLoadType = comb;
}

// constructor for product loads
CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition(
IDType id,
LPCTSTR name,
xbrTypes::ProductForceType type
): m_ID(id),m_Name(name)
{
   m_GraphType = graphProduct;
   m_LoadType.ProductLoadType = type;
}

// constructor for vehicular live loads
CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) :
m_ID(id),
m_Name(name)
{
   m_GraphType = graphVehicularLiveLoad;
   m_LoadType.LiveLoadType = ratingType;
   m_VehicleIndex = vehicleIdx;
}

// constructor for live loads
CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType) :
m_ID(id),
m_Name(name)
{
   m_GraphType = graphLiveLoad;
   m_LoadType.LiveLoadType = ratingType;
}

// constructor for ultimate forces
CXBRLiveLoadGraphDefinition::CXBRLiveLoadGraphDefinition(
IDType id,
LPCTSTR name
): m_ID(id),m_Name(name)
{
   m_GraphType = graphCapacity;
}

//void CAnalysisResultsGraphDefinition::AddIntervals(const std::vector<IntervalIndexType>& intervals)
//{
//   // add new intervals to vector
//   m_IntervalApplicability.insert(intervals.begin(),intervals.end());
//}

//////////////////////////////////////////////////////////////////////
CXBRLiveLoadGraphDefinitions::CXBRLiveLoadGraphDefinitions()
{
}

void CXBRLiveLoadGraphDefinitions::AddGraphDefinition(const CXBRLiveLoadGraphDefinition& def)
{
   m_Definitions.push_back(def);
}

IndexType CXBRLiveLoadGraphDefinitions::GetGraphDefinitionCount() const
{
   return m_Definitions.size();
}

CXBRLiveLoadGraphDefinition& CXBRLiveLoadGraphDefinitions::GetGraphDefinition(IndexType graphIdx)
{
   return m_Definitions[graphIdx];
}

const CXBRLiveLoadGraphDefinition& CXBRLiveLoadGraphDefinitions::GetGraphDefinition(IndexType graphIdx) const
{
   return m_Definitions[graphIdx];
}

CXBRLiveLoadGraphDefinition& CXBRLiveLoadGraphDefinitions::FindGraphDefinition(IDType graphID)
{
   GraphDefinitionIterator iter(m_Definitions.begin());
   GraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++ )
   {
      CXBRLiveLoadGraphDefinition& def(*iter);
      if ( def.m_ID == graphID )
      {
         return def;
      }
   }

   ATLASSERT(false); // should never get here
   return m_Definitions.back();
}

const CXBRLiveLoadGraphDefinition& CXBRLiveLoadGraphDefinitions::FindGraphDefinition(IDType graphID) const
{
   ConstGraphDefinitionIterator iter(m_Definitions.begin());
   ConstGraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++ )
   {
      const CXBRLiveLoadGraphDefinition& def(*iter);
      if ( def.m_ID == graphID )
      {
         return def;
      }
   }

   ATLASSERT(false); // should never get here
   return m_Definitions.back();
}

IndexType CXBRLiveLoadGraphDefinitions::GetGraphIndex(IDType graphID) const
{
   IndexType index = 0;
   ConstGraphDefinitionIterator iter(m_Definitions.begin());
   ConstGraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++, index++ )
   {
      const CXBRLiveLoadGraphDefinition& def = *iter;
      if ( def.m_ID == graphID )
      {
         return index;
      }
   }

   ATLASSERT(false); // graphID not found
   return INVALID_INDEX;
}

void CXBRLiveLoadGraphDefinitions::RemoveGraphDefinition(IndexType graphIdx)
{
   m_Definitions.erase(m_Definitions.begin()+graphIdx);
}

void CXBRLiveLoadGraphDefinitions::RemoveGraphDefinitionByID(IDType graphID)
{
   RemoveGraphDefinition(GetGraphIndex(graphID));
}

//std::_tstring CXBRLiveLoadGraphDefinitions::GetDefaultLoadCase(IntervalIndexType intervalIdx) const
//{
//   // return the of the first graph definition that is applicable to the given stage
//   ConstGraphDefinitionIterator iter;
//   for ( iter = m_Definitions.begin(); iter != m_Definitions.end(); iter++ )
//   {
//      const CXBRLiveLoadGraphDefinition& def = *iter;
//      std::set<IntervalIndexType>::const_iterator found = def.m_IntervalApplicability.find(intervalIdx);
//      if (found != def.m_IntervalApplicability.end())
//      {
//         return def.m_Name;
//      }
//   }
//
//   return _T("DC");
//}
//   
//std::vector< std::pair<std::_tstring,IDType> > CXBRLiveLoadGraphDefinitions::GetLoadings(IntervalIndexType intervalIdx, ActionType action) const
//{
//   std::vector< std::pair<std::_tstring,IDType> > lcNames;
//
//   ConstGraphDefinitionIterator iter(m_Definitions.begin());
//   ConstGraphDefinitionIterator iterEnd(m_Definitions.end());
//   for ( ; iter != iterEnd; iter++ )
//   {
//      const CAnalysisResultsGraphDefinition& def = *iter;
//
//      bool bApplicableAction = true;
//      switch(action)
//      {
//         case actionMoment:
//            bApplicableAction = def.m_ApplicableActions & ACTIONS_MOMENT_ONLY ? true : false;
//            break;
//
//         case actionShear:
//         case actionReaction:
//            bApplicableAction = def.m_ApplicableActions & ACTIONS_SHEAR_ONLY ? true : false;
//            break;
//
//         case actionDeflection:
//         case actionRotation:
//            bApplicableAction = def.m_ApplicableActions & ACTIONS_DEFLECTION_ONLY ? true : false;
//            break;
//
//         case actionStress:
//            bApplicableAction = def.m_ApplicableActions & ACTIONS_STRESS_ONLY ? true : false;
//            break;
//
//         default:
//            ATLASSERT(false);
//            break;
//      }
//
//      if ( intervalIdx == INVALID_INDEX && bApplicableAction )
//      {
//         lcNames.push_back( std::make_pair( def.m_Name, def.m_ID ) );
//      }
//      else
//      {
//         std::set<IntervalIndexType>::const_iterator found = def.m_IntervalApplicability.find(intervalIdx);
//         if (found != def.m_IntervalApplicability.end() && bApplicableAction)
//         {
//            lcNames.push_back( std::make_pair( def.m_Name, def.m_ID ) );
//         }
//      }
//   }
//   return lcNames;
//}

void CXBRLiveLoadGraphDefinitions::Clear()
{
   m_Definitions.clear();
}
