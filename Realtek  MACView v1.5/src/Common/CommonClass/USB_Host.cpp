// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop

#include "USB_Host.h"
#include <tchar.h>



//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
USB_Host::USB_Host(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort):
USB_Device(devicePath, displayName, pUpstreamDevice, ulUpstreamPort),
pRootHub(NULL)
{}
//---------------------------------------------------------------------------
USB_Host::~USB_Host(){
	SAFE_DELETE(pRootHub)
}
//---------------------------------------------------------------------------
USB_Hub *USB_Host::GetRootHub(){
	wstring rootHubName;

	if(this->pRootHub == NULL){
		if(IOCTL_GET_ROOT_HUB_NAME(rootHubName)){
			this->pRootHub = new USB_Hub(rootHubName, L"ROOT_HUB", NULL, 0);
			if(!this->pRootHub->Initialize()){
				delete this->pRootHub;
				this->pRootHub = NULL;
			}
		}
	}

	return this->pRootHub;
}
//---------------------------------------------------------------------------
bool USB_Host::IOCTL_GET_ROOT_HUB_NAME(wstring &rootHubName){
	USB_ROOT_HUB_NAME *pRootHub_Symbol_Link;
	unsigned long rslt, nBytes;
	bool bl;

	if(!this->Open()) return false;

	__try {
		nBytes = sizeof(USB_ROOT_HUB_NAME);
		pRootHub_Symbol_Link = (USB_ROOT_HUB_NAME*)malloc(nBytes);

		if(pRootHub_Symbol_Link == NULL){
			bl = false;
			throw false;
		}

		// 1. Get the length of the name of the USB_ROOT_HUB_NAME.
		bl = DeviceIoControl(
			this->handle,
			IOCTL_USB_GET_ROOT_HUB_NAME,
			NULL,
			0,
			pRootHub_Symbol_Link,
			nBytes,
			&nBytes,
			NULL);


		if(!bl){
			throw bl;
		}

		nBytes = pRootHub_Symbol_Link->ActualLength;

		//2. Allocate memory for a USB_ROOT_HUB_NAME.
		free(pRootHub_Symbol_Link);
		pRootHub_Symbol_Link = (USB_ROOT_HUB_NAME *)malloc(nBytes);

		if(pRootHub_Symbol_Link == NULL){
			bl = false;
			throw false;
		}

		//3. Get USB_ROOT_HUB_NAME.
		bl = DeviceIoControl(
			this->handle,
			IOCTL_USB_GET_ROOT_HUB_NAME,
			NULL,
			0,
			pRootHub_Symbol_Link,
			nBytes,
			&nBytes,
			NULL);

		if(bl){
			rootHubName = wstring(L"\\\\.\\");
			rootHubName.append(pRootHub_Symbol_Link->RootHubName);
		}
	}__finally{
		this->Close();

		if(pRootHub_Symbol_Link != NULL){
			free(pRootHub_Symbol_Link);
		}
	}

	return bl;
}
//---------------------------------------------------------------------------
