#include "MyAacLedDeviceHalImp.h"
#include <map>
#include <string>
#include "../xml/GsXml.h"
#include "config.h"

extern  ULONG g_Components;    // number of components;



#define MY_DEVICE_NAME  L"MyComponentName";
#define MY_DEVICE_COUNT   1 
#define MY_LED_COUNT      5

wchar_t* g_MyLedLoaction[MY_LED_COUNT] = {
	L"Loaction_1",
	L"Loaction_2",
	L"Loaction_3",
	L"Loaction_4",
	L"Loaction_5",
};



// ==== CAP ====

std::vector<DeviceEffect> g_deviceEffectInfo = {

	{ DeviceEffect(EFEFCT_0_NAME, EFEFCT_0_ID, true, false, false, false, 0, 0, 0, 0) },
	{ DeviceEffect(EFEFCT_1_NAME, EFEFCT_1_ID, true, false, false, false, 0, 0, 0, 0) },
	{ DeviceEffect(EFEFCT_FF_NAME, EFEFCT_FF_ID, false, false, false, false, 0, 0, 0, 0)}
	//{ DeviceEffect(EFEFCT_USER, L"User Effect 1", false, false, false, false, 0, 0, 0, 0) },
	//{ DeviceEffect(EFEFCT_USER + 1, L"User Effect 2", false, false, false, false, 0, 0, 0, 0) }
};



//------------------------------------------------------------------------------------
std::string Wcs2Str(const wchar_t* wcs)
{
	int wlen = wcslen(wcs);
	int len = WideCharToMultiByte(CP_ACP, 0, wcs, wlen, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	str[len] = 0;
	WideCharToMultiByte(CP_ACP, 0, wcs, wlen, str, len + 1, NULL, NULL);
	std::string ret = str;
	delete[] str;
	return ret;
}


std::wstring Str2Wcs(const char* str)
{
	int len = strlen(str);
	int wlen = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
	wchar_t* wcs = new wchar_t[wlen + 1];
	wcs[wlen] = 0;
	MultiByteToWideChar(CP_ACP, 0, str, len, wcs, wlen + 1);
	std::wstring ret = wcs;
	delete[] wcs;
	return ret;
}

//-----------------------------------------------------------------------------

// MyAacLedDeviceHal
MyAacLedDeviceHal::~MyAacLedDeviceHal(void) 
{
}



HRESULT MyAacLedDeviceHal::CreateLedDevice(std::vector<IAacLedDevice*>& devices)
{
	DeviceLightControl* DeviceLightCtrl = new DeviceLightControl;

	devices.clear();

	for (size_t i = 0; i < MY_DEVICE_COUNT; ++i)
	{
		
		MyAacLedDevice* device = new MyAacLedDevice();
		device->Init(DeviceLightCtrl, i);
		devices.push_back(device);
	}

	return S_OK;
}



//MyAacLedDevice

MyAacLedDevice::MyAacLedDevice(void)
{
	InitMutex();

}


MyAacLedDevice::~MyAacLedDevice(void)
{

}




HRESULT STDMETHODCALLTYPE MyAacLedDevice::GetCapability(BSTR *capability)
{
	EnterMutex();





	std::wstring Capability_profile;

	gs::GsConfigFile XMLprofile;
	XMLprofile.Create();

	// root:
	gs::GsElementNode root = XMLprofile.Root();

	// version
	gs::GsElementNode element;
	element = root->AddElement("version");
	element->SetInt(1);

	// Type
	element = root->AddElement("type");
	element->SetInt(aura::DRAM_RGB_LIGHTING);

	// device :
	gs::GsElementNode device_node;
	std::wstring id = doGetDeviceId();
	device_node = root->AddElement(L"device");
	device_node->AddElementValue(L"name", id.c_str());

	//Index:
	gs::GsElementNode indexNode = device_node->AddElement(L"id");
	indexNode->SetInt(m_index);

	// layout
	gs::GsElementNode layoutNode;
	layoutNode = device_node->AddElement(L"layout");

	// ledcount :
	int led_count = doGetLedCount();
	gs::GsElementNode ledCountNode = layoutNode->AddElement(L"led_count");
	ledCountNode->SetInt(led_count);

	// size :
	gs::GsElementNode sizeNode;
	sizeNode = layoutNode->AddElement(L"size");
	gs::GsElementNode widthNode = sizeNode->AddElement(L"width");
	widthNode->SetInt(led_count);
	gs::GsElementNode heightNode = sizeNode->AddElement(L"height");
	heightNode->SetInt(1);

	// led_name :
	gs::GsElementNode ledListNode;
	ledListNode = layoutNode->AddElement(L"led_name");

	// ledLocationNode :
	for (int led = 0; led < led_count; led++) {

		gs::GsElementNode ledLocationNode;
		std::wstring location_str = doGetLedLocation(led);

		ledLocationNode = ledListNode->AddElementValue(L"led", location_str.c_str());
	}

	// <DeviceModeList>, under device node:
	std::vector<DeviceEffect> effect_list = g_deviceEffectInfo;
	gs::GsElementNode  deviceModeListNode;
	deviceModeListNode = device_node->AddElement(L"supported_effect");

	for (size_t i = 0; i < effect_list.size(); ++i)
	{
		gs::GsElementNode deviceModeNode = deviceModeListNode->AddElementWithoutKey(L"effect");

		gs::GsElementNode effectNode_Name;
		effectNode_Name = deviceModeNode->AddElementValue(L"name", effect_list[i].EffectName.c_str());

		gs::GsElementNode effectNode_Index;
		effectNode_Index = deviceModeNode->AddElementValue(L"id", std::to_wstring(effect_list[i].EffectId).c_str());

		gs::GsElementNode effectNode_Support_set_frame;
		effectNode_Index = deviceModeNode->AddElementValue(L"synchronizable", std::to_wstring(effect_list[i].SynchronizationSupported).c_str());

		gs::GsElementNode node;

		// customized color?
		node = deviceModeNode->AddElementValue(L"customized_color", std::to_wstring(effect_list[i].CustomizedColorSupported).c_str());

		// support speed ?
		node = deviceModeNode->AddElementValue(L"speed_supported", std::to_wstring(effect_list[i].DirectionSupported).c_str());
		if (effect_list[i].SpeedSupported)
		{
			gs::GsElementNode value;
			value = node->AddElement("levels");
			value->SetInt(effect_list[i].MaxSpeedLevels);
			value = node->AddElement("default");
			value->SetInt(effect_list[i].DefaultSpeedLevel);
		}

		// support direction?
		node = deviceModeNode->AddElementValue(L"direction_supported", std::to_wstring(effect_list[i].DirectionSupported).c_str());
		if (effect_list[i].DirectionSupported)
		{
			gs::GsElementNode value;
			value = node->AddElement("levels");
			value->SetInt(effect_list[i].MaxDirectionLevels);
			value = node->AddElement("default");
			value->SetInt(effect_list[i].DefaultDirectionLevel);
		}
	}

	// turn to string:
	XMLprofile.ToString(Capability_profile);
	if (Capability_profile.length() > 0)
	{
		// TODO: trun wstring profile to BSTR
		*capability = SysAllocString(Capability_profile.c_str());
	}

	LeaveMutex();
	return S_OK;
}


HRESULT STDMETHODCALLTYPE MyAacLedDevice::SetEffect(ULONG effectId, ULONG *colors, ULONG numberOfColors)
{
	HRESULT hr = S_OK;
	EnterMutex();

	DeviceEffect* effect = GetEffectInfo(effectId);
	if (effect != nullptr)
		hr = E_FAIL;

	ULONG speed = 0;
	ULONG direction = 0;
	speed = effect->DefaultSpeedLevel;
	direction = effect->DefaultDirectionLevel;
		
	return DoSetEffectOptSpeed(effectId, colors, numberOfColors, speed, direction);

	LeaveMutex();
	return hr;
}


HRESULT MyAacLedDevice::SetEffectOptSpeed(ULONG effectId, ULONG *colors, ULONG numberOfColors, ULONG speed, ULONG direction)
{
	DeviceEffect* effect = GetEffectInfo(effectId);
	if (effect == nullptr || (!effect->SpeedSupported && !effect->DirectionSupported) )
		return E_FAIL;

	if (effect->SpeedSupported && speed >= effect->MaxSpeedLevels)
		return E_FAIL;

	if (effect->DirectionSupported && speed >= effect->MaxDirectionLevels)
		return E_FAIL;

	////TODO

	return DoSetEffectOptSpeed(effectId, colors, numberOfColors, speed, direction);
}

HRESULT STDMETHODCALLTYPE MyAacLedDevice::Synchronize(ULONG effectId, ULONGLONG tickcount)
{
	////TODO

	return E_FAIL;
}

HRESULT MyAacLedDevice::Init(DeviceLightControl* deviceControl, int index)
{
	m_deviceControl = deviceControl;
	m_index = index;
	//CreateDeviceCapability();

	return S_OK;
}

void MyAacLedDevice::LeaveMutex(void)
{
	ReleaseMutex(m_hHalMutex);
}

void MyAacLedDevice::EnterMutex(void)
{
	DWORD result;
	// result = WaitForSingleObject(m_hMutex, 1000);
	result = WaitForSingleObject(m_hHalMutex, MUTEX_WAITTINGTIME);		

	if (result == WAIT_OBJECT_0)
	{
	}
	else
	{
	}
}

void MyAacLedDevice::InitMutex(void)
{
	SECURITY_DESCRIPTOR sid;

	::InitializeSecurityDescriptor(&sid, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&sid, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof SECURITY_ATTRIBUTES;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sid;

	m_hHalMutex = CreateMutex(
		&sa,              // security attributes
		FALSE,             // initially not owned
		MY_MUTEXT_NAME);             // SMBus Mutex name
}


void MyAacLedDevice::EnterSmbusMutex(void)
{
	DWORD result;
	result = WaitForSingleObject(m_hSmbusMutex, MUTEX_WAITTINGTIME);

	if (result == WAIT_OBJECT_0)
	{
	}
	else
	{
	}
}


void MyAacLedDevice::LeaveSmbusMutex(void)
{
	ReleaseMutex(m_hSmbusMutex);
}


void MyAacLedDevice::InitSmbusMutex(void)
{
	SECURITY_DESCRIPTOR sid;

	::InitializeSecurityDescriptor(&sid, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&sid, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof SECURITY_ATTRIBUTES;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sid;

	m_hSmbusMutex = CreateMutexW(
		&sa,              // security attributes
		FALSE,             // initially not owned
		L"Global\\Access_SMBUS.HTP.Method");             // SMBus Mutex name
}


DeviceEffect* MyAacLedDevice::GetEffectInfo(ULONG effectId)
{
	size_t i = 0;

	for (auto e : g_deviceEffectInfo)
	{
		if (e.EffectId == effectId)
			return &g_deviceEffectInfo[i];
	}

	return nullptr;
}


std::wstring MyAacLedDevice::doGetDeviceId()
{
	////TODO
	return MY_DEVICE_NAME;
}


size_t   MyAacLedDevice::doGetLedCount()
{
	////TODO
	return MY_LED_COUNT;
}


std::wstring MyAacLedDevice::doGetLedLocation(int led)
{
	////TODO
	return g_MyLedLoaction[led];
}


HRESULT MyAacLedDevice::DoSetEffectOptSpeed(ULONG effectId, ULONG *colors, ULONG numberOfColors, ULONG speed, ULONG direction)
{
	////TODO

	return S_OK;
}

DeviceEffect::DeviceEffect() {

}

DeviceEffect::DeviceEffect(std::wstring effectName, size_t effectId, bool customizedColorSupported, bool synchronizationSupported, bool speedSupported, bool directionSupported, size_t maxSpeedLevels, size_t defaultSpeedLevel, size_t maxDirectionLevels, size_t defaultDirectionLevel)
{

	//m_isEcControlMode = isEcControlMode;
	SynchronizationSupported = synchronizationSupported;
	EffectName = effectName;
	EffectId = effectId;
	CustomizedColorSupported = customizedColorSupported;
	SpeedSupported = speedSupported;
	DirectionSupported = directionSupported;
	MaxSpeedLevels = maxSpeedLevels;
	DefaultSpeedLevel = defaultSpeedLevel;
	MaxDirectionLevels = maxDirectionLevels;
	DefaultDirectionLevel = defaultDirectionLevel;
}
