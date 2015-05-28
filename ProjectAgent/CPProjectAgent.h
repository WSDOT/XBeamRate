
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
