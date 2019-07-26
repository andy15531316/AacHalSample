#ifndef  __IAAC_LED_DEVICE_HAL_H__
#define  __IAAC_LED_DEVICE_HAL_H__

#include <Windows.h>

EXTERN_C const IID IID_IAacLedDeviceHal;
EXTERN_C const IID IID_IAacLedDevice;

class IAacLedDevice;

class IAacLedDeviceHal: public IUnknown
{
public:

	virtual HRESULT STDMETHODCALLTYPE Enumerate(
		/* [in] */ IAacLedDevice **iDevices,
		ULONG *count) = 0;
};



// IAacLedDevice
class IAacLedDevice: public IUnknown
{
public:

	virtual HRESULT STDMETHODCALLTYPE GetCapability(
		/* [out] */ BSTR *capability) = 0;

	virtual  HRESULT STDMETHODCALLTYPE SetEffect(
		/* [in] */ ULONG effectId,
		/* [in] */ ULONG *colors,
		/* [in] */ ULONG numberOfColors) = 0;

	virtual  HRESULT STDMETHODCALLTYPE SetFrame(
		/* [in] */ ULONG frame) = 0;

};




#endif //__IAAC_LED_DEVICE_HAL_H__