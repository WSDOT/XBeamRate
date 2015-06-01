
#include "stdafx.h"
#include "GraphDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGraphDefinition::CGraphDefinition()
{
}
//
//CGraphDefinition::CGraphDefinition(
//IDType id,
//const std::_tstring& name,
//pgsTypes::LimitState ls,
//const std::vector<IntervalIndexType>& intervals,
//int actions
//): m_ID(id),m_Name(name)
//{
//   m_GraphType = graphLimitState;
//   m_LoadType.LimitStateType = ls;
//   m_IntervalApplicability.insert(intervals.begin(),intervals.end());
//
//   m_ApplicableActions = actions;
//}
//
// constructor for combinations
CGraphDefinition::CGraphDefinition(IDType id,const std::_tstring& name,XBRCombinedForceType comb) :
m_ID(id), m_Name(name)
{
   m_GraphType = graphCombined;
   m_LoadType.CombinedLoadType = comb;
}

// constructor for product loads
CGraphDefinition::CGraphDefinition(
IDType id,
const std::_tstring& name,
XBRProductForceType type
): m_ID(id),m_Name(name)
{
   m_GraphType = graphProduct;
   m_LoadType.ProductLoadType = type;
}
//
//// constructor for live loads
//CGraphDefinition::CGraphDefinition(
//IDType id,
//const std::_tstring& name,
//const std::vector<IntervalIndexType>& intervals,
//int actions
//): m_ID(id),m_Name(name)
//{
//   m_GraphType = graphLiveLoad;
//   m_IntervalApplicability.insert(intervals.begin(),intervals.end());
//
//   m_ApplicableActions = actions;
//}
//
//// constructor for demands
//CGraphDefinition::CGraphDefinition(
//IDType id,
//const std::_tstring& name,
//pgsTypes::LimitState lstype,
//GraphType grtype,
//const std::vector<IntervalIndexType>& intervals
//): m_ID(id),m_Name(name)
//{
//   m_GraphType = grtype;
//   m_LoadType.LimitStateType = lstype;
//   m_IntervalApplicability.insert(intervals.begin(),intervals.end());
//
//   m_ApplicableActions = ACTIONS_STRESS_ONLY;
//}
//
//// constructor for vehicular live loads
//CGraphDefinition::CGraphDefinition(
//IDType id,
//const std::_tstring& name,
//pgsTypes::LiveLoadType llType,
//VehicleIndexType vehicleIndex,
//const std::vector<IntervalIndexType>& intervals,
//int apaction
//): m_ID(id),m_Name(name)
//{
//   m_GraphType = graphVehicularLiveLoad;
//   m_LoadType.LiveLoadType = llType;
//   m_IntervalApplicability.insert(intervals.begin(),intervals.end());
//
//   m_ApplicableActions = apaction;
//   m_VehicleIndex = vehicleIndex;
//}
//
// constructor for ultimate forces
CGraphDefinition::CGraphDefinition(
IDType id,
const std::_tstring& name
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
CGraphDefinitions::CGraphDefinitions()
{
}

void CGraphDefinitions::AddGraphDefinition(const CGraphDefinition& def)
{
   m_Definitions.push_back(def);
}

IndexType CGraphDefinitions::GetGraphDefinitionCount() const
{
   return m_Definitions.size();
}

CGraphDefinition& CGraphDefinitions::GetGraphDefinition(IndexType graphIdx)
{
   return m_Definitions[graphIdx];
}

const CGraphDefinition& CGraphDefinitions::GetGraphDefinition(IndexType graphIdx) const
{
   return m_Definitions[graphIdx];
}

CGraphDefinition& CGraphDefinitions::FindGraphDefinition(IDType graphID)
{
   GraphDefinitionIterator iter(m_Definitions.begin());
   GraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++ )
   {
      CGraphDefinition& def(*iter);
      if ( def.m_ID == graphID )
      {
         return def;
      }
   }

   ATLASSERT(false); // should never get here
   return m_Definitions.back();
}

const CGraphDefinition& CGraphDefinitions::FindGraphDefinition(IDType graphID) const
{
   ConstGraphDefinitionIterator iter(m_Definitions.begin());
   ConstGraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++ )
   {
      const CGraphDefinition& def(*iter);
      if ( def.m_ID == graphID )
      {
         return def;
      }
   }

   ATLASSERT(false); // should never get here
   return m_Definitions.back();
}

IndexType CGraphDefinitions::GetGraphIndex(IDType graphID) const
{
   IndexType index = 0;
   ConstGraphDefinitionIterator iter(m_Definitions.begin());
   ConstGraphDefinitionIterator end(m_Definitions.end());
   for ( ; iter != end; iter++, index++ )
   {
      const CGraphDefinition& def = *iter;
      if ( def.m_ID == graphID )
      {
         return index;
      }
   }

   ATLASSERT(false); // graphID not found
   return INVALID_INDEX;
}

void CGraphDefinitions::RemoveGraphDefinition(IndexType graphIdx)
{
   m_Definitions.erase(m_Definitions.begin()+graphIdx);
}

void CGraphDefinitions::RemoveGraphDefinitionByID(IDType graphID)
{
   RemoveGraphDefinition(GetGraphIndex(graphID));
}

//std::_tstring CGraphDefinitions::GetDefaultLoadCase(IntervalIndexType intervalIdx) const
//{
//   // return the of the first graph definition that is applicable to the given stage
//   ConstGraphDefinitionIterator iter;
//   for ( iter = m_Definitions.begin(); iter != m_Definitions.end(); iter++ )
//   {
//      const CGraphDefinition& def = *iter;
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
//std::vector< std::pair<std::_tstring,IDType> > CGraphDefinitions::GetLoadings(IntervalIndexType intervalIdx, ActionType action) const
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

void CGraphDefinitions::Clear()
{
   m_Definitions.clear();
}
