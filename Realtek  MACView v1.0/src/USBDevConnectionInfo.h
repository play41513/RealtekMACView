//---------------------------------------------------------------------------
#include <StrUtils.hpp>
#include "winioctl.h"
#include "usbioctl.h"
#include "malloc.h"
//#include "cfgmgr32.h"
#include "vndrlist.h"
#include "stdio.h"
#include "SETUPAPI.H"
#ifndef USBDevConnectionInfoH
#define USBDevConnectionInfoH
//---------------------------------------------------------------------------
class USBDevConnectionInfo
{
public:
	bool GetDevUSB(AnsiString VID,AnsiString PID);
	bool RestartDev(AnsiString VID,AnsiString PID);
	bool USBDevConnectionInfo::DevNameGetVPID(AnsiString VID);
private:

	SECURITY_ATTRIBUTES     SA;         // Needed for Win2000
	TStringList *TListUSBDEV_CONN_INFO;
	//------------------------------------------------------------------------
	//USB Device Information Array
	struct _USB_Device_Information
	{
	  AnsiString deviceDesc;
	  AnsiString localInfo;
	  AnsiString DeviceID;
	  AnsiString devicePath;
	  AnsiString deviceSN;
	}USB_Device_Information;
	//------------------------------------------------------------

	//Define all stuctures using UCHAR or BOOLEAN so that
	//the variables are not 'aligned' by the compiler
	typedef struct _HUB_DESCRIPTOR
	{
	  UCHAR bDescriptorLength;
	  UCHAR bDescriptorType;
	  UCHAR bNumberOfPorts;
	  UCHAR wHubCharacteristics[2];
	  UCHAR bPowerOnToPowerGood;
	  UCHAR bHubControlCurrent;
	  UCHAR bRemoveAndPowerMask[64];
	}HUB_DESCRIPTOR, *PHUB_DESCRIPTOR;

	typedef struct _USBDEVICEINFO
	{
	  PUSB_NODE_INFORMATION               HubInfo;        // NULL if not a HUB
	  PCHAR                               HubName;        // NULL if not a HUB
	  PUSB_NODE_CONNECTION_INFORMATION    ConnectionInfo; // NULL if root HUB
	  PUSB_DESCRIPTOR_REQUEST             ConfigDesc;     // NULL if root HUB
	  //PSTRING_DESCRIPTOR_NODE             StringDescs;
	} USBDEVICEINFO, *PUSBDEVICEINFO;

	typedef struct _DEVICE_DESCRIPTOR
	{
	  UCHAR bLength;
	  UCHAR bDescriptorType;
	  UCHAR bcdUSB[2];
	  UCHAR bDeviceClass;
	  UCHAR bDeviceSubClass;
	  UCHAR bDeviceProtocol;
	  UCHAR bMaxPacketSize0;
	  UCHAR idVendor[2];
	  UCHAR idProduct[2];
	  UCHAR bcdDevice[2];
	  UCHAR iManufacturer;
	  UCHAR iProduct;
	  UCHAR iSerialNumber;
	  UCHAR bNumConfigurations;
	}DEVICE_DESCRIPTOR;

	typedef struct _NODE_INFORMATION
	{
	  USB_HUB_NODE                        NodeType;
	  HUB_DESCRIPTOR                      HubDescriptor;
	  BOOLEAN                             HubIsBusPowered;
	}NODE_INFORMATION, *PNODE_INFORMATION;

	typedef struct _NODE_CONNECTION_INFORMATION
	{
	  ULONG ConnectionIndex;
	  DEVICE_DESCRIPTOR DeviceDescriptor;
	  UCHAR CurrentConfigurationValue;
	  bool LowSpeed;
	  bool DeviceIsHub;
	  UCHAR DeviceAddress[2];
	  UCHAR NumberOfOpenPipes[4];
	  UCHAR ConnectionStatus[4];
	  USB_PIPE_INFO PipeList[32];
	}NODE_CONNECTION_INFORMATION;

	struct
	{
	  ULONG ConnectionIndex;
	  ULONG ActualLength;
	  WCHAR Name[256];
	}ConnectedHub;

	struct
	{
	  ULONG Length;
	  WCHAR Name[256];
	} UnicodeName;
	DWORD EnumerateHostController(HANDLE HostControllerHandle);
	PCHAR WideStrToMultiStr (PWCHAR WideStr);
	PCHAR GetDriverKeyName (HANDLE Hub, ULONG ConnectionIndex);
	USHORT DisplayDeviceDescriptor (USHORT LanguageID, PUCHAR BufferPtr);
	AnsiString LookingString(AnsiString Sour, AnsiString Dest);
	void GetPortData(HANDLE HubHandle, UCHAR PortCount, int HubDepth);
	void DriverNameToDeviceDescInfo(PCHAR DriverName);
	void DisplayPipeInfo(ULONG NumPipes, USB_PIPE_INFO *PipeInfo);
	void DisplayEndpointDescriptor(PUSB_ENDPOINT_DESCRIPTOR EndpointDesc);
	bool ControlDevice(DWORD dwDevID, HDEVINFO hDevInfo);
	bool StateChange( DWORD dwNewState, DWORD dwDevID, HDEVINFO hDevInfo);
	bool CheckReltekName(AnsiString USBPath);
};
#endif
