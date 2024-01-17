//---------------------------------------------------------------------------
#include <windows.h>    // �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <SetupAPI.h> // �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <cfgmgr32.h>
#pragma hdrstop

#include "Uint1.h"
#include "USBDevConnectionInfo.h"
#define BUFSIZE 2048   //For buf buffer size
//---------------------------------------------------------------------------

#pragma package(smart_init)
const char ConnectionStatus[] [30] = {
"No device connected", "Device connected", "Device FAILED enumeration",
"Device general FAILURE", "Device caused overcurrent", "Not enough power for device"
};
bool USBDevConnectionInfo::GetDevUSB(AnsiString VID,AnsiString PID)
{
	TListUSBDEV_CONN_INFO = new TStringList;
	HANDLE HostControllerHandle;
	char HostControllerName[] = "\\\\.\\HCD0";
	PCHAR driverKeyName, deviceDesc;
	AnsiString str1;   //Tree node name
	USBHIDForm->memoMsg->Lines->Add(VID+"&"+PID);

	SA.nLength = sizeof(SECURITY_ATTRIBUTES);
	SA.lpSecurityDescriptor = NULL;
	SA.bInheritHandle = false;
	bool bhave = false;

	USB_Device_Information.devicePath=NULL;    //Device Path Clean at begining

	for (int i=0; i<10; i++)
	{
		HostControllerName[7] = i + '0';
		HostControllerHandle = CreateFile(HostControllerName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);

		if (HostControllerHandle != INVALID_HANDLE_VALUE)
		{
			EnumerateHostController(HostControllerHandle);
		}
		CloseHandle(HostControllerHandle);
	}
	AnsiString a,b,c,d;
	for(int j=0;j<TListUSBDEV_CONN_INFO->Count;j++)
	{
		a = TListUSBDEV_CONN_INFO->Strings[j];
		if (TListUSBDEV_CONN_INFO->Strings[j].Pos("VID :"+VID))//�T�{�˸mVID
		{
			USBHIDForm->memoMsg->Lines->Add(TListUSBDEV_CONN_INFO->Strings[j]);
			j++;
			b = TListUSBDEV_CONN_INFO->Strings[j];
			if (TListUSBDEV_CONN_INFO->Strings[j].Pos("PID :"+PID))//�T�{�˸mPID
			{
				USBHIDForm->memoMsg->Lines->Add(TListUSBDEV_CONN_INFO->Strings[j]);
				while(j+1!=TListUSBDEV_CONN_INFO->Count)
				{
					j++;
					USBHIDForm->memoMsg->Lines->Add(TListUSBDEV_CONN_INFO->Strings[j]);
					if(TListUSBDEV_CONN_INFO->Strings[j].Pos("wMaxPacketSize"))
					{
						if(TListUSBDEV_CONN_INFO->Strings[j].Pos("400"))
						{
							delete TListUSBDEV_CONN_INFO;
							return true;
						}
						else if(TListUSBDEV_CONN_INFO->Strings[j].Pos("200"))
						{
							delete TListUSBDEV_CONN_INFO;
							return false;
						}
					}
					if(TListUSBDEV_CONN_INFO->Strings[j].Pos("====="))
						break;
				}
			}
		}
	}
	USBHIDForm->memoMsg->Lines->Add(TListUSBDEV_CONN_INFO->Text);
	delete TListUSBDEV_CONN_INFO;
	return false;
}
DWORD USBDevConnectionInfo::EnumerateHostController(HANDLE HostControllerHandle)
{
  DWORD BytesReturned;
  bool Success;
  char RootHubName[256] = "\\\\.\\";
  HANDLE RootHubHandle;
  PUSBDEVICEINFO  info;

  info = NULL;
  info = (PUSBDEVICEINFO) alloca(sizeof(USBDEVICEINFO));
  info->HubInfo = (PUSB_NODE_INFORMATION)alloca(sizeof(USB_NODE_INFORMATION));


  // First get the system name of the host controller for display
  Success = DeviceIoControl(HostControllerHandle, IOCTL_GET_HCD_DRIVERKEY_NAME,
		   &UnicodeName,sizeof(UnicodeName),&UnicodeName, sizeof(UnicodeName), &BytesReturned, NULL);

  if(Success)
  {
    // Now get the system name of it's root hub for interrogation
    Success = DeviceIoControl(HostControllerHandle, IOCTL_USB_GET_ROOT_HUB_NAME, &UnicodeName,
            sizeof(UnicodeName),&UnicodeName, sizeof(UnicodeName), &BytesReturned, NULL);

	if(Success)
	{
      // Now open the root hub. Need to construct a char name from "\\.\" + UnicodeName
      WideCharToMultiByte(CP_ACP, 0, &UnicodeName.Name[0], (UnicodeName.Length)/2, &RootHubName[4], 252, NULL,NULL);

	  RootHubHandle = CreateFile(RootHubName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);
	  if(RootHubHandle != INVALID_HANDLE_VALUE)
	  {
        // Root hub is open. Collect the node information
        Success = DeviceIoControl(RootHubHandle, IOCTL_USB_GET_NODE_INFORMATION, info->HubInfo,
                  sizeof(USB_NODE_INFORMATION), info->HubInfo, sizeof(USB_NODE_INFORMATION), &BytesReturned, NULL);

		if (!Success)
          return GetLastError();
        else
		{
		  // Get the ports and take the data
		  GetPortData(RootHubHandle, info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts, 0);
        }
      }
	  CloseHandle(RootHubHandle);
    }
  }
  return 0;
}
//---------------------------------------------------------------------------
void USBDevConnectionInfo::GetPortData(HANDLE HubHandle, UCHAR PortCount, int HubDepth)
{
  DWORD BytesReturned;
  bool Success;
  int i;
  ULONG PortIndex;
  USHORT LanguageID;
  UCHAR PortStatus;
  char ConnectedHubName[256] = "\\\\.\\";
  HANDLE ConnectedHubHandle;
  NODE_INFORMATION NodeInformation;
  NODE_CONNECTION_INFORMATION ConnectionInformation;
  AnsiString str1;
  PCHAR driverKeyName, deviceDesc;
  PUSBDEVICEINFO  info;

  info = NULL;
  info = (PUSBDEVICEINFO) alloca(sizeof(USBDEVICEINFO));
  info->HubInfo = (PUSB_NODE_INFORMATION)alloca(sizeof(USB_NODE_INFORMATION));

  // Iterate over the ports to discover what is connected to each one
  for (PortIndex = 1; PortIndex < (ULONG)PortCount + 1; PortIndex++)
  {
	LanguageID = 0; // Reset for each port
	ConnectionInformation.ConnectionIndex = PortIndex;

    //Get Connection's Node Informations
	Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION, &ConnectionInformation,
         sizeof(ConnectionInformation), &ConnectionInformation, sizeof(ConnectionInformation), &BytesReturned, NULL);

	PortStatus = ConnectionInformation.ConnectionStatus[0]; // Save some typing!

	str1= "";

	// Now the connection specific information
	if (PortStatus != DeviceConnected)
    {
	  str1=str1 + ConnectionStatus[PortStatus];
	}
	else
	{
		TListUSBDEV_CONN_INFO->Add("======================================================");
	  //Have a device or a hub connected to this port
	  if (!ConnectionInformation.DeviceIsHub)
	  {
		USB_Device_Information.devicePath="";

        // There is an I/O device connected. Print out it's descriptors
        // Note that many current devices do not respond correctly if ConfigID != 0.
        // So only request the first configuration
        driverKeyName = GetDriverKeyName(HubHandle, PortIndex);
        if (driverKeyName)
        {
		   DriverNameToDeviceDescInfo(driverKeyName);

		   if (USB_Device_Information.deviceDesc != NULL)
           {
             str1=str1 + USB_Device_Information.deviceDesc +
				 " (" + LookingString(USB_Device_Information.DeviceID,"\\") + ")";
           }
		}
		//Add USB device to TListUSBDEV_CONN_INFO
		TListUSBDEV_CONN_INFO->Add(str1);

		LanguageID = DisplayDeviceDescriptor(LanguageID,
					 &ConnectionInformation.DeviceDescriptor.bLength);
		if(*ConnectionInformation.NumberOfOpenPipes)
		  DisplayPipeInfo(*ConnectionInformation.NumberOfOpenPipes, ConnectionInformation.PipeList);
	  }
	  else
      {
		//Have no a device or a hub connected to this port
		// There is a hub connected and we need to iterate over it's ports
		str1=str1 + "Hub connected : ";

        // Get the system name of the connected hub so that we can make a connection to it
		ConnectedHub.ConnectionIndex = PortIndex;

        driverKeyName = GetDriverKeyName(HubHandle, PortIndex);
        if (driverKeyName)
        {
           DriverNameToDeviceDescInfo(driverKeyName);

           if (USB_Device_Information.deviceDesc != NULL)
			 str1=str1 + USB_Device_Information.deviceDesc +
				 " (" + LookingString(USB_Device_Information.DeviceID,"\\") + ")";
		}
        //try to connect nest node
		Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_NAME, &ConnectedHub,
                sizeof(ConnectedHub), &ConnectedHub, sizeof(ConnectedHub), &BytesReturned, NULL);

        if (Success)
        {
		  TListUSBDEV_CONN_INFO->Add(str1);

          WideCharToMultiByte(CP_ACP, 0, &ConnectedHub.Name[0], (ConnectedHub.ActualLength)/2, &ConnectedHubName[4], 252, NULL, NULL);

          ConnectedHubHandle = CreateFile(ConnectedHubName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);

          if(ConnectedHubHandle != INVALID_HANDLE_VALUE)
		  {
            // Root hub is open. Collect the node information
			Success = DeviceIoControl(ConnectedHubHandle, IOCTL_USB_GET_NODE_INFORMATION, info->HubInfo,
                    sizeof(USB_NODE_INFORMATION), info->HubInfo, sizeof(USB_NODE_INFORMATION), &BytesReturned, NULL);

            if (Success)
			{
			  // Get the ports and take the data
			  GetPortData(ConnectedHubHandle, info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts, HubDepth+1);
            }
			else
			{
			  TListUSBDEV_CONN_INFO->Add(str1);
            }
          }
		  CloseHandle(ConnectedHubHandle);
		}
      };
    };
  };
}
//---------------------------------------------------------------------------
USHORT USBDevConnectionInfo::DisplayDeviceDescriptor (USHORT LanguageID, PUCHAR BufferPtr)
{
  AnsiString strVendor,str;
  USHORT tmp;
  UCHAR LowByte;
  char *buf;

  TListUSBDEV_CONN_INFO->Add("------------------------------------------");
  TListUSBDEV_CONN_INFO->Add("Device Descriptor");

  BufferPtr--; // Backup pointer to prepare for pre-increment
  for(int i =0;i<8;i++)
	*++BufferPtr;
  LowByte = *++BufferPtr;

  tmp=LowByte + (*++BufferPtr << 8);
  str=IntToHex(tmp,4);

  TListUSBDEV_CONN_INFO->Add("VID :" + str);

  LowByte = *++BufferPtr;
  TListUSBDEV_CONN_INFO->Add("PID :" + IntToHex(LowByte + (*++BufferPtr << 8),4) + "  (" + USB_Device_Information.localInfo + ")");

  LowByte = *++BufferPtr;
  TListUSBDEV_CONN_INFO->Add("REV :" + IntToHex(LowByte + (*++BufferPtr << 8),4));

  return LanguageID;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void USBDevConnectionInfo::DriverNameToDeviceDescInfo(PCHAR DriverName)
{
	CHAR buf[BUFSIZE]; // XXXXX How big does this have to be? Dynamically size it?

	DEVINST     devInst;
	DEVINSTID   pDeviceID;
    DEVINST     devInstNext;
    CONFIGRET   cr;
	ULONG       walkDone = 0;
	ULONG       len;
    GUID        GuidPtr;

	//Symbolic Name for all USB device interface
    GuidPtr.Data1=2782707472;
    GuidPtr.Data2=25904;
    GuidPtr.Data3 = 4562;
    GuidPtr.Data4[0] = 144;
    GuidPtr.Data4[1] = 31;
    GuidPtr.Data4[2] = 0;
    GuidPtr.Data4[3] = 192;
    GuidPtr.Data4[4] = 79;
    GuidPtr.Data4[5] = 185;
	GuidPtr.Data4[6] = 81;
    GuidPtr.Data4[7] = 237;

    USB_Device_Information.deviceDesc=NULL;
    USB_Device_Information.localInfo=NULL;
    USB_Device_Information.DeviceID=NULL;

    // Get Root DevNode
    cr = CM_Locate_DevNode(&devInst, NULL, 0);

    if (cr != CR_SUCCESS)
      USB_Device_Information.deviceDesc=NULL;

    // Do a depth first search for the DevNode with a matching
    // DriverName value
    while (!walkDone)
    {
        // Get the DriverName value
        len = sizeof(buf);
        cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DRIVER, NULL, buf, &len, 0);

        // If the DriverName value matches, return the DeviceDescription
        if (cr == CR_SUCCESS && strcmp(DriverName, buf) == 0)
        {
            //Location Information
            len = sizeof(buf);
            cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_LOCATION_INFORMATION, NULL, buf, &len, 0);

            if (cr == CR_SUCCESS)
            {
			  USB_Device_Information.localInfo=buf;
            }

            //Device Decsription
            len = sizeof(buf);
            cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DEVICEDESC, NULL, buf, &len, 0);

            if (cr == CR_SUCCESS)
              USB_Device_Information.deviceDesc=buf;

            //Device ID
            len = sizeof(buf);
            cr= CM_Get_Device_ID(devInst, buf, len, 0);

            if (cr == CR_SUCCESS)
            {
              USB_Device_Information.DeviceID=buf;
              pDeviceID=buf;
            }

            //Device Interface Path
            len = sizeof(buf);
            cr =  CM_Get_Device_Interface_List(&GuidPtr, pDeviceID, buf, len, 0);

            if (cr == CR_SUCCESS)
              USB_Device_Information.devicePath=buf;

        }

        // This DevNode didn't match, go down a level to the first child.
        cr = CM_Get_Child(&devInstNext, devInst, 0);

        if (cr == CR_SUCCESS)
        {
          devInst = devInstNext;
          continue;
        }

        // Can't go down any further, go across to the next sibling.  If
        // there are no more siblings, go back up until there is a sibling.
        // If we can't go up any further, we're back at the root and we're
        // done.
        for (;;)
        {
            cr = CM_Get_Sibling(&devInstNext, devInst, 0);

            if (cr == CR_SUCCESS)
            {
              devInst = devInstNext;
              break;
            }

            cr = CM_Get_Parent(&devInstNext, devInst, 0);


            if (cr == CR_SUCCESS)
                devInst = devInstNext;
            else
            {
                walkDone = 1;
                break;
            }
        }
    }

}
//---------------------------------------------------------------------------
PCHAR USBDevConnectionInfo::WideStrToMultiStr (PWCHAR WideStr)
{
    ULONG nBytes;
    PCHAR MultiStr;

    // Get the length of the converted string
    //
    nBytes = WideCharToMultiByte(CP_ACP, 0, WideStr, -1, NULL, 0, NULL, NULL);

    if (nBytes == 0)
      return NULL;

    // Allocate space to hold the converted string
    MultiStr =(PCHAR )alloca(nBytes);

    if (MultiStr == NULL)
      return NULL;

    // Convert the string
    //
    nBytes = WideCharToMultiByte(CP_ACP, 0, WideStr, -1, MultiStr, nBytes, NULL, NULL);

    if (nBytes == 0)
    {
        delete(MultiStr);
        return NULL;
    }

    return MultiStr;
}
//---------------------------------------------------------------------------
PCHAR USBDevConnectionInfo::GetDriverKeyName (HANDLE Hub, ULONG ConnectionIndex)
{
    BOOL                                success;
    ULONG                               nBytes;
	USB_NODE_CONNECTION_DRIVERKEY_NAME  driverKeyName;
	PUSB_NODE_CONNECTION_DRIVERKEY_NAME driverKeyNameW;

	driverKeyNameW = NULL;

    // Get the length of the name of the driver key of the device attached to
    // the specified port.
    driverKeyName.ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub, IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME, &driverKeyName,
         sizeof(driverKeyName), &driverKeyName, sizeof(driverKeyName), &nBytes, NULL);

    if (!success)
      return NULL;

    // Allocate space to hold the driver key name
	nBytes = driverKeyName.ActualLength;

    if (nBytes <= sizeof(driverKeyName))
      return NULL;

	driverKeyNameW = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME) alloca(nBytes);

    if (driverKeyNameW == NULL)
      return NULL;
    // Get the name of the driver key of the device attached to
	// the specified port.

	driverKeyNameW->ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub, IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
              driverKeyNameW, nBytes, driverKeyNameW, nBytes, &nBytes, NULL);

    if (success)
	{
	  // All done, free the uncoverted driver key name and return the
	  // converted driver key name
      return WideStrToMultiStr(driverKeyNameW->DriverKeyName);
    }
    else
      return NULL;
}
//---------------------------------------------------------------------------
void USBDevConnectionInfo::DisplayPipeInfo(ULONG NumPipes, USB_PIPE_INFO *PipeInfo)
{
	DWORD i;

	for (i=0; i < NumPipes; i++)
	{
	  TListUSBDEV_CONN_INFO->Add(" ");
	  TListUSBDEV_CONN_INFO->Add("Endpoint Descriptor: [" + AnsiString(i) + "]");
	  DisplayEndpointDescriptor(&PipeInfo[i].EndpointDescriptor);
	}
}
//---------------------------------------------------------------------------
void USBDevConnectionInfo::DisplayEndpointDescriptor(PUSB_ENDPOINT_DESCRIPTOR EndpointDesc)
{
	TListUSBDEV_CONN_INFO->Add(" wMaxPacketSize :0x" + IntToHex(EndpointDesc->wMaxPacketSize,4) +
						 "(" + IntToHex(EndpointDesc->wMaxPacketSize,4) + ")");
}
//---------------------------------------------------------------------------
AnsiString USBDevConnectionInfo::LookingString(AnsiString Sour, AnsiString Dest)
{
  for(int i=0; i < Sour.Length() - Dest.Length(); i++)
  {
	if(Sour.SubString(i+1, Dest.Length())==Dest)
	  Sour=MidStr(Sour,i+2,100);
  }
  return Sour;
}
bool USBDevConnectionInfo::RestartDev(AnsiString VID,AnsiString PID)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_USB_HID =StringToGUID("{A5DCBF10-6530-11D2-901F-00C04FB951ED}");
	//DEBUG("[ �˸m�C�| ]");
	//--------------------------------------------------------------------------
	//   ����]�Ƹ�T
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_USB_HID,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if   (hDevInfo   ==   INVALID_HANDLE_VALUE){
		//DEBUG("ERROR - SetupDiGetClassDevs()"); //   �d�߸�T����
	}
	else
	{
	//--------------------------------------------------------------------------
		SP_DEVICE_INTERFACE_DATA   ifdata;
		SP_DEVINFO_DATA            spdd;
		DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
		for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   �T�|�C��USB�]��
		{
			//�γ\�]�ƪ����A
			DWORD  Status, Problem;
			if (CM_Get_DevNode_Status(&Status, &Problem, DeviceInfoData.DevInst,0) != CR_SUCCESS)
			continue;
			// �]�ƦW
			//
			ULONG   len;
			CONFIGRET cr;
			PNP_VETO_TYPE   pnpvietotype;
			CHAR   vetoname[MAX_PATH];
			ULONG   ulStatus;
			ULONG   ulProblemNumber;
			ifdata.cbSize   =   sizeof(ifdata);
			if (SetupDiEnumDeviceInterfaces(								//   �T�|�ŦX��GUID���]�Ƥ���
			hDevInfo,           //   �]�Ƹ�T������X
			NULL,                         //   �����B�~���]�ƴy�z
			(LPGUID)&GUID_USB_HID,//GUID_CLASS_USB_DEVICE,                     //   GUID
			(ULONG)i,       //   �]�Ƹ�T���ت��]�ƧǸ�
			&ifdata))                 //   �]�Ƥ�����T
			{
				ULONG predictedLength   =   0;
				ULONG requiredLength   =   0;
				//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
				SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
					&ifdata,          //   �]�Ƥ�����T
					NULL,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
					0,         	      //   ��X�w�İϤj�p
					&requiredLength,  //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
					NULL);            //   �����B�~���]�ƴy�z
				//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
				predictedLength=requiredLength;
				pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
				pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				//
				ZeroMemory(&spdd, sizeof(spdd));
				spdd.cbSize = sizeof(spdd);

				if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
					&ifdata,             //   �]�Ƥ�����T
					pDetail,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
					predictedLength,     //   ��X�w�İϤj�p
					&requiredLength,     //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
					&spdd))               //   �����B�~���]�ƴy�z
				{
					try
					{
						char ch[512];
						for(j=0;j<predictedLength;j++)
							ch[j]=*(pDetail->DevicePath+8+j);
						SS=ch;
						//DEBUG(SS);
						if(SS.Pos("vid_"+VID.LowerCase())&&SS.Pos("pid_"+PID.LowerCase()))
						{
                            if(ControlDevice(i,hDevInfo))
							{
								//DEBUG("Successful\n");
							}
							else
							{
								GlobalFree(pDetail);
								SetupDiDestroyDeviceInfoList(hDevInfo);
								return false;
								//DEBUG("FAILED\n");
							}
                        }

					}
					catch(...)
					{
						//DEBUG(SS+"�C�|����");
					}

				}
				else
				{
					//DEBUG("SetupDiGetInterfaceDeviceDetail F");
				}
				GlobalFree(pDetail);
			}
			else
			{
				//DEBUG("SetupDiEnumDeviceInterfaces F");
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return false;
}
bool USBDevConnectionInfo::StateChange( DWORD dwNewState, DWORD dwDevID, HDEVINFO hDevInfo)
{
	SP_PROPCHANGE_PARAMS PropChangeParams;
    SP_DEVINFO_DATA        DevInfoData = {sizeof(SP_DEVINFO_DATA)};
	SP_DEVINSTALL_PARAMS devParams;

	//�d�߳]�ƫH��
	if (!SetupDiEnumDeviceInfo( hDevInfo, dwDevID, &DevInfoData))
    {
		//DEBUG("SetupDiEnumDeviceInfo FAILED");
		return FALSE;
	}

	//���ܳ]���ݩ��ܤưѼ�
	PropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	PropChangeParams.Scope = DICS_FLAG_GLOBAL; //�ϭק諸�ݩʫO�s�b���w���ݩʤ��
	PropChangeParams.StateChange = dwNewState;
	PropChangeParams.HwProfile = 0;


	//���ܳ]���ݩ�
	if (!SetupDiSetClassInstallParams( hDevInfo,
                                        &DevInfoData,
                                        (SP_CLASSINSTALL_HEADER *)&PropChangeParams,
                                        sizeof(PropChangeParams)))
	{
		//DEBUG("SetupDiSetClassInstallParams FAILED");
		return false;
    }


    PropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	PropChangeParams.Scope = DICS_FLAG_CONFIGSPECIFIC;//�ϭק諸�ݩʫO�s�b���w���ݩʤ��
	PropChangeParams.StateChange = dwNewState;
    PropChangeParams.HwProfile = 0;

	//���ܳ]���ݩʨýեΦw�˪A��
	if (!SetupDiSetClassInstallParams( hDevInfo,
									   &DevInfoData,
									   (SP_CLASSINSTALL_HEADER *)&PropChangeParams,
									   sizeof(PropChangeParams)))
	{
		//DEBUG(GetLastError());
		//DEBUG("SetupDiSetClassInstallParams or SetupDiCallClassInstaller FAILED");
		return false;
	}
	else
	{
		SetupDiChangeState(hDevInfo, &DevInfoData);
	}
	return true;
}
bool USBDevConnectionInfo::ControlDevice(DWORD dwDevID, HDEVINFO hDevInfo)
{
	if(!StateChange(DICS_DISABLE,dwDevID,hDevInfo))
		return false;
	if(!StateChange(DICS_ENABLE,dwDevID,hDevInfo))
		return false;
	return true;
}
bool USBDevConnectionInfo::DevNameGetVPID(AnsiString VID)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_USB_HID =StringToGUID("{A5DCBF10-6530-11D2-901F-00C04FB951ED}");
	//DEBUG("[ �˸m�C�| ]");
	//--------------------------------------------------------------------------
	//   ����]�Ƹ�T
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_USB_HID,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if   (hDevInfo   ==   INVALID_HANDLE_VALUE){
		//DEBUG("ERROR - SetupDiGetClassDevs()"); //   �d�߸�T����
	}
	else{
	//--------------------------------------------------------------------------
		SP_DEVICE_INTERFACE_DATA   ifdata;
		SP_DEVINFO_DATA            spdd;
		DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
		for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   �T�|�C��USB�]��
		{
			ifdata.cbSize   =   sizeof(ifdata);
			if (SetupDiEnumDeviceInterfaces(								//   �T�|�ŦX��GUID���]�Ƥ���
			hDevInfo,           //   �]�Ƹ�T������X
			NULL,                         //   �����B�~���]�ƴy�z
			(LPGUID)&GUID_USB_HID,//GUID_CLASS_USB_DEVICE,                     //   GUID
			(ULONG)i,       //   �]�Ƹ�T���ت��]�ƧǸ�
			&ifdata))                 //   �]�Ƥ�����T
			{
				ULONG predictedLength   =   0;
				ULONG requiredLength   =   0;
				//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
				SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
					&ifdata,          //   �]�Ƥ�����T
					NULL,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
					0,         	      //   ��X�w�İϤj�p
					&requiredLength,  //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
					NULL);            //   �����B�~���]�ƴy�z
				//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
				predictedLength=requiredLength;
				pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
				pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				//
				ZeroMemory(&spdd, sizeof(spdd));
				spdd.cbSize = sizeof(spdd);

				if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
					&ifdata,             //   �]�Ƥ�����T
					pDetail,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
					predictedLength,     //   ��X�w�İϤj�p
					&requiredLength,     //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
					&spdd))               //   �����B�~���]�ƴy�z
				{
					try
					{
						char ch[512];
						for(j=0;j<predictedLength;j++)
							ch[j]=*(pDetail->DevicePath+8+j);
						SS=ch;
						//DEBUG(SS);
						if(SS.Pos(VID))
						{
							if(SS.Pos("vid_0bda")||SS.Pos("vid_056e"))
							{
								SS = SS.SubString(1,SS.Pos("{")-2);
								SS = SS.SubString(1,17).UpperCase()+"\\"+SS.SubString(19,SS.Length()-18);
								if(CheckReltekName(SS))
								{
									USBHIDForm->g_DevVPID = SS.SubString(1,17);
									GlobalFree(pDetail);
									SetupDiDestroyDeviceInfoList(hDevInfo);
									return true;
								}
							}
						}

					}
					catch(...)
					{
						//DEBUG(SS+"�C�|����");
					}

				}
				else
				{
					//DEBUG("SetupDiGetInterfaceDeviceDetail F");
				}
				GlobalFree(pDetail);
			}
			else
			{
				//DEBUG("SetupDiEnumDeviceInterfaces F");
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return false;
}
bool USBDevConnectionInfo::CheckReltekName(AnsiString USBPath)
{
	AnsiString SS;
	bool bl = false;
	AnsiString DevInfo="";

	HKEY hSoftKey = NULL;

	hSoftKey = NULL;
	AnsiString temppath = "system\\CurrentControlSet\\Enum\\USB\\"+USBPath;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, temppath.c_str(), 0, KEY_READ,&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dwType=REG_SZ;
		DWORD dwCount=0;
		LONG lResult = RegQueryValueEx(hSoftKey, "FriendlyName", NULL, &dwType,NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			LPBYTE company_Get=new BYTE [dwCount];
			lResult = RegQueryValueEx(hSoftKey, "FriendlyName", NULL, &dwType,
			company_Get, &dwCount);
			if(lResult == ERROR_SUCCESS)
			{
				SS = AnsiString((char*)company_Get);
				if(SS.UpperCase().Pos("REALTEK USB") || SS.UpperCase().Pos("EDC-G01")
					|| SS.UpperCase().Pos("REALTEK GAMING USB"))
					bl = true;
			}

			delete company_Get;
		}
	}
	//
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	return bl;
}
