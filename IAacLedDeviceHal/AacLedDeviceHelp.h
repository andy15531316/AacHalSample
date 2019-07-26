#ifndef __AAC_LED_DEVICE_HELP_H__
#define __AAC_LED_DEVICE_HELP_H__

#include "AacDef_i.h"
#include <vector>

class IAacLedDeviceHalImpHelp : public IAacLedDeviceHal
{
public:

	
	IAacLedDeviceHalImpHelp();
	virtual ~IAacLedDeviceHalImpHelp();

	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppv);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	//IAacLedDeviceHal
	virtual HRESULT STDMETHODCALLTYPE Enumerate(IAacLedDevice **iDevices, ULONG *count);

	HRESULT Init();

protected:

	virtual HRESULT CreateLedDevice(std::vector<IAacLedDevice*>& devices) = 0;
	
	volatile long m_lRef;

	std::vector<IAacLedDevice*> m_aacLedDevice;

};



// IAacLedDeviceImpHelp
class IAacLedDeviceImpHelp : public IAacLedDeviceOptSpeed
{
public:

	IAacLedDeviceImpHelp(void);
	~IAacLedDeviceImpHelp(void);

	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppv);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

protected:

	volatile long m_lRef;

};


#endif //__AAC_LED_DEVICE_HELP_H__