// IAacLedDeviceHal.cpp : Defines the exported functions for the DLL application.
//
#include "AacDef_c.h"


#include "config.h"

extern "C" const WCHAR FName[]  = MY_HAL_FRIEND_NAME;
extern "C" const WCHAR VerInd[] = MY_HAL_VER_ID;
extern "C" const WCHAR ProgId[] = MY_HAL_PROG_ID;


AAC_BEGIN_DEFINE_CLSID

extern "C" const CLSID CLSID_MYLEDDEVICE = MY_HAL_CLSID;

AAC_END_DEFINE_CLSID

