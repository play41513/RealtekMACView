// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef DeviceH
#define DeviceH
//---------------------------------------------------------------------------
#include "PreDefine.h"
//---------------------------------------------------------------------------
class Device{
protected:
	wstring hwid; //Hardware ID
	wstring devicePath;
	wstring displayName;
	void *pUpstreamDevice;
	ULONG ulUpstreamPort;
	HANDLE handle;
public:
	wstring GetDevicePath(){return devicePath;}
	wstring GetDisplayName(){return displayName;}
	void Close();
	bool Open();
public:
	Device(wstring hwid);
	Device(wstring devicePath, wstring displayName, void *pUpstreamDevice, ULONG ulUpstreamPort);
	Device(){}
	~Device();
};
//---------------------------------------------------------------------------
#endif
