// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop

 #include <sstream>
#include <iomanip>
using namespace std;

#include "USB_Hub.h"
#include <dbt.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
USB_NODE_INFORMATION g_UsbNodeInformation;
//---------------------------------------------------------------------------
USB_Hub::USB_Hub(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort):
USB_Device(devicePath, displayName, pUpstreamDevice, ulUpstreamPort),
ucDownstreamPorts('0')
{

}
//---------------------------------------------------------------------------
USB_Hub::~USB_Hub(){
	Close();
}
//---------------------------------------------------------------------------
wstring USB_Hub::IoctlGetNodeConnectionName(unsigned long index){
	USB_NODE_CONNECTION_NAME name;
	USB_NODE_CONNECTION_NAME *pName;
	unsigned long nBytes, szName;
	wstring nodeName;
	bool bl;

	szName = sizeof(name);
	ZeroMemory(&name, szName);
	name.ConnectionIndex = index;

	// 1. Get the length of the name of the driver key.
	bl = DeviceIoControl(
		handle, IOCTL_USB_GET_NODE_CONNECTION_NAME,
//		NULL, 0,
		&name, szName,
		&name, szName,
		&nBytes,
		NULL);

	if(!bl){
//		printf("First IOCTL_GET_HCD_DRIVERKEY_NAME request failed\n");
		return NULL;
	}

	// 2. Get the length of the driver key name.
	nBytes = name.ActualLength;

	if(nBytes < szName){
// 		printf("Incorrect length received by IOCTL_GET_HCD_DRIVERKEY_NAME.\n");
		return NULL;
	}

//  3. Allocate memory for a USB_HCD_DRIVERKEY_NAME to hold the driver key name.
	pName = (USB_NODE_CONNECTION_NAME *)malloc(nBytes);

	if(pName == NULL){
//     	printf("Failed to allocate memory.\n");
		return NULL;
	}

	pName->ConnectionIndex = index;

// 4. Get the name of the driver key of the device attached to the specified port.
	bl = DeviceIoControl(
		handle, IOCTL_USB_GET_NODE_CONNECTION_NAME,
//		NULL, 0,
		pName, nBytes,
		pName, nBytes,
		&nBytes,
		NULL);

	if(!bl){
//		printf("Second IOCTL_GET_HCD_DRIVERKEY_NAME request failed.\n");
		free(pName);
		return NULL;
	}

	nodeName = pName->NodeName;

	free(pName);

	return nodeName;
}
//---------------------------------------------------------------------------
bool USB_Hub::FindNextUsbHubSibling(USB_Hub **ppUsbHub){
	USB_Hub *pHub;

	if(ulUpstreamPort == 0){
		return NULL;
	}

	pHub = (USB_Hub*)this->pUpstreamDevice;

	return pHub->FindNextUsbHubChild(ulUpstreamPort + 1, ppUsbHub);
}
//---------------------------------------------------------------------------
bool USB_Hub::Initialize(){
	USB_HUB_DESCRIPTOR *pUsbHubDescriptor;

	if(!USB_Device::Initialize()){
		return false;
	}

	pUsbHubDescriptor = &(this->pUsbNodeInformation->u.HubInformation.HubDescriptor);

	//Get the information of the USB HUB.
	if(IOCTL_GET_NODE_INFORMATION()){
		this->ucDownstreamPorts = pUsbHubDescriptor->bNumberOfPorts;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool USB_Hub::FindNextUsbHubChild(UCHAR startPort, USB_Hub **ppUsbHub){
	USB_NODE_CONNECTION_INFORMATION_EX *pUsbNodeConnectionInformationEx;
	bool bl;
	DWORD dwSize;
	USB_Hub *pUsbHub = NULL;
	wstring devicePath, displayName;
	wostringstream woss;

	*ppUsbHub = NULL;

	if(startPort > this->ucDownstreamPorts){
		return true;
	}

	try {
		//配置記憶體
		pUsbNodeConnectionInformationEx = (USB_NODE_CONNECTION_INFORMATION_EX*)malloc(sizeof(USB_NODE_CONNECTION_INFORMATION_EX));

		if(pUsbNodeConnectionInformationEx == NULL){
			throw false;
		}

		//打開設備
		if(!this->Open()){
			throw false;
		}

		for(int i=startPort; i<=this->ucDownstreamPorts; i++){
			ZeroMemory(pUsbNodeConnectionInformationEx, sizeof(USB_NODE_CONNECTION_INFORMATION_EX));
			pUsbNodeConnectionInformationEx->ConnectionIndex = i;

			bl = DeviceIoControl(
				this->handle, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX,
				pUsbNodeConnectionInformationEx, sizeof(USB_NODE_CONNECTION_INFORMATION_EX),
				pUsbNodeConnectionInformationEx, sizeof(USB_NODE_CONNECTION_INFORMATION_EX),
				&dwSize, 0);

			if(!bl){
				throw false;
			}

			//if(pUsbNodeConnectionInformationEx->ConnectionStatus != DeviceConnected){
			if(pUsbNodeConnectionInformationEx->ConnectionStatus == NoDeviceConnected ||
			   pUsbNodeConnectionInformationEx->ConnectionIndex ==0
			){
				continue;
			}

			if(!pUsbNodeConnectionInformationEx->DeviceIsHub){
				continue;
			}

			devicePath.append(L"\\\\.\\" + IoctlGetNodeConnectionName(i));

			woss<< L"[" <<
				   dec << setw(2) << setfill(L'0') <<
				   pUsbNodeConnectionInformationEx->ConnectionIndex <<
				   L"]" <<
				   L"HUB#VID_" <<
				   hex << setw(4) << setfill(L'0') << uppercase <<
				   pUsbNodeConnectionInformationEx->DeviceDescriptor.idVendor <<
				   L"&PID_" <<
				   hex << setw(4) << setfill(L'0') << uppercase <<
				   pUsbNodeConnectionInformationEx->DeviceDescriptor.idProduct <<
				   L"&REV_" <<
				   hex << setw(4) << setfill(L'0') << uppercase <<
				   pUsbNodeConnectionInformationEx->DeviceDescriptor.bcdDevice <<
				   std::ends;

			displayName.append(woss.str());
			pUsbHub = new USB_Hub(devicePath, displayName, (void*)this, pUsbNodeConnectionInformationEx->ConnectionIndex);

			break;
		}

	} catch (...) {

	}

	if(pUsbHub != NULL){
		if(!pUsbHub->Initialize()){
			delete pUsbHub;
			pUsbHub = NULL;
			bl = false;
		}else{
			*ppUsbHub = pUsbHub;
			bl = true;
		}
	}


	this->Close();

	if(pUsbNodeConnectionInformationEx != NULL){
		free(pUsbNodeConnectionInformationEx);
		pUsbNodeConnectionInformationEx = NULL;
	}

	return bl;
}
//---------------------------------------------------------------------------
bool USB_Hub::FindFirstUsbHubChild(USB_Hub **ppUsbHub){
	return FindNextUsbHubChild(1, ppUsbHub);
}
//---------------------------------------------------------------------------
