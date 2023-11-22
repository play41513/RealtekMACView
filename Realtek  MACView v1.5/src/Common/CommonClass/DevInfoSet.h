// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef DevInfoSetH
#define DevInfoSetH
//---------------------------------------------------------------------------
#include "PreDefine.h"

#ifdef __cplusplus
	extern "C" {
#endif
#include <setupapi.h> //HDEVINFO
#ifdef __cplusplus
	}
#endif
//---------------------------------------------------------------------------
class DevInfoSet
{
private:
	const PCTSTR enumerator;
	const HWND hwnd;
	static wchar_t *wstr;
protected:
	HDEVINFO hdevinfo;
	GUID *pClassGuid;
private:
	bool GetDeviceRegistryProperty(tstring &property, SP_DEVINFO_DATA *pspDeviceInfoData, DWORD dwProperty);
//	bool EnumUsbHubTree(tinyxml2::XMLDocument *doc);
//	bool ProcessTreeItem(USB_Hub *pHub, XMLElement *pNode);
//	void ClearHusb();
protected:
	inline void Close(){
		if(this->hdevinfo!=INVALID_HANDLE_VALUE){
			SetupDiDestroyDeviceInfoList(this->hdevinfo);
			this->hdevinfo=INVALID_HANDLE_VALUE;
		}
	}
	inline bool Open(){
		this->hdevinfo = SetupDiGetClassDevs(
			this->pClassGuid, this->enumerator,
//			NULL, DIGCF_DEVICEINTERFACE | DIGCF_ALLCLASSES | DIGCF_PRESENT);
			this->hwnd, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
		return this->hdevinfo != INVALID_HANDLE_VALUE;
	}

	bool ConstructDisplayName(tstring &property, SP_DEVINFO_DATA *pspDevinfoData);
	bool GetDevicePath(tstring &devicePath, SP_DEVINFO_DATA *pDevInfoData);
public:
	inline GUID* GetClassGuid(){return pClassGuid;}
public:
	DevInfoSet(GUID *pClassGuid, const PCTSTR enumerator = NULL, HWND hwnd = NULL);
	~DevInfoSet();
};
//---------------------------------------------------------------------------
#endif
