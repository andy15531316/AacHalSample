#ifndef __AAC_INTERFACE_DEF_C_H__
#define __AAC_INTERFACE_DEF_C_H__

#include <windows.h>
#include "rpc.h"
#include "rpcndr.h"
#include "oaidl.h"
#include "ocidl.h"


#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED


#ifdef __cplusplus

#define AAC_BEGIN_DEFINE_CLSID extern "C" { \
	extern "C" const IID IID_IAacLedDeviceHal = { 0xF2C8D5B4, 0x3854, 0x4325, {0x8A, 0x4F, 0xFD, 0x7C, 0x50, 0x72, 0xE3, 0xB9} }; \
	extern "C" const IID IID_IAacLedDevice    = { 0x61711778, 0xAB59, 0x4026, {0x89, 0xE8, 0x7A, 0x63, 0x42, 0x2C, 0x29, 0xC2} }; \
	extern "C" const IID IID_IAacLedDeviceOpt = { 0x68f0c6e1, 0x7469, 0x40b3,{ 0x84, 0xd5, 0xe0, 0x79, 0x3f, 0x44, 0x9e, 0x4d } }; 

#define AAC_DEFINE_CLSID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        extern "C" const CLSID name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}


#define AAC_END_DEFINE_CLSID }

#else 

#define BEGIN_DEFINE_CLSID \
	const IID IID_IAacLedDeviceHal = { 0xF2C8D5B4, 0x3854, 0x4325, {0x8A, 0x4F, 0xFD, 0x7C, 0x50, 0x72, 0xE3, 0xB9} } \
	const IID IID_IAacLedDevice    = { 0x61711778, 0xAB59, 0x4026, {0x89, 0xE8, 0x7A, 0x63, 0x42, 0x2C, 0x29, 0xC2} }  \
	const IID IID_IAacLedDeviceOpt = { 0x68f0c6e1, 0x7469, 0x40b3,{ 0x84, 0xd5, 0xe0, 0x79, 0x3f, 0x44, 0x9e, 0x4d } }; 


#define AAC_DEFINE_CLSID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const CLSID name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#define END_DEFINE_CLSID  

#endif 




#endif //__AAC_INTERFACE_DEF_C_H__