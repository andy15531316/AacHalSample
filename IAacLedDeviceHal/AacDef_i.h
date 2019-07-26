#ifndef __AAC_INTERFACE_DEF_I_H__
#define __AAC_INTERFACE_DEF_I_H__

#include <Windows.h>
#include "AacDevDef.h"

#define AAC_DECLARE_CLSID(MY_CLSID)  EXTERN_C const CLSID MY_CLSID;


 extern "C" {

	 EXTERN_C const IID IID_IAacLedDeviceHal;
	 EXTERN_C const IID IID_IAacLedDevice;
	 EXTERN_C const IID IID_IAacLedDeviceOpt;

	 class IAacLedDevice;

	 class IAacLedDeviceHal : public IUnknown
	 {
	 public:

		 virtual HRESULT STDMETHODCALLTYPE Enumerate(
			 /* [in] */ IAacLedDevice **iDevices,
			 ULONG *count) = 0;
	 };



	 // IAacLedDevice
	 class IAacLedDevice : public IUnknown
	 {
	 public:

		 virtual HRESULT STDMETHODCALLTYPE GetCapability(
			 /* [out] */ BSTR *capability) = 0;

		 virtual  HRESULT STDMETHODCALLTYPE SetEffect(
			 /* [in] */ ULONG effectId,
			 /* [in] */ ULONG *colors,
			 /* [in] */ ULONG numberOfColors) = 0;

		 virtual  HRESULT STDMETHODCALLTYPE Synchronize(
			 /* [in] */ ULONG effectId,
			 /* [in] */ ULONGLONG milliseconds) = 0;

	 };
 }


 // IAacLedDeviceOptSpeed
 class IAacLedDeviceOptSpeed : public IAacLedDevice
 {
 public:

	 virtual  HRESULT STDMETHODCALLTYPE SetEffectOptSpeed(
		 /* [in] */ ULONG effectId,
		 /* [in] */ ULONG *colors,
		 /* [in] */ ULONG numberOfColors,
		 /* [in] */ ULONG speed,
		 /* [in] */ ULONG direction) = 0;

 };


#endif //__AAC_INTERFACE_DEF_I_H__

