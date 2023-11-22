// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------
#pragma hdrstop

#include "USB_Device.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//USB_Device::USB_Device(wstring hwid)
//:Device(hwid)
//{}
//---------------------------------------------------------------------------
USB_Device::USB_Device(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort):
Device(devicePath, displayName, pUpstreamDevice, ulUpstreamPort),
pUsbNodeInformation(NULL)
{}
//---------------------------------------------------------------------------
USB_Device::~USB_Device(){
	SAFE_DELETE(pUsbNodeInformation);
}
//---------------------------------------------------------------------------
bool USB_Device::Initialize(){
	if(pUsbNodeInformation == NULL){
		pUsbNodeInformation = new USB_NODE_INFORMATION;
//		ZeroMemory(pUsbNodeInformation, sizeof(USB_NODE_INFORMATION)):

		if(pUsbNodeInformation != NULL){
        	return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
bool USB_Device::IOCTL_GET_NODE_INFORMATION(){
	bool bl;
	DWORD dwSize = sizeof(USB_NODE_INFORMATION);

	if(this->Open()){
		if(DeviceIoControl(
			this->handle, IOCTL_USB_GET_NODE_INFORMATION,
			this->pUsbNodeInformation, dwSize,
			this->pUsbNodeInformation, dwSize,
			&dwSize, NULL)
		){
			bl = true;
		}else{
        	bl = false;
		}

		this->Close();
		return bl;
	}

	return false;
}
//---------------------------------------------------------------------------
