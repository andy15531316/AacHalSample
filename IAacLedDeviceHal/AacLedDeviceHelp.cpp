#include "AacLedDeviceHelp.h"

extern  ULONG g_Components;    // number of components;

IAacLedDeviceHalImpHelp::IAacLedDeviceHalImpHelp(void)
	: m_lRef(1)
{
	InterlockedIncrement((LONG*)&g_Components);
}


IAacLedDeviceHalImpHelp::~IAacLedDeviceHalImpHelp(void)
{
	InterlockedDecrement((LONG*)&g_Components);
	for (size_t i = 0; i < m_aacLedDevice.size(); ++i)
	{
		delete m_aacLedDevice[i];
		m_aacLedDevice[i] = NULL;
	}
}


HRESULT STDMETHODCALLTYPE IAacLedDeviceHalImpHelp::QueryInterface(REFIID riid, void  **ppv) {
	HRESULT rc = S_OK;
	*ppv = NULL;

	if (riid == IID_IUnknown || riid == IID_IAacLedDeviceHal)
		*ppv = (IAacLedDeviceHal*)this;
	else
		rc = E_NOINTERFACE;

	if (rc == S_OK)
		this->AddRef();

	return rc;
}

ULONG   STDMETHODCALLTYPE IAacLedDeviceHalImpHelp::AddRef(void)
{
	InterlockedIncrement(&m_lRef);
	return this->m_lRef;
}

ULONG   STDMETHODCALLTYPE IAacLedDeviceHalImpHelp::Release(void)
{
	InterlockedDecrement(&m_lRef);

	if (m_lRef == 0) {

		this->Release();

		delete this;
		return 0;
	}
	else
		return this->m_lRef;
}

HRESULT STDMETHODCALLTYPE IAacLedDeviceHalImpHelp::Enumerate(IAacLedDevice **iDevices, ULONG *count)
{
	HRESULT hr = S_OK;
	ULONG numberOfBuffer = *count;

	*count = m_aacLedDevice.size();

	if (iDevices == NULL)
	{
		return S_OK;
	}
	else if (numberOfBuffer > 0 && numberOfBuffer < m_aacLedDevice.size())
	{
		hr = E_BOUNDS;
	}
	else
	{
		for (size_t i = 0; i < m_aacLedDevice.size(); ++i)
		{
			iDevices[i] = m_aacLedDevice[i];
			iDevices[i]->AddRef();
		}
	}

	return S_OK;
}


HRESULT IAacLedDeviceHalImpHelp::Init()
{
	
	HRESULT hr = CreateLedDevice(m_aacLedDevice);

	return hr;
}




// IAacLedDeviceImpHelp

IAacLedDeviceImpHelp::IAacLedDeviceImpHelp(void)
	:m_lRef(1)
{

}


IAacLedDeviceImpHelp::~IAacLedDeviceImpHelp(void)
{

}


//IUnknown
HRESULT STDMETHODCALLTYPE IAacLedDeviceImpHelp::QueryInterface(REFIID riid, void  **ppv)
{
	HRESULT rc = S_OK;
	*ppv = NULL;

	if (riid == IID_IUnknown || riid == IID_IAacLedDevice)
		*ppv = (IAacLedDeviceHal*)this;
	else
		rc = E_NOINTERFACE;

	if (rc == S_OK)
		this->AddRef();

	return rc;
}


ULONG   STDMETHODCALLTYPE IAacLedDeviceImpHelp::AddRef(void)
{
	InterlockedIncrement(&m_lRef);
	return this->m_lRef;
}


ULONG   STDMETHODCALLTYPE IAacLedDeviceImpHelp::Release(void)
{
	InterlockedDecrement(&m_lRef);

	if (m_lRef == 0) {

		this->Release();

		delete this;
		return 0;
	}
	else
		return this->m_lRef;
}
