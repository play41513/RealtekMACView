// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef DeviceNotificationH
#define DeviceNotificationH

#include <dbt.h>

#ifdef __cplusplus
	extern "C" {
#endif
#include <initguid.h>
#include "usbiodef.h"  //需先#include <initguid.h>
//#include <devguid.h>
//#include <regstr.h>
//#include "usbioctl.h"
#ifdef __cplusplus                                                                               d
	}
#endif
//---------------------------------------------------------------------------
class DeviceNotification
{
private:
	HDEVNOTIFY hdevnotify;
private:
	inline void Unregister(){
		if(hdevnotify){UnregisterDeviceNotification(hdevnotify);hdevnotify = NULL;}
	}

	bool Register(HWND hwnd, GUID guid, DWORD flags){
		// 註冊要取得的裝置消息
		// #include <dbt.h>
		DEV_BROADCAST_DEVICEINTERFACE notification_Filter;

		ZeroMemory(&notification_Filter, sizeof(notification_Filter));
		notification_Filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		notification_Filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

		// 註冊獲取裝置訊息
		notification_Filter.dbcc_classguid = guid;

		hdevnotify = RegisterDeviceNotification(
			hwnd, &notification_Filter, flags);

		return(hdevnotify != NULL);
	}
public:
	bool RegisterWindowsUsbDeviceNotification(HWND hwnd){
		return Register(hwnd, GUID_DEVINTERFACE_USB_DEVICE, DEVICE_NOTIFY_WINDOW_HANDLE);
	}

	bool RegisterWindowsUsbHubNotification(HWND hwnd){
		return Register(hwnd, GUID_DEVINTERFACE_USB_HUB, DEVICE_NOTIFY_WINDOW_HANDLE);
	}

	bool RegisterWindowsDeviceInterfaceNotification(HWND hwnd, GUID guid){
		return Register(hwnd, guid, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
public:
	DeviceNotification():hdevnotify(NULL){}
	~DeviceNotification(){Unregister();}
};
//---------------------------------------------------------------------------
#endif
