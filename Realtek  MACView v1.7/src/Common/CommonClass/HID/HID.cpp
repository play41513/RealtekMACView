// Module: Hid.cpp
// Notices: Copyright(c) 2014 Robbin Hsiao.
// Purpose: USB HID ª«¥ó±±¨î
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.0.1
//---------------------------------------------------------------------------
#pragma hdrstop

#include "HID.h"
#include <tchar.h>
#include <strsafe.h>
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TCHAR * HID::GetDevicePath(){
	return m_devicePath;
}

unsigned long HID::GetRxReportSize(){
	return m_nRxByte;
}

unsigned long HID::GetTxReportSize(){
    return m_nTxByte;
}

DeviceStatus::DeviceStatus HID::GetStatus(){
	return m_status;
}
bool HID::GetReportSize(){	bool bl = false;

	PHIDP_PREPARSED_DATA  pData;
	HIDP_CAPS caps;
	NTSTATUS nt;// = HIDP_STATUS_INVALID_PREPARSED_DATA;

	if(HidD_GetPreparsedData(m_hRead, &pData)){
		nt = HidP_GetCaps(pData, &caps);

		if(nt == HIDP_STATUS_SUCCESS){
			m_nRxByte = caps.InputReportByteLength;
			m_nTxByte = caps.OutputReportByteLength;

			bl = true;
		}

		HidD_FreePreparsedData(pData);
	}

	return bl;
}
bool HID::Open(){
	return OpenAsyncHid();
}

bool HID::GetInputReport(AsyncIO *paio, int waitTime, PLARGE_INTEGER pliOffset){
	return paio->Read(m_hRead, m_nRxByte, waitTime);
}

bool HID::SetOutputReport(const void *outputReport, AsyncIO *paio, int waitTime,  PLARGE_INTEGER pliOffset){
	return paio->Write(m_hWrite, m_nTxByte, outputReport, waitTime);
}

bool HID::Open(DWORD dwFlagsAndAttributes){
	bool bl;

	assert(m_devicePath);

	for(int i=0; i<2; i++){
		bl = CreateHandle(*m_handle[i], m_devicePath, dwFlagsAndAttributes);

		if(!bl){
			::MessageBox(NULL, _T("_Create the HID Handle fail!"), _T("HID"), MB_ICONSTOP);
			break;
		}
	}

	m_status = DeviceStatus::OPEN;

	if(!GetReportSize()){
		MessageBox(NULL, _T("Get HID report size fail!"), _T("ERROR"), MB_ICONWARNING);
		Close();
	}

	return bl;
}

void HID::Close(){
	for(int i=0; i<2; i++){
		ReleaseHandle(*m_handle[i]);
	}

	m_status = DeviceStatus::MOUNT;
}

void HID::ReleaseHandle(HANDLE &handle){
	if(handle != INVALID_HANDLE_VALUE){
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
}

bool HID::OpenSyncHid(){
	return Open(0);
}

bool HID::OpenAsyncHid(){
	return Open(FILE_FLAG_OVERLAPPED);
}

bool HID::CreateHandle(HANDLE &handle, TCHAR *path, DWORD dwFlagsAndAttributes){
	bool bl = false;

	try{
		handle = CreateFile(
			path,
			GENERIC_WRITE|GENERIC_READ,
			FILE_SHARE_WRITE|FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			dwFlagsAndAttributes,//FILE_FLAG_OVERLAPPED,
			NULL);

		if(INVALID_HANDLE_VALUE != handle){ // cannot open the drive
			bl = true;
		}
	}catch (...){
		ReleaseHandle(handle);
	}

	return bl;
}

HID::HID(TCHAR *devicePath){
	m_status = DeviceStatus::MOUNT;
	m_hRead = INVALID_HANDLE_VALUE;
	m_hWrite = INVALID_HANDLE_VALUE;
	m_handle[0] = &m_hRead;
	m_handle[1] = &m_hWrite;
	ZeroMemory(m_devicePath, MAX_DEVICE_PATH*sizeof(TCHAR));

	if(_tcscpy_s(m_devicePath, MAX_DEVICE_PATH, devicePath) != 0){
		MessageBox(NULL, _T("Copy device path fail!"), _T("ERROR"), MB_ICONWARNING);
	}
}

HID::~HID(){
	Close();
}
