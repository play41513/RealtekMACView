// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop

#include "USB_Composite_Device.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

USB_Composite_Device::USB_Composite_Device(tstring hwid)
:USB_Device(hwid)
{}

USB_Composite_Device::~USB_Composite_Device(){}
