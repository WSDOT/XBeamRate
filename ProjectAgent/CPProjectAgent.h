
//////////////////////////////////////////////////////////////////////////////
// CProxyIProjectEventSink
template <class T>
class CProxyIProjectEventSink : public IConnectionPointImpl<T, &IID_IProjectEventSink, CComDynamicUnkArray>
{
public:

//IProjectEventSink : IUnknown
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
				IProjectEventSink* pEventSink = reinterpret_cast<IProjectEventSink*>(*pp);
				ret = pEventSink->OnProjectChanged();
			}
			pp++;
		}
		pT->Unlock();
		return ret;
	}
};
