// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------


#pragma hdrstop

#include "HidInfoSet.h"

	#ifdef __cplusplus
	extern "C" {
	#endif
#include "hidsdi.h"  //HidD_GetHidGuid(&HidGuid);
	#ifdef __cplusplus
	}
	#endif
//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------
HidInfoSet::HidInfoSet(const PCTSTR enumerator, HWND hwnd)
:DevInfoSet(GetHidGuid(), enumerator, hwnd)
{

}
//---------------------------------------------------------------------------
HidInfoSet::~HidInfoSet(){

}
//---------------------------------------------------------------------------
GUID* HidInfoSet::GetHidGuid(){
	HidD_GetHidGuid(pClassGuid);
	return pClassGuid;
}
//---------------------------------------------------------------------------
