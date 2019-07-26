#ifndef __MY_AAC_LED_DEVICE_IMP_H__
#define __MY_AAC_LED_DEVICE_IMP_H__

#include "AacLedDeviceHelp.h"
#include "MyLedDevice_i.h"

typedef void* DeviceLightControl;

class MyAacLedDevice;

class MyAacLedDeviceHal: public IAacLedDeviceHalImpHelp
{
public:

	virtual ~MyAacLedDeviceHal(void);


protected:

	HRESULT CreateLedDevice(std::vector<IAacLedDevice*>& devices);

};

// To describe the capability of the device.
struct DeviceEffect
{
	DeviceEffect();
	DeviceEffect(std::wstring effectName, size_t effectId, bool customizedColorSupported, bool synchronizationSupported, bool speedSupported, bool directionSupported, size_t maxSpeedLevels, size_t defaultSpeedLevel, size_t maxDirectionLevels, size_t defaultDirectionLevel);

	
	std::wstring EffectName;
	size_t EffectId;
	size_t TargetId;

	size_t MaxSpeedLevels;
	size_t DefaultSpeedLevel;

	size_t MaxDirectionLevels;
	size_t DefaultDirectionLevel;

	bool SynchronizationSupported;
	bool CustomizedColorSupported;
	bool SpeedSupported;
	bool DirectionSupported;
};



// MyAacLedDevice
class MyAacLedDevice: public IAacLedDeviceImpHelp
{
public:

	MyAacLedDevice(void);
	virtual ~MyAacLedDevice(void);

	//IAacLedDevice
	virtual HRESULT STDMETHODCALLTYPE GetCapability( BSTR *capability);

	virtual  HRESULT STDMETHODCALLTYPE SetEffect(ULONG effectId, ULONG *colors, ULONG numberOfColors);

	virtual  HRESULT STDMETHODCALLTYPE SetEffectOptSpeed(ULONG effectId, ULONG *colors, ULONG numberOfColors, ULONG speed, ULONG direction);

	virtual  HRESULT STDMETHODCALLTYPE Synchronize(ULONG effectId, ULONGLONG tickcount);

	

	HRESULT Init(DeviceLightControl* deviceControl, int index);

private:

	DeviceLightControl* m_deviceControl;
	int     m_index;


	HANDLE m_hHalMutex;

	void EnterMutex(void);
	void LeaveMutex(void);
	void InitMutex(void);

	HANDLE m_hSmbusMutex;

	void EnterSmbusMutex(void);
	void LeaveSmbusMutex(void);
	void InitSmbusMutex(void);

	DeviceEffect* GetEffectInfo(ULONG effectId);

	std::wstring doGetDeviceId();
	size_t       doGetLedCount();
	std::wstring doGetLedLocation(int led);

	HRESULT DoSetEffectOptSpeed(ULONG effectId, ULONG *colors, ULONG numberOfColors, ULONG speed, ULONG direction);


};


#endif  __MY_AAC_LED_DEVICE_IMP_H__


