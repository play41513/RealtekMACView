// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop

#include "Device.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
Device::Device(wstring hwid)
:hwid(hwid)
,devicePath(L"")
,displayName(L"")
,pUpstreamDevice(NULL)
,ulUpstreamPort(0)
,handle(INVALID_HANDLE_VALUE)
{}
//---------------------------------------------------------------------------
Device::Device(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort)
:hwid(L"")
,devicePath(devicePath)
,displayName(displayName)
,pUpstreamDevice(pUpstreamDevice)
,ulUpstreamPort(ulUpstreamPort)
,handle(INVALID_HANDLE_VALUE)
{}
//---------------------------------------------------------------------------
Device::~Device(){
	Close();
}
//---------------------------------------------------------------------------
void Device::Close(){
	SAFE_CLOSE_INVALID_HANDLE(this->handle);
}
//---------------------------------------------------------------------------
bool Device::Open(){
	this->handle = CreateFileW(
		this->devicePath.c_str(),
		GENERIC_WRITE, FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		0,
		0);

	return (this->handle != INVALID_HANDLE_VALUE);
}
//---------------------------------------------------------------------------
