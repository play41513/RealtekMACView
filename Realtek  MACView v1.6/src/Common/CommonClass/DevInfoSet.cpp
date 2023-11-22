// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------


#pragma hdrstop

#include "DevInfoSet.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
wchar_t *DevInfoSet::wstr = NULL;
//---------------------------------------------------------------------------
DevInfoSet::DevInfoSet(GUID *pClassGuid, const PCTSTR enumerator, HWND hwnd)
:pClassGuid(pClassGuid)
,enumerator(enumerator)
,hwnd(hwnd)
,hdevinfo(INVALID_HANDLE_VALUE)
{

}
//---------------------------------------------------------------------------
DevInfoSet::~DevInfoSet(){
	Close();

	if(wstr != NULL){
		LocalFree(wstr);
		wstr = NULL;
	}
}
//---------------------------------------------------------------------------
bool DevInfoSet::GetDeviceRegistryProperty(tstring &property, SP_DEVINFO_DATA *pspDeviceInfoData, DWORD dwProperty){
	DWORD dwPropertyRegDataType;
	DWORD buffersize = 0;
	TCHAR *tmp;

	while (!SetupDiGetDeviceRegistryProperty(
			hdevinfo,
			pspDeviceInfoData,
			dwProperty,
			&dwPropertyRegDataType,
			(PBYTE)tmp,
			buffersize,
			&buffersize))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER){
			tmp = (TCHAR *)malloc(buffersize);

			if(tmp == NULL){
				return false;
			}
		}else{
			// Insert error handling here.
//			MessageBox(NULL, _T("_SetupDiGetDeviceRegistryProperty fail!"), _T("List"), MB_ICONSTOP);
			if(tmp){free(tmp);}

			return false;
		}
	}

	property = tmp;
	if(tmp){free(tmp);}

	return true;
}
//---------------------------------------------------------------------------
bool DevInfoSet::ConstructDisplayName(tstring &property, SP_DEVINFO_DATA *pspDevinfoData) {
	property = _T("");

	if(!GetDeviceRegistryProperty(property, pspDevinfoData, SPDRP_FRIENDLYNAME)) {
		if(!GetDeviceRegistryProperty(property, pspDevinfoData, SPDRP_DEVICEDESC)) {
			if(!GetDeviceRegistryProperty(property, pspDevinfoData, SPDRP_CLASS)) {
				if(!GetDeviceRegistryProperty(property, pspDevinfoData, SPDRP_CLASSGUID)) {
					return false;
				}
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool DevInfoSet::GetDevicePath(tstring &devicePath, SP_DEVINFO_DATA *pDevInfoData){
	bool bl;
	SP_DEVICE_INTERFACE_DATA spDeviceInterfaceData = {0};
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDeviceInterfaceDetailData;
	DWORD numberOfBytes;

	devicePath = _T("");

	spDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	bl = SetupDiEnumDeviceInterfaces(
		hdevinfo,
		(SP_DEVINFO_DATA*)pDevInfoData,
		this->pClassGuid,
		0,
		&spDeviceInterfaceData);

	if(bl){
		// Get details about a device interface.
		// 1. Get the required buffer size
		bl = SetupDiGetDeviceInterfaceDetail(hdevinfo, // 取得裝置路徑
			&spDeviceInterfaceData, NULL, 0, &numberOfBytes, NULL);

		if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
			// Allocate an appropriately sized buffer and call the function again to get the interface details.
			// 配置記憶體
			pDeviceInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)
				malloc(numberOfBytes);
			// 初始結構
			pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 第二次呼叫取得裝置路徑
			bl = SetupDiGetDeviceInterfaceDetail(hdevinfo, // 取得裝置路徑
				&spDeviceInterfaceData, pDeviceInterfaceDetailData,
				numberOfBytes, NULL, NULL);

			if (bl) {
				devicePath = pDeviceInterfaceDetailData->DevicePath;
//				int cc = _tcslen(pDeviceInterfaceDetailData->DevicePath) + 1;
////				int cb = cc * sizeof(TCHAR);
//
//				SAFE_ARRAY_DELETE(wstr);
//				wstr = new wchar_t[cc];
//
//				if(wcscpy_s(wstr, cc, pDeviceInterfaceDetailData->DevicePath) != 0){
//					bl= false;
//				}
			}

			free(pDeviceInterfaceDetailData);
		}
	}

	return bl;
}
//---------------------------------------------------------------------------
