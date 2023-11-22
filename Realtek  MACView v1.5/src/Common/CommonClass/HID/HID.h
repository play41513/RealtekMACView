// Module: Hid.h
// Notices: Copyright(c) 2014 Robbin Hsiao.
// Purpose: USB HID 物件控制
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.0.1
//---------------------------------------------------------------------------

#ifndef HIDH
#define HIDH
//---------------------------------------------------------------------------
#define MAX_DEVICE_PATH 256
//---------------------------------------------------------------------------
#include <initguid.h>	//定義 GUID 結構及方法 包含#include <guiddef.h>
#include "windows.h"
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"{
#endif
#include "hidsdi.h"  //HidD_GetHidGuid(&HidGuid);
#include "setupapi.h"
//#include "hidusage.h"
#include "Hidclass.h" //GUID_DEVINTERFACE_HID
#ifdef __cplusplus
}
#endif

#include "AsyncIO.h"
//---------------------------------------------------------------------------
namespace DeviceStatus{enum DeviceStatus{UNKNOW, UNMOUNT, MOUNT, OPEN, TURN_OFF, TURN_ON};}
//---------------------------------------------------------------------------
class HID{
private:
	unsigned long m_nRxByte, m_nTxByte;
	HANDLE m_hRead;
	HANDLE m_hWrite;
	HANDLE *m_handle[2];
	TCHAR m_devicePath[MAX_DEVICE_PATH];
protected:
	DeviceStatus::DeviceStatus m_status;
protected:
	bool Open(DWORD dwFlagsAndAttributes);
	void ReleaseHandle(HANDLE &handle);
	bool CreateHandle(HANDLE &handle, TCHAR *path, DWORD dwFlagsAndAttributes);
	bool GetReportSize();
public:
	bool Open();
	bool OpenAsyncHid();
	bool OpenSyncHid();
	void Close();
	bool GetInputReport(AsyncIO *paio, int waitTime, PLARGE_INTEGER pliOffset = NULL);
	bool SetOutputReport(const void *outputReport, AsyncIO *paio, int waitTime, PLARGE_INTEGER pliOffset = NULL);
//	bool GetFeatureReport();
//	bool SetFeatureReport();
public:
	DeviceStatus::DeviceStatus GetStatus();
	TCHAR * GetDevicePath();
	unsigned long GetRxReportSize();
	unsigned long GetTxReportSize();
	HID(TCHAR *devicePath);
	~HID();
};
#endif
