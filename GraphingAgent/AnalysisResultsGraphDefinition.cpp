
#include "stdafx.h"
#include "AnalysisResultsGraphDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition()
{
}

CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition(
IDType id,
LPCTSTR name,
pgsTypes::LimitState ls
): m_ID(id),m_Name(name)
{
   m_GraphType = graphLimitState;
   m_LoadType.LimitStateType = ls;
}

// constructor for combinations
CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,xbrTypes::CombinedForceType comb) :
m_ID(id), m_Name(name)
{
   m_GraphType = graphCombined;
   m_LoadType.CombinedLoadType = comb;
}

// constructor for product loads
CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition(
IDType id,
LPCTSTR name,
xbrTypes::ProductForceType type
): m_ID(id),m_Name(name)
{
   m_GraphType = graphProduct;
   m_LoadType.ProductLoadType = type;
}

// constructor for vehicular live loads
CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) :
m_ID(id),
m_Name(name)
{
   m_GraphType = graphVehicularLiveLoad;
   m_LoadType.LiveLoadType = ratingType;
   m_VehicleIndex = vehicleIdx;
}

// constructor for live loads
CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition(IDType id,LPCTSTR name,pgsTypes::LoadRatingType ratingType) :
m_ID(id),
m_Name(name)
{
   m_GraphType = graphLiveLoad;
   m_LoadType.LiveLoadType = ratingType;
}

// constructor for ultimate forces
CXBRAnalysisResultsGraphDefinition::CXBRAnalysisResultsGraphDefinition(
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
CXBRAnalysisResultsGraphDefinitions::CXBRAnalysisResultsGraphDefinitions()
{
}

void CXBRAnalysisResultsGraphDefinitions::AddGraphDefinition(const CXBRAnalysisResultsGraphDefinition& def)
{
   m_Definitions.push_back(def);
}

IndexType CXBRAnalysisResultsGraphDefinitions::GetGraphDefinitionCount() const
{
   return m_Definitions.size();
}

CXBRAnalysisResultsGraphDefinition& CXBRAnalysisResultsGraphDefinitions::GetGraphDefinition(IndexType graphIdx)
{
   return m_Definitions[graphIdx];
}

const CXBRAnalysisResultsGraphDefinition& CXBRAnalysisResultsGraphDefinitions::GetGraphDefinition(IndexType graphIdx) const
{
   return m_Definitions[graphIdx];
}

CXBRAnalysisResultsGraphDefinition& CXBRAnalysisResultsGraphDefinitions::FindGraphDefinition(IDType graphID)
{
   GraphDefinitionIterator iter(m_Definitions.begin());
   GraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++ )
   {
      CXBRAnalysisResultsGraphDefinition& def(*iter);
      if ( def.m_ID == graphID )
      {
         return def;
      }
   }

   ATLASSERT(false); // should never get here
   return m_Definitions.back();
}

const CXBRAnalysisResultsGraphDefinition& CXBRAnalysisResultsGraphDefinitions::FindGraphDefinition(IDType graphID) const
{
   ConstGraphDefinitionIterator iter(m_Definitions.begin());
   ConstGraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++ )
   {
      const CXBRAnalysisResultsGraphDefinition& def(*iter);
      if ( def.m_ID == graphID )
      {
         return def;
      }
   }

   ATLASSERT(false); // should never get here
   return m_Definitions.back();
}

IndexType CXBRAnalysisResultsGraphDefinitions::GetGraphIndex(IDType graphID) const
{
   IndexType index = 0;
   ConstGraphDefinitionIterator iter(m_Definitions.begin());
   ConstGraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++, index++ )
   {
      const CXBRAnalysisResultsGraphDefinition& def = *iter;
      if ( def.m_ID == graphID )
      {
         return index;
      }
   }

   ATLASSERT(false); // graphID not found
   return INVALID_INDEX;
}

void CXBRAnalysisResultsGraphDefinitions::RemoveGraphDefinition(IndexType graphIdx)
{
   m_Definitions.erase(m_Definitions.begin()+graphIdx);
}

void CXBRAnalysisResultsGraphDefinitions::RemoveGraphDefinitionByID(IDType graphID)
{
   RemoveGraphDefinition(GetGraphIndex(graphID));
}

//std::_tstring CXBRAnalysisResultsGraphDefinitions::GetDefaultLoadCase(IntervalIndexType intervalIdx) const
//{
//   // return the of the first graph definition that is applicable to the given stage
//   ConstGraphDefinitionIterator iter;
//   for ( iter = m_Definitions.begin(); iter != m_Definitions.end(); iter++ )
//   {
//      const CXBRAnalysisResultsGraphDefinition& def = *iter;
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
//std::vector< std::pair<std::_tstring,IDType> > CXBRAnalysisResultsGraphDefinitions::GetLoadings(IntervalIndexType intervalIdx, ActionType action) const
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

void CXBRAnalysisResultsGraphDefinitions::Clear()
{
   m_Definitions.clear();
}
