#pragma once
#include <ObjBase.h>

extern HMODULE g_hModule;

// {347CC716-94FA-412C-8B04-AAF0116CC8F0

/*GUID CLSID_MYLEDDEVICE = { 0xC532D261, 0x993A, 0x444C, {0xB4, 0x01, 0xB9, 0x85, 0xF3, 0x43, 0xA7, 0xFF} 

};*/


/*const WCHAR FName[]  = L"MyLedDevice";
const WCHAR VerInd[] = L"MyLedDevice.Component";
const WCHAR ProgId[] = L"MyLedDevice.Component.3";*/


STDAPI DllInstall(char* s);
STDAPI DllRegisterServer();
STDAPI DllUnregisterServer();


// This function will register a component in the Registry.
// The component calls this function from its DllRegisterServer function.
HRESULT RegisterServer(HMODULE hModule, 
                       const CLSID& clsid, 
                       const WCHAR* szFriendlyName,
                       const WCHAR* szVerIndProgID,
                       const WCHAR* szProgID) ;

// This function will unregister a component.  Components
// call this function from their DllUnregisterServer function.
HRESULT UnregisterServer(const CLSID& clsid,
                         const WCHAR* szVerIndProgID,
                         const WCHAR*  szProgID) ;