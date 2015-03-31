
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

#pragma Reminder("UPDATE: add event holding")
      //if ( 0 < pT->m_EventHoldCount )
      //{
      //   sysFlags<Uint32>::Set(&pT->m_PendingEvents,EVT_LOSSPARAMETERS);
      //   return S_OK;
      //}

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
