// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef UsbHostInfoSetH
#define UsbHostInfoSetH
//---------------------------------------------------------------------------
#include "DevInfoSet.h"
#include "USB_Host.h"
#include "tinyxml2.h"
using namespace tinyxml2;
//---------------------------------------------------------------------------
class UsbHostInfoSet : /*public*/ DevInfoSet
{
private:
	vector<void*> usbHubs;
private:
	bool EnumUsbHubTree(tinyxml2::XMLDocument *doc);
	bool ProcessTreeItem(USB_Hub *pHub, XMLElement *pNode);
	void ClearHusb();
public:
	bool GetUsbHubTree(tinyxml2::XMLDocument *doc);
public:
	UsbHostInfoSet(GUID *pClassGuid, const PCTSTR enumerator = NULL, HWND hwnd = NULL);
	~UsbHostInfoSet();
};
//---------------------------------------------------------------------------
#endif
