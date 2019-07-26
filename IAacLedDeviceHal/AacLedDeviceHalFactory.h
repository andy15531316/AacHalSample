#ifndef __AAC_LED_DEVICE_FACTORY_H__
#define __AAC_LED_DEVICE_FACTORY_H__

#pragma once
#include <ObjBase.h>

extern ULONG g_ServerLocks;     // server locks 

class AacLedDeviceHalFactory : public IClassFactory
{
protected:
   // Reference count
   long m_lRef;

public:

	AacLedDeviceHalFactory(void);
	~AacLedDeviceHalFactory(void);

	//IUnknown
	;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	/*IClassFactory implementation:*/
	virtual HRESULT STDMETHODCALLTYPE CreateInstance(LPUNKNOWN pUnk,  const IID& id, void** ppv); //creates COM instance
	virtual HRESULT STDMETHODCALLTYPE LockServer (BOOL fLock); //provides server locking
	
};

#endif // __AAC_LED_DEVICE_FACTORY_H__