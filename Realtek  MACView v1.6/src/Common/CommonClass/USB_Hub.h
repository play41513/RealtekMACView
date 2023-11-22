// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef USB_HubH
#define USB_HubH
//---------------------------------------------------------------------------
//#include <windows.h>
//#include "PreDefine.h"
#include "USB_Device.h"
//---------------------------------------------------------------------------
//#ifdef __cplusplus
//extern "C" {
//#endif
//#include <setupapi.h> //HDEVINFO
//#ifdef __cplusplus                                                                               d
//}
//#endif
//---------------------------------------------------------------------------
class USB_Hub : public USB_Device{
private:
	UCHAR  ucDownstreamPorts;
public:
	virtual bool Initialize();
	wstring IoctlGetNodeConnectionName(unsigned long index);
//	USB GetFirstChild();
	bool FindFirstUsbHubChild(USB_Hub **ppUsbHub);
	bool FindNextUsbHubChild(UCHAR startPort, USB_Hub **ppUsbHub);
	bool FindNextUsbHubSibling(USB_Hub **ppUsbHub);
public:
	USB_Hub(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort);
	~USB_Hub();
};
//---------------------------------------------------------------------------
#endif
