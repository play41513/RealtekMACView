// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef USB_Composite_DeviceH
#define USB_Composite_DeviceH
#include "USB_Device.h"
//---------------------------------------------------------------------------
class USB_Composite_Device : public USB_Device
{
protected:
public:
public:
	USB_Composite_Device(tstring hwid);
	~USB_Composite_Device();
};
//---------------------------------------------------------------------------
#endif
