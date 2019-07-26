#include "StdAfx.h"
#include "AacLedDeviceHalFactory.h"
#include "MyAacLedDeviceHalImp.h"

extern HMODULE g_hModule;

AacLedDeviceHalFactory::AacLedDeviceHalFactory(void) 
	: m_lRef (1) 
{
	
}


AacLedDeviceHalFactory::~AacLedDeviceHalFactory(void) 
{

}


//IUnknown
HRESULT STDMETHODCALLTYPE AacLedDeviceHalFactory::QueryInterface(REFIID riid,void  **ppv) 
{
	HRESULT rc = S_OK;

	if (riid == IID_IUnknown) *ppv = (IUnknown*)this;
	else if (riid == IID_IClassFactory) *ppv = (IClassFactory*)this;
	else rc = E_NOINTERFACE;

	if (rc == S_OK)
		this->AddRef();

	return rc;
}

ULONG   STDMETHODCALLTYPE AacLedDeviceHalFactory::AddRef(void) 
{
	InterlockedIncrement(&m_lRef);

	return this->m_lRef;
}

ULONG   STDMETHODCALLTYPE AacLedDeviceHalFactory::Release(void)
{
	InterlockedDecrement(&m_lRef);

	if ( this->m_lRef == 0) {
		//when there are not references to the object, we can delete it
		delete this;
		return 0;
	}
	else
		return this->m_lRef;
}

HRESULT STDMETHODCALLTYPE AacLedDeviceHalFactory::CreateInstance(LPUNKNOWN pUnk, const IID& id, void** ppv) 
{
	HRESULT rc = E_UNEXPECTED;

	if (pUnk != NULL) rc = CLASS_E_NOAGGREGATION;
	else if (id == IID_IAacLedDeviceHal || id == IID_IUnknown)
	{
		MyAacLedDeviceHal* pA = new MyAacLedDeviceHal;
		if (pA  == NULL)
			rc = E_OUTOFMEMORY;
		else		
			rc = pA->Init();

		if (FAILED(rc)) {
			// initialization failed, delete component
			pA->Release();
			return rc;
		}
		rc = pA->QueryInterface(id,ppv);
		pA->Release();
		return rc;
	}
	return rc;
}

HRESULT STDMETHODCALLTYPE AacLedDeviceHalFactory::LockServer (BOOL fLock) 
{
	if (fLock)  
		InterlockedIncrement ((LONG*)&(g_ServerLocks));

	else    
		InterlockedDecrement ((LONG*)&(g_ServerLocks));

	return S_OK;
}