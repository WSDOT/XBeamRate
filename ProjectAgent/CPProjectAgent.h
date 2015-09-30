///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2015  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

#define EVT_PROJECTPROPERTIES    0x0001
#define EVT_PROJECT              0x0002
#define EVT_RATINGSPECIFICATION  0x0004

//////////////////////////////////////////////////////////////////////////////
// CProxyIXBRProjectPropertiesEventSink
template <class T>
class CProxyIXBRProjectPropertiesEventSink : public IConnectionPointImpl<T, &IID_IXBRProjectPropertiesEventSink, CComDynamicUnkArray>
{
public:

//IXBRProjectPropertiesEventSink : IUnknown
public:
	HRESULT Fire_OnProjectPropertiesChanged()
	{
		T* pT = (T*)this;

      if ( 0 < pT->m_EventHoldCount )
      {
         sysFlags<Uint32>::Set(&pT->m_PendingEvents,EVT_PROJECTPROPERTIES);
         return S_OK;
      }

      pT->Lock();
		HRESULT ret = S_OK;
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				IXBRProjectPropertiesEventSink* pEventSink = reinterpret_cast<IXBRProjectPropertiesEventSink*>(*pp);
				ret = pEventSink->OnProjectPropertiesChanged();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}
};

//////////////////////////////////////////////////////////////////////////////
// CProxyIXBRProjectEventSink
template <class T>
class CProxyIXBRProjectEventSink : public IConnectionPointImpl<T, &IID_IXBRProjectEventSink, CComDynamicUnkArray>
{
public:

//IXBRProjectEventSink : IUnknown
public:
	HRESULT Fire_OnProjectChanged()
	{
		T* pT = (T*)this;

      if ( 0 < pT->m_EventHoldCount )
      {
         sysFlags<Uint32>::Set(&pT->m_PendingEvents,EVT_PROJECT);
         return S_OK;
      }

      pT->Lock();
		HRESULT ret = S_OK;
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				IXBRProjectEventSink* pEventSink = reinterpret_cast<IXBRProjectEventSink*>(*pp);
				ret = pEventSink->OnProjectChanged();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}
};

//////////////////////////////////////////////////////////////////////////////
// CProxyIXBREventsEventSink
template <class T>
class CProxyIXBREventsEventSink : public IConnectionPointImpl<T, &IID_IXBREventsSink, CComDynamicUnkArray>
{
public:

//IXBREventsSink : IUnknown
public:
	HRESULT Fire_OnHoldEvents()
	{
		T* pT = (T*)this;

      pT->Lock();
		HRESULT ret = S_OK;
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				IXBREventsSink* pEventSink = reinterpret_cast<IXBREventsSink*>(*pp);
				ret = pEventSink->OnHoldEvents();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}

	HRESULT Fire_OnFirePendingEvents()
	{
		T* pT = (T*)this;

      pT->Lock();
		HRESULT ret = S_OK;
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				IXBREventsSink* pEventSink = reinterpret_cast<IXBREventsSink*>(*pp);
				ret = pEventSink->OnFirePendingEvents();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}

	HRESULT Fire_OnCancelPendingEvents()
	{
		T* pT = (T*)this;

      pT->Lock();
		HRESULT ret = S_OK;
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				IXBREventsSink* pEventSink = reinterpret_cast<IXBREventsSink*>(*pp);
				ret = pEventSink->OnCancelPendingEvents();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}
};


//////////////////////////////////////////////////////////////////////////////
// CProxyIXBRRatingSpecificationEventSink
template <class T>
class CProxyIXBRRatingSpecificationEventSink : public IConnectionPointImpl<T, &IID_IXBRRatingSpecificationEventSink, CComDynamicUnkArray>
{
public:

//IXBRRatingSpecificationEventSink : IUnknown
public:
	HRESULT Fire_OnRatingSpecificationChanged()
	{
		T* pT = (T*)this;

      if ( 0 < pT->m_EventHoldCount )
      {
         sysFlags<Uint32>::Set(&pT->m_PendingEvents,EVT_RATINGSPECIFICATION);
         return S_OK;
      }

      pT->Lock();
		HRESULT ret = S_OK;
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				IXBRRatingSpecificationEventSink* pEventSink = reinterpret_cast<IXBRRatingSpecificationEventSink*>(*pp);
				ret = pEventSink->OnRatingSpecificationChanged();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}
};
