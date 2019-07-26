// IAacLedDeviceHal.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include "IAacLedDeviceHal.h"



#include "rpc.h"
#include "rpcndr.h"
#include "oaidl.h"
#include "ocidl.h"


#ifdef __cplusplus
extern "C" {
#endif 


#ifndef CLSID_DEFINED
#define CLSID_DEFINED
	typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}


	MIDL_DEFINE_GUID(IID, IID_IAacLedDeviceHal, 0xF2C8D5B4, 0x3854, 0x4325, 0x8A, 0x4F, 0xFD, 0x7C, 0x50, 0x72, 0xE3, 0xB9);


	MIDL_DEFINE_GUID(IID, IID_IAacLedDevice, 0x61711778, 0xAB59, 0x4026, 0x89, 0xE8, 0x7A, 0x63, 0x42, 0x2C, 0x29, 0xC2);



	MIDL_DEFINE_GUID(CLSID, CLSID_AacLedDeviceHal, 0xB83E70EA, 0x182B, 0x40A9, 0xB5, 0x5E, 0xC4, 0xF9, 0x47, 0xF7, 0xC8, 0x56);


	//MIDL_DEFINE_GUID(CLSID, CLSID_AacLedDevice, 0xC532D261, 0x993A, 0x444C, 0xB4, 0x01, 0xB9, 0x85, 0xF3, 0x43, 0xA7, 0xFF);

#undef MIDL_DEFINE_GUID


#ifdef __cplusplus
}
#endif


