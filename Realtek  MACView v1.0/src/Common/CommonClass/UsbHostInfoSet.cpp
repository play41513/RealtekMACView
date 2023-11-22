// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop

#include "UsbHostInfoSet.h"
#include "StringFunction.h"
#include "ConstantString.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
UsbHostInfoSet::UsbHostInfoSet(GUID *pClassGuid, const PCTSTR enumerator, HWND hwnd)
:DevInfoSet(pClassGuid, enumerator, hwnd)
{}
//---------------------------------------------------------------------------
UsbHostInfoSet::~UsbHostInfoSet(){
	ClearHusb();
}
//---------------------------------------------------------------------------
void UsbHostInfoSet::ClearHusb(){
	for(size_t i=0; i<usbHubs.size(); i++){
		SAFE_DELETE((USB_Hub*)usbHubs[i]);
	}

	usbHubs.clear();
}
//---------------------------------------------------------------------------
bool UsbHostInfoSet::EnumUsbHubTree(tinyxml2::XMLDocument *doc){
	bool bl;
	USB_Host *pUsbHost;
	SP_DEVINFO_DATA spDevinfoData;
	XMLElement *host;
	tstring displayName, devicePath;
	wstring wDisplayName, wDevicePath;

	for(int i=0; ; i++){
		this->ClearHusb();

		ZeroMemory(&spDevinfoData, sizeof(SP_DEVINFO_DATA));
		spDevinfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		if(!SetupDiEnumDeviceInfo(this->hdevinfo, i, &spDevinfoData)){
			if(GetLastError() == ERROR_NO_MORE_ITEMS){
				bl = true;
			}else{
				bl = false;
			}

			break;
		}

		bl = ConstructDisplayName(displayName, &spDevinfoData);

		if(!bl){
        	break;
		}

//		Ws2Cs cs(wstr);

		host = doc->NewElement("HOST");

		if(displayName != _T("")){
			host->LinkEndChild(doc->NewText(displayName.c_str()));
		}else{
			host->LinkEndChild(doc->NewText(CHAR_UNKNOW));
		}

		doc->RootElement()->LinkEndChild(host);

		bl = GetDevicePath(devicePath, &spDevinfoData);

		if(!bl){
        	break;
		}

		wDisplayName = StringToWstring(displayName);
		wDevicePath = StringToWstring(devicePath);

		pUsbHost = new USB_Host(wDevicePath, wDisplayName, NULL, 0);

		if(pUsbHost == NULL){
			bl = false;
			break;
		}

		USB_Hub *pRootHub = pUsbHost->GetRootHub();

		if(pRootHub == NULL){
        	bl = false;
			break;
		}

//		while(pRootHub){
			ProcessTreeItem(pRootHub, host);
//			pRootHub = pRootHub->NextSibling();
//		}

		ClearHusb();
		SAFE_DELETE(pUsbHost)
	}

	return bl;
}
//---------------------------------------------------------------------------
bool UsbHostInfoSet::ProcessTreeItem(USB_Hub *pHub, XMLElement *pNode){
	wstring ws;
	string cs;

	XMLElement *pNewNode = pNode->GetDocument()->NewElement("HUB");

	ws = pHub->GetDisplayName();
	cs = WstringToString(ws);

	pNewNode->LinkEndChild(pNode->GetDocument()->NewText(cs.c_str()));
	pNode->LinkEndChild(pNewNode);


	//¤l¸`ÂI
	if(!pHub->FindFirstUsbHubChild(&pHub)){
		return false;
	}

	while(pHub){
		usbHubs.push_back((void*)pHub);

		if(!ProcessTreeItem(pHub, pNewNode)){
			return false;
		}

		if(!pHub->FindNextUsbHubSibling(&pHub)){
			return false;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool UsbHostInfoSet::GetUsbHubTree(tinyxml2::XMLDocument *doc){
	bool bl;

	if(this->Open()){
		bl = EnumUsbHubTree(doc);
		this->Close();
	}else{
    	bl = false;
	}

	return bl;
}
//---------------------------------------------------------------------------
