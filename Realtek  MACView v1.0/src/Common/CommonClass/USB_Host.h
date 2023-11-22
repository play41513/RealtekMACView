// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef USB_HostH
#define USB_HostH
//---------------------------------------------------------------------------
#include "USB_Device.h"
#include "USB_Hub.h"
//---------------------------------------------------------------------------
class USB_Host : public USB_Device
{
private:
	USB_Hub *pRootHub;
private:
	bool IOCTL_GET_ROOT_HUB_NAME(wstring &rootHubName);
public:
	USB_Hub *GetRootHub();

//	unsigned long Create(HDEVINFO hdevinfo, int index);
public:
	USB_Host(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort);
	~USB_Host();
};
//---------------------------------------------------------------------------
#endif
