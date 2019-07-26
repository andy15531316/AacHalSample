#include <windows.h>
#include <string>
#include <MyLedDevice_i.h>
#include <vector>
#include <tinyxml.h>
#include <map>



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

struct LedDeviceLayout
{
	DWORD					  ledCount;
	DWORD					  width;
	DWORD					  height;
	std::vector<std::string>  ledNames;
};

struct LedEffect
{
	std::string name;
	DWORD       id;
	int         synchronized;
	int         direction_supported;
	int         speed_supported;
	size_t      default_speed;
	size_t      default_direction;
	size_t      max_speed;
	size_t      max_direction;
};


struct LedCapability
{
	std::string	 version;
	DWORD        type;

	std::string     name;
	DWORD           id;
	LedDeviceLayout layout;
	std::vector<LedEffect> effects;

};

template <class T, class U>
bool GetElementInt(TiXmlElement* parent, const char* tag, T& value, U defaultValue)
{
	TiXmlElement* node = parent->FirstChildElement(tag);
	value = defaultValue;
	if (node != nullptr)
	{
		std::string s = node->GetText();
		value = stoi(s);
		return true;
	}

	return false;
}

void QueryDeviceCapability(TiXmlElement* root, LedCapability& capability)
{
	//LedDeviceInfo info;

	//name:
	TiXmlElement* element = root->FirstChildElement("name");
	if (element != NULL)
		capability.name = element->GetText();

	//id:
	TiXmlElement* element_id = root->FirstChildElement("id");
	std::string     id;
	if (element_id != NULL)
	{
		id = element_id->GetText();
		capability.id = stoi(id);
	}
	

	//layout:
	TiXmlElement* element_layout = root->FirstChildElement("layout");

	if (element_layout != NULL)
	{
		//led_count
		TiXmlElement* led_count = element_layout->FirstChildElement("led_count");
		std::string     count;
		if (led_count != NULL)
		{
			count = led_count->GetText();
			capability.layout.ledCount = stoi(count);
		}

		//size
		TiXmlElement* led_size = element_layout->FirstChildElement("size");
		if (led_size != NULL)
		{
			//width:
			TiXmlElement* width = led_size->FirstChildElement("width");
			std::string     s_width;
			if (width != NULL)
			{
				s_width = width->GetText();
				capability.layout.width = stoi(s_width);
			}
			//height:
			TiXmlElement* height = led_size->FirstChildElement("height");
			std::string     s_height;
			if (width != NULL)
			{
				s_height = height->GetText();
				capability.layout.height = stoi(s_height);
			}
		}

		//led_name
		TiXmlElement* led_name = element_layout->FirstChildElement("led_name");
		if (led_name != NULL)
		{
			//led:
			TiXmlElement* led = led_name->FirstChildElement("led");
			while (led != NULL)
			{
				std::string     s_led;
				s_led = led->GetText();
				capability.layout.ledNames.push_back(s_led);

				led = led->NextSiblingElement("led");
			}
		}
	}
	

	//supporeted_effect:
	TiXmlElement* supporeted_effect = root->FirstChildElement("supported_effect");

	if (supporeted_effect != NULL){

		//effect:
		TiXmlElement* effect = supporeted_effect->FirstChildElement("effect");
		while (effect != NULL)
		{
			LedEffect eff;

			// name:
			TiXmlElement* eff_name = effect->FirstChildElement("name");
			std::string     s_eff_name;
			if (eff_name != NULL)
			{
				s_eff_name = eff_name->GetText();
				eff.name = s_eff_name;
			}

			// id:
			TiXmlElement* eff_id = effect->FirstChildElement("id");
			std::string     s_eff_id;
			if (eff_id != NULL)
			{
				s_eff_id = eff_id->GetText();
				eff.id = stoi(s_eff_id);
			}

			// can sync:
			TiXmlElement* eff_sync = effect->FirstChildElement("synchronizable");
			std::string     s_eff_sync;
			if (eff_sync != NULL)
			{
				s_eff_sync = eff_sync->GetText();
				eff.synchronized = stoi(s_eff_sync);
			}

			// speed

			if (GetElementInt(effect, "speed_supported", eff.speed_supported, 0))
			{
				GetElementInt(effect, "levels", eff.max_speed, 0);
				GetElementInt(effect, "default", eff.default_speed, 0);
			}

			// direction
			if (GetElementInt(effect, "direction_supported", eff.speed_supported, 0))
			{
				GetElementInt(effect, "levels", eff.max_speed, 0);
				GetElementInt(effect, "default", eff.default_speed, 0);
			}
			capability.effects.push_back(eff);
			
			effect = effect->NextSiblingElement("effect");
		}



	}

}

bool ParseCapability(const BSTR xmlData, LedCapability& capability)
{
	//TiXmlDocument* xml= new TiXmlDocument;

	TiXmlDocument* xml = new TiXmlDocument;
	xml->Parse(Wcs2Str(xmlData).c_str(), 0, TIXML_ENCODING_UTF8);

	TiXmlElement* root = xml->RootElement();

	if (root == NULL)
		return false;

	TiXmlElement* element;

	// version
	element = root->FirstChildElement("version");
	if (element != NULL)
		capability.version = element->GetText();
	else
		capability.version = "";

	// type
	element = root->FirstChildElement("type");
	if (element != NULL)
		capability.type = atoi( element->GetText() );
	else
		capability.type = 0;

	// device
	element = root->FirstChildElement("device");
	if (element != NULL)
		QueryDeviceCapability(element, capability);



	return true;
}


bool CheckDeviceIdRegistered(const wchar_t* guid)
{
	HKEY hKey;
	WCHAR szKeyBuf[1024];

	std::wstring path = L"CLSID\\{109DC3E4-B9FF-4AF3-9008-AB13705D4E5F}\\Instance\\{E9BBD754-6CF4-492E-BA89-782177A2771B}\\Instance\\";
	path += guid;
	wcscpy_s(szKeyBuf, path.c_str());

	long lResult = RegOpenKey(HKEY_CLASSES_ROOT,
		szKeyBuf,
		&hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return false;
	}

	return true;
}



// test software: 
void doSoftware(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s mode. Press any key to continue. \n", effect.name.c_str());
	getchar();

	BYTE x = 255;
	for (int k = 0; k < 120; k++)
	{
		x = 255;
		size_t count = cap.layout.ledCount;
		COLORREF* colors = new COLORREF[count];
		for (int j = 0; j < count; ++j)
		{
			if (j == (k % count))
				colors[j] = RGB(x, 0, 0);
			else
				colors[j] = RGB(0, 0, 0);
		}

		(*device)->SetEffect(0, colors, count);
		delete[] colors;
		Sleep(10);
	}
}




// test static: 
void doStatic(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s mode. Press any key to continue. \n", effect.name.c_str());
	getchar();


	for (int k = 0; k < 3; k++)
	{
		BYTE x = 255;
		size_t count = cap.layout.ledCount;
		COLORREF* colors = new COLORREF[count];
		for (int j = 0; j < count; ++j)
		{
			if (k == 0)
				colors[j] = RGB(x, 0, 0);
			else if (k == 1)
				colors[j] = RGB(0, x, 0);
			else
				colors[j] = RGB(0, 0, x);

		}

		(*device)->SetEffect(1, colors, count);
		delete[] colors;
		Sleep(1000);
	}
}




// test breathing: 
void doBreathing(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s mode. Press any key to continue. \n", effect.name.c_str());
	getchar();

	BYTE x = 255;
	size_t count = cap.layout.ledCount;
	COLORREF* colors = new COLORREF[count];
	colors = new COLORREF[count];
	for (int j = 0; j < count; ++j)
		colors[j] = RGB(0, 0, x);

	(*device)->SetEffect(2, colors, count);
	delete[] colors;

	Sleep(3000);
}




// test strobing: 
void doStrobing(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s. Press any key to continue. \n", effect.name.c_str());
	getchar();

	BYTE x = 255;
	size_t count = cap.layout.ledCount;
	COLORREF* colors = new COLORREF[count];
	for (int j = 0; j < count; ++j)
		colors[j] = RGB(0, x, 0);

	(*device)->SetEffect(3, colors, count);
	delete[] colors;

	Sleep(3000);
}





// test colorcyle: 
void doColorcycle(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s mode. Press any key to continue. \n", effect.name.c_str());
	getchar();

	size_t count = cap.layout.ledCount;
	COLORREF* colors = new COLORREF[count];
	(*device)->SetEffect(4, colors, count);
	delete[] colors;

	Sleep(3000);
}






// test rainbow:
void doRainbow(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s mode. Press any key to continue. \n", effect.name.c_str());
	getchar();

	ULONG colors[5];
	(*device)->SetEffect(5, colors, 5);

	Sleep(3000);
}






// test default: 
void doDefault(LedCapability cap, IAacLedDevice** device, LedEffect effect)
{
	printf("Test %s. Press any key to continue. \n", effect.name.c_str());
	getchar();

	size_t count = cap.layout.ledCount;
	COLORREF* colors = new COLORREF[count];

	(*device)->SetEffect(0xFF, colors, 5);	//1:static form capability report.
	delete[] colors;

	Sleep(3000);
}





void main()
{
	HRESULT hr;
	IUnknown *pUnknown;
	IAacLedDeviceHal* hal;

	OLECHAR* guidString;
	StringFromCLSID(CLSID_MYLEDDEVICE, &guidString);
	std::wstring key = guidString;

	CheckDeviceIdRegistered(key.c_str());

	CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_MYLEDDEVICE, NULL, CLSCTX_INPROC_SERVER,
		IID_IAacLedDeviceHal, (void **)&hal); 
	if (!SUCCEEDED(hr))
		return;

	hal->AddRef();
	ULONG count = 0;

	hal->Enumerate(NULL, &count);
	if (count <= 0)
		return;

	IAacLedDevice** devices = new IAacLedDevice*[count];
	hal->Enumerate(devices, &count);

	std::vector<LedCapability> caps;

	// Enumerate each device and read its capablility.
	for (size_t i = 0; i < count; ++i)
	{
		BSTR capData = NULL;
		devices[i]->GetCapability(&capData);

		LedCapability cap;
		ParseCapability(capData, cap);
		caps.push_back(cap);

		::SysFreeString(capData);
	}

	
	std::vector<IAacLedDevice**>devices_vector;
	for (int c = 0; c < count; c++){
		devices_vector.push_back(devices);
		devices++;
	}

	typedef void (*func)(LedCapability cap, IAacLedDevice** device, LedEffect effect);
	std::map<DWORD, func> getFunction = {
		{ 0, &doSoftware },
		{ 1, &doStatic },
		{ 2, &doBreathing },
		{ 3, &doStrobing },
		{ 4, &doColorcycle },
		{ 5, &doRainbow },
		{ 0xFF, &doDefault }
	};



	for (int i = 0; i < devices_vector.size(); ++i)
		for each(LedEffect effect  in caps[i].effects)
		{
			(*getFunction[effect.id])(caps[i], devices_vector[i], effect);
		}
}
