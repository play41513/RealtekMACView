// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------
#ifndef SetupDiH
#define SetupDiH
// ---------------------------------------------------------------------------
extern "C"{
#include <setupapi.h>
}

//©wÄ³tstring«¬ºA
#include <vector>
#ifndef TSTRING
typedef std::basic_string<TCHAR> tstring;
//typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
#endif
//#ifndef UNICODE
//  typedef std::string  tstring;
//#else
//  typedef std::wstring tstring;
//#endif
using namespace std;
//---------------------------------------------------------------------------
class SetupDi
{
private:
	HDEVINFO m_hDevInfo;
	const TCHAR* m_id;
	TCHAR *m_hardwareId;

	int m_level;
//	bool SetupDi::FindU3HostController();
	bool SetupDi::FindDevicesOnHubHandle(HANDLE hHub, unsigned short idVender, unsigned short idProduct, unsigned short idRev, TCHAR *pCompositeHardwareId, int iNumOfDevice, TCHAR *errBuffer, unsigned long errBufferSize);
	bool SetupDi::FindDevicesOnHubPort(HANDLE hub, int port, unsigned short idVender, unsigned short idProduct, unsigned short idRev, TCHAR *pCompositeHardwareId, int iNumOfDevice, TCHAR *errBuffer, unsigned long errBufferSize);
	HANDLE SetupDi::CreateHandle(const TCHAR *pDevicePath);
	TCHAR* SetupDi::GetDeviceRegistryProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA *pspDeviceInfoData, DWORD dwProperty);
protected:
		int SetupDi::GetHardwareId(int index);
public:
	SetupDi();
	~SetupDi();
	//////////////////////////////////////////////////////////////////////////////////////////
	bool Create(const GUID *pClassGuid, const TCHAR* pEnumerator, const unsigned long flag);
	bool Create();
	bool CreateHub();
	void Close();
	//////////////////////////////////////////////////////////////////////////////////////////
	bool OpenDeviceInfo(SP_DEVINFO_DATA &spDevinfoData);
	bool FindDevicesOnHubPath(const TCHAR *pHubDevicePath, unsigned short idVender, unsigned short idProduct, unsigned short idRev, TCHAR *pCompositeHardwareId, int iNumOfDevice, TCHAR *errBuffer, unsigned long errBufferSize);
	bool GetDevicePathByDeviceInfoData(const GUID *pInterfaceClassGuid, SP_DEVINFO_DATA &spDeviceInfoData, tstring &path);
	int FindDevicesPathByHardwareId(vector<tstring> &idList, vector<tstring> &pathList, vector<int> &ibFind);
	int FindDevicesPathByPartDevicePath(const GUID *pClassGuid, TCHAR partDevicePath[1], vector<tstring> *pList);
	int FindDevicesRegistryPropertyById(TCHAR pvmid[1], vector<tstring> *pList);

	int EnumHardwareId(vector<tstring> &ids, TCHAR *id = NULL);
	bool GetRevFromHardwareId(TCHAR * id, TCHAR *rev, int max);
	bool IsPlug(TCHAR *hardwareId);
};
#endif
