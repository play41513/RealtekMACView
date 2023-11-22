// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------
#ifndef USB_DeviceH
#define USB_DeviceH
//---------------------------------------------------------------------------
//#include <windows.h>
#include "Device.h"
#include <tchar.h>

//#include <string>
//#ifndef TSTRING
//typedef std::basic_string<TCHAR> tstring;
////typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
//#endif

#include <vector>
using namespace std;

#ifdef __cplusplus
	extern "C" {
#endif
#include <initguid.h>
#include "usbiodef.h"  //»Ý¥ý#include <initguid.h>
//#include <devguid.h>
//#include <regstr.h>
#include "usbioctl.h"
#ifdef __cplusplus                                                                               d
	}
#endif
//---------------------------------------------------------------------------
class USB_Device : public Device
{
protected:
	USB_NODE_INFORMATION *pUsbNodeInformation;
public:
	virtual bool Initialize();
	bool IOCTL_GET_NODE_INFORMATION();
public:
//	USB_Device(wstring hwid);
	USB_Device(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort);
	~USB_Device();
};
// ---------------------------------------------------------------------------
#endif
