// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
// ---------------------------------------------------------------------------
#include <vcl.h>
// #include "common.h"
#pragma hdrstop



#include "SetupDi.h"


#include <initguid.h>
#include "usbiodef.h"    //需先#include <initguid.h>

// usb200.h
#include "usbioctl.h"
// 使用CTL_CODE必须加入winioctl.h
#include <winioctl.h>

#include <strsafe.h>
// ---------------------------------------------------------------------------

#pragma package(smart_init)

// SetupDi::SetupDi(USHORT idVendor, USHORT idProduct):idVendor(idVendor), idProduct(idProduct) {
// }

SetupDi::SetupDi() : m_hDevInfo(INVALID_HANDLE_VALUE), m_hardwareId(NULL) {
	// SetupDi(0x0000, 0x0000);
}

SetupDi::~SetupDi() {
	Close();
}
///////////////////////////////////////////////////////////////////////////////
bool SetupDi::Create(const GUID *pClassGuid, const TCHAR* pEnumerator, const unsigned long flag) {
	m_id = pEnumerator;

	Close();

	m_hDevInfo = SetupDiGetClassDevs(
		pClassGuid,// &GUID_DEVINTERFACE_USB_DEVICE
		pEnumerator,
		NULL,
		flag);

	return m_hDevInfo != INVALID_HANDLE_VALUE;
}

//Create all device
bool SetupDi::Create() {
	return Create(NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_ALLCLASSES | DIGCF_PRESENT);
}

//Create a USB HUB
bool SetupDi::CreateHub(){
	return Create(&GUID_DEVINTERFACE_USB_HUB, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
}

void SetupDi::Close(){
	if(m_hardwareId){
        LocalFree(m_hardwareId);
	}

	if (m_hDevInfo != INVALID_HANDLE_VALUE) {
		SetupDiDestroyDeviceInfoList(m_hDevInfo);
	}
}
///////////////////////////////////////////////////////////////////////////////
int SetupDi::GetHardwareId(int index){
	// Enumerate through all devices in Set.
	SP_DEVINFO_DATA spDeviceInfoData;
	spDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	if (!SetupDiEnumDeviceInfo(m_hDevInfo, index, &spDeviceInfoData)) {
		if (GetLastError() == ERROR_NO_MORE_ITEMS) {
			return 0;
		}else{
			return -1;
		}
	}

	if(m_hardwareId){
		LocalFree(m_hardwareId);
	}

//	m_hardwareId = GetDeviceRegistryProperty(m_hDevInfo, &spDeviceInfoData,	SPDRP_COMPATIBLEIDS);
//	if(m_hardwareId == NULL || _tcscmp(m_hardwareId, _T("USB\\USB30_HUB")) != 0){
//		return -1;
//	}
//
//	LocalFree(m_hardwareId);
	m_hardwareId = GetDeviceRegistryProperty(m_hDevInfo, &spDeviceInfoData,	SPDRP_HARDWAREID);

	if(m_hardwareId == NULL) {
		return -1;
	}

	return 1;
}

bool SetupDi::GetRevFromHardwareId(TCHAR * id, TCHAR *rev, int max){
	TCHAR *pos;

	pos = _tcsrchr(id, _T('_'));

	if(pos != NULL){
		if(_tcscpy_s(rev, max, ++pos) == 0){
			return true;
		}
	}

	return false;
}

int SetupDi::EnumHardwareId(vector<tstring> &ids, TCHAR *id){
	bool bl;
	DWORD index = 0;

	if(!ids.empty()){
		ids.clear();
	}

	while(true){
		int rst = GetHardwareId(index);

		if(rst == 0){
        	break;
		}else if(rst == 1){
        	if(id == NULL){
				bl = true;
			}else{
				if(_tcsstr(_tcsupr(m_hardwareId), _tcsupr(id))){
					bl = true;
				}else{
					bl = false;
				}
			}


			if(bl){
				ids.push_back(m_hardwareId);
			}
		}

        index++;
	}

//	while(GetHardwareId(index) != 0){
//		if(GetHardwareId(index) == 1){
//			if(id == NULL){
//				bl = true;
//			}else{
//				if(_tcsstr(_tcsupr(m_hardwareId), _tcsupr(id))){
//					bl = true;
//				}else{
//					bl = false;
//				}
//			}
//
//
//			if(bl){
//				ids.push_back(m_hardwareId);
//            }
//		}
//
//		index++;
//	}

	return ids.size();
}

bool SetupDi::OpenDeviceInfo(SP_DEVINFO_DATA &spDevinfoData) {
	return SetupDiOpenDeviceInfo(m_hDevInfo, m_id, NULL, 0, &spDevinfoData)
	!= false;
}

bool SetupDi::GetDevicePathByDeviceInfoData(const GUID *pInterfaceClassGuid, SP_DEVINFO_DATA &spDeviceInfoData, tstring &path){
	bool bl;
	DWORD numberOfBytes;
	SP_DEVICE_INTERFACE_DATA spDeviceInterfaceData = {
		0
	};
	spDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDeviceInterfaceDetailData;

	if (SetupDiEnumDeviceInterfaces(m_hDevInfo, // 辨識每一個HID介面
			&spDeviceInfoData, pInterfaceClassGuid, 0, &spDeviceInterfaceData))
	{
		// Get details about a device interface.
		// 1. Get the required buffer size
		bl = SetupDiGetDeviceInterfaceDetail(m_hDevInfo, // 取得裝置路徑
			&spDeviceInterfaceData, NULL, 0, &numberOfBytes, NULL);

		if (!bl && ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
			// Allocate an appropriately sized buffer and call the function again to get the interface details.
			// 配置記憶體
			pDeviceInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)
				malloc(numberOfBytes);
			// 初始結構
			pDeviceInterfaceDetailData->cbSize = sizeof
				(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 第二次呼叫取得裝置路徑
			bl = SetupDiGetDeviceInterfaceDetail(m_hDevInfo, // 取得裝置路徑
				&spDeviceInterfaceData, pDeviceInterfaceDetailData,
				numberOfBytes, NULL, NULL);

			if (bl) {
				path = tstring(pDeviceInterfaceDetailData->DevicePath);
//				path.push_back(tstring(pDeviceInterfaceDetailData->DevicePath));
			}

			free(pDeviceInterfaceDetailData);
		}
	}

	return bl;
}

bool SetupDi::IsPlug(TCHAR *hardwareId) {
	DWORD index = 0;

	while(GetHardwareId(index) != 0){
		if(GetHardwareId(index) == 1){


			if(_tcscmp(hardwareId, m_hardwareId) == 0){
				return true;
			}
		}
		index++;
	}

	return false;
}

int SetupDi::FindDevicesPathByHardwareId(vector<tstring> &idList, vector<tstring> &pathList, vector<int> &iFind) {
	int cnt = 0;
	tstring path;

	// Enumerate through all devices in Set.
	SP_DEVINFO_DATA spDeviceInfoData;
	spDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD index = 0; ; index++) {
		if (!SetupDiEnumDeviceInfo(m_hDevInfo, index, &spDeviceInfoData)) {
			if (GetLastError() == ERROR_NO_MORE_ITEMS) {
				break;
			}
			else {
				continue;
			}
		}

		DWORD dwPropertyRegDataType;
		LPTSTR pBuffer;
//		DWORD buffersize = 0;

		pBuffer = GetDeviceRegistryProperty(m_hDevInfo, &spDeviceInfoData,
			SPDRP_HARDWAREID);

		if (pBuffer == NULL) {
			continue;
		}

		for(unsigned int i = 0; i < idList.size(); i++) {
			// 驗證 Hardware ID
			if (_tcsstr(_tcsupr(pBuffer), idList[i].c_str())) {
				if(GetDevicePathByDeviceInfoData(
					&GUID_DEVINTERFACE_USB_HUB,
					spDeviceInfoData,
					path
				)){
					pathList.push_back(path.c_str());
					iFind.push_back(i);
					cnt++;
					break;
				}else{
					LocalFree(pBuffer);

					return -1;
				}
			}
		}

		LocalFree(pBuffer);
	}

	return cnt;
}

int SetupDi::FindDevicesPathByPartDevicePath(const GUID *pClassGuid,
	TCHAR partDevicePath[1], vector<tstring> *pList = NULL) {
	HDEVINFO hDevInfo; // 需釋放
	int cnt = 0;

	// Create a HDEVINFO with all present devices.
	// hDevInfo = SetupDiGetClassDevs(
	// &GUID_DEVINTERFACE_USB_DEVICE,
	// 0, // Enumerator
	// 0,
	// DIGCF_PRESENT | DIGCF_ALLCLASSES );

	hDevInfo = SetupDiGetClassDevs(pClassGuid, // &GUID_DEVINTERFACE_USB_DEVICE
		0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	if (hDevInfo == INVALID_HANDLE_VALUE) {
		// Insert error handling here.
		return -1;
	}

	// Enumerate through all devices in Set.
	SP_DEVICE_INTERFACE_DATA spDeviceInterfaceData;

	for (DWORD index = 0; ; index++) {
		ZeroMemory(&spDeviceInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
		spDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		if (!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, pClassGuid, index,
				&spDeviceInterfaceData)) {
			if (GetLastError() == ERROR_NO_MORE_ITEMS) {
				break;
			}
			else {
				continue;
			}
		}

		PSP_DEVICE_INTERFACE_DETAIL_DATA pspDeviceInterfaceDetailData = NULL;
		DWORD dwSize = 0;

		// buffer = GetDeviceRegistryProperty(hDevInfo, &spDeviceInfoData, SPDRP_HARDWAREID);

		int count = 0;
		while (!SetupDiGetDeviceInterfaceDetail(hDevInfo,
				&spDeviceInterfaceData,
				pspDeviceInterfaceDetailData, dwSize, &dwSize, 0)) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				// Change the buffer size.
				if (pspDeviceInterfaceDetailData)
					LocalFree(pspDeviceInterfaceDetailData);
				// Double the size to avoid problems on
				// W2k MBCS systems per KB 888609.
				pspDeviceInterfaceDetailData =
					(SP_DEVICE_INTERFACE_DETAIL_DATA*)LocalAlloc(LPTR, dwSize);

				if (pspDeviceInterfaceDetailData) {
					pspDeviceInterfaceDetailData->cbSize = sizeof
						(SP_DEVICE_INTERFACE_DETAIL_DATA);
				}
			}
			else {
				// Insert error handling here.
				// MessageBox(NULL, _T("_SetupDiGetDeviceRegistryProperty fail!"), _T("FindDevicesById"), MB_ICONSTOP);
				// SetupDiGetDeviceInterfaceDetail(hDevInfo, &spDeviceInterfaceData, pspDeviceInterfaceDetailData, dwSize, &dwSize, 0);
				// break;
			}

			if (++count > 3) {
				if (pspDeviceInterfaceDetailData)
					LocalFree(pspDeviceInterfaceDetailData);
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return cnt;
			}
		}

		if (partDevicePath == NULL) {
			if (pList && pspDeviceInterfaceDetailData) {
				pList->push_back
					(tstring(pspDeviceInterfaceDetailData->DevicePath));
			}
			LocalFree(pspDeviceInterfaceDetailData);
			cnt++;
		}
		else if (pspDeviceInterfaceDetailData && _tcsstr
			(_tcsupr(pspDeviceInterfaceDetailData->DevicePath),
				_tcsupr(partDevicePath))) {
			if (pList) {
				pList->push_back
					(tstring(pspDeviceInterfaceDetailData->DevicePath));
			}
			LocalFree(pspDeviceInterfaceDetailData->DevicePath);
			cnt++;
			// GetDeviceRegistryProperty(hDevInfo, &spDeviceInfoData, SPDRP_MAXIMUM_PROPERTY);
		}
	}

	// Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return cnt;
}

int SetupDi::FindDevicesRegistryPropertyById(TCHAR pvmid[1],
	vector<tstring> *pList = NULL) {
	int cnt = 0;

	// Enumerate through all devices in Set.
	SP_DEVINFO_DATA spDeviceInfoData;
	spDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD index = 0; ; index++) {
		if (!SetupDiEnumDeviceInfo(m_hDevInfo, index, &spDeviceInfoData)) {
			if (GetLastError() == ERROR_NO_MORE_ITEMS) {
				break;
			}
			else {
				continue;
			}
		}

		DWORD dwPropertyRegDataType;
		LPTSTR buffer;
//		DWORD buffersize = 0;

		buffer = GetDeviceRegistryProperty(m_hDevInfo, &spDeviceInfoData,
			SPDRP_HARDWAREID);

		if (pvmid == NULL) {
			if (pList && buffer) {
				pList->push_back(tstring(buffer));
			}
			LocalFree(buffer);
			cnt++;
		}
		else if (buffer && _tcsstr(_tcsupr(buffer), _tcsupr(pvmid))) {
			if (pList) {
				pList->push_back(tstring(buffer));
			}
			LocalFree(buffer);
			cnt++;
			// GetDeviceRegistryProperty(hDevInfo, &spDeviceInfoData, SPDRP_MAXIMUM_PROPERTY);
		}
	}

	return cnt;
}

TCHAR* SetupDi::GetDeviceRegistryProperty(HDEVINFO hDevInfo,
	SP_DEVINFO_DATA *pspDeviceInfoData, DWORD dwProperty) {
	DWORD dwPropertyRegDataType;
	LPTSTR buffer = NULL;
	DWORD buffersize = 0;

	while (!SetupDiGetDeviceRegistryProperty(hDevInfo, pspDeviceInfoData,
			dwProperty, &dwPropertyRegDataType, (PBYTE)buffer, buffersize,
			&buffersize)) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			// Change the buffer size.
			if (buffer)
				LocalFree(buffer);
			// Double the size to avoid problems on
			// W2k MBCS systems per KB 888609.
			buffer = (TCHAR*)LocalAlloc(LPTR, buffersize);
		}
		else {
			// Insert error handling here.
			// MessageBox(NULL, _T("_SetupDiGetDeviceRegistryProperty fail!"), _T("List"), MB_ICONSTOP);
			if (buffer) {
				LocalFree(buffer);
				buffer = NULL;
			}
			break;
		}
	}

	return buffer;
}

bool SetupDi::FindDevicesOnHubPath(const TCHAR *pHubDevicePath, unsigned short idVender, unsigned short idProduct, unsigned short idRev, TCHAR *pCompositeHardwareId, int iNumOfDevice, TCHAR *errBuffer, unsigned long errBufferSize) {
	bool bl;
	// String devicePath;
	// USB_NODE_CONNECTION_INFORMATION_EX info;
	// DWORD dwSize;
	HANDLE hHub;

	m_level = 0;

	hHub = CreateHandle(pHubDevicePath);

	if (hHub == NULL) {
		_tcscat_s(errBuffer, errBufferSize, _T("無法開啟HUB_"));
		MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPath"), MB_ICONSTOP);

		return false;
	}

	bl = FindDevicesOnHubHandle(hHub, idVender, idProduct, idRev, pCompositeHardwareId, iNumOfDevice, errBuffer, errBufferSize);

	CloseHandle(hHub);

	return bl;
}

bool SetupDi::FindDevicesOnHubPort(HANDLE hHub, int port, unsigned short idVender, unsigned short idProduct, unsigned short idRev, TCHAR *pCompositeHardwareId, int iNumOfDevice, TCHAR *errBuffer, unsigned long errBufferSize) {
	bool bl;
	DWORD dwSize;
	HANDLE hSubHub;
	USB_NODE_CONNECTION_INFORMATION_EX *pUsbNodeConnectionInformationEx;
	USB_NODE_CONNECTION_NAME *pUsbNodeConnectionName;

	pUsbNodeConnectionInformationEx = (USB_NODE_CONNECTION_INFORMATION_EX*)
		malloc(sizeof(USB_NODE_CONNECTION_INFORMATION_EX));
	if (!pUsbNodeConnectionInformationEx) {
		_tcscat_s(errBuffer, errBufferSize, _T("USB_NODE_CONNECTION_INFORMATION_EX配置記憶體失敗"));
		MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
		return false;
	}

	// 取得連結資訊
	pUsbNodeConnectionInformationEx->ConnectionIndex = port;

	bl = DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX,
		pUsbNodeConnectionInformationEx,
		sizeof(USB_NODE_CONNECTION_INFORMATION_EX),
		pUsbNodeConnectionInformationEx,
		sizeof(USB_NODE_CONNECTION_INFORMATION_EX), &dwSize, 0);

	if (bl) {
		bl = false;

		//The port of hub has being connected to a device.
		if (pUsbNodeConnectionInformationEx->ConnectionStatus == DeviceConnected){
			// 配置記憶體
			pUsbNodeConnectionName = (USB_NODE_CONNECTION_NAME*)malloc
				(sizeof(USB_NODE_CONNECTION_NAME));
			if (!pUsbNodeConnectionName) {
				free(pUsbNodeConnectionInformationEx);
//				pUsbNodeConnectionInformationEx = NULL;
				_tcscat_s(errBuffer, errBufferSize, _T("USB_NODE_CONNECTION_NAME配置記憶體失敗"));
				MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);

				return false;
			}

			// Device is HUB
			if (pUsbNodeConnectionInformationEx->DeviceIsHub) {
				pUsbNodeConnectionName->ConnectionIndex = port;

				bl = DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_NAME,
					pUsbNodeConnectionName, sizeof(USB_NODE_CONNECTION_NAME),
					pUsbNodeConnectionName, sizeof(USB_NODE_CONNECTION_NAME),
					&dwSize, 0);

				if (bl) {
					bl = false;

					dwSize = pUsbNodeConnectionName->ActualLength;
					free(pUsbNodeConnectionName);
					pUsbNodeConnectionName = (USB_NODE_CONNECTION_NAME*)malloc
						(dwSize);

					if (pUsbNodeConnectionName) {
						pUsbNodeConnectionName->ConnectionIndex = port;

						bl = DeviceIoControl(hHub,
							IOCTL_USB_GET_NODE_CONNECTION_NAME,
							pUsbNodeConnectionName, dwSize,
							pUsbNodeConnectionName, dwSize, &dwSize, 0);

						if (bl) {
							bl = false;
							String hub_path;
							hub_path.printf(L"\\\\.\\%s", pUsbNodeConnectionName->NodeName);

							hSubHub = CreateFile(hub_path.t_str(),
								GENERIC_WRITE, FILE_SHARE_WRITE,
								0, OPEN_EXISTING, 0, 0);

							if (hSubHub != INVALID_HANDLE_VALUE) {
								bl = FindDevicesOnHubHandle(hSubHub, idVender, idProduct, idRev, pCompositeHardwareId, iNumOfDevice, errBuffer, errBufferSize);
								CloseHandle(hSubHub);
							}else{
								_tcscat_s(errBuffer, errBufferSize, _T("子HUB開啟失敗"));
								MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
							}
						}else{
							_tcscat_s(errBuffer, errBufferSize, _T("IOCTL_USB_GET_NODE_CONNECTION_NAME失敗"));
							MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
						}
					}else{
						_tcscat_s(errBuffer, errBufferSize, _T("USB_NODE_CONNECTION_NAME配置記憶體失敗"));
						MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
					}
				}else{
						_tcscat_s(errBuffer, errBufferSize, _T("IOCTL_USB_GET_NODE_CONNECTION_NAME失敗"));
						MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
				}
			}else{
				// Device is not HUB
				if(
					pUsbNodeConnectionInformationEx->DeviceDescriptor.idVendor == idVender &&
					pUsbNodeConnectionInformationEx->DeviceDescriptor.idProduct == idProduct &&
					pUsbNodeConnectionInformationEx->DeviceDescriptor.bcdDevice == idRev
				){

//					TCHAR id[14] = {0};

					// 被搜尋的裝置數量(VID=idVendor PID=idProduct)
//					_stprintf_s(id, 14, _T("%04X&PID_%04X"), idVender, idProduct);

//					for (int i = 0; i < 2; i++) {
						if (FindDevicesRegistryPropertyById(pCompositeHardwareId)	== iNumOfDevice) {
							bl = true;
//							break;
						}
						else {
							Application->ProcessMessages();
//							Sleep(1000);
						}
//					}

//					_tcscat_s(errBuffer, errBufferSize, _T("的Composite裝置掛載不完整"));
//					MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
				}
			}

			// 釋放記憶體
			if (pUsbNodeConnectionName) {
				free(pUsbNodeConnectionName);
			}
		}
	}else{
		_tcscat_s(errBuffer, errBufferSize, _T("USB_NODE_CONNECTION_INFORMATION_EX配置記憶體失敗"));
		MessageBox(NULL, errBuffer, _T("FindDevicesOnHubPort"), MB_ICONSTOP);
	}

	if (pUsbNodeConnectionInformationEx) {
		free(pUsbNodeConnectionInformationEx);
	}

	return bl;
}

bool SetupDi::FindDevicesOnHubHandle(HANDLE hHub, unsigned short idVender, unsigned short idProduct, unsigned short idRev, TCHAR *pCompositeHardwareId, int iNumOfDevice, TCHAR *errBuffer, unsigned long errBufferSize) {
	String hubPath;
	DWORD dwSize;
	bool bl;
	PUSB_NODE_INFORMATION pHubInfo;
	USB_HUB_DESCRIPTOR *pHubDescriptor;

	m_level++;

	pHubInfo = (PUSB_NODE_INFORMATION)malloc(sizeof(USB_NODE_INFORMATION));

	if (!pHubInfo) {
		_tcscat_s(errBuffer, errBufferSize, _T("USB_NODE_INFORMATION配置記憶體失敗"));
		MessageBox(NULL, errBuffer, _T("FindDevicesOnHubHandle"), MB_ICONSTOP);
		return false;
	}

	pHubDescriptor = &(pHubInfo->u.HubInformation.HubDescriptor);

	if (!DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, 0, 0, pHubInfo,
			sizeof(USB_NODE_INFORMATION), &dwSize, 0)) {
		free(pHubInfo);
		_tcscat_s(errBuffer, errBufferSize, _T("IOCTL_USB_GET_NODE_INFORMATION失敗"));
		MessageBox(NULL, errBuffer, _T("FindDevicesOnHubHandle"), MB_ICONSTOP);
		return false;
	}

	for (int port_idx = 1; port_idx <= pHubDescriptor->bNumberOfPorts;
		port_idx++) {
		bl = FindDevicesOnHubPort(hHub, port_idx, idVender, idProduct, idRev, pCompositeHardwareId, iNumOfDevice, errBuffer, errBufferSize);

		if (bl) {
			break;
		}
	}

	free(pHubInfo);

	m_level--;

	return bl;
}

HANDLE SetupDi::CreateHandle(const TCHAR *pDevicePath) {
	HANDLE hHandle;

	hHandle = CreateFile(pDevicePath, GENERIC_WRITE, FILE_SHARE_WRITE, 0,
		OPEN_EXISTING, 0, 0);

	// devicePath = _tcsupr(pspDeviceInterfaceDetailData->DevicePath);
	// devicePath = _tcslwr(pspDeviceInterfaceDetailData->DevicePath);
	// _tprintf(_T("HUB DEVICE PATH : %s\r\n"), pspDeviceInterfaceDetailData->DevicePath);

	if (hHandle == INVALID_HANDLE_VALUE) {
		hHandle = NULL;
	}

	return hHandle;
}

// Intel(R) USB 3.0 可延伸主機控制器
//bool SetupDi::FindU3HostController() {
//	SP_DEVINFO_DATA spDeviceInfoData;
//	HDEVINFO hDevinfo;
//	DWORD dwPropertyRegDataType;
//	DWORD dwPropertyBufferSize;
//	DWORD dwRequiredSize;
//	LPTSTR pPropertyBuffer;
//
//	// 釋放 DeviceInfoSet
//	// if(hDevinfo){
//	// SetupDiDestoryDeviceInfoList(hDevinfo);
//	// }
//
//	hDevinfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_HOST_CONTROLLER,
//		NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
//
//	if (hDevinfo == INVALID_HANDLE_VALUE) {
//		return false;
//	}
//
//	// Enumerate through all devices in set
//	spDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//
//	for (int i = 0; SetupDiEnumDeviceInfo(hDevinfo, i, &spDeviceInfoData); i++)
//	{
//		pPropertyBuffer = NULL;
//		dwPropertyBufferSize = 0;
//		dwRequiredSize = 0;
//
//		while (true) {
//			SetupDiGetDeviceRegistryProperty(hDevinfo, &spDeviceInfoData,
//				SPDRP_DEVICEDESC, &dwPropertyRegDataType,
//				(PBYTE)pPropertyBuffer, dwPropertyBufferSize,
//				&dwRequiredSize);
//
//			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
//				// Change the buffer size.
//				if (pPropertyBuffer) {
//					LocalFree(pPropertyBuffer);
//				}
//
//				dwPropertyBufferSize = dwRequiredSize;
//				pPropertyBuffer = (LPTSTR)LocalAlloc(LPTR,
//					dwPropertyBufferSize);
//			}
//			else {
//				// Insert error handling here.
//				break;
//			}
//		}
//
//		if (_tcscmp(_T("Intel(R) USB 3.0 可延伸主機控制器"), pPropertyBuffer) == 0) {
//			if (pPropertyBuffer) {
//				LocalFree(pPropertyBuffer);
//			}
//			return true;
//		}
//
//		if (pPropertyBuffer) {
//			LocalFree(pPropertyBuffer);
//		}
//	}
//
//	return false;
//}
